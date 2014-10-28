// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TestTopOpeTools.hxx>
#include <TestTopOpeTools_Trace.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <TopOpeBRepTool_define.hxx>

#ifdef OCCT_DEBUG
TestTopOpeTools_Trace theTrace(100,"trace");
TestTopOpeTools_Trace theContext(100,"context");

Standard_IMPORT void TopClass_SettraceFC(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceEND(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceKRO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SetcontextNOSEW(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SetcontextNOPNC(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceEDSF(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSP(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSF(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSFK(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSNC(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSLT(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDEGEN(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceDSFD(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettracePCI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettracePEI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettracePFI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettracePI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceSTRANGE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceGAP(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SetcontextNOGAP(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettraceFITOL(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettraceBIPS(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettraceSI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettraceSIFF(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRep_SettraceSAVFF(const Standard_Boolean);
Standard_IMPORT Standard_Integer TopOpeBRep_SettraceNVP(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT Standard_Integer TopOpeBRep_SettraceEEFF(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT Standard_Integer TopOpeBRep_SettraceSHA(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRep_SetcontextREST1(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextREST2(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextINL(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextNEWKP(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextTOL0(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextNONOG(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceCHK(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceCHKOK(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceCHKNOK(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceCU(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceCUV(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceSPSX(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepDS_SettraceCX(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepDS_SettraceSPSXX(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepBuild_SettraceSPF(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceSPS(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceSHEX(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceSS(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceAREA(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceKPB(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettracePURGE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceSAVFREGU(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceSAVSREGU(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOSG(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SettraceFE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextSSCONNEX(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextSF2(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextSPEON(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOPURGE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOREGUFA(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOREGUSO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextREGUXPU(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOCORRISO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOFUFA(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextNOFE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextEINTERNAL(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepBuild_SetcontextEEXTERNAL(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceNYI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceVC(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettracePCURV(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceBOXPERSO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceBOX(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceCLOV(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceCHKBSPL(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceREGUFA(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceREGUSO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceC2D(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceCORRISO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettraceCONIC(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SettracePROEDG(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextNOPROEDG(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextFFOR(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextNOPUNK(const Standard_Boolean);
Standard_IMPORT void TopOpeBRep_SetcontextNOFEI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepTool_SettraceCL2DDR(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepTool_SettraceCL2DPR(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepTool_SettraceCL3DDR(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepTool_SettraceCL3DPR(const Standard_Boolean, Standard_Integer, char**);
Standard_IMPORT void TopOpeBRepDS_SettraceBUTO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceTRPE(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceEDPR(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceISTO(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SettraceSANTRAN(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SetcontextNOPFI(const Standard_Boolean);
Standard_IMPORT void TopOpeBRepDS_SetcontextMKTONREG(const Standard_Boolean);
#endif

#ifdef OCCT_DEBUG
//----------------------------------------------------------------------------
static Standard_Integer InitTraceTopOpeKernel (TestTopOpeTools_Trace& T)
//----------------------------------------------------------------------------
{
  if (T.Add("tfc",(tf_value)TopClass_SettraceFC)) return 1;
  if (T.Add("tend",(tf_value)TopOpeBRepTool_SettraceEND)) return 1;
  if (T.Add("tkro",(tf_value)TopOpeBRepTool_SettraceKRO)) return 1;
  if (T.Add("tdsfk",(tf_value)TopOpeBRepDS_SettraceDSFK)) return 1;
  if (T.Add("tedsf",(tf_value)TopOpeBRepDS_SettraceEDSF)) return 1;
  if (T.Add("tdsp",(tf_value)TopOpeBRepDS_SettraceDSP)) return 1;
  if (T.Add("tdsf",(tf_value)TopOpeBRepDS_SettraceDSF)) return 1;
  if (T.Add("tdsnc",(tf_value)TopOpeBRepDS_SettraceDSNC)) return 1;
  if (T.Add("tdegen",(tf_value)TopOpeBRepDS_SettraceDEGEN)) return 1;
  if (T.Add("tdslt",(tf_value)TopOpeBRepDS_SettraceDSLT)) return 1;
  if (T.Add("tdsfd",(tf_value)TopOpeBRepDS_SettraceDSFD)) return 1;
  if (T.Add("tpci",(tf_value)TopOpeBRepDS_SettracePCI)) return 1;
  if (T.Add("tpei",(tf_value)TopOpeBRepDS_SettracePEI)) return 1;
  if (T.Add("tpfi",(tf_value)TopOpeBRepDS_SettracePFI)) return 1;
  if (T.Add("tpi",(tf_value)TopOpeBRepDS_SettracePI)) return 1;
  if (T.Add("tstrange",(tf_value)TopOpeBRepDS_SettraceSTRANGE)) return 1;
  if (T.Add("tgap",(tf_value)TopOpeBRepDS_SettraceGAP)) return 1;
  if (T.Add("tfitol",(tf_value)TopOpeBRep_SettraceFITOL)) return 1;
  if (T.Add("tbips",(tf_value)TopOpeBRep_SettraceBIPS)) return 1;
  if (T.Add("tsi",(tf_value)TopOpeBRep_SettraceSI)) return 1;
  if (T.Add("tsiff",(tf_intarg)TopOpeBRep_SettraceSIFF)) return 1; // tsiff 0|[1 s1 s2 name]
  if (T.Add("tsavff",(tf_value)TopOpeBRep_SettraceSAVFF)) return 1;
  if (T.Add("tnvp",(tf_int_intarg)TopOpeBRep_SettraceNVP)) return 1; // tnvp 0|1 if1 if2 ili ivp isi
  if (T.Add("teeff",(tf_int_intarg)TopOpeBRep_SettraceEEFF)) return 1; // teeff 0|1 ie1 ie2 if1 if2
  if (T.Add("tsha",(tf_int_intarg)TopOpeBRep_SettraceSHA)) return 1; // tsha 0|1 isha
  if (T.Add("tchk",(tf_value)TopOpeBRepBuild_SettraceCHK)) return 1;  
  if (T.Add("tok",(tf_value)TopOpeBRepBuild_SettraceCHKOK)) return 1;
  if (T.Add("tnok",(tf_value)TopOpeBRepBuild_SettraceCHKNOK)) return 1;
  if (T.Add("tcu",(tf_value)TopOpeBRepBuild_SettraceCU)) return 1;
  if (T.Add("tcuv",(tf_value)TopOpeBRepBuild_SettraceCUV)) return 1;
  if (T.Add("tcx",(tf_intarg)TopOpeBRepDS_SettraceCX)) return 1; // tcx 0|1 [i1 ...]
  if (T.Add("tspsx",(tf_intarg)TopOpeBRepDS_SettraceSPSX)) return 1; // tspsx 0|1 [i1 ...]
  if (T.Add("tspsxx",(tf_intarg)TopOpeBRepDS_SettraceSPSXX)) return 1; // tspsxx 0|1 [i1 i2]
  if (T.Add("tspf",(tf_value)TopOpeBRepBuild_SettraceSPF)) return 1; // splitface
  if (T.Add("tsps",(tf_value)TopOpeBRepBuild_SettraceSPS)) return 1; // splitsolid
  if (T.Add("tshex",(tf_value)TopOpeBRepBuild_SettraceSHEX)) return 1;
  if (T.Add("tss",(tf_value)TopOpeBRepBuild_SettraceSS)) return 1;
  if (T.Add("tarea",(tf_value)TopOpeBRepBuild_SettraceAREA)) return 1;
  if (T.Add("tkpb",(tf_value)TopOpeBRepBuild_SettraceKPB)) return 1;
  if (T.Add("tpurge",(tf_value)TopOpeBRepBuild_SettracePURGE)) return 1;
  if (T.Add("tsavfregu",(tf_value)TopOpeBRepBuild_SettraceSAVFREGU)) return 1;
  if (T.Add("tsavsregu",(tf_value)TopOpeBRepBuild_SettraceSAVSREGU)) return 1;
  if (T.Add("tfe",(tf_value)TopOpeBRepBuild_SettraceFE)) return 1;
  if (T.Add("tnyi",(tf_value)TopOpeBRepTool_SettraceNYI)) return 1;
  if (T.Add("tvc",(tf_value)TopOpeBRepTool_SettraceVC)) return 1;
  if (T.Add("tpcurv",(tf_value)TopOpeBRepTool_SettracePCURV)) return 1;
  if (T.Add("tbp",(tf_value)TopOpeBRepTool_SettraceBOXPERSO)) return 1;
  if (T.Add("tbox",(tf_value)TopOpeBRepTool_SettraceBOX)) return 1;
  if (T.Add("tclov",(tf_value)TopOpeBRepTool_SettraceCLOV)) return 1;
  if (T.Add("tchkbspl",(tf_value)TopOpeBRepTool_SettraceCHKBSPL)) return 1;
  if (T.Add("tregufa",(tf_value)TopOpeBRepTool_SettraceREGUFA)) return 1;
  if (T.Add("treguso",(tf_value)TopOpeBRepTool_SettraceREGUSO)) return 1;
  if (T.Add("tc2d",(tf_value)TopOpeBRepTool_SettraceC2D)) return 1;
  if (T.Add("tcorriso",(tf_value)TopOpeBRepTool_SettraceCORRISO)) return 1;
  if (T.Add("tcl2ddr",(tf_intarg)TopOpeBRepTool_SettraceCL2DDR)) return 1;
  if (T.Add("tcl2dpr",(tf_intarg)TopOpeBRepTool_SettraceCL2DPR)) return 1;
  if (T.Add("tcl3ddr",(tf_intarg)TopOpeBRepTool_SettraceCL3DDR)) return 1;
  if (T.Add("tcl3dpr",(tf_intarg)TopOpeBRepTool_SettraceCL3DPR)) return 1;
  if (T.Add("tbuto",(tf_value)TopOpeBRepDS_SettraceBUTO)) return 1;
  if (T.Add("ttrpe",(tf_value)TopOpeBRepDS_SettraceTRPE)) return 1;
  if (T.Add("tedpr",(tf_value)TopOpeBRepDS_SettraceEDPR)) return 1;
  if (T.Add("tisto",(tf_value)TopOpeBRepDS_SettraceISTO)) return 1;
  if (T.Add("tproedg",(tf_value)TopOpeBRep_SettracePROEDG)) return 1;
  if (T.Add("tconic",(tf_value)TopOpeBRep_SettraceCONIC)) return 1;
  return 0;
}
#endif
#ifdef OCCT_DEBUG
//----------------------------------------------------------------------------
Standard_Integer InitContextTopOpeKernel (TestTopOpeTools_Trace& T)
//----------------------------------------------------------------------------
{
  if (T.Add("nosew",(tf_value)TopOpeBRepTool_SetcontextNOSEW)) return 1;
  if (T.Add("nopnc",(tf_value)TopOpeBRepDS_SetcontextNOPNC)) return 1;
  if (T.Add("nosg",(tf_value)TopOpeBRepBuild_SetcontextNOSG)) return 1;
  if (T.Add("ssconnex",(tf_value)TopOpeBRepBuild_SetcontextSSCONNEX)) return 1;
  if (T.Add("sf2",(tf_value)TopOpeBRepBuild_SetcontextSF2)) return 1;
  if (T.Add("speon",(tf_value)TopOpeBRepBuild_SetcontextSPEON)) return 1;
  if (T.Add("nopurge",(tf_value)TopOpeBRepBuild_SetcontextNOPURGE)) return 1;
  if (T.Add("noregufa",(tf_value)TopOpeBRepBuild_SetcontextNOREGUFA)) return 1;
  if (T.Add("noreguso",(tf_value)TopOpeBRepBuild_SetcontextNOREGUSO)) return 1;
  if (T.Add("reguxpu",(tf_value)TopOpeBRepBuild_SetcontextREGUXPU)) return 1;
  if (T.Add("nocorriso",(tf_value)TopOpeBRepBuild_SetcontextNOCORRISO)) return 1;
  if (T.Add("nofe",(tf_value)TopOpeBRepBuild_SetcontextNOFE)) return 1;
  if (T.Add("nofufa",(tf_value)TopOpeBRepBuild_SetcontextNOFUFA)) return 1;
  if (T.Add("einternal",(tf_value)TopOpeBRepBuild_SetcontextEINTERNAL)) return 1;
  if (T.Add("eexternal",(tf_value)TopOpeBRepBuild_SetcontextEEXTERNAL)) return 1;
  if (T.Add("santran",(tf_value)TopOpeBRepDS_SettraceSANTRAN)) return 1;
  if (T.Add("noproedg",(tf_value)TopOpeBRep_SetcontextNOPROEDG)) return 1;
  if (T.Add("rest1",(tf_value)TopOpeBRep_SetcontextREST1)) return 1;
  if (T.Add("rest2",(tf_value)TopOpeBRep_SetcontextREST2)) return 1;
  if (T.Add("inl",(tf_value)TopOpeBRep_SetcontextINL)) return 1;
  if (T.Add("newkp",(tf_value)TopOpeBRep_SetcontextNEWKP)) return 1;
  if (T.Add("tol0",(tf_value)TopOpeBRep_SetcontextTOL0)) return 1;
  if (T.Add("nonog",(tf_value)TopOpeBRep_SetcontextNONOG)) return 1;
  if (T.Add("ffor",(tf_value)TopOpeBRep_SetcontextFFOR)) return 1;
  if (T.Add("nopunk",(tf_value)TopOpeBRep_SetcontextNOPUNK)) return 1;
  if (T.Add("nofei",(tf_value)TopOpeBRep_SetcontextNOFEI)) return 1;
  if (T.Add("nopfi",(tf_value)TopOpeBRepDS_SetcontextNOPFI)) return 1;
  if (T.Add("mktonreg",(tf_value)TopOpeBRepDS_SetcontextMKTONREG)) return 1;
  if (T.Add("nogap",(tf_value)TopOpeBRepDS_SetcontextNOGAP)) return 1;

  return 0;
}
#else 
Standard_Integer InitContextTopOpeKernel (TestTopOpeTools_Trace&) { return 0;}
#endif

Standard_EXPORT Standard_Integer TestTopOpeTools_SetFlags
(TestTopOpeTools_Trace& theFlags, Standard_Integer mute, Standard_Integer n , const char** a) 
{
  if (n == 1) { 
    theFlags.Dump();
    return 0;
  }
  if (n < 2) {
    return 1;
  }
  const char* flag = a[1];
  t_value value; // new value of flag
  if (n == 2) value = Standard_True;
  else value = (Draw::Atoi(a[2]) ? Standard_True : Standard_False);

  if (mute == 0) {
    if ( n == 2 ) {
      if      (!strcasecmp(a[1],"0")) {
	theFlags.Reset(Standard_False);
	cout<<theFlags.Genre()<<"s desactivated"<<endl;
	return 0;
      }
      else if (!strcasecmp(a[1],"1")) {
	theFlags.Reset(Standard_True);
	cout<<theFlags.Genre()<<"s activated"<<endl;
	return 0;
      }
    }
  }

  Standard_Integer res = 1;
  if (res) res=theFlags.Set(mute,flag,value,(n>3)?(n-3):0,(n>3)?&a[3]:NULL);
  if (res) res=theFlags.Set(mute,flag,value);
  return res;
}

//=========================================================================
// a1 = flag (for example tbs) <a2> = value, if omitted flag becomes True
//=========================================================================

 
#ifdef OCCT_DEBUG
Standard_Integer TestTopOpeTools_SetTrace(Draw_Interpretor&, Standard_Integer n ,const char** a)
#else
Standard_Integer TestTopOpeTools_SetTrace(Draw_Interpretor&, Standard_Integer,const char**)
#endif
{
  Standard_Integer ok = Standard_True;
#ifdef OCCT_DEBUG
  InitTraceTopOpeKernel(theTrace);
  Standard_Integer mute = 0; if (!strcasecmp(a[0],"trcmute")) mute = 1;
  ok = TestTopOpeTools_SetFlags(theTrace,mute,n,a);
#endif
  return ok;
}

//=========================================================================
// a1 = flag (for example tbs) <a2> = value, if omitted flag becomes True
//=========================================================================

#ifdef OCCT_DEBUG
Standard_Integer TestTopOpeTools_SetContext(Draw_Interpretor&, Standard_Integer n ,const char** a)
{
  Standard_Integer ok = Standard_True;
  InitContextTopOpeKernel(theContext);
  Standard_Integer mute = 0; if (!strcasecmp(a[0],"ctxmute")) mute = 1;
  ok = TestTopOpeTools_SetFlags(theContext,mute,n,a);
  return ok;
}
#else
Standard_Integer TestTopOpeTools_SetContext(Draw_Interpretor&, Standard_Integer,const char**) 
{ return Standard_True;}
#endif

Standard_Integer dstrace(Draw_Interpretor& di, Standard_Integer n , const char** a) {
  const char ** pa = NULL; TCollection_AsciiString a1;
  if      (!strcasecmp(a[0],"tsx")) a1 = "tspsx";
  else if (!strcasecmp(a[0],"tcx")) a1 = "tcx";
  else return 0;
  TCollection_AsciiString a0("trc");
  Standard_Integer npa = n+1;pa = (const char**) malloc((size_t)((npa)*sizeof(char*)));
  pa[0] = (char *)a0.ToCString();
  pa[1] = (char *)a1.ToCString();
  for (Standard_Integer i=1;i<n;i++) pa[i+1] = a[i];
  TestTopOpeTools_SetTrace(di,npa,pa);
  if (pa) free(pa);
  return 0;
}

//=======================================================================
//function : TraceCommands
//purpose  : 
//=======================================================================

void  TestTopOpeTools::TraceCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "trace commands";
  
  theCommands.Add("tsx","tsx 0|1 [{shape index}]",__FILE__,dstrace,g);
  theCommands.Add("tsxx","tsxx 0|1 is1 is2",__FILE__,dstrace,g);
  theCommands.Add("tcx","tcx 0|1 [{curve index}]",__FILE__,dstrace,g);

  theCommands.Add("trc","Trace <flag> <value>",__FILE__,TestTopOpeTools_SetTrace,g);
  theCommands.Add("trcmute","Trace <flag> <value>",__FILE__,TestTopOpeTools_SetTrace,g);
  theCommands.Add("ctx","Context <flag> <value>",__FILE__,TestTopOpeTools_SetContext,g);
  theCommands.Add("ctxmute","Context <flag> <value>",__FILE__,TestTopOpeTools_SetContext,g);
}
