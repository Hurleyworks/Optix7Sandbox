// This header file was auto-generated by ClassMate++
// Created: 1 Aug 2014 4:58:42 pm
// Copyright (c) 2014, HurleyWorks

#pragma  once

//http://stackoverflow.com/questions/6121792/how-to-check-if-a-cpu-supports-the-sse3-instruction-set/22521619#22521619

#ifdef _WIN32

//  windows
#define cpuID(info,x)    __cpuidex(info,x,0)

#else

//  GCC Inline Assembly
inline void cpuID(int CPUInfo[4],int InfoType){
    __asm__ __volatile__ (
        "cpuID":
        "=a" (CPUInfo[0]),
        "=b" (CPUInfo[1]),
        "=c" (CPUInfo[2]),
        "=d" (CPUInfo[3]) :
        "a" (InfoType), "c" (0)
    );
}

#endif

struct CPU
{
	//  Misc.
	bool HW_MMX;
	bool HW_x64;
	bool HW_ABM;      // Advanced Bit Manipulation
	bool HW_RDRAND;
	bool HW_BMI1;
	bool HW_BMI2;
	bool HW_ADX;

	//  SIMD: 128-bit
	bool HW_SSE;
	bool HW_SSE2;
	bool HW_SSE3;
	bool HW_SSSE3;
	bool HW_SSE41;
	bool HW_SSE42;
	bool HW_SSE4a;
	bool HW_AES;
	bool HW_SHA;

	//  SIMD: 256-bit
	bool HW_AVX;
	bool HW_XOP;
	bool HW_FMA3;
	bool HW_FMA4;
	bool HW_AVX2;

	//  SIMD: 512-bit
	bool HW_AVX512F;   //  AVX512 Foundation
	bool HW_AVX512CD;  //  AVX512 Conflict Detection
	bool HW_AVX512PF;  //  AVX512 Prefetch
	bool HW_AVX512ER;  //  AVX512 Exponential + Reciprocal
	bool HW_AVX512VL;  //  AVX512 Vector Length Extensions
	bool HW_AVX512BW;  //  AVX512 Byte + Word
	bool HW_AVX512DQ;  //  AVX512 Doubleword + Quadword

