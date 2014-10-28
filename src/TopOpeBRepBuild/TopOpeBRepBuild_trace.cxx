// Created on: 1996-01-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifdef OCCT_DEBUG

#include <Standard_Type.hxx>

static Standard_Boolean TopOpeBRepBuild_traceCU = Standard_False;  // dump curves
Standard_EXPORT void TopOpeBRepBuild_SettraceCU(const Standard_Boolean b) { TopOpeBRepBuild_traceCU = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceCU() { return TopOpeBRepBuild_traceCU; }

static Standard_Boolean TopOpeBRepBuild_traceCUV = Standard_False; // dump curves verbose or not
Standard_EXPORT void TopOpeBRepBuild_SettraceCUV(const Standard_Boolean b) { TopOpeBRepBuild_traceCUV = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceCUV() { return TopOpeBRepBuild_traceCUV; }

static Standard_Boolean TopOpeBRepBuild_traceSPF = Standard_False;   // SplitFace
Standard_EXPORT void TopOpeBRepBuild_SettraceSPF(const Standard_Boolean b) { TopOpeBRepBuild_traceSPF = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSPF() { return TopOpeBRepBuild_traceSPF; }

static Standard_Boolean TopOpeBRepBuild_traceSPS = Standard_False;   // SplitSolid
Standard_EXPORT void TopOpeBRepBuild_SettraceSPS(const Standard_Boolean b) { TopOpeBRepBuild_traceSPS = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSPS() { return TopOpeBRepBuild_traceSPS; }

static Standard_Boolean TopOpeBRepBuild_traceSHEX = Standard_False;  // Check edge
Standard_EXPORT void TopOpeBRepBuild_SettraceSHEX(const Standard_Boolean b) { TopOpeBRepBuild_traceSHEX = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSHEX() { return TopOpeBRepBuild_traceSHEX; }

static Standard_Boolean TopOpeBRepBuild_contextSF2 = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextSF2(const Standard_Boolean b) { TopOpeBRepBuild_contextSF2 = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextSF2() { return TopOpeBRepBuild_contextSF2; }

static Standard_Boolean TopOpeBRepBuild_contextSPEON = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextSPEON(const Standard_Boolean b) { TopOpeBRepBuild_contextSPEON = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextSPEON() { return TopOpeBRepBuild_contextSPEON; }

static Standard_Boolean TopOpeBRepBuild_traceSS = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceSS(const Standard_Boolean b) { TopOpeBRepBuild_traceSS = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSS() { return TopOpeBRepBuild_traceSS; }

static Standard_Boolean TopOpeBRepBuild_contextSSCONNEX = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextSSCONNEX(const Standard_Boolean b) { TopOpeBRepBuild_contextSSCONNEX = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextSSCONNEX() { return TopOpeBRepBuild_contextSSCONNEX; }

static Standard_Boolean TopOpeBRepBuild_traceAREA = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceAREA(const Standard_Boolean b) {TopOpeBRepBuild_traceAREA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceAREA() { return TopOpeBRepBuild_traceAREA; }

static Standard_Boolean TopOpeBRepBuild_traceKPB = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceKPB(const Standard_Boolean b) 
{ TopOpeBRepBuild_traceKPB = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceKPB() 
{ return TopOpeBRepBuild_traceKPB; }

static Standard_Boolean TopOpeBRepBuild_traceCHK = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceCHK(const Standard_Boolean b) 
{ TopOpeBRepBuild_traceCHK = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceCHK() 
{ return TopOpeBRepBuild_traceCHK; }

static Standard_Boolean TopOpeBRepBuild_traceCHKOK = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceCHKOK(const Standard_Boolean b) 
{ TopOpeBRepBuild_traceCHKOK = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceCHKOK() 
{ return TopOpeBRepBuild_traceCHKOK; }

static Standard_Boolean TopOpeBRepBuild_traceCHKNOK = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceCHKNOK(const Standard_Boolean b) 
{ TopOpeBRepBuild_traceCHKNOK = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceCHKNOK() 
{ return TopOpeBRepBuild_traceCHKNOK; }

static Standard_Boolean TopOpeBRepBuild_tracePURGE = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettracePURGE(const Standard_Boolean b) 
{ TopOpeBRepBuild_tracePURGE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettracePURGE() 
{ return TopOpeBRepBuild_tracePURGE; }

static Standard_Boolean TopOpeBRepBuild_traceSAVFREGU = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceSAVFREGU(const Standard_Boolean b) { TopOpeBRepBuild_traceSAVFREGU = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSAVFREGU() { return TopOpeBRepBuild_traceSAVFREGU; }
static Standard_Boolean TopOpeBRepBuild_traceSAVSREGU = Standard_False;
Standard_EXPORT void TopOpeBRepBuild_SettraceSAVSREGU(const Standard_Boolean b) { TopOpeBRepBuild_traceSAVSREGU = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceSAVSREGU() { return TopOpeBRepBuild_traceSAVSREGU; }

static Standard_Boolean TopOpeBRepBuild_traceFUFA = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SettraceFUFA(const Standard_Boolean b) { TopOpeBRepBuild_traceFUFA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceFUFA() { return TopOpeBRepBuild_traceFUFA; }

static Standard_Boolean TopOpeBRepBuild_contextNOPURGE = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOPURGE(const Standard_Boolean b) { TopOpeBRepBuild_contextNOPURGE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOPURGE() { 
  Standard_Boolean b = TopOpeBRepBuild_contextNOPURGE;
  if (b) cout<<"context (TopOpeBRepBuild) NOPURGE actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextNOREGUFA = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOREGUFA(const Standard_Boolean b) { TopOpeBRepBuild_contextNOREGUFA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOREGUFA() { 
  Standard_Boolean b = TopOpeBRepBuild_contextNOREGUFA;
  if (b) cout<<"context (TopOpeBRepBuild) NOREGUFA actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextNOREGUSO = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOREGUSO(const Standard_Boolean b) { TopOpeBRepBuild_contextNOREGUSO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOREGUSO() {
  Standard_Boolean b = TopOpeBRepBuild_contextNOREGUSO;
  if (b) cout<<"context (TopOpeBRepBuild) NOREGUSO actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextREGUXPU = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextREGUXPU(const Standard_Boolean b) { TopOpeBRepBuild_contextREGUXPU = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextREGUXPU() { return TopOpeBRepBuild_contextREGUXPU; }

static Standard_Boolean TopOpeBRepBuild_contextNOCORRISO = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOCORRISO(const Standard_Boolean b) { TopOpeBRepBuild_contextNOCORRISO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOCORRISO() { return TopOpeBRepBuild_contextNOCORRISO; }

static Standard_Boolean TopOpeBRepBuild_contextEINTERNAL = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextEINTERNAL(const Standard_Boolean b) { TopOpeBRepBuild_contextEINTERNAL = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEINTERNAL() { 
  Standard_Boolean b = TopOpeBRepBuild_contextEINTERNAL;
  if (b) cout<<"context (TopOpeBRepBuild) EINTERNAL actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextEEXTERNAL = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextEEXTERNAL(const Standard_Boolean b) { TopOpeBRepBuild_contextEEXTERNAL = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextEEXTERNAL() { 
  Standard_Boolean b = TopOpeBRepBuild_contextEEXTERNAL;
  if (b) cout<<"context (TopOpeBRepBuild) EEXTERNAL actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextNOSG = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOSG(const Standard_Boolean b) { TopOpeBRepBuild_contextNOSG = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOSG() {
  Standard_Boolean b = TopOpeBRepBuild_contextNOSG;
  if (b) cout<<"context (TopOpeBRepBuild) NOSG actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextNOFUFA = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOFUFA(const Standard_Boolean b) { TopOpeBRepBuild_contextNOFUFA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOFUFA() { 
  Standard_Boolean b = TopOpeBRepBuild_contextNOFUFA;
  if (b) cout<<"context (TopOpeBRepBuild) NOFUFA actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_contextNOFE = Standard_False;  
Standard_EXPORT void TopOpeBRepBuild_SetcontextNOFE(const Standard_Boolean b) { TopOpeBRepBuild_contextNOFE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOFE() { 
  Standard_Boolean b = TopOpeBRepBuild_contextNOFE;
  if (b) cout<<"context (TopOpeBRepBuild) NOFE actif"<<endl;
  return b;
}

static Standard_Boolean TopOpeBRepBuild_traceFE = Standard_False;  // trace FuseEdges
Standard_EXPORT void TopOpeBRepBuild_SettraceFE(const Standard_Boolean b) { TopOpeBRepBuild_traceFE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GettraceFE() { return TopOpeBRepBuild_traceFE; }

// #ifdef OCCT_DEBUG
#endif
