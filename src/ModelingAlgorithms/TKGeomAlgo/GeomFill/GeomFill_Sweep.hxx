// Created on: 1997-11-20
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

#ifndef _GeomFill_Sweep_HeaderFile
#define _GeomFill_Sweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <GeomFill_ApproxStyle.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class GeomFill_LocationLaw;
class GeomFill_SectionLaw;
class Geom_Surface;
class Geom2d_Curve;

//! Geometrical Sweep Algorithm
class GeomFill_Sweep
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_Sweep(const occ::handle<GeomFill_LocationLaw>& Location,
                                 const bool              WithKpart = true);

  //! Set parametric information
  //! [<First>, <Last>] Sets the parametric bound of the
  //! sweeping surface to build.
  //! <SectionFirst>, <SectionLast> gives corresponding
  //! bounds parameter on the section law of <First> and <Last>
  //!
  //! V-Iso on Sweeping Surface S(u,v) is defined by
  //! Location(v) and Section(w) where
  //! w = SectionFirst + (v - First) / (Last-First)
  //! * (SectionLast - SectionFirst)
  //!
  //! By default w = v, and First and Last are given by
  //! First and Last parameter stored in LocationLaw.
  Standard_EXPORT void SetDomain(const double First,
                                 const double Last,
                                 const double SectionFirst,
                                 const double SectionLast);

  //! Set Approximation Tolerance
  //! Tol3d : Tolerance to surface approximation
  //! Tol2d : Tolerance used to perform curve approximation
  //! Normally the 2d curve are approximated with a
  //! tolerance given by the resolution method define in
  //! <LocationLaw> but if this tolerance is too large Tol2d
  //! is used.
  //! TolAngular : Tolerance (in radian) to control the angle
  //! between tangents on the section law and
  //! tangent of iso-v on approximated surface
  Standard_EXPORT void SetTolerance(const double Tol3d,
                                    const double BoundTol   = 1.0,
                                    const double Tol2d      = 1.0e-5,
                                    const double TolAngular = 1.0);

  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  Standard_EXPORT void SetForceApproxC1(const bool ForceApproxC1);

  //! returns true if sections are U-Iso
  //! This can be produce in some cases when <WithKpart> is True.
  Standard_EXPORT bool ExchangeUV() const;

  //! returns true if Parametrisation sens in U is inverse of
  //! parametrisation sens of section (or of path if ExchangeUV)
  Standard_EXPORT bool UReversed() const;

  //! returns true if Parametrisation sens in V is inverse of
  //! parametrisation sens of path (or of section if ExchangeUV)
  Standard_EXPORT bool VReversed() const;

  //! Build the Sweeep Surface
  //! ApproxStyle defines Approximation Strategy
  //! - GeomFill_Section : The composed Function : Location X Section
  //! is directly approximated.
  //! - GeomFill_Location : The location law is approximated, and the
  //! SweepSurface is build algebric composition
  //! of approximated location law and section law
  //! This option is Ok, if Section.Surface() methode
  //! is effective.
  //! Continuity : The continuity in v waiting on the surface
  //! Degmax     : The maximum degree in v required on the surface
  //! Segmax     : The maximum number of span in v required on
  //! the surface
  //!
  //! raise If Domain are infinite or Profile not set.
  Standard_EXPORT void Build(const occ::handle<GeomFill_SectionLaw>& Section,
                             const GeomFill_ApproxStyle         Methode    = GeomFill_Location,
                             const GeomAbs_Shape                Continuity = GeomAbs_C2,
                             const int             Degmax     = 10,
                             const int             Segmax     = 30);

  //! Tells if the Surface is Built.
  Standard_EXPORT bool IsDone() const;

  //! Gets the Approximation error.
  Standard_EXPORT double ErrorOnSurface() const;

  //! Gets the Approximation error.
  Standard_EXPORT void ErrorOnRestriction(const bool IsFirst,
                                          double&         UError,
                                          double&         VError) const;

  //! Gets the Approximation error.
  Standard_EXPORT void ErrorOnTrace(const int IndexOfTrace,
                                    double&         UError,
                                    double&         VError) const;

  Standard_EXPORT occ::handle<Geom_Surface> Surface() const;

  Standard_EXPORT occ::handle<Geom2d_Curve> Restriction(const bool IsFirst) const;

  Standard_EXPORT int NumberOfTrace() const;

  Standard_EXPORT occ::handle<Geom2d_Curve> Trace(const int IndexOfTrace) const;

private:
  Standard_EXPORT bool Build2d(const GeomAbs_Shape    Continuity,
                                           const int Degmax,
                                           const int Segmax);

  Standard_EXPORT bool BuildAll(const GeomAbs_Shape    Continuity,
                                            const int Degmax,
                                            const int Segmax);

  Standard_EXPORT bool BuildProduct(const GeomAbs_Shape    Continuity,
                                                const int Degmax,
                                                const int Segmax);

  Standard_EXPORT bool BuildKPart();

  double                     First;
  double                     Last;
  double                     SFirst;
  double                     SLast;
  double                     Tol3d;
  double                     BoundTol;
  double                     Tol2d;
  double                     TolAngular;
  double                     SError;
  bool                  myForceApproxC1;
  occ::handle<GeomFill_LocationLaw>      myLoc;
  occ::handle<GeomFill_SectionLaw>       mySec;
  occ::handle<Geom_Surface>              mySurface;
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_Curve>>> myCurve2d;
  occ::handle<NCollection_HArray2<double>>     CError;
  bool                  done;
  bool                  myExchUV;
  bool                  isUReversed;
  bool                  isVReversed;
  bool                  myKPart;
};

#endif // _GeomFill_Sweep_HeaderFile
