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

static bool TopOpeBRepBuild_traceCU = false; // dump curves

Standard_EXPORT void TopOpeBRepBuild_SettraceCU(const bool b)
{
  TopOpeBRepBuild_traceCU = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceCU()
{
  return TopOpeBRepBuild_traceCU;
}

static bool TopOpeBRepBuild_traceCUV = false; // dump curves verbose or not

Standard_EXPORT void TopOpeBRepBuild_SettraceCUV(const bool b)
{
  TopOpeBRepBuild_traceCUV = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceCUV()
{
  return TopOpeBRepBuild_traceCUV;
}

static bool TopOpeBRepBuild_traceSPF = false; // SplitFace

Standard_EXPORT void TopOpeBRepBuild_SettraceSPF(const bool b)
{
  TopOpeBRepBuild_traceSPF = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSPF()
{
  return TopOpeBRepBuild_traceSPF;
}

static bool TopOpeBRepBuild_traceSPS = false; // SplitSolid

Standard_EXPORT void TopOpeBRepBuild_SettraceSPS(const bool b)
{
  TopOpeBRepBuild_traceSPS = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSPS()
{
  return TopOpeBRepBuild_traceSPS;
}

static bool TopOpeBRepBuild_traceSHEX = false; // Check edge

Standard_EXPORT void TopOpeBRepBuild_SettraceSHEX(const bool b)
{
  TopOpeBRepBuild_traceSHEX = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSHEX()
{
  return TopOpeBRepBuild_traceSHEX;
}

static bool TopOpeBRepBuild_contextSF2 = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextSF2(const bool b)
{
  TopOpeBRepBuild_contextSF2 = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextSF2()
{
  return TopOpeBRepBuild_contextSF2;
}

static bool TopOpeBRepBuild_contextSPEON = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextSPEON(const bool b)
{
  TopOpeBRepBuild_contextSPEON = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextSPEON()
{
  return TopOpeBRepBuild_contextSPEON;
}

static bool TopOpeBRepBuild_traceSS = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceSS(const bool b)
{
  TopOpeBRepBuild_traceSS = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSS()
{
  return TopOpeBRepBuild_traceSS;
}

static bool TopOpeBRepBuild_contextSSCONNEX = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextSSCONNEX(const bool b)
{
  TopOpeBRepBuild_contextSSCONNEX = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextSSCONNEX()
{
  return TopOpeBRepBuild_contextSSCONNEX;
}

static bool TopOpeBRepBuild_traceAREA = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceAREA(const bool b)
{
  TopOpeBRepBuild_traceAREA = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceAREA()
{
  return TopOpeBRepBuild_traceAREA;
}

static bool TopOpeBRepBuild_traceKPB = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceKPB(const bool b)
{
  TopOpeBRepBuild_traceKPB = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceKPB()
{
  return TopOpeBRepBuild_traceKPB;
}

static bool TopOpeBRepBuild_traceCHK = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceCHK(const bool b)
{
  TopOpeBRepBuild_traceCHK = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceCHK()
{
  return TopOpeBRepBuild_traceCHK;
}

static bool TopOpeBRepBuild_traceCHKOK = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceCHKOK(const bool b)
{
  TopOpeBRepBuild_traceCHKOK = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceCHKOK()
{
  return TopOpeBRepBuild_traceCHKOK;
}

static bool TopOpeBRepBuild_traceCHKNOK = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceCHKNOK(const bool b)
{
  TopOpeBRepBuild_traceCHKNOK = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceCHKNOK()
{
  return TopOpeBRepBuild_traceCHKNOK;
}

static bool TopOpeBRepBuild_tracePURGE = false;

Standard_EXPORT void TopOpeBRepBuild_SettracePURGE(const bool b)
{
  TopOpeBRepBuild_tracePURGE = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettracePURGE()
{
  return TopOpeBRepBuild_tracePURGE;
}

static bool TopOpeBRepBuild_traceSAVFREGU = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceSAVFREGU(const bool b)
{
  TopOpeBRepBuild_traceSAVFREGU = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSAVFREGU()
{
  return TopOpeBRepBuild_traceSAVFREGU;
}

static bool TopOpeBRepBuild_traceSAVSREGU = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceSAVSREGU(const bool b)
{
  TopOpeBRepBuild_traceSAVSREGU = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceSAVSREGU()
{
  return TopOpeBRepBuild_traceSAVSREGU;
}

static bool TopOpeBRepBuild_traceFUFA = false;

Standard_EXPORT void TopOpeBRepBuild_SettraceFUFA(const bool b)
{
  TopOpeBRepBuild_traceFUFA = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceFUFA()
{
  return TopOpeBRepBuild_traceFUFA;
}

static bool TopOpeBRepBuild_contextNOPURGE = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOPURGE(const bool b)
{
  TopOpeBRepBuild_contextNOPURGE = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOPURGE()
{
  bool b = TopOpeBRepBuild_contextNOPURGE;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOPURGE actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextNOREGUFA = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOREGUFA(const bool b)
{
  TopOpeBRepBuild_contextNOREGUFA = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOREGUFA()
{
  bool b = TopOpeBRepBuild_contextNOREGUFA;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOREGUFA actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextNOREGUSO = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOREGUSO(const bool b)
{
  TopOpeBRepBuild_contextNOREGUSO = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOREGUSO()
{
  bool b = TopOpeBRepBuild_contextNOREGUSO;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOREGUSO actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextREGUXPU = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextREGUXPU(const bool b)
{
  TopOpeBRepBuild_contextREGUXPU = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextREGUXPU()
{
  return TopOpeBRepBuild_contextREGUXPU;
}

static bool TopOpeBRepBuild_contextNOCORRISO = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOCORRISO(const bool b)
{
  TopOpeBRepBuild_contextNOCORRISO = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOCORRISO()
{
  return TopOpeBRepBuild_contextNOCORRISO;
}

static bool TopOpeBRepBuild_contextEINTERNAL = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextEINTERNAL(const bool b)
{
  TopOpeBRepBuild_contextEINTERNAL = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextEINTERNAL()
{
  bool b = TopOpeBRepBuild_contextEINTERNAL;
  if (b)
    std::cout << "context (TopOpeBRepBuild) EINTERNAL actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextEEXTERNAL = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextEEXTERNAL(const bool b)
{
  TopOpeBRepBuild_contextEEXTERNAL = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextEEXTERNAL()
{
  bool b = TopOpeBRepBuild_contextEEXTERNAL;
  if (b)
    std::cout << "context (TopOpeBRepBuild) EEXTERNAL actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextNOSG = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOSG(const bool b)
{
  TopOpeBRepBuild_contextNOSG = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOSG()
{
  bool b = TopOpeBRepBuild_contextNOSG;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOSG actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextNOFUFA = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOFUFA(const bool b)
{
  TopOpeBRepBuild_contextNOFUFA = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOFUFA()
{
  bool b = TopOpeBRepBuild_contextNOFUFA;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOFUFA actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_contextNOFE = false;

Standard_EXPORT void TopOpeBRepBuild_SetcontextNOFE(const bool b)
{
  TopOpeBRepBuild_contextNOFE = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GetcontextNOFE()
{
  bool b = TopOpeBRepBuild_contextNOFE;
  if (b)
    std::cout << "context (TopOpeBRepBuild) NOFE actif" << std::endl;
  return b;
}

static bool TopOpeBRepBuild_traceFE = false; // trace FuseEdges

Standard_EXPORT void TopOpeBRepBuild_SettraceFE(const bool b)
{
  TopOpeBRepBuild_traceFE = b;
}

Standard_EXPORT bool TopOpeBRepBuild_GettraceFE()
{
  return TopOpeBRepBuild_traceFE;
}

// #ifdef OCCT_DEBUG
#endif
