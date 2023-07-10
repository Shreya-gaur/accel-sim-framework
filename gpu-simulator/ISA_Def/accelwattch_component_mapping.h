// Copyright (c) 2018-2021, Mahmoud Khairy, Vijay Kandiah, Timothy Rogers, Tor M. Aamodt, Nikos Hardavellas
// Northwestern University, Purdue University, The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer;
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution;
// 3. Neither the names of Northwestern University, Purdue University,
//    The University of British Columbia nor the names of their contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include "abstract_hardware_model.h"
#include "trace_opcode.h"
#include <string>
#include <unordered_map>


/* This is the SASS instruction to Power Component mapping for AccelWattch */ 
static const std::unordered_map<unsigned, unsigned> OpcodePowerMap = {
    // Volta (includes common insts for others cards as well)
    {OP_FADD, FP__OP},
    {OP_FADD32I, FP__OP},
    {OP_FCHK, FP__OP},
    {OP_FFMA32I, FP_MUL_OP},
    {OP_FFMA, FP_MUL_OP},
    {OP_FMNMX, FP__OP},
    {OP_FMUL, FP_MUL_OP},
    {OP_FMUL32I, FP_MUL_OP},
    {OP_FSEL, FP__OP},
    {OP_FSET, FP__OP},
    {OP_FSETP, FP__OP},
    {OP_FSWZADD, FP__OP},
    {OP_MUFU, FP_SIN_OP}, // We differentiate between different MUFU operations (SIN,EX2,RSQ,LG2) in ../trace-driven/trace_driven.cc 
    {OP_HADD2, FP__OP},
    {OP_HADD2_32I, FP__OP},
    {OP_HFMA2, FP_MUL_OP},
    {OP_HFMA2_32I, FP_MUL_OP},
    {OP_HMUL2, FP_MUL_OP},
    {OP_HMUL2_32I, FP_MUL_OP},
    {OP_HSET2, FP__OP},
    {OP_HSETP2, FP__OP},
    {OP_HMMA, TENSOR__OP},
    {OP_DADD, DP___OP},
    {OP_DFMA, DP_MUL_OP},
    {OP_DMUL, DP_MUL_OP},
    {OP_DSETP, DP___OP},
    {OP_BMSK, INT__OP},
    {OP_BREV, INT__OP},
    {OP_FLO, INT__OP},
    {OP_IABS, INT__OP},
    {OP_IADD, INT__OP},
    {OP_IADD3, INT__OP},
    {OP_IADD32I, INT__OP},
    {OP_IDP, INT_MUL_OP},
    {OP_IDP4A, INT_MUL_OP},
    {OP_IMAD, INT_MUL_OP},
    {OP_IMMA, TENSOR__OP},
    {OP_IMNMX, INT__OP},
    {OP_IMUL, INT_MUL_OP},
    {OP_IMUL32I, INT_MUL_OP},
    {OP_ISCADD, INT_MUL_OP},
    {OP_ISCADD32I, INT_MUL_OP},
    {OP_ISETP, INT__OP},
    {OP_LEA, INT_MUL_OP},
    {OP_LOP, INT__OP},
    {OP_LOP3, INT__OP},
    {OP_LOP32I, INT__OP},
    {OP_POPC, INT__OP},
    {OP_SHF, INT__OP},
    {OP_SHR, INT__OP},
    {OP_VABSDIFF, INT__OP},
    {OP_VABSDIFF4, INT__OP},
    {OP_F2F, FP__OP},
    {OP_F2FP, FP__OP},
    {OP_F2I, FP__OP},
    {OP_I2F, FP__OP},
    {OP_I2I, INT__OP},
    {OP_I2IP, INT__OP},
    {OP_FRND, INT__OP},
    {OP_MOV, INT__OP},
    {OP_MOV32I, INT__OP},
    {OP_PRMT, INT__OP},
    {OP_SEL, INT__OP},
    {OP_SGXT, INT__OP},
    {OP_SHFL, INT__OP},
    {OP_PLOP3, INT__OP},
    {OP_PSETP, INT__OP},
    {OP_P2R, INT__OP},
    {OP_R2P, INT__OP},
    {OP_LD, OTHER_OP},
    {OP_LDC, OTHER_OP},
    {OP_LDG, OTHER_OP},
    {OP_LDL, OTHER_OP},
    {OP_LDS, OTHER_OP},
    {OP_ST, OTHER_OP},
    {OP_STG, OTHER_OP},
    {OP_STL, OTHER_OP},
    {OP_STS, OTHER_OP},
    {OP_MATCH, OTHER_OP},
    {OP_QSPC, OTHER_OP},
    {OP_ATOM, OTHER_OP},
    {OP_ATOMS, OTHER_OP},
    {OP_ATOMG, OTHER_OP},
    {OP_RED, OTHER_OP},
    {OP_CCTL, OTHER_OP},
    {OP_CCTLL, OTHER_OP},
    {OP_ERRBAR, OTHER_OP},
    {OP_MEMBAR, OTHER_OP},
    {OP_CCTLT, OTHER_OP},
    {OP_TEX, TEX__OP},
    {OP_TLD, TEX__OP},
    {OP_TLD4, TEX__OP},
    {OP_TMML, TEX__OP},
    {OP_TXD, TEX__OP},
    {OP_TXQ, TEX__OP},
    {OP_BMOV, OTHER_OP},
    {OP_BPT, OTHER_OP},
    {OP_BRA, OTHER_OP},
    {OP_BREAK, OTHER_OP},
    {OP_BRX, OTHER_OP},
    {OP_BSSY, OTHER_OP},
    {OP_BSYNC, OTHER_OP},
    {OP_CALL, OTHER_OP},
    {OP_EXIT, OTHER_OP},
    {OP_JMP, OTHER_OP},
    {OP_JMX, OTHER_OP},
    {OP_KILL, OTHER_OP},
    {OP_NANOSLEEP, OTHER_OP},
    {OP_RET, OTHER_OP},
    {OP_RPCMOV, OTHER_OP},
    {OP_RTT, OTHER_OP},
    {OP_WARPSYNC, OTHER_OP},
    {OP_YIELD, OTHER_OP},
    {OP_B2R, OTHER_OP},
    {OP_BAR, OTHER_OP},
    {OP_CS2R, INT__OP},
    {OP_CSMTEST, OTHER_OP},
    {OP_DEPBAR, OTHER_OP},
    {OP_GETLMEMBASE, OTHER_OP},
    {OP_LEPC, OTHER_OP},
    {OP_NOP, OTHER_OP},
    {OP_PMTRIG, OTHER_OP},
    {OP_R2B, OTHER_OP},
    {OP_S2R, OTHER_OP},
    {OP_SETCTAID, OTHER_OP},
    {OP_SETLMEMBASE, OTHER_OP},
    {OP_VOTE, OTHER_OP},
    {OP_VOTE_VTG, OTHER_OP},
    // unique insts for pascal
    {OP_RRO, FP__OP},
    {OP_DMNMX, DP___OP},
    {OP_DSET, DP___OP},
    {OP_BFE, INT__OP},
    {OP_BFI, INT__OP},
    {OP_ICMP, INT__OP},
    {OP_IMADSP, INT_MUL_OP},
    {OP_SHL, INT__OP},
    {OP_XMAD, INT_MUL_OP},
    {OP_CSET, INT__OP},
    {OP_CSETP, INT__OP},
    {OP_TEXS, TEX__OP},
    {OP_TLD4S, TEX__OP},
    {OP_TLDS, TEX__OP},
    {OP_CAL, OTHER_OP},
    {OP_JCAL, OTHER_OP},
    {OP_PRET, OTHER_OP},
    {OP_BRK, OTHER_OP},
    {OP_PBK, OTHER_OP},
    {OP_CONT, OTHER_OP},
    {OP_PCNT, OTHER_OP},
    {OP_PEXIT, OTHER_OP},
    {OP_SSY, OTHER_OP},
    {OP_SYNC, OTHER_OP},
    {OP_PSET, INT__OP},
    {OP_VMNMX, INT__OP},
    {OP_ISET, INT__OP},
    // unique insts for turing
    {OP_BMMA, TENSOR__OP},
    {OP_MOVM, INT__OP},
    {OP_LDSM, OTHER_OP},
    {OP_R2UR, INT__OP},
    {OP_S2UR, INT__OP},
    {OP_UBMSK, INT__OP},
    {OP_UBREV, INT__OP},
    {OP_UCLEA, INT_MUL_OP},
    {OP_UFLO, INT__OP},
    {OP_UIADD3, INT__OP},
    {OP_UIMAD, INT_MUL_OP},
    {OP_UISETP, INT__OP},
    {OP_ULDC, OTHER_OP},
    {OP_ULEA, INT__OP},
    {OP_ULOP, INT__OP},
    {OP_ULOP3, INT__OP},
    {OP_ULOP32I, INT__OP},
    {OP_UMOV, INT__OP},
    {OP_UP2UR, INT__OP},
    {OP_UPLOP3, INT__OP},
    {OP_UPOPC, INT__OP},
    {OP_UPRMT, INT__OP},
    {OP_UPSETP, INT__OP},
    {OP_UR2UP, INT__OP},
    {OP_USEL, INT__OP},
    {OP_USGXT, INT__OP},
    {OP_USHF, INT__OP},
    {OP_USHL, INT__OP},
    {OP_USHR, INT__OP},
    {OP_VOTEU, INT__OP},
    {OP_SUATOM, OTHER_OP},
    {OP_SULD, OTHER_OP},
    {OP_SURED, OTHER_OP},
    {OP_SUST, OTHER_OP},
    {OP_BRXU, OTHER_OP},
    {OP_JMXU, OTHER_OP},
    // unique insts for kepler
    {OP_FCMP, FP__OP},
    {OP_FSWZ, FP__OP},
    {OP_ISAD, INT__OP},
    {OP_LDSLK, OTHER_OP},
    {OP_STSCUL, OTHER_OP},
    {OP_SUCLAMP, OTHER_OP},
    {OP_SUBFM, OTHER_OP},
    {OP_SUEAU, OTHER_OP},
    {OP_SULDGA, OTHER_OP},
    {OP_SUSTGA, OTHER_OP},
    {OP_ISUB, INT__OP},
    // unique insts for ampere
    {OP_HMNMX2, FP__OP},
    {OP_DMMA, TENSOR__OP},
    {OP_I2FP, FP__OP},
    {OP_F2IP, FP__OP},
    {OP_LDGDEPBAR, OTHER_OP},
    {OP_LDGSTS, OTHER_OP},
    {OP_REDUX, INT__OP},
    {OP_UF2FP, FP__OP},
    {OP_SUQUERY, OTHER_OP},
};
