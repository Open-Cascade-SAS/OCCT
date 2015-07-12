// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_TheMultiLineToolOfApprox_HeaderFile
#define _BRepApprox_TheMultiLineToolOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <Approx_Status.hxx>
class BRepApprox_TheMultiLineOfApprox;
class ApproxInt_SvSurfaces;



class BRepApprox_TheMultiLineToolOfApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Integer FirstPoint (const BRepApprox_TheMultiLineOfApprox& ML);
  
    static Standard_Integer LastPoint (const BRepApprox_TheMultiLineOfApprox& ML);
  
    static Standard_Integer NbP2d (const BRepApprox_TheMultiLineOfApprox& ML);
  
    static Standard_Integer NbP3d (const BRepApprox_TheMultiLineOfApprox& ML);
  
    static void Value (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt);
  
    static void Value (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt2d& tabPt2d);
  
    static void Value (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d);
  
    static Standard_Boolean Tangency (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
    static Standard_Boolean Tangency (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Tangency (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Curvature (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
    static Standard_Boolean Curvature (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Curvature (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
    static BRepApprox_TheMultiLineOfApprox MakeMLBetween (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer NbPMin);
  
    static Approx_Status WhatStatus (const BRepApprox_TheMultiLineOfApprox& ML, const Standard_Integer I1, const Standard_Integer I2);
  
  Standard_EXPORT static void Dump (const BRepApprox_TheMultiLineOfApprox& ML);




protected:





private:





};

#define TheMultiLine BRepApprox_TheMultiLineOfApprox
#define TheMultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define TheMultiMPoint ApproxInt_SvSurfaces
#define TheMultiMPoint_hxx <ApproxInt_SvSurfaces.hxx>
#define ApproxInt_MultiLineTool BRepApprox_TheMultiLineToolOfApprox
#define ApproxInt_MultiLineTool_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>

#include <ApproxInt_MultiLineTool.lxx>

#undef TheMultiLine
#undef TheMultiLine_hxx
#undef TheMultiMPoint
#undef TheMultiMPoint_hxx
#undef ApproxInt_MultiLineTool
#undef ApproxInt_MultiLineTool_hxx




#endif // _BRepApprox_TheMultiLineToolOfApprox_HeaderFile
