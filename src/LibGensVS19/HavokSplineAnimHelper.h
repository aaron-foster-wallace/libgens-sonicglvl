#pragma once
#include <cstdio>
#include "HavokApi.hpp"
//#include "datas/masterprinter.hpp"
//#include "datas/fileinfo.hpp"
#include "hkInternalInterfaces.h"

#include "hkaSplineDecompressor.h"
#include "hkObjectBase.h"
#include "hkaSplineCompressedAnimation.h"
namespace LibGens {
#pragma optimize( "", off )
	template<class C> struct Public_hkaSplineCompressedAnimation_t : hkaSplineCompressedAnimationInternalInterface, hkaSkeletalAnimation_t<typename C::parentClass>
	{
		typedef C value_type;
		typedef hkaSkeletalAnimation_t<typename C::parentClass> parent;
		//		hkClassConstructor(SplineCompressedAnimation_t);
		void SwapEndian() { hkaSkeletalAnimation_t<typename C::parentClass>::SwapEndian(); static_cast<value_type*>(this->Data)->SwapEndian(masterBuffer); }
		void Process() { decomp.Assign(this); };

		hkaSplineDecompressor decomp;
	};



	hkaSplineDecompressor*  getSplineDecompressor(const hkaAnimation* anim) {
		hkaAnimation* fanim = const_cast<hkaAnimation*>(anim);
		auto* forces = dynamic_cast<hkaSplineCompressedAnimation_t<hkaSplineCompressedSkeletalAnimation_t_shared<hkPointerX64, hkaSplineCompressedAnimation2016_t_sharedData>>*>(fanim);
		if (forces != NULL)return &forces->decomp;
		auto* generations = dynamic_cast<hkaSplineCompressedAnimation_t<hkaSplineCompressedAnimation2010_t<hkPointerX86>>*>(fanim);
		if (generations != NULL)return &generations->decomp;

		//hkaSplineCompressedAnimation_t<hkaSplineCompressedSkeletalAnimation_t_shared<hkPointerX64,hkaSplineCompressedAnimation2016_t_sharedData>> 
		
		//
		
		//auto* o = dynamic_cast<hkaSplineCompressedAnimation_t<hkaSplineCompressedSkeletalAnimation_t_shared<hkPointerX86>>*>(fanim);

		//auto* opener = reinterpret_cast<Public_hkaSplineCompressedAnimation_t<hkaSplineCompressedAnimation2010_t<hkPointerX86>>*>(o);
		
	}
#pragma optimize( "", on )
}