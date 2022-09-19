﻿#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "stdint.h"
#include <fstream>
#include <stack>
#include <list>
#include <vector>
#include <iostream>

#define ISET_ARM		0x0
#define ISET_THUMB		0x1
#define ISET_JAZELLE    0x2
#define ISET_AARCH64    0x4

//[modified by pz]
#define SREG_SIZE		7	
#define DREG_SIZE		6

#define INVAL_REG		100
#define NINF			INT_MIN
/* v8 isa size */
#define INST_A64_SIZE	1000
#define INST_A32_SIZE	1000

#include "data_struct.h"

typedef uint32_t MachInst;
typedef uint64_t ExtMachInst;

using namespace std;

template <class T>
inline T bits(T val, int first, int last);

template <class T>
inline T bits(T val, int bit);

uint64_t minus64(const uint64_t &a, const uint64_t &b);

inline uint64_t mask(int nbits);

struct InstType
{
	bool isLoad = false;
	bool isStore = false;
	bool isBranch = false;
	bool isSerial = false;
	bool isFloat = false;
	bool isCc = false;
	bool isMISC = false;
	bool isUnKnown = false;
};

#define REG_NUM	100 /* [modified by pz] both Fp regs & gen-purp regs */

class InstReg
{
public:
	uint32_t	DReg[DREG_SIZE];/*  [pz]destation register, 2-5 indices are Fp regs */
	uint32_t	SReg[SREG_SIZE];/*  [pz]source register, 3-6 indices are Fp regs */
	//uint64_t    numSrcRegs;
	//uint64_t    numDestRegs;
	uint32_t	delay;
	bool		AluShift;		//[pz]a flag used to define the latency of ALU instructions when shifted or extended

	InstReg() {// : numDestRegs(0), numSrcRegs(0) {
		for (int i = 0; i < DREG_SIZE; ++i)
			DReg[i] = INVAL_REG;
		for (int i = 0; i < SREG_SIZE; ++i)
			SReg[i] = INVAL_REG;

		AluShift = false;	//[pz]when true,delay += 1
	}

	const bool isDesLegel(const int &i) { return DReg[i] < INVAL_REG; }

	const bool isSrcLegel(const int &i) { return SReg[i] < INVAL_REG; }
	/* 2-3 indices are destination Fp regs */
	const bool isFpDesReg(const int &i) { return i > 1; }
	/* 3-5 indices are Fp source regs */
	const bool isFpSrcReg(const int &i) { return i > 2; }
};

struct Branch
{
	uint32_t CtrlNum = 0;
	uint32_t BasicBlockSize = 0;
	uint32_t bwBranchs = 0;
	uint32_t fwBranchs = 0;
	uint32_t notTakenBranchs = 0;
	uint32_t takenChanged = 0;
};

struct Trace
{
	uint32_t	intRegReadNum = 0;
	uint32_t	intRegWriteNum = 0;
	uint32_t	fpRegReadNum = 0;
	uint32_t	fpRegWriteNum = 0;
	uint32_t	ccRegReadNum = 0;
	uint32_t	ccRegWriteNum = 0;
	uint32_t	miscRegReadNum = 0;
	uint32_t	miscRegWriteNum = 0;
};

struct InstMix {
	uint32_t	intAluNum = 0;
	uint32_t	intMultiDivNum = 0;
	uint32_t	intMulNum = 0;
	uint32_t	fpNeonAluNum = 0;
	uint32_t	fpNeonDivNum = 0;
	uint32_t	fpNeonMulNum = 0;
	uint32_t	intInstNum = 0;
	uint32_t	fpInstNum = 0;
	uint32_t	loadInstNum = 0;
	uint32_t	storeInstNum = 0;
	uint32_t	branchInstNum = 0;
	uint32_t	SIMDInstNum = 0;
	uint32_t	serialInstNum = 0;
	uint32_t    unKnownNum = 0;
};

/* Arm V8 instruction Architecture Set */
namespace aarch32 {
	enum Aarch32Set {
		Unknown32 = 0,

		//Branch Instructin
		// latency = 1
		B,
		Bl,
		BlxImm,
		BlxReg,
		BxReg,
		Cbnz,
		Cbz,

		//ALU Instruction
		// latency = 1
		// shen 11/12
		AdcsImmPclr,
		AdcsRegPclr,
		AddsImmPclr,
		AddsRegPclr,
		AndsImmPclr,
		AndsRegPclr,
		BicsImmPclr,
		BicsRegPclr,
		Breakpoint,
		EorsImmPclr,
		EorsRegPclr,
		// shen 11/12
		AdcReg,
		AdcImm,
		AddImm,
		AddReg,
		AdrImm,
		AndImm,
		AndReg,
		BicImm,
		BicReg,
		CmnImm,
		CmnReg,
		CmpImm,
		CmpReg,
		EorImm,
		EorReg,
		OrnImm,
		OrnReg,
		OrrImm,
		OrrReg,
		RsbImm,
		RsbReg,

		// shen 11/12
		RsbsImmPclr,
		RsbsRegPclr,
		RscImm,
		RscImmCc,
		RscReg,
		RscRegCc,
		RscsImmPclr,
		RscsRegPclr,
		// shen 11/12

		SbcImm,
		SbcReg,
		SubImm,
		SubReg,
		TeqImm,
		TeqReg,
		TstImm,
		TstReg,
		TeqImmCc,
		TeqRegCc,
		TstImmCc,
		TstRegCc,

		//Miscellaneous Data-Processing Instructions
		// latency = 1
		Sbfx,
		Ubfx,
		Clz,
		Rbit,
		Rev,
		Rev16,
		Revsh,
		Sel,
		Sxtb,
		Sxth,
		Uxtb,
		Uxth,

		//Move And Shift Instruction
		// latency = 1
		MovRegRegASR,
		MovRegRegLSL,
		MovRegRegLSR,
		MovRegRegROR,
		MovRegReg,
		MvnReg,
		MvnImm,
		MovReg,
		MovImm,
		MovImmCc,
		MovRegCc,

		// latency = 2
		AdcImmCc,
		AdcRegCc,
		AddImmCc,
		AddImmS,
		AddRegCc,
		AndImmCc,
		AndRegCc,
		BicImmCc,
		BicRegCc,
		CmnImmCc,
		CmnRegCc,
		CmpImmCc,
		CmpRegCc,
		EorImmCc,
		EorRegCc,
		OrnImmCc,
		OrnRegCc,
		OrrImmCc,
		OrrRegCc,
		// shen 11/12
		OrrsImmPclr,
		OrrsRegPclr,
		// shen 11/12
		RsbImmCc,
		RsbRegCc,
		SbcImmCc,
		SbcRegCc,
		SubImmCc,
		SubImmS,
		SubRegCc,
		SubsRegPclr,
		SubsImmPclr,
		SbcsImmPclr,
		SbcsRegPclr,

		//Saturating and Parallel Arithmetic Instructions
		// latency = 2
		Qadd16Reg,
		Qadd8Reg,
		Qsub16Reg,
		Qsub8Reg,
		QAddRegCc,
		QsubRegCc,
		QaddRegCc,
		QdaddRegCc,
		Sadd16RegCc,
		Sadd8RegCc,
		Ssub16RegCc,
		Ssub8RegCc,
		Shadd16Reg,
		Shadd8Reg,
		Shsub16Reg,
		Shsub8Reg,
		Ssat,
		Ssat16,
		Uadd16RegCc,
		Uadd8RegCc,
		Usub16RegCc,
		Usub8RegCc,
		Uhadd16Reg,
		Uhadd8Reg,
		Uhsub16Reg,
		Uhsub8Reg,
		Uqadd16Reg,
		Uqadd8Reg,
		Uqsub16Reg,
		Uqsub8Reg,
		Usat,
		Usat16,

		// latency = 2
		Bfc,
		Bfi,
		PkhbtReg,
		PkhtbReg,
		Sxtb16,
		Uxtb16,
		Sxtab,
		Sxtah,
		Uxtab,
		Uxtah,

		// latency is 2
		// shen 11/12
		MvnImmCc,
		MvnsImmPclr,
		MvnsRegPclr,
		// shen 11/12

		// latency = 2
		MovRegRegASRCc,
		MovRegRegCc,
		MovRegRegLSLCc,
		MovRegRegLSRCc,
		MovRegRegRORCc,
		MovtImm,
		MovsImmPclr,
		MovsRegPclr,

		// latency = 3
		QasxReg,
		QsaxReg,
		QdAddRegCc,
		QdsubRegCc,
		SasxRegCc,
		SsaxRegCc,
		ShasxReg,
		ShsaxReg,
		UasxRegCc,
		UsaxRegCc,
		UhasxReg,
		UhsaxReg,
		UqasxReg,
		UqsaxReg,

		//Divide and Multiply Instructions
		// latency = 3
		Mla,
		// shen 11/12
		MlaCc,
		// shen 11/12
		Mls,

