#include "fourier_transform.hpp"

namespace hpce{

// Declare factory functions which are implemented elsewhere.
std::shared_ptr<fourier_transform> Create_fast_fourier_transform();
std::shared_ptr<fourier_transform> Create_direct_fourier_transform();

// TODO : Declare your factories here

namespace hs2715{
	std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor_inner();
	std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor_outer();
	std::shared_ptr<fourier_transform> Create_fast_fourier_transform_taskgroup();
	std::shared_ptr<fourier_transform> Create_fast_fourier_transform_parfor();
}


void fourier_transform::RegisterDefaultFactories()
{
	static const unsigned MYSTERIOUS_LINE=0; // Don't remove me!

	RegisterTransformFactory("hpce.fast_fourier_transform", Create_fast_fourier_transform);
	RegisterTransformFactory("hpce.direct_fourier_transform", Create_direct_fourier_transform);

	// TODO : Add your factories here
	RegisterTransformFactory("hpce.hs2715.direct_fourier_transform_parfor_inner", hpce::hs2715::Create_direct_fourier_transform_parfor_inner);
	RegisterTransformFactory("hpce.hs2715.direct_fourier_transform_parfor_outer", hpce::hs2715::Create_direct_fourier_transform_parfor_outer);
	RegisterTransformFactory("hpce.hs2715.fast_fourier_transform_taskgroup", hpce::hs2715::Create_fast_fourier_transform_taskgroup);
	RegisterTransformFactory("hpce.hs2715.fast_fourier_transform_parfor", hpce::hs2715::Create_fast_fourier_transform_parfor);
}

}; // namespace hpce
