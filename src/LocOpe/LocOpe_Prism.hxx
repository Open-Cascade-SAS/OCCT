// Created on: 1997-02-24
// Created by: Jacques GOUSSARD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _LocOpe_Prism_HeaderFile
#define _LocOpe_Prism_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
class Standard_NoSuchObject;
class StdFail_NotDone;
class TopoDS_Shape;
class gp_Vec;
class Geom_Curve;


//! Defines a prism (using Prism from BRepSweep)
//! with modifications provided for the Prism feature.
class LocOpe_Prism 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT LocOpe_Prism();
  
  Standard_EXPORT LocOpe_Prism(const TopoDS_Shape& Base, const gp_Vec& V);
  
  Standard_EXPORT LocOpe_Prism(const TopoDS_Shape& Base, const gp_Vec& V, const gp_Vec& Vectra);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Base, const gp_Vec& V);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& Base, const gp_Vec& V, const gp_Vec& Vtra);
  
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;
  
  Standard_EXPORT const TopoDS_Shape& LastShape() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT const TopTools_ListOfShape& Shapes (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void Curves (TColGeom_SequenceOfCurve& SCurves) const;
  
  Standard_EXPORT Handle(Geom_Curve) BarycCurve() const;




protected:





private:

  
  Standard_EXPORT void IntPerf();


  TopoDS_Shape myBase;
  gp_Vec myVec;
  gp_Vec myTra;
  Standard_Boolean myIsTrans;
  Standard_Boolean myDone;
  TopoDS_Shape myRes;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape myLastShape;
  TopTools_DataMapOfShapeListOfShape myMap;


};







#endif // _LocOpe_Prism_HeaderFile
