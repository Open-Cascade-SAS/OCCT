// Created on: 1997-08-20
// Created by: Guest Design
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

#ifndef _TPrsStd_ConstraintTools_HeaderFile
#define _TPrsStd_ConstraintTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class TDataXtd_Constraint;
class AIS_InteractiveObject;
class TCollection_ExtendedString;
class TopoDS_Shape;
class Geom_Geometry;



class TPrsStd_ConstraintTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void UpdateOnlyValue (const Handle(TDataXtd_Constraint)& aConst, const Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeDistance (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeParallel (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeTangent (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputePerpendicular (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeConcentric (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeSymmetry (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeMidPoint (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeAngle (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeRadius (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeMinRadius (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeMaxRadius (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeEqualDistance (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeEqualRadius (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeFix (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeDiameter (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeOffset (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputePlacement (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeCoincident (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeRound (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeOthers (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);
  
  Standard_EXPORT static void ComputeTextAndValue (const Handle(TDataXtd_Constraint)& aConst, Standard_Real& aValue, TCollection_ExtendedString& aText, const Standard_Boolean anIsAngle);
  
  Standard_EXPORT static void ComputeAngleForOneFace (const Handle(TDataXtd_Constraint)& aConst, Handle(AIS_InteractiveObject)& anAIS);




protected:





private:

  
  Standard_EXPORT static void GetOneShape (const Handle(TDataXtd_Constraint)& aConst, TopoDS_Shape& aShape);
  
  Standard_EXPORT static void GetGeom (const Handle(TDataXtd_Constraint)& aConst, Handle(Geom_Geometry)& aGeom);
  
  Standard_EXPORT static void GetTwoShapes (const Handle(TDataXtd_Constraint)& aConst, TopoDS_Shape& aShape1, TopoDS_Shape& aShape2);
  
  Standard_EXPORT static void GetShapesAndGeom (const Handle(TDataXtd_Constraint)& aConst, TopoDS_Shape& aShape1, TopoDS_Shape& aShape2, Handle(Geom_Geometry)& aGeom);
  
  Standard_EXPORT static void GetShapesAndGeom (const Handle(TDataXtd_Constraint)& aConst, TopoDS_Shape& aShape1, TopoDS_Shape& aShape2, TopoDS_Shape& aShape3, Handle(Geom_Geometry)& aGeom);
  
  Standard_EXPORT static void GetShapesAndGeom (const Handle(TDataXtd_Constraint)& aConst, TopoDS_Shape& aShape1, TopoDS_Shape& aShape2, TopoDS_Shape& aShape3, TopoDS_Shape& aShape4, Handle(Geom_Geometry)& aGeom);




};







#endif // _TPrsStd_ConstraintTools_HeaderFile