		Mul,
		MvnRegCc,
		Smulbb,
		Smulbt,
		Smultb,
		Smultt,
		Smulwb,
		Smulwt,
		Smmul,
		Smmulr,
		Smusd,
		Smusdx,
		Smmla,
		Smmlar,
		Smmls,
		Smmlsr,

		// latency = 3
		Usad8,
		Usada8,

		// latency = 4
		Sxtab16,
		Uxtab16,

		// latency = 4
		MulCc,
		Sdiv,
		SmuadCc,
		SmuadxCc,
		SmlabbCc,
		SmlabtCc,
		SmladCc,
		SmladxCc,
		Smlald,
		Smlaldx,
		Smlalbb,
		Smlalbt,
		Smlaltb,
		Smlaltt,
		Smlal,
		SmlatbCc,
		SmlattCc,
		SmlawbCc,
		SmlawtCc,
		SmlsdCc,
		SmlsdxCc,
		Smlsld,
		Smlsldx,
		Smull,
		SmlalCc,
		SmullCc,
		Udiv,
		UmlalCc,
		UmullCc,
		Umaal,
		Umlal,
		Umull,

		//Load Instruction
		// latency = 4
		LdmStmLd,
		LdmStmSt,
		Ldr,
		Ldrb,
		Ldrb_iadd,
		Ldrb_ip,
		Ldrb_ipuw,
		Ldrb_ipw,
		Ldrb_iuw,
		Ldrb_iw,
		Ldrb_lit,
		Ldrb_lit_u,
		Ldrb_radd,
		Ldrbt,
		Ldrd_p,
		Ldrd_pu,
		Ldrd_puw,
		Ldrd_pw,
		Ldrd_uw,
		Ldrd_w,
		Ldrex,
		Ldrexb,
		Ldrexd,
		Ldrexh,
		Ldrh,
		Ldrh_iadd,
		Ldrh_ip,
		Ldrh_ipuw,
		Ldrh_ipw,
		Ldrh_iuw,
		Ldrh_iw,
		Ldrh_lit,
		Ldrh_lit_u,
		Ldrh_radd,
		Ldrht,
		Ldrimm_p,
		Ldrimm_pu,
		Ldrimm_puw,
		Ldrimm_pw,
		Ldrimm_uw,
		Ldrimm_w,
		Ldrlit,
		Ldrliteral,
		Ldrliteral_u,
		Ldrreg,
		LdrS,
		Ldrsb,
		Ldrsb_iadd,
		Ldrsb_ip,
		Ldrsb_ipuw,
		Ldrsb_ipw,
		Ldrsb_iuw,
		Ldrsb_iw,
		Ldrsb_lit,
		Ldrsb_lit_u,
		Ldrsb_radd,
		Ldrsbt,
		Ldrsh,
		Ldrsh_iadd,
		Ldrsh_ip,
		Ldrsh_ipuw,
		Ldrsh_ipw,
		Ldrsh_iuw,
		Ldrsh_iw,
		Ldrsh_lit,
		Ldrsh_lit_u,
		Ldrsh_radd,
		Ldrsht,
		Ldrt,

		// shen 11/12
		LDREXB_LOAD_IMM_AY_PN_SN_UN_WN_SZ1,
		LDREXD_LOAD_IMMD_AY_PN_SN_UN_WN_SZ4,
		LDREXH_LOAD_IMM_AY_PN_SN_UN_WN_SZ2,
		LDREX_LOAD_IMM_AY_PN_SN_UN_WN_SZ4,
		LOAD_IMM_AN_PN_SN_UN_WN_SZ1,
		LOAD_IMM_AN_PN_SN_UN_WN_SZ2,
		LOAD_IMM_AN_PN_SN_UN_WN_SZ4,
		LOAD_IMM_AN_PN_SN_UN_WY_SZ1,
		LOAD_IMM_AN_PN_SN_UN_WY_SZ2,
		LOAD_IMM_AN_PN_SN_UN_WY_SZ4,
		LOAD_IMM_AN_PN_SY_UN_WN_SZ1,
		LOAD_IMM_AN_PN_SY_UN_WN_SZ2,
		LOAD_IMM_AN_PN_SY_UN_WY_SZ1,
		LOAD_IMM_AN_PN_SY_UN_WY_SZ2,
		LOAD_IMM_AN_PY_SN_UN_WY_SZ1,
		LOAD_IMM_AN_PY_SN_UN_WY_SZ2,
		LOAD_IMM_AN_PY_SN_UN_WY_SZ4,
		LOAD_IMM_AN_PY_SN_UY_WY_SZ1,
		LOAD_IMM_AN_PY_SN_UY_WY_SZ2,
		LOAD_IMM_AN_PY_SN_UY_WY_SZ4,
		LOAD_IMM_AN_PY_SY_UN_WY_SZ1,
		LOAD_IMM_AN_PY_SY_UN_WY_SZ2,
		LOAD_IMM_AN_PY_SY_UY_WY_SZ1,
		LOAD_IMM_AN_PY_SY_UY_WY_SZ2,
		LOAD_IMM_AY_PN_SN_UN_WN_SZ1,
		LOAD_IMM_AY_PN_SN_UN_WN_SZ2,
		LOAD_IMM_AY_PN_SN_UN_WN_SZ4,
		LOAD_IMM_AY_PN_SN_UN_WY_SZ1,
		LOAD_IMM_AY_PN_SN_UN_WY_SZ2,
		LOAD_IMM_AY_PN_SN_UN_WY_SZ4,
		LOAD_IMM_AY_PN_SY_UN_WN_SZ1,
		LOAD_IMM_AY_PN_SY_UN_WN_SZ2,
		LOAD_IMM_AY_PN_SY_UN_WY_SZ1,
		LOAD_IMM_AY_PN_SY_UN_WY_SZ2,
		LOAD_IMM_AY_PY_SN_UN_WY_SZ1,
		LOAD_IMM_AY_PY_SN_UN_WY_SZ2,
		LOAD_IMM_AY_PY_SN_UN_WY_SZ4,
		LOAD_IMM_AY_PY_SN_UY_WY_SZ1,
		LOAD_IMM_AY_PY_SN_UY_WY_SZ2,
		LOAD_IMM_AY_PY_SN_UY_WY_SZ4,
		LOAD_IMM_AY_PY_SY_UN_WY_SZ1,
		LOAD_IMM_AY_PY_SY_UN_WY_SZ2,
		LOAD_IMM_AY_PY_SY_UY_WY_SZ1,
		LOAD_IMM_AY_PY_SY_UY_WY_SZ2,
		LOAD_IMMD_AN_PN_SN_UN_WN_SZ4,
		LOAD_IMMD_AN_PN_SN_UN_WY_SZ4,
		LOAD_IMMD_AN_PY_SN_UN_WY_SZ4,
		LOAD_IMMD_AY_PN_SN_UN_WN_SZ4,
		LOAD_IMMD_AY_PN_SN_UN_WY_SZ4,
		LOAD_IMMD_AY_PY_SN_UN_WY_SZ4,
		LOAD_REG_AN_PN_SN_UN_WN_SZ1,
		LOAD_REG_AN_PN_SN_UN_WN_SZ2,
		LOAD_REG_AN_PN_SN_UN_WN_SZ4,
		LOAD_REG_AN_PN_SN_UN_WY_SZ1,
		LOAD_REG_AN_PN_SN_UN_WY_SZ2,
		LOAD_REG_AN_PN_SN_UN_WY_SZ4,
		LOAD_REG_AN_PN_SY_UN_WN_SZ1,
		LOAD_REG_AN_PN_SY_UN_WN_SZ2,
		LOAD_REG_AN_PN_SY_UN_WY_SZ1,
		LOAD_REG_AN_PN_SY_UN_WY_SZ2,
		LOAD_REG_AN_PY_SN_UN_WY_SZ1,
		LOAD_REG_AN_PY_SN_UN_WY_SZ2,
		LOAD_REG_AN_PY_SN_UN_WY_SZ4,
		LOAD_REG_AN_PY_SN_UY_WY_SZ1,
		LOAD_REG_AN_PY_SN_UY_WY_SZ2,
		LOAD_REG_AN_PY_SN_UY_WY_SZ4,
		LOAD_REG_AN_PY_SY_UN_WY_SZ1,
		LOAD_REG_AN_PY_SY_UN_WY_SZ2,
		LOAD_REG_AN_PY_SY_UY_WY_SZ1,
		LOAD_REG_AN_PY_SY_UY_WY_SZ2,
		LOAD_REG_AY_PN_SN_UN_WN_SZ1,
		LOAD_REG_AY_PN_SN_UN_WN_SZ2,
		LOAD_REG_AY_PN_SN_UN_WN_SZ4,
		LOAD_REG_AY_PN_SN_UN_WY_SZ1,
		LOAD_REG_AY_PN_SN_UN_WY_SZ2,
		LOAD_REG_AY_PN_SN_UN_WY_SZ4,
		LOAD_REG_AY_PN_SY_UN_WN_SZ1,
		LOAD_REG_AY_PN_SY_UN_WN_SZ2,
		LOAD_REG_AY_PN_SY_UN_WY_SZ1,
		LOAD_REG_AY_PN_SY_UN_WY_SZ2,
		LOAD_REG_AY_PY_SN_UN_WY_SZ1,
		LOAD_REG_AY_PY_SN_UN_WY_SZ2,
		LOAD_REG_AY_PY_SN_UN_WY_SZ4,
		LOAD_REG_AY_PY_SN_UY_WY_SZ1,
		LOAD_REG_AY_PY_SN_UY_WY_SZ2,
		LOAD_REG_AY_PY_SN_UY_WY_SZ4,
		LOAD_REG_AY_PY_SY_UN_WY_SZ1,
		LOAD_REG_AY_PY_SY_UN_WY_SZ2,
		LOAD_REG_AY_PY_SY_UY_WY_SZ1,
		LOAD_REG_AY_PY_SY_UY_WY_SZ2,
		LOAD_REGD_AN_PN_SN_UN_WN_SZ4,
		LOAD_REGD_AN_PN_SN_UN_WY_SZ4,
		LOAD_REGD_AN_PY_SN_UN_WY_SZ4,
		LOAD_REGD_AY_PN_SN_UN_WN_SZ4,
		LOAD_REGD_AY_PN_SN_UN_WY_SZ4,
		LOAD_REGD_AY_PY_SN_UN_WY_SZ4,
		// shen 11/12

