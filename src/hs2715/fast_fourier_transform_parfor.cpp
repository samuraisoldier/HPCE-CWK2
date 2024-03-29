#include "fourier_transform.hpp"

#include <cmath>
#include <cassert>
#include <cstdlib>
#include "tbb/parallel_for.h"

namespace hpce
{

namespace hs2715
{

class fast_fourier_transform_parfor
	: public fourier_transform
{
size_t K; 
protected:
	/* Standard radix-2 FFT only supports binary power lengths */
	virtual size_t calc_padded_size(size_t n) const
	{
		assert(n!=0);

		size_t ret=1;
		while(ret<n){
			ret<<=1;
		}

		return ret;
	}

	virtual void recurse(
		size_t n,	const complex_t &wn,
		const complex_t *pIn, size_t sIn,
		complex_t *pOut, size_t sOut
	) const
	{
		assert(n>0);

		if (n == 1){
			pOut[0] = pIn[0];
		}else if (n == 2){
			pOut[0] = pIn[0]+pIn[sIn];
			pOut[sOut] = pIn[0]-pIn[sIn];
		}else{
			size_t m = n/2;

			recurse(m,wn*wn,pIn,2*sIn,pOut,sOut);
			recurse(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);

			if(m<=K){ //if the chunks are too small do it the normal way
				complex_t w=complex_t(1, 0);
				for (size_t j=0;j<m;j++){
				 	complex_t t1 = w*pOut[m+j];
				  	complex_t t2 = pOut[j]-t1;
				  	pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
				  	pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
				  	w = w*wn;
				}
			}else{ //else parallel it
			//typedef tbb::blocked_range<unsigned> my_range_t;
			
			//my_range_t range(0,m,K);				

				
				tbb::parallel_for(tbb::blocked_range<unsigned>(0,m,K), [&](const tbb::blocked_range<unsigned> &chunk){
				
				//auto f=[&](const my_range_t &chunk){

					
					complex_t w=complex_t(1, 0);

					w = std::pow(wn,chunk.begin()); //can this be done better

					
					for(unsigned j=chunk.begin(); j!=chunk.end(); j++){
						complex_t t1 = w*pOut[m+j];
						complex_t t2 = pOut[j]-t1; 
						pOut[j] = pOut[j]+t1; 
						pOut[j+m] = t2; 
						w = w*wn; 
					}
				}, tbb::simple_partitioner()); 
			//tbb::parallel_for(range, f, tbb::simple_partitioner());				
			}
		}
	}

	virtual void forwards_impl(
		size_t n,	const complex_t &wn,
		const complex_t *pIn,
		complex_t *pOut
	) const
	{
		assert(n>0);

		recurse(n,wn,pIn,1,pOut,1);
	}

	virtual void backwards_impl(
		size_t n,	const complex_t &wn,
		const complex_t *pIn,
		complex_t *pOut
	) const
	{
		complex_t reverse_wn=real_t(1)/wn;
		recurse(n, reverse_wn, pIn, 1, pOut, 1);

		real_t scale=real_t(1)/n;
		for(size_t i=0;i<n;i++){
			pOut[i]=pOut[i]*scale;
		}
	}

public:
	fast_fourier_transform_parfor(){ //fn to get K from env once
		
		char *K_env = getenv("HPCE_FFT_LOOP_K"); 
		if (K_env != NULL){
			K = atoi(K_env); 
		}else{
			K=16; 
		}
	}
	virtual std::string name() const
	{ return "hpce.hs2715.fast_fourier_transform_parfor"; }

	virtual bool is_quadratic() const
	{ return false; }
};

std::shared_ptr<fourier_transform> Create_fast_fourier_transform_parfor()
{
	return std::make_shared<fast_fourier_transform_parfor>();
}

}; //namespace hs2715

}; // namespace hpce
