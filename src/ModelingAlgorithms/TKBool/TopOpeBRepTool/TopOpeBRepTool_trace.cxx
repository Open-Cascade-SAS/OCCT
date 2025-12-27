// Created on: 1994-03-10
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

#ifdef OCCT_DEBUG

  #include <Standard_Type.hxx>

Standard_EXPORT int TopOpeBRepTool_BOOOPE_CHECK_DEB = 1;

static bool TopOpeBRepTool_traceBOXPERSO = false;

Standard_EXPORT void TopOpeBRepTool_SettraceBOXPERSO(const bool b)
{
  TopOpeBRepTool_traceBOXPERSO = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceBOXPERSO()
{
  return TopOpeBRepTool_traceBOXPERSO;
}

static bool TopOpeBRepTool_traceBOX = false;

Standard_EXPORT void TopOpeBRepTool_SettraceBOX(const bool b)
{
  TopOpeBRepTool_traceBOX = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceBOX()
{
  return TopOpeBRepTool_traceBOX;
}

static bool TopOpeBRepTool_traceVC = false;

Standard_EXPORT void TopOpeBRepTool_SettraceVC(const bool b)
{
  TopOpeBRepTool_traceVC = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceVC()
{
  return TopOpeBRepTool_traceVC;
}

static bool TopOpeBRepTool_traceNYI = false;

Standard_EXPORT void TopOpeBRepTool_SettraceNYI(const bool b)
{
  TopOpeBRepTool_traceNYI = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceNYI()
{
  return TopOpeBRepTool_traceNYI;
}

static bool TopOpeBRepTool_tracePCURV = false;

Standard_EXPORT void TopOpeBRepTool_SettracePCURV(const bool b)
{
  TopOpeBRepTool_tracePCURV = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettracePCURV()
{
  return TopOpeBRepTool_tracePCURV;
}

static bool TopOpeBRepTool_traceCLOV = false;

Standard_EXPORT void TopOpeBRepTool_SettraceCLOV(const bool b)
{
  TopOpeBRepTool_traceCLOV = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceCLOV()
{
  return TopOpeBRepTool_traceCLOV;
}

static bool TopOpeBRepTool_traceCHKBSPL = false;

Standard_EXPORT void TopOpeBRepTool_SettraceCHKBSPL(const bool b)
{
  TopOpeBRepTool_traceCHKBSPL = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceCHKBSPL()
{
  return TopOpeBRepTool_traceCHKBSPL;
}

static bool TopOpeBRepTool_tracePURGE = false;

Standard_EXPORT void TopOpeBRepTool_SettracePURGE(const bool b)
{
  TopOpeBRepTool_tracePURGE = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettracePURGE()
{
  return TopOpeBRepTool_tracePURGE;
}

static bool TopOpeBRepTool_traceREGUFA = false;

Standard_EXPORT void TopOpeBRepTool_SettraceREGUFA(const bool b)
{
  TopOpeBRepTool_traceREGUFA = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceREGUFA()
{
  return TopOpeBRepTool_traceREGUFA;
}

static bool TopOpeBRepTool_traceREGUSO = false;

Standard_EXPORT void TopOpeBRepTool_SettraceREGUSO(const bool b)
{
  TopOpeBRepTool_traceREGUSO = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceREGUSO()
{
  return TopOpeBRepTool_traceREGUSO;
}

static bool TopOpeBRepTool_traceC2D = false;

Standard_EXPORT void TopOpeBRepTool_SettraceC2D(const bool b)
{
  TopOpeBRepTool_traceC2D = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceC2D()
{
  return TopOpeBRepTool_traceC2D;
}

static bool TopOpeBRepTool_traceCORRISO = false;

Standard_EXPORT void TopOpeBRepTool_SettraceCORRISO(const bool b)
{
  TopOpeBRepTool_traceCORRISO = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceCORRISO()
{
  return TopOpeBRepTool_traceCORRISO;
}

static bool TopOpeBRepTool_traceKRO = false;

Standard_EXPORT void TopOpeBRepTool_SettraceKRO(const bool b)
{
  TopOpeBRepTool_traceKRO = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceKRO()
{
  return TopOpeBRepTool_traceKRO;
}

static bool TopOpeBRepTool_traceEND = false;

Standard_EXPORT void TopOpeBRepTool_SettraceEND(const bool b)
{
  TopOpeBRepTool_traceEND = b;
}

Standard_EXPORT bool TopOpeBRepTool_GettraceEND()
{
  return TopOpeBRepTool_traceEND;
}

static bool TopOpeBRepTool_contextNOSEW = false;

Standard_EXPORT void TopOpeBRepTool_SetcontextNOSEW(const bool b)
{
  TopOpeBRepTool_contextNOSEW = b;
}

Standard_EXPORT bool TopOpeBRepTool_GetcontextNOSEW()
{
  return TopOpeBRepTool_contextNOSEW;
}

// #ifdef OCCT_DEBUG
#endif