		Pld_iadd,
		Pld_ilit,
		Pld_isub,
		Pld_iulit,

		// shen 11/12
		PLD_LOAD_IMM_AN_PN_SN_UN_WN_SZ1,
		PLD_LOAD_IMM_AY_PN_SN_UN_WN_SZ1,
		PLD_LOAD_REG_AN_PN_SN_UN_WN_SZ1,
		PLD_LOAD_REG_AY_PN_SN_UN_WN_SZ1,
		PLDW_LOAD_IMM_AN_PN_SN_UN_WN_SZ1,
		PLDW_LOAD_IMM_AY_PN_SN_UN_WN_SZ1,
		PLDW_LOAD_REG_AN_PN_SN_UN_WN_SZ1,
		PLDW_LOAD_REG_AY_PN_SN_UN_WN_SZ1,
		// shen 11/12

		// shen 11/12
		PLI_LOAD_IMM_AN_PN_SN_UN_WN_SZ1,
		PLI_LOAD_IMM_AY_PN_SN_UN_WN_SZ1,
		PLI_LOAD_REG_AN_PN_SN_UN_WN_SZ1,
		PLI_LOAD_REG_AY_PN_SN_UN_WN_SZ1,
		//shen 11/12

		Pld_radd,
		Pldw_iadd,
		Pldw_isub,
		Pldw_radd,
		Pli_ilit,
		Pli_iulit,
		Pli_radd,

		//store instructions
		// latency = 1
		Str,

		// shen 11/12
		STORE_IMM_AN_PN_SN_UN_WN_SZ1,
		STORE_IMM_AN_PN_SN_UN_WN_SZ2,
		STORE_IMM_AN_PN_SN_UN_WN_SZ4,
		STORE_IMM_AN_PN_SN_UN_WY_SZ1,
		STORE_IMM_AN_PN_SN_UN_WY_SZ2,
		STORE_IMM_AN_PN_SN_UN_WY_SZ4,
		STORE_IMM_AN_PY_SN_UN_WY_SZ1,
		STORE_IMM_AN_PY_SN_UN_WY_SZ2,
		STORE_IMM_AN_PY_SN_UN_WY_SZ4,
		STORE_IMM_AN_PY_SN_UY_WY_SZ1,
		STORE_IMM_AN_PY_SN_UY_WY_SZ2,
		STORE_IMM_AN_PY_SN_UY_WY_SZ4,
		STORE_IMM_AY_PN_SN_UN_WN_SZ1,
		STORE_IMM_AY_PN_SN_UN_WN_SZ2,
		STORE_IMM_AY_PN_SN_UN_WN_SZ4,
		STORE_IMM_AY_PN_SN_UN_WY_SZ1,
		STORE_IMM_AY_PN_SN_UN_WY_SZ2,
		STORE_IMM_AY_PN_SN_UN_WY_SZ4,
		STORE_IMM_AY_PY_SN_UN_WY_SZ1,
		STORE_IMM_AY_PY_SN_UN_WY_SZ2,
		STORE_IMM_AY_PY_SN_UN_WY_SZ4,
		STORE_IMM_AY_PY_SN_UY_WY_SZ1,
		STORE_IMM_AY_PY_SN_UY_WY_SZ2,
		STORE_IMM_AY_PY_SN_UY_WY_SZ4,
		STORE_IMMD_AN_PN_SN_UN_WN_SZ4,
		STORE_IMMD_AN_PN_SN_UN_WY_SZ4,
		STORE_IMMD_AN_PY_SN_UN_WY_SZ4,
		STORE_IMMD_AY_PN_SN_UN_WN_SZ4,
		STORE_IMMD_AY_PN_SN_UN_WY_SZ4,
		STORE_IMMD_AY_PY_SN_UN_WY_SZ4,
		STORE_REG_AN_PN_SN_UN_WN_SZ1,
		STORE_REG_AN_PN_SN_UN_WN_SZ2,
		STORE_REG_AN_PN_SN_UN_WN_SZ4,
		STORE_REG_AN_PN_SN_UN_WY_SZ1,
		STORE_REG_AN_PN_SN_UN_WY_SZ2,
		STORE_REG_AN_PN_SN_UN_WY_SZ4,
		STORE_REG_AN_PY_SN_UN_WY_SZ1,
		STORE_REG_AN_PY_SN_UN_WY_SZ2,
		STORE_REG_AN_PY_SN_UN_WY_SZ4,
		STORE_REG_AN_PY_SN_UY_WY_SZ1,
		STORE_REG_AN_PY_SN_UY_WY_SZ2,
		STORE_REG_AN_PY_SN_UY_WY_SZ4,
		STORE_REG_AY_PN_SN_UN_WN_SZ1,
		STORE_REG_AY_PN_SN_UN_WN_SZ2,
		STORE_REG_AY_PN_SN_UN_WN_SZ4,
		STORE_REG_AY_PN_SN_UN_WY_SZ1,
		STORE_REG_AY_PN_SN_UN_WY_SZ2,
		STORE_REG_AY_PN_SN_UN_WY_SZ4,
		STORE_REG_AY_PY_SN_UN_WY_SZ1,
		STORE_REG_AY_PY_SN_UN_WY_SZ2,
		STORE_REG_AY_PY_SN_UN_WY_SZ4,
		STORE_REG_AY_PY_SN_UY_WY_SZ1,
		STORE_REG_AY_PY_SN_UY_WY_SZ2,
		STORE_REG_AY_PY_SN_UY_WY_SZ4,
		STORE_REGD_AN_PN_SN_UN_WN_SZ4,
		STORE_REGD_AN_PN_SN_UN_WY_SZ4,
		STORE_REGD_AN_PY_SN_UN_WY_SZ4,
		STORE_REGD_AY_PN_SN_UN_WN_SZ4,
		STORE_REGD_AY_PN_SN_UN_WY_SZ4,
		STORE_REGD_AY_PY_SN_UN_WY_SZ4,
		// shen 11/12

		Str_imm,
		Str_reg,
		Strb,
		Strb_imm,
		Strb_reg,
		Strbimm_p,
		Strbimm_pu,
		Strbimm_puw,
		Strbimm_pw,
		Strbimm_uw,
		Strbimm_w,
		Strbt,
		Strd_p,
		Strd_pu,
		Strd_puw,
		Strd_pw,
		Strd_uw,
		Strd_w,
		Strex,
		Strexb,

		// shen 11/12
		STREXB_STORE_IMM_AY_PN_SN_UN_WN_SZ1,
		STREXD_STORE_IMMD_AY_PN_SN_UN_WN_SZ4,
		STREXH_STORE_IMM_AY_PN_SN_UN_WN_SZ2,
		STREX_STORE_IMM_AY_PN_SN_UN_WN_SZ4,
		// shen 11/12

		Strexd,
		Strexh,
		Strh,
		Strh_imm,
		Strh_reg,
		Strhimm_p,
		Strhimm_pu,
		Strhimm_puw,
		Strhimm_pw,
		Strhimm_uw,
		Strhimm_w,
		Strht,
		Strimm_p,
		Strimm_pu,
		Strimm_puw,
		Strimm_pw,
		Strimm_uw,
		Strimm_w,
		StrS,
		Strt,

		//FP Data Processing Instructions
		// latency = 3
		VabsD,
		VabsS,
		VcmpD,
		VcmpeD,
		VcmpeS,
		VnegD,
		Swp,
		// pf select
		// shen 11/12
		VselD,
		VselS,

