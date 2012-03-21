// Created on: 1994-03-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifdef DEB

#include <Standard_Type.hxx>

Standard_EXPORT Standard_Integer TopOpeBRepTool_BOOOPE_CHECK_DEB = 1;

static Standard_Boolean TopOpeBRepTool_traceBOXPERSO = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceBOXPERSO(const Standard_Boolean b) 
{ TopOpeBRepTool_traceBOXPERSO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceBOXPERSO() 
{ return TopOpeBRepTool_traceBOXPERSO; }

static Standard_Boolean TopOpeBRepTool_traceBOX = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceBOX(const Standard_Boolean b) 
{ TopOpeBRepTool_traceBOX = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceBOX() 
{ return TopOpeBRepTool_traceBOX; }

static Standard_Boolean TopOpeBRepTool_traceVC = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceVC(const Standard_Boolean b) 
{ TopOpeBRepTool_traceVC = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceVC() 
{ return TopOpeBRepTool_traceVC; }

static Standard_Boolean TopOpeBRepTool_traceNYI = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceNYI(const Standard_Boolean b) 
{ TopOpeBRepTool_traceNYI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceNYI() 
{ return TopOpeBRepTool_traceNYI; }

static Standard_Boolean TopOpeBRepTool_tracePCURV = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettracePCURV(const Standard_Boolean b) 
{ TopOpeBRepTool_tracePCURV = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettracePCURV() 
{ return TopOpeBRepTool_tracePCURV; }

static Standard_Boolean TopOpeBRepTool_traceCLOV = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceCLOV(const Standard_Boolean b) 
{ TopOpeBRepTool_traceCLOV = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCLOV() 
{ return TopOpeBRepTool_traceCLOV; }

static Standard_Boolean TopOpeBRepTool_traceCHKBSPL = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceCHKBSPL(const Standard_Boolean b) 
{ TopOpeBRepTool_traceCHKBSPL = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCHKBSPL() 
{ return TopOpeBRepTool_traceCHKBSPL; }

static Standard_Boolean TopOpeBRepTool_tracePURGE = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettracePURGE(const Standard_Boolean b) 
{ TopOpeBRepTool_tracePURGE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettracePURGE() 
{ return TopOpeBRepTool_tracePURGE; }

static Standard_Boolean TopOpeBRepTool_traceREGUFA = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceREGUFA(const Standard_Boolean b) 
{ TopOpeBRepTool_traceREGUFA = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceREGUFA() 
{ return TopOpeBRepTool_traceREGUFA; }

static Standard_Boolean TopOpeBRepTool_traceREGUSO = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceREGUSO(const Standard_Boolean b) 
{ TopOpeBRepTool_traceREGUSO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceREGUSO() 
{ return TopOpeBRepTool_traceREGUSO; }

static Standard_Boolean TopOpeBRepTool_traceC2D = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceC2D(const Standard_Boolean b) 
{ TopOpeBRepTool_traceC2D = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceC2D() 
{ return TopOpeBRepTool_traceC2D; }

static Standard_Boolean TopOpeBRepTool_traceCORRISO = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceCORRISO(const Standard_Boolean b) 
{ TopOpeBRepTool_traceCORRISO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCORRISO() 
{ return TopOpeBRepTool_traceCORRISO; }

static Standard_Boolean TopOpeBRepTool_traceKRO = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceKRO(const Standard_Boolean b)
{ TopOpeBRepTool_traceKRO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceKRO() 
{ return TopOpeBRepTool_traceKRO; }

static Standard_Boolean TopOpeBRepTool_traceEND = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SettraceEND(const Standard_Boolean b) { TopOpeBRepTool_traceEND = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceEND() { return TopOpeBRepTool_traceEND; }

static Standard_Boolean TopOpeBRepTool_contextNOSEW = Standard_False;
Standard_EXPORT void TopOpeBRepTool_SetcontextNOSEW(const
Standard_Boolean b) { TopOpeBRepTool_contextNOSEW = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GetcontextNOSEW() {
return TopOpeBRepTool_contextNOSEW; }

// #ifdef DEB
#endif
