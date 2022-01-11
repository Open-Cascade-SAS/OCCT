// Created on: 1998-05-12
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepBlend_AppFuncRstRst_HeaderFile
#define _BRepBlend_AppFuncRstRst_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepBlend_AppFuncRoot.hxx>
#include <math_Vector.hxx>
class BRepBlend_Line;
class Blend_RstRstFunction;
class Blend_AppFunction;
class Blend_Point;


class BRepBlend_AppFuncRstRst;
DEFINE_STANDARD_HANDLE(BRepBlend_AppFuncRstRst, BRepBlend_AppFuncRoot)

//! Function to approximate by AppSurface for Edge/Face (Curve/Curve contact).
class BRepBlend_AppFuncRstRst : public BRepBlend_AppFuncRoot
{

public:

  
  Standard_EXPORT BRepBlend_AppFuncRstRst(Handle(BRepBlend_Line)& Line, Blend_RstRstFunction& Func, const Standard_Real Tol3d, const Standard_Real Tol2d);
  
  Standard_EXPORT void Point (const Blend_AppFunction& Func, const Standard_Real Param, const math_Vector& Sol, Blend_Point& Pnt) const Standard_OVERRIDE;
  
  Standard_EXPORT void Vec (math_Vector& Sol, const Blend_Point& Pnt) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRepBlend_AppFuncRstRst,BRepBlend_AppFuncRoot)

protected:




private:




};







#endif // _BRepBlend_AppFuncRstRst_HeaderFile