		VnegS,
		//FP Miscellaneous Instructions
		// latency = 3
		VmovD,
		VmovImmD,
		VmovImmS,
		VmovQ,
		VmovRegD,
		VmovRegS,

		// latency = 5
		NVcvt2sfxD,
		NVcvt2sfxQ,
		NVcvt2ufxD,
		NVcvt2ufxQ,
		NVcvth2s,
		NVcvts2h,
		VaddD,
		Vaddl,
		VaddS,
		Vaddw,
		VcvtFpDFpS,
		VcvtFpHBFpS,
		VcvtFpHTFpS,
		VcvtFpSFixedD,
		VcvtFpSFixedS,
		VcvtFpSFpD,
		VcvtFpSFpHB,
		VcvtFpSFpH,
		VcvtFpSHFixedD,
		VcvtFpSHFixedS,
		VcvtFpSIntD,
		VcvtFpSIntDR,
		VcvtFpSIntS,
		VcvtFpSIntSR,
		VcvtFpUFixedD,
		VcvtFpUFixedS,
		VcvtFpUHFixedD,
		VcvtFpUHFixedS,
		VcvtFpUIntD,
		VcvtFpUIntDR,
		VcvtFpUIntS,
		VcvtFpUIntSR,
		VcvtSFixedFpD,
		VcvtSFixedFpS,
		VcvtSHFixedFpD,
		VcvtSHFixedFpS,
		VcvtSIntFpD,
		VcvtSIntFpS,
		VcvtUFixedFpD,
		VcvtUFixedFpS,
		VcvtUHFixedFpD,
		VcvtUHFixedFpS,
		VcvtUIntFpD,
		VcvtUIntFpS,
		VsubD,
		Vsubl,
		VsubS,
		Vsubw,

		// latency = 5
		Vmov2Core2Reg,
		Vmov2Reg2Core,

		// add by shen
		//11/11 fp round to int
		VRIntAD,
		VRIntND,
		VRIntPD,
		VRIntMD,
		VRIntAS,
		VRIntNS,
		VRIntPS,
		VRIntMS,
		VmulsQFp,
		VmulsDFp,

		// latency = 6
		VcmpeZeroD,
		VcmpeZeroS,
		VcmpS,
		VcmpZeroD,
		VcmpZeroS,
		VmulD,
		Vmull,
		Vmullp,
		Vmulls,
		VmulS,
		VmulsD,
		VmulsQ,
		VnmulD,
		VnmulS,

		// latency = 7
		VdivD,
		VdivS,
		VsqrtD,
		VsqrtS,

		// latency = 8
		VmovCoreRegB,
		VmovCoreRegH,
		VmovCoreRegW,
		VmovRegCoreSB,
		VmovRegCoreSH,
		VmovRegCoreUB,
		VmovRegCoreUH,
		VmovRegCoreW,

		// latency = 9
		VmlasDFp,
		VmlasQFp,
		VmlssDFp,
		VmlssQFp,

		// latency = 10
		VfmaD,
		VfmaS,
		VfmsD,
		VfmsS,
		VfnmaD,
		VfnmaS,
		VfnmsD,
		VfnmsS,
		VmlaD,
		Vmlal,
		Vmlals,
		VmlaS,
		VmlasD,
		VmlasQ,
		VmlsD,
		Vmlsl,
		Vmlsls,
		VmlsS,
		VmlssD,
		VmlssQ,
		VnmlaD,
		VnmlaS,
		VnmlsD,
		VnmlsS,

		//FP Load Instructions
		// latency = 5
		Vldr_d,
		Vldr_s,
		Vldr_ud,
		Vldr_us,

		// FP Store Instructions
		// latency = 1
		Vstr_d,
		Vstr_s,
		Vstr_ud,
		Vstr_us,

		//ASIMD Interger Instructions
		// latency = 3
		NVabs,
		NVbiciD,
		NVbiciQ,
		NVceq,
		NVcge,
		NVcgt,
		NVmvnD,
		NVmvniD,
		NVmvniQ,
		NVmvnQ,
		NVneg,
		NVorriD,
		NVorriQ,
		NVpaddD,
		NVpaddl,
		NVqabs,
		NVqneg,
		NVshl,
		NVshll,
		NVshr,
		NVshrn,
		Vabd,
		Vabdl,
		Vmax,
		Vmin,
		Vpmax,
		Vpmin,
		Vcge,
		Vcgt,
		Vceq,
		Vaddhn,
		Vhadd,
		Vhsub,
		Vqadd,
		VqsubS,
		VqsubU,
		Vraddhn,
		Vrhadd,
		Vrsubhn,
		Vsubhn,
		Vtst,
		VandD,
		VandQ,
		VbicD,
		VbicQ,
		VeorD,
		VeorQ,
		VornD,
		VornQ,
		VorrD,
		VorrQ,
		Vshl,

		//ASIMD Floating-Point Instructions
		// latency = 3
		NVnegDFp,
		NVnegQFp,
		NVabsDFp,
		NVabsQFp,

		//ASIMD Miscellaneous Instructions
		// latency = 3
		NVcls,
		NVclz,
		NVcnt,
		NVextD,
		NVextQ,
		NVmoviD,
		NVmoviQ,
		NVmovl,
		NVmovn,
		NVrev16D,
		NVrev16Q,
		NVrev32D,
		NVrev32Q,
		NVrev64D,
		NVrev64Q,
		NVswpD,
		NVswpQ,
		NVtbl1,
		NVtbl2,
		NVtbl3,
		NVtbl4,
		NVtbx1,
		NVtbx2,
		NVtbx3,
		NVtbx4,
		NVtrn,
		NVuzp,
		NVzip,
		Tbb,
		Tbh,
		VbifD,
		VbifQ,
		VbitD,
		VbitQ,
		VbslD,
		VbslQ,

		// latency = 4
		NVqmovn,
		NVqmovun,
		NVqmovuns,

		// latency = 4
		NVpadal,
		NVqshl,
		NVqshlu,
		NVqshlus,
		NVqshrn,
		NVqshrun,
		NVqshruns,
		NVrshr,
		NVrsra,
		NVsli,
		NVsra,
		NVsri,
		Vaba,
		Vabal,
		Vrshl,

		// latency = 5
		NVrecpeD,
		NVrecpeQ,
		NVrsqrteD,
		NVrsqrteQ,

		// latency = 5
		NVadd,
		NVmul,
		NVmulp,
		NVsub,
		NVmla,
		NVmls,
		Vqdmulh,
		VqdmulhsD,
		Vqrdmulh,
		VqrdmulhsD,
		Vqdmlal,
		Vqdmlals,
		Vqdmlsl,
		Vqdmlsls,
		Vqdmull,
		Vqdmulls,
		VqrshlS,
		VqrshlU,
		VqshlS,
		VqshlU,

		// latency = 5
		NVcle,
		NVclt,
		NVcgeQFp,
		NVmulDFp,
		NVmulQFp,
		NVrecpeDFp,
		NVrecpeQFp,
		NVceqDFp,
		NVceqQFp,
		NVcgeDFp,
		NVcgtDFp,
		NVcgtQFp,
		NVcleDFp,
		NVcleQFp,
		NVcltDFp,
		NVcltQFp,
		NVcvts2fpD,
		NVcvts2fpQ,
		NVcvtu2fpD,
		NVcvtu2fpQ,
		VpaddDFp,
		VpaddQFp,
		VsubDFp,
		VsubQFp,
		VabdDFp,
		VabdQFp,
		VaddDFp,
		VaddQFp,
		VacgeDFp,
		VacgeQFp,
		VacgtDFp,
		VacgtQFp,
		VceqDFp,
		VceqQFp,
		VcgeDFp,
		VcgeQFp,
		VcgtDFp,
		VcgtQFp,
		VmaxDFp,
		VmaxQFp,
		VminDFp,
		VminQFp,
		VpmaxDFp,
		VpmaxQFp,
		VpminDFp,
		VpminQFp,

		// latency = 6
		VqdmulhsQ,
		VqrdmulhsQ,

		// latency = 8
		NVdup,
		NVdupDGpr,
		NVdupQGpr,

		// latency = 9
		NVfmaDF,
		NVfmaQF,
		NVfmsDF,
		NVfmsQF,
		NVmlsDFp,
		NVmlsQFp,
		NVfmaQFp,
		NVfmsDFp,
		NVfmsQFp,
		NVmlaDFp,
		NVmlaQFp,



		// latency = 9
		VrecpsDFp,
		VrsqrtsDFp,
		NVrsqrteDFp,
		NVrsqrteQFp,

		// latency = 10
		VrecpsQFp,
		VrsqrtsQFp,

		//ASIMD Load Instructions
		// latency = 8
		VLdmStmLd,
		VldSingle,
		VldMult,
		//ASIMD Store Instructions
		// latency = 3
		VLdmStmSt,
		VstSingle,
		VstMult,
		//Cryptography Extensions
		// none
		//CRC
		// none