	void cpuFeatures()
	{
		int info[4];
		cpuID(info, 0);
		int nIds = info[0];

		cpuID(info, 0x80000000);
		unsigned nExIds = info[0];

		//  Detect Features
		if (nIds >= 0x00000001)
		{
			cpuID(info,0x00000001);
			HW_MMX    = (info[3] & ((int)1 << 23)) != 0;
			HW_SSE    = (info[3] & ((int)1 << 25)) != 0;
			HW_SSE2   = (info[3] & ((int)1 << 26)) != 0;
			HW_SSE3   = (info[2] & ((int)1 <<  0)) != 0;

			HW_SSSE3  = (info[2] & ((int)1 <<  9)) != 0;
			HW_SSE41  = (info[2] & ((int)1 << 19)) != 0;
			HW_SSE42  = (info[2] & ((int)1 << 20)) != 0;
			HW_AES    = (info[2] & ((int)1 << 25)) != 0;

			HW_AVX    = (info[2] & ((int)1 << 28)) != 0;
			HW_FMA3   = (info[2] & ((int)1 << 12)) != 0;

			HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
		}
		if (nIds >= 0x00000007)
		{
			cpuID(info,0x00000007);
			HW_AVX2   = (info[1] & ((int)1 <<  5)) != 0;

			HW_BMI1   = (info[1] & ((int)1 <<  3)) != 0;
			HW_BMI2   = (info[1] & ((int)1 <<  8)) != 0;
			HW_ADX    = (info[1] & ((int)1 << 19)) != 0;
			HW_SHA    = (info[1] & ((int)1 << 29)) != 0;

			HW_AVX512F  = (info[1] & ((int)1 << 16)) != 0;
			HW_AVX512CD = (info[1] & ((int)1 << 28)) != 0;
			HW_AVX512PF = (info[1] & ((int)1 << 26)) != 0;
			HW_AVX512ER = (info[1] & ((int)1 << 27)) != 0;
			HW_AVX512VL = (info[1] & ((int)1 << 31)) != 0;
			HW_AVX512BW = (info[1] & ((int)1 << 30)) != 0;
			HW_AVX512DQ = (info[1] & ((int)1 << 17)) != 0;
		}
		if (nExIds >= 0x80000001)
		{
			cpuID(info,0x80000001);
			HW_x64   = (info[3] & ((int)1 << 29)) != 0;
			HW_ABM   = (info[2] & ((int)1 <<  5)) != 0;
			HW_SSE4a = (info[2] & ((int)1 <<  6)) != 0;
			HW_FMA4  = (info[2] & ((int)1 << 16)) != 0;
			HW_XOP   = (info[2] & ((int)1 << 11)) != 0;
		}

		bool avxSupported = false;

		int cpuInfo[4];
		//__cpuid(cpuInfo, 1);

		bool osUsesXSAVE_XRSTORE = cpuInfo[2] & (1 << 27) || false;
		bool cpuAVXSuport = cpuInfo[2] & (1 << 28) || false;

		if ( HW_MMX) LOG(INFO) << "MMX" << " supported";
		if ( HW_x64) LOG(INFO) << "x64" << " supported";
		if ( HW_ABM) LOG(INFO) << "ABM" << " supported";      // Advanced Bit Manipulation
		if ( HW_RDRAND) LOG(INFO) << "RDRAND" << " supported";
		if ( HW_BMI1) LOG(INFO) << "BMI1" << " supported";
		if ( HW_BMI2) LOG(INFO) << "BMI2" << " supported";
		if ( HW_ADX) LOG(INFO) << "ADX" << " supported";

		//  SIMD: 128-bit
		if ( HW_SSE) LOG(INFO) << "SSE" << " supported";
		if ( HW_SSE2) LOG(INFO) << "SSE2" << " supported";
		if ( HW_SSE3) LOG(INFO) << "SSE3" << " supported";
		if ( HW_SSSE3) LOG(INFO) << "SSSE3" << " supported";
		if ( HW_SSE41) LOG(INFO) << "SSE41" << " supported";
		if ( HW_SSE42) LOG(INFO) << "SSE42" << " supported";
		if ( HW_SSE4a) LOG(INFO) << "SSE4a" << " supported";
		if ( HW_AES) LOG(INFO) << "AES" << " supported";
		if ( HW_SHA) LOG(INFO) << "SHA" << " supported";

		//  SIMD: 256-bit
		if ( HW_AVX) LOG(INFO) << "AVX" << " supported";
		if ( HW_XOP) LOG(INFO) << "XOP" << " supported";
		if ( HW_FMA3) LOG(INFO) << "FMA3" << " supported";
		if ( HW_FMA4) LOG(INFO) << "FMA4" << " supported";
		if ( HW_AVX2) LOG(INFO) << "AVX2" << " supported";

		//  SIMD: 512-bit
		if ( HW_AVX512F) LOG(INFO) << "AVX512F" << " supported";   //  AVX512 Foundation
		if ( HW_AVX512CD) LOG(INFO) << "AVX512CD" << " supported";  //  AVX512 Conflict Detection
		if ( HW_AVX512PF) LOG(INFO) << "AVX512PF" << " supported";  //  AVX512 Prefetch
		if ( HW_AVX512ER) LOG(INFO) << "AVX512ER" << " supported";  //  AVX512 Exponential + Reciprocal
		if ( HW_AVX512VL) LOG(INFO) << "AVX512VL" << " supported";  //  AVX512 Vector Length Extensions
		if ( HW_AVX512BW) LOG(INFO) << "AVX512BW" << " supported";  //  AVX512 Byte + Word
		if ( HW_AVX512DQ) LOG(INFO) << "AVX512DQ" << " supported";  //  AVX512 Doubleword + Quadword

		
		if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
		{
			//unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			//avxSupported = (xcrFeatureMask & 0x6) || false;
			//if (avxSupported)
			//	LOG(INFO) << "This OS supports AVX";
		}
	}
};