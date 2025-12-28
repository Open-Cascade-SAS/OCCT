// Created on: 1994-02-28
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

#ifndef _GeomFill_SweepSectionGenerator_HeaderFile
#define _GeomFill_SweepSectionGenerator_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

class Geom_BSplineCurve;
class Geom_Curve;
class gp_Trsf;

//! class for instantiation of AppBlend.
//! evaluate the sections of a sweep surface.
class GeomFill_SweepSectionGenerator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_SweepSectionGenerator();

  //! Create a sweept surface with a constant radius.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const occ::handle<Geom_Curve>& Path,
                                                 const double                   Radius);

  //! Create a sweept surface with a constant section
  Standard_EXPORT GeomFill_SweepSectionGenerator(const occ::handle<Geom_Curve>& Path,
                                                 const occ::handle<Geom_Curve>& FirstSect);

  //! Create a sweept surface with an evolving section
  //! The section evaluate from First to Last Section
  Standard_EXPORT GeomFill_SweepSectionGenerator(const occ::handle<Geom_Curve>& Path,
                                                 const occ::handle<Geom_Curve>& FirstSect,
                                                 const occ::handle<Geom_Curve>& LastSect);

  //! Create a pipe with a constant radius with 2
  //! guide-line.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const occ::handle<Geom_Curve>& Path,
                                                 const occ::handle<Geom_Curve>& Curve1,
                                                 const occ::handle<Geom_Curve>& Curve2,
                                                 const double                   Radius);

  //! Create a pipe with a constant radius with 2
  //! guide-line.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const occ::handle<Adaptor3d_Curve>& Path,
                                                 const occ::handle<Adaptor3d_Curve>& Curve1,
                                                 const occ::handle<Adaptor3d_Curve>& Curve2,
                                                 const double                        Radius);

  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& Path, const double Radius);

  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& Path,
                            const occ::handle<Geom_Curve>& FirstSect);

  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& Path,
                            const occ::handle<Geom_Curve>& FirstSect,
                            const occ::handle<Geom_Curve>& LastSect);

  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& Path,
                            const occ::handle<Geom_Curve>& Curve1,
                            const occ::handle<Geom_Curve>& Curve2,
                            const double                   Radius);

  Standard_EXPORT void Init(const occ::handle<Adaptor3d_Curve>& Path,
                            const occ::handle<Adaptor3d_Curve>& Curve1,
                            const occ::handle<Adaptor3d_Curve>& Curve2,
                            const double                        Radius);

  Standard_EXPORT void Perform(const bool Polynomial = false);

  Standard_EXPORT void GetShape(int& NbPoles, int& NbKnots, int& Degree, int& NbPoles2d) const;

  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) const;

  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) const;

  int NbSections() const;

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

  Standard_EXPORT void Section(const int                     P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) const;

  //! raised if <Index> not in the range [1,NbSections()]
  Standard_EXPORT const gp_Trsf& Transformation(const int Index) const;

  //! Returns the parameter of <P>, to impose it for the
  //! approximation.
  Standard_EXPORT double Parameter(const int P) const;

private:
  occ::handle<Geom_BSplineCurve> myPath;
  occ::handle<Geom_BSplineCurve> myFirstSect;
  occ::handle<Geom_BSplineCurve> myLastSect;
  occ::handle<Adaptor3d_Curve>   myAdpPath;
  occ::handle<Adaptor3d_Curve>   myAdpFirstSect;
  occ::handle<Adaptor3d_Curve>   myAdpLastSect;
  gp_Ax1                         myCircPathAxis;
  double                         myRadius;
  bool                           myIsDone;
  int                            myNbSections;
  NCollection_Sequence<gp_Trsf>  myTrsfs;
  int                            myType;
  bool                           myPolynomial;
};

#include <GeomFill_SweepSectionGenerator.lxx>

#endif // _GeomFill_SweepSectionGenerator_HeaderFile
