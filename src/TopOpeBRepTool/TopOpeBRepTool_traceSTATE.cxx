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
#include <TopOpeBRepTool_STATE.hxx>

static TopOpeBRepTool_STATE TopOpeBRepTool_CL3DDR("draw 3d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL3DPR("print 3d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL2DDR("draw 2d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL2DPR("print 2d classification states");

Standard_EXPORT void TopOpeBRepTool_SettraceCL3DDR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL3DDR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DDR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL3DDR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DDR()
{ return TopOpeBRepTool_CL3DDR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL3DPR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL3DPR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DPR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL3DPR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DPR()
{ return TopOpeBRepTool_CL3DPR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL2DDR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL2DDR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DDR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL2DDR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DDR()
{ return TopOpeBRepTool_CL2DDR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL2DPR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL2DPR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DPR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL2DPR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DPR() 
{ return TopOpeBRepTool_CL2DPR.Get(); }

// #ifdef DEB
#endif
