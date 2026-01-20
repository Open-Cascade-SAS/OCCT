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

  Standard_EXPORT static void UpdateOnlyValue(const occ::handle<TDataXtd_Constraint>&   aConst,
                                              const occ::handle<AIS_InteractiveObject>& anAIS);

  Standard_EXPORT static void ComputeDistance(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeParallel(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeTangent(const occ::handle<TDataXtd_Constraint>& aConst,
                                             occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputePerpendicular(const occ::handle<TDataXtd_Constraint>& aConst,
                                                   occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeConcentric(const occ::handle<TDataXtd_Constraint>& aConst,
                                                occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeSymmetry(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeMidPoint(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeAngle(const occ::handle<TDataXtd_Constraint>& aConst,
                                           occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeMinRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeMaxRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeEqualDistance(const occ::handle<TDataXtd_Constraint>& aConst,
                                                   occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeEqualRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                                 occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeFix(const occ::handle<TDataXtd_Constraint>& aConst,
                                         occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeDiameter(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeOffset(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputePlacement(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeCoincident(const occ::handle<TDataXtd_Constraint>& aConst,
                                                occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeRound(const occ::handle<TDataXtd_Constraint>& aConst,
                                           occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeOthers(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeTextAndValue(const occ::handle<TDataXtd_Constraint>& aConst,
                                                  double&                     aValue,
                                                  TCollection_ExtendedString&        aText,
                                                  const bool             anIsAngle);

  Standard_EXPORT static void ComputeAngleForOneFace(const occ::handle<TDataXtd_Constraint>& aConst,
                                                     occ::handle<AIS_InteractiveObject>&     anAIS);

private:
  Standard_EXPORT static void GetOneShape(const occ::handle<TDataXtd_Constraint>& aConst,
                                          TopoDS_Shape&                      aShape);

  Standard_EXPORT static void GetGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                      occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetTwoShapes(const occ::handle<TDataXtd_Constraint>& aConst,
                                           TopoDS_Shape&                      aShape1,
                                           TopoDS_Shape&                      aShape2);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                      aShape1,
                                               TopoDS_Shape&                      aShape2,
                                               occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                      aShape1,
                                               TopoDS_Shape&                      aShape2,
                                               TopoDS_Shape&                      aShape3,
                                               occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                      aShape1,
                                               TopoDS_Shape&                      aShape2,
                                               TopoDS_Shape&                      aShape3,
                                               TopoDS_Shape&                      aShape4,
                                               occ::handle<Geom_Geometry>&             aGeom);
};

#endif // _TPrsStd_ConstraintTools_HeaderFile
