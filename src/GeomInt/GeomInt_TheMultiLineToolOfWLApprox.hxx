// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile
#define _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <Approx_Status.hxx>
class GeomInt_TheMultiLineOfWLApprox;
class ApproxInt_SvSurfaces;



class GeomInt_TheMultiLineToolOfWLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Integer FirstPoint (const GeomInt_TheMultiLineOfWLApprox& ML);
  
    static Standard_Integer LastPoint (const GeomInt_TheMultiLineOfWLApprox& ML);
  
    static Standard_Integer NbP2d (const GeomInt_TheMultiLineOfWLApprox& ML);
  
    static Standard_Integer NbP3d (const GeomInt_TheMultiLineOfWLApprox& ML);
  
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt);
  
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt2d& tabPt2d);
  
    static void Value (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d);
  
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Tangency (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV);
  
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d);
  
    static Standard_Boolean Curvature (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d);
  
    static GeomInt_TheMultiLineOfWLApprox MakeMLBetween (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer I1, const Standard_Integer I2, const Standard_Integer NbPMin);
  
    static Approx_Status WhatStatus (const GeomInt_TheMultiLineOfWLApprox& ML, const Standard_Integer I1, const Standard_Integer I2);
  
  Standard_EXPORT static void Dump (const GeomInt_TheMultiLineOfWLApprox& ML);




protected:





private:





};

#define TheMultiLine GeomInt_TheMultiLineOfWLApprox
#define TheMultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define TheMultiMPoint ApproxInt_SvSurfaces
#define TheMultiMPoint_hxx <ApproxInt_SvSurfaces.hxx>
#define ApproxInt_MultiLineTool GeomInt_TheMultiLineToolOfWLApprox
#define ApproxInt_MultiLineTool_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>

#include <ApproxInt_MultiLineTool.lxx>

#undef TheMultiLine
#undef TheMultiLine_hxx
#undef TheMultiMPoint
#undef TheMultiMPoint_hxx
#undef ApproxInt_MultiLineTool
#undef ApproxInt_MultiLineTool_hxx




#endif // _GeomInt_TheMultiLineToolOfWLApprox_HeaderFile
