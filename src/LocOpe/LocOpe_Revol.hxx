// Created on: 1997-02-24
// Created by: Olga PILLOT
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

#ifndef _LocOpe_Revol_HeaderFile
#define _LocOpe_Revol_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Ax1.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
class Geom_Curve;

//! Defines a prism (using Prism from BRepSweep)
//! with modifications provided for the Prism feature.
class LocOpe_Revol
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT LocOpe_Revol();

  Standard_EXPORT LocOpe_Revol(const TopoDS_Shape& Base,
                               const gp_Ax1&       Axis,
                               const Standard_Real Angle,
                               const Standard_Real angledec);

  Standard_EXPORT LocOpe_Revol(const TopoDS_Shape& Base,
                               const gp_Ax1&       Axis,
                               const Standard_Real Angle);

  Standard_EXPORT void Perform(const TopoDS_Shape& Base,
                               const gp_Ax1&       Axis,
                               const Standard_Real Angle,
                               const Standard_Real angledec);

  Standard_EXPORT void Perform(const TopoDS_Shape& Base,
                               const gp_Ax1&       Axis,
                               const Standard_Real Angle);

  Standard_EXPORT const TopoDS_Shape& FirstShape() const;

  Standard_EXPORT const TopoDS_Shape& LastShape() const;

  Standard_EXPORT const TopoDS_Shape& Shape() const;

  Standard_EXPORT const TopTools_ListOfShape& Shapes(const TopoDS_Shape& S) const;

  Standard_EXPORT void Curves(TColGeom_SequenceOfCurve& SCurves) const;

  Standard_EXPORT Handle(Geom_Curve) BarycCurve() const;

protected:
private:
  Standard_EXPORT void IntPerf();

  TopoDS_Shape                       myBase;
  gp_Ax1                             myAxis;
  Standard_Real                      myAngle;
  Standard_Real                      myAngTra;
  Standard_Boolean                   myIsTrans;
  Standard_Boolean                   myDone;
  TopoDS_Shape                       myRes;
  TopoDS_Shape                       myFirstShape;
  TopoDS_Shape                       myLastShape;
  TopTools_DataMapOfShapeListOfShape myMap;
};

#endif // _LocOpe_Revol_HeaderFile
