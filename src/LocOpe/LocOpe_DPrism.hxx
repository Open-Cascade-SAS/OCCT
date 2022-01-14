// Created on: 1996-09-04
// Created by: Jacques GOUSSARD
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

#ifndef _LocOpe_DPrism_HeaderFile
#define _LocOpe_DPrism_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_Evolved.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class Geom_Curve;


//! Defines a  pipe  (near from   Pipe from BRepFill),
//! with modifications provided for the Pipe feature.
class LocOpe_DPrism 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT LocOpe_DPrism(const TopoDS_Face& Spine, const Standard_Real Height1, const Standard_Real Height2, const Standard_Real Angle);
  
  Standard_EXPORT LocOpe_DPrism(const TopoDS_Face& Spine, const Standard_Real Height, const Standard_Real Angle);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT const TopoDS_Shape& Spine() const;
  
  Standard_EXPORT const TopoDS_Shape& Profile() const;
  
  Standard_EXPORT const TopoDS_Shape& FirstShape() const;
  
  Standard_EXPORT const TopoDS_Shape& LastShape() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT const TopTools_ListOfShape& Shapes (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void Curves (TColGeom_SequenceOfCurve& SCurves) const;
  
  Standard_EXPORT Handle(Geom_Curve) BarycCurve() const;




protected:





private:

  
  Standard_EXPORT void IntPerf();


  BRepFill_Evolved myDPrism;
  TopoDS_Shape myRes;
  TopoDS_Face mySpine;
  TopoDS_Wire myProfile;
  TopoDS_Edge myProfile1;
  TopoDS_Edge myProfile2;
  TopoDS_Edge myProfile3;
  Standard_Real myHeight;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape myLastShape;
  TColGeom_SequenceOfCurve myCurvs;
  TopTools_DataMapOfShapeListOfShape myMap;


};







#endif // _LocOpe_DPrism_HeaderFile
