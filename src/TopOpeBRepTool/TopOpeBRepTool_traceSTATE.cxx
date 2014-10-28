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

// #ifdef OCCT_DEBUG
#endif