		//Special Register Access
		// latency = default
		Cps,
		Dmb,
		Dsb,
		Isb,
		Mcr14,
		Mcr15,
		Mcrr15,
		Mrc14,
		Mrc15,
		MrsBankedReg,
		MrsCpsr,
		MrsSpsr,
		MsrBankedReg,
		MsrCpsrReg,
		MsrSpsrReg,
		Setend,
		Vmrs,
		VmrsApsrFpscr,
		VmrsFpscr,
		Vmsr,
		VmsrFpscr,
		// shen 11/12
		MsrCpsrImm,
		MsrSpsrImm,

		// can not locate
		// latency = default
		// decode in Thumb16Misc()
		// Breakpoint
		BkptInst,
		// If-Then
		ItInst,
		// decode in Thumb16CondBranchAndSvc()
		// Supervisor Call
		Svc,
		// decode in decodeMcrMrc15()
		FlushPipeInst,
		// latency = default
		// decode in Thumb32BranchesAndMiscCtrl()
		// Branch and Exchange
		BxjReg,
		// Exception Return
		Eret,
		// Hypervisor Call
		Hvc,
		// Secure Monitor Call
		Smc,
		// Clear-Exclusive
		Clrex,
		Enterx,
		Leavex,
		// Debug Hint
		Dbg,
		NopInst,
		SevInst,
		WfeInst,
		WfiInst,
		YieldInst,
		// decode in Thumb32SrsRfe()
		// Return From Exception,  load
		// Store Return State, store
		Rfe,
		Rfe_u,
		Rfe_uw,
		Rfe_w,

		// shen 11/12
		RFE_LOAD_IMM_AN_PN_SN_UN_WN_SZ8,
		RFE_LOAD_IMM_AN_PN_SN_UN_WY_SZ8,
		RFE_LOAD_IMM_AN_PY_SN_UN_WN_SZ8,
		RFE_LOAD_IMM_AN_PY_SN_UN_WY_SZ8,
		RFE_LOAD_IMM_AY_PN_SN_UN_WN_SZ8,
		RFE_LOAD_IMM_AY_PN_SN_UN_WY_SZ8,
		RFE_LOAD_IMM_AY_PY_SN_UN_WN_SZ8,
		RFE_LOAD_IMM_AY_PY_SN_UN_WY_SZ8,
		// shen 11/12
		Srs,
		Srs_u,
		Srs_uw,
		Srs_w,

		// shen 11/12
		SRS_STORE_IMM_AN_PN_SN_UN_WN_SZ8,
		SRS_STORE_IMM_AN_PN_SN_UN_WY_SZ8,
		SRS_STORE_IMM_AN_PY_SN_UN_WN_SZ8,
		SRS_STORE_IMM_AN_PY_SN_UN_WY_SZ8,
		SRS_STORE_IMM_AY_PN_SN_UN_WN_SZ8,
		SRS_STORE_IMM_AY_PN_SN_UN_WY_SZ8,
		SRS_STORE_IMM_AY_PY_SN_UN_WN_SZ8,
		SRS_STORE_IMM_AY_PY_SN_UN_WY_SZ8,
		// shen 11/12

		// decode in M5ops()
		Arm,
		Deprecated_exit,
		Deprecated_ivlb,
		Deprecated_ivle,
		Dumpresetstats,
		Dumpstats,
		Initparam,
		Loadsymbol,
		M5addsymbol,
		M5break,
		M5checkpoint,
		M5exit,
		M5fail,
		M5panic,
		M5readfile,
		M5switchcpu,
		M5workbegin,
		M5workend,
		M5writefile,
		Resetstats,
		Rpns,
		Quiesce,
		QuiesceCycles,
		QuiesceNs,
		QuiesceTime,
		WakeCPU,

		FailUnimplemented,
		WarnUnimplemented
	};
}

namespace aarch64 {
	enum Aarch64Set {
		Unknown64 = 0,

		/* ALU includes imm and reg */
		// Arithmetic Instructions
		//latency = 1
		AddXImm,
		AddXImmCc,
		AdcImm,
		SubXImm,
		SubXImmCc,
		CcmnReg64,
		CcmpReg64,
		CcmnImm64,
		CcmpImm64,
		Csel64,//Conditional Select returns
		Csinc64,
		Csinv64,
		Csneg64,
		//Move and Shift Instructions
		AdrXImm,
		AdrpXImm,
		Movn,
		Movz,
		Movk,
		Asrv64,//Arithmetic Shift Right Variable
		Lslv64,//Logical Shift Left Variable
		Lsrv64,
		Rorv64,
		//Miscellaneous Data-Processing Instructions
		Extr64,
		Sbfm64,//Signed Bitfield Move
		Ubfm64,
		Rbit64,
		Rev1664,
		Rev64,
		Rev3264,
		Clz64,
		Cls64,
		AndXImm,//Logical Instructions
		AndXImmCc,
		OrrXImm,
		EorXImm,

		// latency = 2
		// Arithmetic and Logical Instructions
		OrrXSReg,
		OrnXSReg,
		EorXSReg,
		EonXSReg,
		AndXSReg,
		AndXSRegCc,
		BicXSReg,//Bitwise Bit Clear
		BicXSRegCc,
		AddXSReg,// add 64bit shifted reg
		AddXSRegCc,
		SubXSReg,
		SubXSRegCc,
		AddXEReg,
		AddXERegCc,
		SubXEReg,//Subtract with Carry
		SubXERegCc,
		AdcXSReg,
		AdcXSRegCc,
		SbcXSReg,
		SbcXSRegCc,
		//Miscellaneous Data-Processing Instructions
		Bfm64,

		//latency = 3
		//Divide and Multiply Instructions
		Madd64,//Multiply-Add
		Msub64,
		Smaddl64,
		Smsubl64,
		Umaddl64,
		Umsubl64,

		//latency = 4
		//Divide and Multiply Instructions
		Udiv64,
		Sdiv64,

		//latency = 6
		//Divide and Multiply Instructions
		Smulh64, // signed mul
		Umulh64,
		/* ALU end */

		/* BranchExcSysInst */
		// latency = 1
		B64,
		BCond64,
		Bl64,
		Br64,
		Ret64,
		Cbz64,
		Cbnz64,
		Tbz64,
		Tbnz64,
		// latency = 2
		Blr64,
		/* branch end */


		/* expection generation and system */
		Svc64,
		Smc64,
		NopInst,
		YieldInst,
		WfeInst,
		WfiInst,
		SevInst,
		SevlInst,
		Clrex64,
		Dsb64,
		Dmb64,
		Isb64,
		MsrSP64,
		MsrDAIFSet64,
		MsrDAIFClr64,
		MrsNZCV64,
		MsrNZCV64,
		Dczva,
		Msr64,
		Mrs64,
		Eret64,
		/* end */

