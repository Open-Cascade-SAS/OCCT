// Created on: 1997-12-15
// Created by: Philippe MANGIN
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

#ifndef _GeomFill_SectionPlacement_HeaderFile
#define _GeomFill_SectionPlacement_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax1.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Extrema_ExtPC.hxx>
#include <gp_Pnt.hxx>
class GeomFill_LocationLaw;
class Geom_Curve;
class Geom_Geometry;
class gp_Trsf;
class gp_Mat;
class gp_Vec;

//! To place section in sweep Function
class GeomFill_SectionPlacement
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_SectionPlacement(const occ::handle<GeomFill_LocationLaw>& L,
                                            const occ::handle<Geom_Geometry>&        Section);

  //! To change the section Law
  Standard_EXPORT void SetLocation(const occ::handle<GeomFill_LocationLaw>& L);

  Standard_EXPORT void Perform(const double Tol);

  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>& Path, const double Tol);

  Standard_EXPORT void Perform(const double ParamOnPath, const double Tol);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT double ParameterOnPath() const;

  Standard_EXPORT double ParameterOnSection() const;

  Standard_EXPORT double Distance() const;

  Standard_EXPORT double Angle() const;

  Standard_EXPORT gp_Trsf
    Transformation(const bool WithTranslation,
                   const bool WithCorrection = false) const;

  //! Compute the Section, in the coordinate system given by
  //! the Location Law.
  //! If <WithTranslation> contact between
  //! <Section> and <Path> is forced.
  Standard_EXPORT occ::handle<Geom_Curve> Section(const bool WithTranslation) const;

  //! Compute the Section, in the coordinate system given by
  //! the Location Law.
  //! To have the Normal to section equal to the Location
  //! Law Normal. If <WithTranslation> contact between
  //! <Section> and <Path> is forced.
  Standard_EXPORT occ::handle<Geom_Curve> ModifiedSection(const bool WithTranslation) const;

private:
  Standard_EXPORT void SectionAxis(const gp_Mat& M, gp_Vec& T, gp_Vec& N, gp_Vec& BN) const;

  Standard_EXPORT bool Choix(const double Dist, const double Angle) const;

  bool             done;
  bool             isplan;
  gp_Ax1                       TheAxe;
  double                Gabarit;
  occ::handle<GeomFill_LocationLaw> myLaw;
  GeomAdaptor_Curve            myAdpSection;
  occ::handle<Geom_Curve>           mySection;
  double                SecParam;
  double                PathParam;
  double                Dist;
  double                AngleMax;
  Extrema_ExtPC                myExt;
  bool             myIsPoint;
  gp_Pnt                       myPoint;
};

#endif // _GeomFill_SectionPlacement_HeaderFile
