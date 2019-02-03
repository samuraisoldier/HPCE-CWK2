#include "fourier_transform.hpp"

#include <cmath>
#include <cassert>

#include "tbb/parallel_for.h"

namespace hpce
{
	namespace hs2715
{

class direct_fourier_transform_parfor_outer
	: public fourier_transform
{
protected:
	/*! We can do any size transform */
	virtual size_t calc_padded_size(size_t n) const
	{
		return n;
	}

	virtual void forwards_impl(
		size_t n,	const complex_t &wn,
		const complex_t *pIn,
		complex_t *pOut
	) const
	{
		assert(n>0);

		const real_t PI2=2*3.1415926535897932384626433832795;

		// = -i*2*pi / n
		complex_t neg_im_2pi_n=-complex_t(0, 1)*PI2 / (real_t)n;

		// Fill the output array with zero
		std::fill(pOut, pOut+n, 0);
		tbb::parallel_for(size_t(0),n,[&](size_t kk){
		//for(size_t ii=0;ii<n;ii++){
			for(size_t ii=0;ii<n;ii++){
				pOut[kk] += pIn[ii] * exp( neg_im_2pi_n * (double)kk * (double)ii );
			}
		});
	}

	virtual void backwards_impl(
		size_t n,	const complex_t &/*wn*/,
		const complex_t *pIn,
		complex_t *pOut
	) const
	{
		assert(n>0);

		const real_t PI2=2*3.1415926535897932384626433832795;

		// = i*2*pi / n
		complex_t im_2pi_n=complex_t(0, 1)*PI2 / (real_t)n;
		const real_t scale=real_t(1)/n;

		// Fill the output array with zero
		std::fill(pOut, pOut+n, 0);

		//for(size_t ii=0;ii<n;ii++){
		tbb::parallel_for(size_t(0),n,[&](size_t kk){
			for(size_t ii=0;ii<n;ii++){
				pOut[kk] += pIn[ii] * exp( im_2pi_n * (double)kk * (double)ii ) * scale;
			}
		});
	}

public:
	virtual std::string name() const
	{ return "hpce.hs2715.direct_fourier_transform_parfor_outer"; }

	virtual bool is_quadratic() const
	{ return true; }
};

std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor_outer()
{
	return std::make_shared<direct_fourier_transform_parfor_outer>();
}

}; // namespace hs2715
}; //namespace hpce