		/* LoadsStoresInst */
		//MemRead MemWrite opLat=2
		// latency = 4
		LDXRB64,
		LDXRH64,
		LDXRW64,
		LDXRX64,
		LDAXRB64,
		LDAXRH64,
		LDAXRW64,
		LDAXRX64,
		LDXPW64,
		LDXPX64,
		LDAXPW64,
		LDAXPX64,
		LDARB64,
		LDARH64,
		LDARW64,
		LDARX64,
		LDRWL64_LIT,
		LDRSFP64_LIT,
		LDRXL64_LIT,
		LDRDFP64_LIT,
		LDRSWL64_LIT,
		//LdpStp,
		Ldnp,
		Ldpsw,
		Ldp,
		LDRB64_IMM,
		LDRSBX64_IMM,
		LDRSBW64_IMM,
		LDRBFP64_IMM,
		LDRH64_IMM,
		LDRSHX64_IMM,
		LDRSHW64_IMM,
		LDRHFP64_IMM,
		LDRW64_IMM,
		LDRSW64_IMM,
		LDRSFP64_IMM,
		LDRX64_IMM,
		LDRDFP64_IMM,
		LDRB64_REG,
		LDRSBX64_REG,
		LDRSBW64_REG,
		LDRBFP64_REG,
		LDRH64_REG,
		LDRSHX64_REG,
		LDRSHW64_REG,
		LDRHFP64_REG,
		LDRW64_REG,
		LDRSW64_REG,
		LDRSFP64_REG,
		LDRX64_REG,
		LDRDFP64_REG,
		LDURB64_IMM,
		LDURSBX64_IMM,
		LDURSBW64_IMM,
		LDURBFP64_IMM,
		LDURH64_IMM,
		LDURSHX64_IMM,
		LDURSHW64_IMM,
		LDURHFP64_IMM,
		LDURW64_IMM,
		LDURSW64_IMM,
		LDURSFP64_IMM,
		LDURX64_IMM,
		LDURDFP64_IMM,
		LDRB64_POST,
		LDRSBX64_POST,
		LDRSBW64_POST,
		LDRBFP64_POST,
		LDRH64_POST,
		LDRSHX64_POST,
		LDRSHW64_POST,
		LDRHFP64_POST,
		LDRW64_POST,
		LDRSW64_POST,
		LDRSFP64_POST,
		LDRX64_POST,
		LDRDFP64_POST,
		LDTRB64_IMM,
		LDTRSBX64_IMM,
		LDTRSBW64_IMM,
		LDTRH64_IMM,
		LDTRSHX64_IMM,
		LDTRSHW64_IMM,
		LDTRW64_IMM,
		LDTRSW64_IMM,
		LDTRX64_IMM,
		LDRB64_PRE,
		LDRSBX64_PRE,
		LDRSBW64_PRE,
		LDRBFP64_PRE,
		LDRH64_PRE,
		LDRSHX64_PRE,
		LDRSHW64_PRE,
		LDRHFP64_PRE,
		LDRW64_PRE,
		LDRSW64_PRE,
		LDRSFP64_PRE,
		LDRX64_PRE,
		LDRDFP64_PRE,
		PRFM64_LIT,//load
		PRFM64_IMM,
		PRFM64_REG,
		PRFUM64_IMM,
		BigFpMemLitLd,//BigFpMemLit
		BigFpMemImmLd,/*BigFpMemImm fp ld*/
		BigFpMemRegLd,//BigFpMemReg
		BigFpMemPostLd,//BigFpMemPost
		BigFpMemPreLd,//BigFpMemPre
		/* load end */
		// latency = 1
		STXRB64,
		STXRH64,
		STXRW64,
		STXRX64,
		STLXRB64,
		STLXRH64,
		STLXRW64,
		STLXRX64,
		STXPW64,
		STXPX64,
		STLXPW64,
		STLXPX64,
		STLRB64,
		STLRH64,
		STLRW64,
		STLRX64,
		Stnp,
		Stp,
		STRB64_IMM,
		STRBFP64_IMM,
		STRH64_IMM,
		STRHFP64_IMM,
		STRW64_IMM,
		STRSFP64_IMM,
		STRX64_IMM,
		STRDFP64_IMM,
		STRB64_REG,
		STRBFP64_REG,
		STRH64_REG,
		STRHFP64_REG,
		STRW64_REG,
		STRSFP64_REG,
		STRX64_REG,
		STRDFP64_REG,
		STURB64_IMM,
		STURBFP64_IMM,
		STURH64_IMM,
		STURHFP64_IMM,
		STURW64_IMM,
		STURSFP64_IMM,
		STURX64_IMM,
		STURDFP64_IMM,
		STRB64_POST,
		STRBFP64_POST,
		STRH64_POST,
		STRHFP64_POST,
		STRW64_POST,
		STRSFP64_POST,
		STRX64_POST,
		STRDFP64_POST,
		STTRB64_IMM,
		STTRH64_IMM,
		STTRW64_IMM,
		STTRX64_IMM,
		STRB64_PRE,
		STRBFP64_PRE,
		STRH64_PRE,
		STRHFP64_PRE,
		STRW64_PRE,
		STRSFP64_PRE,
		STRX64_PRE,
		STRDFP64_PRE,
		BigFpMemImmSt,// BigFpMemImm,
		BigFpMemRegSt,//BigFpMemReg
		BigFpMemPostSt,//BigFpMemPost
		BigFpMemPreSt,//BigFpMemPre
		/* store end */

		/* Fp */
		//FP latency = 3
		FAbsS,
		FAbsD,
		FNegS,
		FNegD,
		FCSelS,//FP Conditional Select
		FCSelD,
		FCmpRegS,
		FCmpImmS,
		FCmpERegS,
		FCmpEImmS,
		FCmpRegD,
		FCmpImmD,
		FCmpERegD,
		FCmpEImmD,
		FCCmpRegS,
		FCCmpERegS,
		FCCmpRegD,
		FCCmpERegD,
		FmovImmS,
		FmovImmD,
		FmovRegS,
		FmovRegD,
		//FP latency = 5
		FAddS,
		FAddD,
		FSubS,
		FSubD,
		FMulS,
		FMulD,
		FNMulS,
		FNMulD,
		FMaxS,
		FMaxD,
		FMinS,
		FMinD,
		FMaxNMS,
		FMaxNMD,
		FMinNMS,
		FMinNMD,
		FmovRegCoreW,
		FmovRegCoreX,
		FmovURegCoreX,
		FmovCoreRegW,
		FmovCoreRegX,
		FmovUCoreRegX,
		//FCVT
		FcvtFpDFpS,
		FcvtFpHFpS,
		FCvtFpSFpD,
		FcvtFpHFpD,
		FcvtFpSFpH,
		FcvtFpDFpH,
		FRIntNS,//float round to integral
		FRIntND,
		FRIntPS,
		FRIntPD,
		FRIntMS,
		FRIntMD,
		FRIntZS,
		FRIntZD,
		FRIntAS,
		FRIntAD,
		FRIntXS,
		FRIntXD,
		FRIntIS,
		FRIntID,
		//FP latency = 7
		FDivS,
		FDivD,
		FSqrtS,
		FSqrtD,
		//FP latency = 9
		FMAddS,
		FMSubS,
		FNMAddS,
		FNMSubS,
		FMAddD,
		FMSubD,
		FNMAddD,
		FNMSubD,
		//FP latency = 10
		//SCVTF
		FcvtWSIntFpS,
		FcvtXSIntFpS,
		FcvtWSIntFpD,
		FcvtXSIntFpD,
		FcvtWUIntFpS,
		FcvtXUIntFpS,
		FcvtWUIntFpD,
		FcvtXUIntFpD,
		FcvtSFixedFpSW,
		FcvtSFixedFpDW,
		FcvtSFixedFpSX,
		FcvtSFixedFpDX,
		//UCVTF
		FcvtFpSIntWSA,
		FcvtFpSIntXSA,
		FcvtFpSIntWDA,
		FcvtFpSIntXDA,
		FcvtFpUIntWSA,
		FcvtFpUIntXSA,
		FcvtFpUIntWDA,
		FcvtFpUIntXDA,
		FcvtUFixedFpSW,
		FcvtUFixedFpDW,
		FcvtUFixedFpSX,
		FcvtUFixedFpDX,
		//FCVTZS
		FcvtFpSFixedSW,
		FcvtFpSFixedDW,
		FcvtFpSFixedSX,
		FcvtFpSFixedDX,
		//FCVTZU
		FcvtFpUFixedSW,
		FcvtFpUFixedDW,
		FcvtFpUFixedSX,
		FcvtFpUFixedDX,
		//FCVTNS
		FcvtFpSIntWSN,
		FcvtFpSIntXSN,
		FcvtFpSIntWDN,
		FcvtFpSIntXDN,
		//FCVTPS
		FcvtFpSIntXSP,
		FcvtFpSIntWSP,
		FcvtFpSIntWDP,
		FcvtFpSIntXDP,
		//FCVTMS
		FcvtFpSIntWSM,
		FcvtFpSIntXSM,
		FcvtFpSIntWDM,
		FcvtFpSIntXDM,
		///FCVTZS
		FcvtFpSIntWSZ,
		FcvtFpSIntXSZ,
		FcvtFpSIntWDZ,
		FcvtFpSIntXDZ,
		//FCVTNU
		FcvtFpUIntWSN,
		FcvtFpUIntXSN,
		FcvtFpUIntWDN,
		FcvtFpUIntXDN,
		//FCVTPU
		FcvtFpUIntWSP,
		FcvtFpUIntXSP,
		FcvtFpUIntWDP,
		FcvtFpUIntXDP,
		//FCVTMU
		FcvtFpUIntWSM,
		FcvtFpUIntXSM,
		FcvtFpUIntWDM,
		FcvtFpUIntXDM,
		//FCVTZU
		FcvtFpUIntWSZ,
		FcvtFpUIntXSZ,
		FcvtFpUIntWDZ,
		FcvtFpUIntXDZ,
		/* FP end*/


