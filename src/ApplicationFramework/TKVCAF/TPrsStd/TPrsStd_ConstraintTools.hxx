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

  //! Computes a distance dimension presentation for the given constraint.
  //! @param[in] aConst the distance constraint
  //! @return interactive object representing the distance, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeDistance(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeDistance() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeDistance() returning handle by value instead")
  Standard_EXPORT static void ComputeDistance(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a parallel relation presentation for the given constraint.
  //! @param[in] aConst the parallel constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeParallel(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeParallel() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeParallel() returning handle by value instead")
  Standard_EXPORT static void ComputeParallel(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a tangent relation presentation for the given constraint.
  //! @param[in] aConst the tangent constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeTangent(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeTangent() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeTangent() returning handle by value instead")
  Standard_EXPORT static void ComputeTangent(const occ::handle<TDataXtd_Constraint>& aConst,
                                             occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a perpendicular relation presentation for the given constraint.
  //! @param[in] aConst the perpendicular constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputePerpendicular(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputePerpendicular() returning handle by value instead.
  Standard_DEPRECATED("Use ComputePerpendicular() returning handle by value instead")
  Standard_EXPORT static void ComputePerpendicular(const occ::handle<TDataXtd_Constraint>& aConst,
                                                   occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a concentric relation presentation for the given constraint.
  //! @param[in] aConst the concentric constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeConcentric(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeConcentric() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeConcentric() returning handle by value instead")
  Standard_EXPORT static void ComputeConcentric(const occ::handle<TDataXtd_Constraint>& aConst,
                                                occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a symmetry relation presentation for the given constraint.
  //! @param[in] aConst the symmetry constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeSymmetry(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeSymmetry() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeSymmetry() returning handle by value instead")
  Standard_EXPORT static void ComputeSymmetry(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a midpoint relation presentation for the given constraint.
  //! @param[in] aConst the midpoint constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeMidPoint(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeMidPoint() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeMidPoint() returning handle by value instead")
  Standard_EXPORT static void ComputeMidPoint(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes an angle dimension presentation for the given constraint.
  //! @param[in] aConst the angle constraint
  //! @return interactive object representing the angle, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeAngle(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeAngle() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeAngle() returning handle by value instead")
  Standard_EXPORT static void ComputeAngle(const occ::handle<TDataXtd_Constraint>& aConst,
                                           occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a radius dimension presentation for the given constraint.
  //! @param[in] aConst the radius constraint
  //! @return interactive object representing the radius, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeRadius(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeRadius() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeRadius() returning handle by value instead")
  Standard_EXPORT static void ComputeRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a minor radius dimension presentation for the given constraint.
  //! @param[in] aConst the minor radius constraint
  //! @return interactive object representing the minor radius, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeMinRadius(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeMinRadius() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeMinRadius() returning handle by value instead")
  Standard_EXPORT static void ComputeMinRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a major radius dimension presentation for the given constraint.
  //! @param[in] aConst the major radius constraint
  //! @return interactive object representing the major radius, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeMaxRadius(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeMaxRadius() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeMaxRadius() returning handle by value instead")
  Standard_EXPORT static void ComputeMaxRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes an equal distance relation presentation for the given constraint.
  //! @param[in] aConst the equal distance constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeEqualDistance(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeEqualDistance() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeEqualDistance() returning handle by value instead")
  Standard_EXPORT static void ComputeEqualDistance(const occ::handle<TDataXtd_Constraint>& aConst,
                                                   occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes an equal radius relation presentation for the given constraint.
  //! @param[in] aConst the equal radius constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeEqualRadius(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeEqualRadius() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeEqualRadius() returning handle by value instead")
  Standard_EXPORT static void ComputeEqualRadius(const occ::handle<TDataXtd_Constraint>& aConst,
                                                 occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a fix constraint presentation for the given constraint.
  //! @param[in] aConst the fix constraint
  //! @return interactive object representing the constraint, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeFix(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeFix() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeFix() returning handle by value instead")
  Standard_EXPORT static void ComputeFix(const occ::handle<TDataXtd_Constraint>& aConst,
                                         occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a diameter dimension presentation for the given constraint.
  //! @param[in] aConst the diameter constraint
  //! @return interactive object representing the diameter, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeDiameter(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeDiameter() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeDiameter() returning handle by value instead")
  Standard_EXPORT static void ComputeDiameter(const occ::handle<TDataXtd_Constraint>& aConst,
                                              occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes an offset relation presentation for the given constraint.
  //! @param[in] aConst the offset constraint
  //! @return interactive object representing the offset, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeOffset(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeOffset() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeOffset() returning handle by value instead")
  Standard_EXPORT static void ComputeOffset(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a placement relation presentation for the given constraint.
  //! @param[in] aConst the placement constraint
  //! @return interactive object representing the placement, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputePlacement(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputePlacement() returning handle by value instead.
  Standard_DEPRECATED("Use ComputePlacement() returning handle by value instead")
  Standard_EXPORT static void ComputePlacement(const occ::handle<TDataXtd_Constraint>& aConst,
                                               occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a coincident relation presentation for the given constraint.
  //! @param[in] aConst the coincident constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeCoincident(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeCoincident() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeCoincident() returning handle by value instead")
  Standard_EXPORT static void ComputeCoincident(const occ::handle<TDataXtd_Constraint>& aConst,
                                                occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a round (fillet) relation presentation for the given constraint.
  //! @param[in] aConst the round constraint
  //! @return interactive object representing the relation, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeRound(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeRound() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeRound() returning handle by value instead")
  Standard_EXPORT static void ComputeRound(const occ::handle<TDataXtd_Constraint>& aConst,
                                           occ::handle<AIS_InteractiveObject>&     anAIS);

  //! Computes a presentation for constraint types not handled by specific methods.
  //! @param[in] aConst the constraint
  //! @return interactive object representing the constraint, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeOthers(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeOthers() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeOthers() returning handle by value instead")
  Standard_EXPORT static void ComputeOthers(const occ::handle<TDataXtd_Constraint>& aConst,
                                            occ::handle<AIS_InteractiveObject>&     anAIS);

  Standard_EXPORT static void ComputeTextAndValue(const occ::handle<TDataXtd_Constraint>& aConst,
                                                  double&                                 aValue,
                                                  TCollection_ExtendedString&             aText,
                                                  const bool anIsAngle);

  //! Computes an angle dimension presentation for a single-face constraint.
  //! @param[in] aConst the angle constraint on one face
  //! @return interactive object representing the angle, or null handle on failure
  [[nodiscard]] Standard_EXPORT static occ::handle<AIS_InteractiveObject> ComputeAngleForOneFace(
    const occ::handle<TDataXtd_Constraint>& aConst);

  //! @deprecated Use ComputeAngleForOneFace() returning handle by value instead.
  Standard_DEPRECATED("Use ComputeAngleForOneFace() returning handle by value instead")
  Standard_EXPORT static void ComputeAngleForOneFace(const occ::handle<TDataXtd_Constraint>& aConst,
                                                     occ::handle<AIS_InteractiveObject>&     anAIS);

private:
  Standard_EXPORT static void GetOneShape(const occ::handle<TDataXtd_Constraint>& aConst,
                                          TopoDS_Shape&                           aShape);

  Standard_EXPORT static void GetGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                      occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetTwoShapes(const occ::handle<TDataXtd_Constraint>& aConst,
                                           TopoDS_Shape&                           aShape1,
                                           TopoDS_Shape&                           aShape2);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                           aShape1,
                                               TopoDS_Shape&                           aShape2,
                                               occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                           aShape1,
                                               TopoDS_Shape&                           aShape2,
                                               TopoDS_Shape&                           aShape3,
                                               occ::handle<Geom_Geometry>&             aGeom);

  Standard_EXPORT static void GetShapesAndGeom(const occ::handle<TDataXtd_Constraint>& aConst,
                                               TopoDS_Shape&                           aShape1,
                                               TopoDS_Shape&                           aShape2,
                                               TopoDS_Shape&                           aShape3,
                                               TopoDS_Shape&                           aShape4,
                                               occ::handle<Geom_Geometry>&             aGeom);
};

#endif // _TPrsStd_ConstraintTools_HeaderFile
