// Created on: 1994-02-18
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_SectionGenerator_HeaderFile
#define _GeomFill_SectionGenerator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_Profiler.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

class Geom_Curve;

//! gives the functions needed for instantiation from
//! AppSurf in AppBlend. Allow to evaluate a surface
//! passing by all the curves if the Profiler.
class GeomFill_SectionGenerator : public GeomFill_Profiler
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_SectionGenerator();

  //! Adds a profile curve. All profile curves must be added before endpoint constraints.
  Standard_EXPORT void AddCurve(const occ::handle<Geom_Curve>& theCurve);

  //! Converts all profile and auxiliary curves to a common B-spline representation.
  //! Raises Standard_ConstructionError if profile curves were added through the
  //! base-class interface after endpoint constraints.
  Standard_EXPORT void Perform(const double thePTol) override;

  Standard_EXPORT void SetParam(const occ::handle<NCollection_HArray1<double>>& Params);

  //! Defines a constant derivative direction at the first section.
  //! The magnitude is preserved relative to the other section pole derivatives;
  //! the surface approximation may apply a common scale factor to the complete
  //! derivative constraint.
  Standard_EXPORT void SetFirstTangent(const gp_Vec& theTangent);

  //! Defines a constant derivative direction at the last section.
  Standard_EXPORT void SetLastTangent(const gp_Vec& theTangent);

  //! Defines a curve whose difference from the first section represents the
  //! desired derivative field. This method must be called after all section
  //! curves have been added and before Perform().
  Standard_EXPORT void SetFirstTangentSection(const occ::handle<Geom_Curve>& theTangentSection);

  //! Defines a curve whose difference from the last section represents the
  //! desired derivative field. This method must be called after all section
  //! curves have been added and before Perform().
  Standard_EXPORT void SetLastTangentSection(const occ::handle<Geom_Curve>& theTangentSection);

  //! Defines a constant second derivative at the first section. A null vector
  //! represents a straight (zero-curvature) continuation.
  Standard_EXPORT void SetFirstCurvature(const gp_Vec& theCurvature);

  //! Defines a constant second derivative at the last section.
  Standard_EXPORT void SetLastCurvature(const gp_Vec& theCurvature);

  //! Defines a curve whose second finite difference from the first section and
  //! its tangent section represents the desired second derivative field. This
  //! method must be called after SetFirstTangentSection().
  Standard_EXPORT void SetFirstCurvatureSection(const occ::handle<Geom_Curve>& theCurvatureSection);

  //! Defines a curve whose second finite difference from the last section and
  //! its tangent section represents the desired second derivative field. This
  //! method must be called after SetLastTangentSection().
  Standard_EXPORT void SetLastCurvatureSection(const occ::handle<Geom_Curve>& theCurvatureSection);

  Standard_EXPORT void GetShape(int& NbPoles, int& NbKnots, int& Degree, int& NbPoles2d) const;

  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) const;

  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) const;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT bool Section(const int                     P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths) const;

  //! Returns first and second derivatives for a G2-constrained endpoint.
  //! Returns false when no curvature constraint is defined at P.
  Standard_EXPORT bool SectionWithCurvature(const int                     P,
                                            NCollection_Array1<gp_Pnt>&   Poles,
                                            NCollection_Array1<gp_Vec>&   DPoles,
                                            NCollection_Array1<gp_Vec>&   D2Poles,
                                            NCollection_Array1<gp_Pnt2d>& Poles2d,
                                            NCollection_Array1<gp_Vec2d>& DPoles2d,
                                            NCollection_Array1<gp_Vec2d>& D2Poles2d,
                                            NCollection_Array1<double>&   Weigths,
                                            NCollection_Array1<double>&   DWeigths,
                                            NCollection_Array1<double>&   D2Weigths) const;

  Standard_EXPORT void Section(const int                     P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) const;

  //! Returns the parameter of Section<P>, to impose it for the
  //! approximation.
  Standard_EXPORT double Parameter(const int P) const;

private:
  void BeginConstraintConfiguration();
  void RemoveAuxiliaryCurve(int& theIndex);
  void ResetFirstCurvature();
  void ResetLastCurvature();

protected:
  occ::handle<NCollection_HArray1<double>> myParams;
  gp_Vec                                   myFirstTangent;
  gp_Vec                                   myLastTangent;
  gp_Vec                                   myFirstCurvature;
  gp_Vec                                   myLastCurvature;
  int                                      myNbSections;
  int                                      myFirstTangentSection;
  int                                      myLastTangentSection;
  int                                      myFirstCurvatureSection;
  int                                      myLastCurvatureSection;
  bool                                     myHasFirstTangent;
  bool                                     myHasLastTangent;
  bool                                     myHasFirstCurvature;
  bool                                     myHasLastCurvature;
};

#endif // _GeomFill_SectionGenerator_HeaderFile
