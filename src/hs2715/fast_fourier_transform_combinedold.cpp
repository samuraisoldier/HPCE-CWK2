#include "fourier_transform.hpp"

#include <cmath>
#include <cassert>
#include <cstdlib>
#include "tbb/parallel_for.h"
#include "tbb/task_group.h"

namespace hpce
{

namespace hs2715
{

class fast_fourier_transform_combined
	: public fourier_transform
{
size_t K_recur; 
size_t K_loop; 
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
	
	
			if (n>K_recur){//parallel only if n is bigger than K otherwise nothing to make parallel
				tbb::task_group group; 
				group.run( [&](){ recurse(m,wn*wn,pIn,2*sIn,pOut,sOut);} );
				group.run( [&](){ recurse(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);} );
				group.wait();
			}else{ //sequential
				recurse(m,wn*wn,pIn,2*sIn,pOut,sOut);
				recurse(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
			}

			if(m<=K_loop){ 
				complex_t w=complex_t(1, 0);
				for (size_t j=0;j<m;j++){
					complex_t t1 = w*pOut[m+j];
					complex_t t2 = pOut[j]-t1;
					pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
					pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
					w = w*wn;
				}
			}else{ 
				tbb::parallel_for(tbb::blocked_range<unsigned>(0,m,K_loop), [&](const tbb::blocked_range<unsigned> &chunk){
					complex_t w=complex_t(1, 0);
					w = std::pow(wn,chunk.begin());					
					for(unsigned j=chunk.begin(); j!=chunk.end(); j++){
						complex_t t1 = w*pOut[m+j];
						complex_t t2 = pOut[j]-t1; 
						pOut[j] = pOut[j]+t1; 
						pOut[j+m] = t2; 
						w = w*wn; 
					}
				}, tbb::simple_partitioner()); 
		
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
	fast_fourier_transform_combined ()
	{
		char *K_loop_env = getenv("HPCE_FFT_LOOP_K"); 
		if (K_loop_env != NULL){
			K_loop = atoi(K_loop_env); 
		}else{
			K_loop=16; 
		}
	
		char *K_recur_env = getenv("HPCE_FFT_RECURSION_K"); 
		if (K_recur_env != NULL){
			K_recur = atoi(K_recur_env);
		}else{
			K_recur=32;
		}
	}
	virtual std::string name() const
	{ return "hpce.hs2715.fast_fourier_transform_combined"; }

	virtual bool is_quadratic() const
	{ return false; }
};

std::shared_ptr<fourier_transform> Create_fast_fourier_transform_combined()
{
	return std::make_shared<fast_fourier_transform_combined>();
}

}; //namespace hs2715

}; // namespace hpce