		/* Neon */
		// latency = 3
		//ASIMD Integer Instructions
		SABD,
		UABD,
		SABDL,
		UABDL,
		ADD,
		AddDX,
		ADDP,
		AddpScQX,
		SADDL,
		SADDLP,
		SADDW,
		SHADD,
		UHADD,
		UADDL,
		UADDW,
		UADDLP,
		ADDHN,
		RADDHN,
		SQADD,
		SQADDS,
		SRHADD,
		SUQADD,
		SUQADDS,
		UQADD,
		UQADDS,
		URHADD,
		USQADD,
		USQADDS,
		SUB,
		SubDX,
		SHSUB,
		SSUBL,
		SSUBW,
		UHSUB,
		USUBW,
		RSUBHN,
		SQSUB,
		SQSUBS,
		SUBHN,
		UQSUB,
		UQSUBS,
		ABS,
		AbsDX,
		NEG,
		NegDX,
		SQABS,
		SQABSS,
		SQNEG,
		SQNEGS,
		SMAX,
		SMIN,
		SMAXP,
		SMINP,
		UMAX,
		UMIN,
		UMAXP,
		UMINP,
		CMHI,
		CmhiDX,
		CMGT,
		CmgtDX,
		CmgtZeroDX,
		CMHS,
		CmhsDX,
		CMGE,
		CMGES,
		CmgeDX,
		CmgeZeroDX,
		CMEQ,
		CmeqDX,
		CmeqZeroDX,
		CMTST,
		CmtstDX,
		CMLT,
		CmltZeroDX,
		CMLE,
		CmleZeroDX,
		MoviDX,
		MoviQX,
		MvnDX,
		MvnQX,
		MvniDX,
		MvniQX,
		AndDX,
		AndQX,
		BicDX,
		BicQX,
		BicImmDX,
		BicImmQX,
		EorDX,
		EorQX,
		OrnDX,
		OrnQX,
		OrrDX,
		OrrQX,
		OrrImmDX,
		OrrImmQX,
		SSHR,
		SshrDX,
		SRI,
		SriDX,
		SLI,
		SliDX,
		SHL,
		SHLS,
		ShlDX,
		SHRN,
		SSHLL,
		SHLL,
		USHR,
		UshrDX,
		USHLL,
		SSHL,
		SshlDX,
		USHL,
		UshlDX,
		// ASIMD Floating-Point Instructions
		FABS,
		FNEG,
		// ASIMD Miscellaneous Instructions
		RbitDX,
		RbitQX,
		BslDX,
		BslQX,
		BifDX,
		BifQX,
		BitDX,
		BitQX,
		CLS,
		CLZ,
		CntDX,
		CntQX,
		DupElem,
		DupElemDX,
		DupElemQX,
		ExtDX,
		ExtQX,
		XTN,
		InsElemX,
		FmovDX,
		FmovQX,
		Rev16DX,
		Rev16QX,
		Rev32DX,
		Rev32QX,
		REV64,
		Tbl1DX,
		Tbx1DX,
		TRN1,
		TRN2,
		UABAL,
		UZP1,
		UZP2,
		ZIP1,
		ZIP2,
		// Cryptography Extensions
		PmulDX,
		PmulQX,
		Pmull2X,
		PmullX,

		// latency = 4
		//ASIMD Integer Instructions
		SABA,
		SABAL,
		UABA,
		USUBL,
		ADDV,
		SADDLV,
		UADDLV,
		SMAXV,
		SMINV,
		UMAXV,
		UMINV,
		RSHRN,
		SRSHR,
		SrshrDX,
		SQSHLU,
		SADALP,
		UADALP,
		SSRA,
		SsraDX,
		USRA,
		UsraDX,
		URSRA,
		UrsraDX,
		SRSRA,
		SrsraDX,
		SQSHRN,
		SQSHRNS,
		SQSHRUN,
		SQSHRUNS,
		SQRSHRN,
		SQRSHRNS,
		SQRSHRUN,
		SQRSHRUNS,
		URSHR,
		UrshrDX,
		UQSHRN,
		UQSHRNS,
		UQRSHRN,
		UQRSHRNS,
		SRSHL,
		SrshlDX,
		SQSHL,
		SQSHLS,
		SQRSHL,
		SQRSHLS,
		URSHL,
		UrshlDX,
		UQSHL,
		UQSHLS,
		UQRSHL,
		UQRSHLS,
		// ASIMD Miscellaneous Instructions
		SQXTN,
		SqxtnScX,
		SQXTUN,
		SqxtunScX,
		UQXTN,
		UqxtnScX,

		// latency = 5
		//ASIMD Integer Instructions
		MUL,
		SQDMULH,
		SQDMULHS,
		SQRDMULH,
		SQRDMULHS,
		SQDMULL,
		SQDMULLS,
		SQDMLAL,
		SQDMLALS,
		SQDMLSL,
		SQDMLSLS,
		MLA,
		MLS,
		SMLAL,
		SMLSL,
		UMLAL,
		UMLSL,
		SMULL,
		UMULL,
		// ASIMD Floating-Point Instructions
		FADDP,
		FADDPS,
		FADD,
		FABD,
		FABDS,
		FSUB,
		FCMGE,
		FCMGES,
		FCMEQ,
		FCMEQS,
		FCMGT,
		FCMGTS,
		FACGE,
		FACGES,
		FACGT,
		FACGTS,
		FCMLE,
		FCMLES,
		FCMLT,
		FCMLTS,
		FCVTNS,
		FCVTPS,
		FCVTMS,
		FCVTZS,
		FCVTAS,
		FcvtxnScX,
		FCVTNSS,
		FCVTPSS,
		FCVTMSS,
		FCVTZSS,
		FCVTASS,
		FCVTNU,
		FCVTPU,
		FCVTMU,
		FCVTZU,
		FCVTAU,
		FCVTNUS,
		FCVTPUS,
		FCVTMUS,
		FCVTZUS,
		FCVTAUS,
		ScvtfFixedDQX,
		ScvtfFixedSQX,
		ScvtfFixedDX,
		ScvtfIntDQX,
		ScvtfIntDX,
		ScvtfIntSQX,
		ScvtfFixedScDX,
		ScvtfFixedScSX,
		ScvtfIntScDX,
		ScvtfIntScSX,
		UCVTF,
		UCVTFS,
		FMAXNMP,
		FMAXNMPS,
		FMAXNM,
		FMINNMP,
		FMINNMPS,
		FMINNM,
		FMAXP,
		FMAXPS,
		FMAX,
		FMINP,
		FMINPS,
		FMIN,
		FMUL,
		FMULS,
		FMULX,
		FMULXS,
		FRINTN,
		FRINTP,
		FRINTM,
		FRINTZ,
		FRINTA,
		FRINTX,
		FRINTI,
		// ASIMD Miscellaneous Instructions
		FRECPE,
		FRECPES,
		FRECPXS,
		FRSQRTE,
		FRSQRTES,
		UrecpeDX,
		UrecpeQX,
		UrsqrteDX,
		UrsqrteQX,
		UmovWX,
		// ASIMD Load Store Instructions
		VldMult64,
		VldSingle64,
		VstMult64,
		VstSingle64,

		// latency = 6
		// ASIMD Miscellaneous Instructions
		Tbl1QX,
		Tbx1QX,
		Tbl2DX,
		Tbx2DX,
		SmovWX,
		SmovXX,
		UmovXX,

		// latency = 7
		// ASIMD Floating-Point Instructions
		FDIV,
		FSQRT,

		// latency = 8
		// ASIMD Floating-Point Instructions
		Fcvtl2X,
		FcvtlX,
		Fcvtn2X,
		FcvtnX,
		FcvtxnX,
		Fcvtxn2X,
		// ASIMD Miscellaneous Instructions
		DupGprWDX,
		DupGprWQX,
		DupGprXQX,
		InsGprWX,
		InsGprXX,

		// latency = 9
		// ASIMD Floating-Point Instructions
		FMLA,
		FMLAelm,
		FMLS,
		FMLAS,
		FMLSS,
		// ASIMD Miscellaneous Instructions
		FRECPS,
		FRECPSS,
		FRSQRTS,
		FRSQRTSS,
		Tbl2QX,
		Tbx2QX,
		Tbl3DX,
		Tbx3DX,

		// latency = 10
		// ASIMD Floating-Point Instructions
		FmaxnmvQX,
		FmaxvQX,
		FminnmvQX,
		FminvQX,

		// latency = 12
		// ASIMD Miscellaneous Instructions
		Tbl3QX,
		Tbx3QX,
		Tbl4DX,
		Tbx4DX,

		// latency = 15
		// ASIMD Miscellaneous Instructions
		Tbl4QX,
		Tbx4QX,
		/* Neon end */

		WarnUnimplemented,
		FailUnimplemented,
	};
}

enum IntRegIndex { /* All the unique register indices. */
	INTREG_R0,
	INTREG_R1,
	INTREG_R2,
	INTREG_R3,
	INTREG_R4,
	INTREG_R5,
	INTREG_R6,
	INTREG_R7,
	INTREG_R8,
	INTREG_R9,
	INTREG_R10,
	INTREG_R11,
	INTREG_R12,
	INTREG_R13,
	INTREG_SP = INTREG_R13,
	INTREG_R14,
	INTREG_LR = INTREG_R14,
	INTREG_R15,
	INTREG_PC = INTREG_R15,

	INTREG_R13_SVC,
	INTREG_SP_SVC = INTREG_R13_SVC,
	INTREG_R14_SVC,
	INTREG_LR_SVC = INTREG_R14_SVC,

	INTREG_R13_MON,
	INTREG_SP_MON = INTREG_R13_MON,
	INTREG_R14_MON,
	INTREG_LR_MON = INTREG_R14_MON,

	INTREG_R13_HYP,
	INTREG_SP_HYP = INTREG_R13_HYP,

	INTREG_R13_ABT,
	INTREG_SP_ABT = INTREG_R13_ABT,
	INTREG_R14_ABT,
	INTREG_LR_ABT = INTREG_R14_ABT,

