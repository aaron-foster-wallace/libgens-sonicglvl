// HavokLibSamples.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include "HavokApi.hpp"
#include "datas/masterprinter.hpp"
#include "datas/fileinfo.hpp"
#include "hkInternalInterfaces.h"



#if _MSC_VER
#include <tchar.h>
#else
#define _tmain main
#define _TCHAR char
#endif

#define MyAssert(thing) if (thing) printerror(#thing, << _T(" :") << __FUNCTION__ <<_T(" at:") <<__LINE__)
#define MyAssertLoop(thing, i) if (thing) printerror(#thing, << _T(" :") << __FUNCTION__ <<_T(" at:") <<__LINE__<< _T(" loop:") << i)

static const TCHAR *compiledFlags[] =
{
	_T("[4001].hkx"),
	_T("[4011].hkx"),
	_T("[4101].hkx"),
	_T("[4111].hkx"),
	_T("[8001].hkx"),
	_T("[8011].hkx"),
	_T("[8101].hkx"),
	_T("[8111].hkx"),
};

static const TCHAR *versions[] =
{
	_T("550/"),
	_T("660/"),
	_T("710/"),
	_T("2010_1/"),
	_T("2011_1/"),
	_T("2011_2/"),
	_T("2012_2/"),
	_T("2013_1/"),
	_T("2014_1/"),
};

static const TCHAR *versions_delta[] =
{
	_T("550/"),
	_T("660/"),
	_T("710/"),
	_T("2010_1/"),
};

#include "SkeletonTesting.h"
#include "InterleavedTesting.h"

#ifdef _MSC_VER
// Use Visual C++'s memory checking functionality
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _MSC_VER
#include <iostream>
#include "../source/hkaSplineDecompressor.h"
#include "../source/hkObjectBase.h"
#include "../source/hkaSplineCompressedAnimation.h"

using namespace std;
void TestMine(const string fleName);

int _tmain(const int argc, const TCHAR *argv[])
{
#ifdef _MSC_VER
	//_crtBreakAlloc = 121467;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _MSC_VER


#ifdef UNICODE
	printer.AddPrinterFunction(wprintf);
#else
	printer.AddPrinterFunction(reinterpret_cast<void*>(printf));
#endif

#define MY_TEST 1

#ifdef MY_TEST
	auto testingPath = R"(D:\Escritorio2\Havoc content tool plugin\sn_dash_loop.anm.hkx)";


	TestMine(testingPath);
#else
	TFileInfo info(argv[0]);

	TSTRING testingPath = info.GetPath() + _T("rc/");

	TestAllosaurSkeleton(testingPath);
	TestAllosaurInterleaved(testingPath);
	TestAllosaurDelta(testingPath);
	TestAllosaurSpline1(testingPath);
	TestAllosaurSpline2(testingPath);
	TestAllosaurSpline3(testingPath);
#endif
	return 0;
}

template<class C> struct PublichkaSplineCompressedAnimation_t : hkaSplineCompressedAnimationInternalInterface, hkaSkeletalAnimation_t<typename C::parentClass>
{
		typedef C value_type;
		typedef hkaSkeletalAnimation_t<typename C::parentClass> parent;
//		hkClassConstructor(SplineCompressedAnimation_t);
		void SwapEndian() { hkaSkeletalAnimation_t<typename C::parentClass>::SwapEndian(); static_cast<value_type*>(this->Data)->SwapEndian(masterBuffer); }
		void Process() { decomp.Assign(this); };

	hkaSplineDecompressor decomp;
};



hkaSplineDecompressor* getSplineDecompressor(const hkaAnimation* anim) {
	hkaAnimation* fanim= const_cast<hkaAnimation*>(anim);
	auto *o=dynamic_cast<hkaSplineCompressedAnimation_t<hkaSplineCompressedAnimation2010_t<hkPointerX86>>*>(fanim);
	auto * opener=	reinterpret_cast<PublichkaSplineCompressedAnimation_t<hkaSplineCompressedAnimation2010_t<hkPointerX86>>*>(o);
	return & opener->decomp;

}


void TestMine(const string fleName)
{
	cout << "testing: " << fleName;
	IhkPackFile* hdr = IhkPackFile::Create(fleName.c_str());

	if (!hdr)
	{
		cout << "testing failed!";
		return;
	}

	hkRootLevelContainer* rcont = hdr->GetRootLevelContainer();

	if (!rcont)
	{
		cout << "Couldn't find hkRootLevelContainer!";
		return;
	}

	const hkaAnimationContainer* aniCont = rcont->GetVariant(0);

	//TestBinding(aniCont);

	const hkaAnimation* anim = aniCont->GetAnimation(0);

	auto x = getSplineDecompressor(anim);

	for (int cAnnot = 0; cAnnot < anim->GetNumAnnotations(); cAnnot++)
	{
		cout <<
			anim->GetAnnotation(cAnnot).get()->GetName() <<
			cAnnot;
	}

	// Add some further testing here?
	//opener->decomp;
	delete hdr;

}