	INTREG_R13_UND,
	INTREG_SP_UND = INTREG_R13_UND,
	INTREG_R14_UND,
	INTREG_LR_UND = INTREG_R14_UND,

	INTREG_R13_IRQ,
	INTREG_SP_IRQ = INTREG_R13_IRQ,
	INTREG_R14_IRQ,
	INTREG_LR_IRQ = INTREG_R14_IRQ,

	INTREG_R8_FIQ,
	INTREG_R9_FIQ,
	INTREG_R10_FIQ,
	INTREG_R11_FIQ,
	INTREG_R12_FIQ,
	INTREG_R13_FIQ,
	INTREG_SP_FIQ = INTREG_R13_FIQ,
	INTREG_R14_FIQ,
	INTREG_LR_FIQ = INTREG_R14_FIQ,

	INTREG_ZERO,
	INTREG_UREG0,
	INTREG_UREG1,
	INTREG_UREG2,
	INTREG_DUMMY, // Dummy reg used to throw away int reg results

	INTREG_SP0,
	INTREG_SP1,
	INTREG_SP2,
	INTREG_SP3,

	NUM_INTREGS,
	NUM_ARCH_INTREGS = 32,

	/* AArch64 registers */
	INTREG_X0 = 0,
	INTREG_X1,
	INTREG_X2,
	INTREG_X3,
	INTREG_X4,
	INTREG_X5,
	INTREG_X6,
	INTREG_X7,
	INTREG_X8,
	INTREG_X9,
	INTREG_X10,
	INTREG_X11,
	INTREG_X12,
	INTREG_X13,
	INTREG_X14,
	INTREG_X15,
	INTREG_X16,
	INTREG_X17,
	INTREG_X18,
	INTREG_X19,
	INTREG_X20,
	INTREG_X21,
	INTREG_X22,
	INTREG_X23,
	INTREG_X24,
	INTREG_X25,
	INTREG_X26,
	INTREG_X27,
	INTREG_X28,
	INTREG_X29,
	INTREG_X30,
	INTREG_X31,

	INTREG_SPX = NUM_INTREGS,

	/* All the aliased indexes. */

	/* USR mode */
	INTREG_R0_USR = INTREG_R0,
	INTREG_R1_USR = INTREG_R1,
	INTREG_R2_USR = INTREG_R2,
	INTREG_R3_USR = INTREG_R3,
	INTREG_R4_USR = INTREG_R4,
	INTREG_R5_USR = INTREG_R5,
	INTREG_R6_USR = INTREG_R6,
	INTREG_R7_USR = INTREG_R7,
	INTREG_R8_USR = INTREG_R8,
	INTREG_R9_USR = INTREG_R9,
	INTREG_R10_USR = INTREG_R10,
	INTREG_R11_USR = INTREG_R11,
	INTREG_R12_USR = INTREG_R12,
	INTREG_R13_USR = INTREG_R13,
	INTREG_SP_USR = INTREG_SP,
	INTREG_R14_USR = INTREG_R14,
	INTREG_LR_USR = INTREG_LR,
	INTREG_R15_USR = INTREG_R15,
	INTREG_PC_USR = INTREG_PC,

	/* SVC mode */
	INTREG_R0_SVC = INTREG_R0,
	INTREG_R1_SVC = INTREG_R1,
	INTREG_R2_SVC = INTREG_R2,
	INTREG_R3_SVC = INTREG_R3,
	INTREG_R4_SVC = INTREG_R4,
	INTREG_R5_SVC = INTREG_R5,
	INTREG_R6_SVC = INTREG_R6,
	INTREG_R7_SVC = INTREG_R7,
	INTREG_R8_SVC = INTREG_R8,
	INTREG_R9_SVC = INTREG_R9,
	INTREG_R10_SVC = INTREG_R10,
	INTREG_R11_SVC = INTREG_R11,
	INTREG_R12_SVC = INTREG_R12,
	INTREG_PC_SVC = INTREG_PC,
	INTREG_R15_SVC = INTREG_R15,

	/* MON mode */
	INTREG_R0_MON = INTREG_R0,
	INTREG_R1_MON = INTREG_R1,
	INTREG_R2_MON = INTREG_R2,
	INTREG_R3_MON = INTREG_R3,
	INTREG_R4_MON = INTREG_R4,
	INTREG_R5_MON = INTREG_R5,
	INTREG_R6_MON = INTREG_R6,
	INTREG_R7_MON = INTREG_R7,
	INTREG_R8_MON = INTREG_R8,
	INTREG_R9_MON = INTREG_R9,
	INTREG_R10_MON = INTREG_R10,
	INTREG_R11_MON = INTREG_R11,
	INTREG_R12_MON = INTREG_R12,
	INTREG_PC_MON = INTREG_PC,
	INTREG_R15_MON = INTREG_R15,

	/* ABT mode */
	INTREG_R0_ABT = INTREG_R0,
	INTREG_R1_ABT = INTREG_R1,
	INTREG_R2_ABT = INTREG_R2,
	INTREG_R3_ABT = INTREG_R3,
	INTREG_R4_ABT = INTREG_R4,
	INTREG_R5_ABT = INTREG_R5,
	INTREG_R6_ABT = INTREG_R6,
	INTREG_R7_ABT = INTREG_R7,
	INTREG_R8_ABT = INTREG_R8,
	INTREG_R9_ABT = INTREG_R9,
	INTREG_R10_ABT = INTREG_R10,
	INTREG_R11_ABT = INTREG_R11,
	INTREG_R12_ABT = INTREG_R12,
	INTREG_PC_ABT = INTREG_PC,
	INTREG_R15_ABT = INTREG_R15,

	/* HYP mode */
	INTREG_R0_HYP = INTREG_R0,
	INTREG_R1_HYP = INTREG_R1,
	INTREG_R2_HYP = INTREG_R2,
	INTREG_R3_HYP = INTREG_R3,
	INTREG_R4_HYP = INTREG_R4,
	INTREG_R5_HYP = INTREG_R5,
	INTREG_R6_HYP = INTREG_R6,
	INTREG_R7_HYP = INTREG_R7,
	INTREG_R8_HYP = INTREG_R8,
	INTREG_R9_HYP = INTREG_R9,
	INTREG_R10_HYP = INTREG_R10,
	INTREG_R11_HYP = INTREG_R11,
	INTREG_R12_HYP = INTREG_R12,
	INTREG_LR_HYP = INTREG_LR,
	INTREG_R14_HYP = INTREG_R14,
	INTREG_PC_HYP = INTREG_PC,
	INTREG_R15_HYP = INTREG_R15,

	/* UND mode */
	INTREG_R0_UND = INTREG_R0,
	INTREG_R1_UND = INTREG_R1,
	INTREG_R2_UND = INTREG_R2,
	INTREG_R3_UND = INTREG_R3,
	INTREG_R4_UND = INTREG_R4,
	INTREG_R5_UND = INTREG_R5,
	INTREG_R6_UND = INTREG_R6,
	INTREG_R7_UND = INTREG_R7,
	INTREG_R8_UND = INTREG_R8,
	INTREG_R9_UND = INTREG_R9,
	INTREG_R10_UND = INTREG_R10,
	INTREG_R11_UND = INTREG_R11,
	INTREG_R12_UND = INTREG_R12,
	INTREG_PC_UND = INTREG_PC,
	INTREG_R15_UND = INTREG_R15,

	/* IRQ mode */
	INTREG_R0_IRQ = INTREG_R0,
	INTREG_R1_IRQ = INTREG_R1,
	INTREG_R2_IRQ = INTREG_R2,
	INTREG_R3_IRQ = INTREG_R3,
	INTREG_R4_IRQ = INTREG_R4,
	INTREG_R5_IRQ = INTREG_R5,
	INTREG_R6_IRQ = INTREG_R6,
	INTREG_R7_IRQ = INTREG_R7,
	INTREG_R8_IRQ = INTREG_R8,
	INTREG_R9_IRQ = INTREG_R9,
	INTREG_R10_IRQ = INTREG_R10,
	INTREG_R11_IRQ = INTREG_R11,
	INTREG_R12_IRQ = INTREG_R12,
	INTREG_PC_IRQ = INTREG_PC,
	INTREG_R15_IRQ = INTREG_R15,

	/* FIQ mode */
	INTREG_R0_FIQ = INTREG_R0,
	INTREG_R1_FIQ = INTREG_R1,
	INTREG_R2_FIQ = INTREG_R2,
	INTREG_R3_FIQ = INTREG_R3,
	INTREG_R4_FIQ = INTREG_R4,
	INTREG_R5_FIQ = INTREG_R5,
	INTREG_R6_FIQ = INTREG_R6,
	INTREG_R7_FIQ = INTREG_R7,
	INTREG_PC_FIQ = INTREG_PC,
	INTREG_R15_FIQ = INTREG_R15
};


#endif
