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

#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColStd_HArray2OfReal.hxx>
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

  Standard_EXPORT GeomFill_Sweep(const Handle(GeomFill_LocationLaw)& Location,
                                 const Standard_Boolean              WithKpart = Standard_True);

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
  Standard_EXPORT void SetDomain(const Standard_Real First,
                                 const Standard_Real Last,
                                 const Standard_Real SectionFirst,
                                 const Standard_Real SectionLast);

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
  Standard_EXPORT void SetTolerance(const Standard_Real Tol3d,
                                    const Standard_Real BoundTol   = 1.0,
                                    const Standard_Real Tol2d      = 1.0e-5,
                                    const Standard_Real TolAngular = 1.0);

  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  Standard_EXPORT void SetForceApproxC1(const Standard_Boolean ForceApproxC1);

  //! returns true if sections are U-Iso
  //! This can be produce in some cases when <WithKpart> is True.
  Standard_EXPORT Standard_Boolean ExchangeUV() const;

  //! returns true if Parametrisation sens in U is inverse of
  //! parametrisation sens of section (or of path if ExchangeUV)
  Standard_EXPORT Standard_Boolean UReversed() const;

  //! returns true if Parametrisation sens in V is inverse of
  //! parametrisation sens of path (or of section if ExchangeUV)
  Standard_EXPORT Standard_Boolean VReversed() const;

  //! Build the Sweeep  Surface
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
  Standard_EXPORT void Build(const Handle(GeomFill_SectionLaw)& Section,
                             const GeomFill_ApproxStyle         Methode    = GeomFill_Location,
                             const GeomAbs_Shape                Continuity = GeomAbs_C2,
                             const Standard_Integer             Degmax     = 10,
                             const Standard_Integer             Segmax     = 30);

  //! Tells if the Surface is Buildt.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Gets the Approximation  error.
  Standard_EXPORT Standard_Real ErrorOnSurface() const;

  //! Gets the Approximation  error.
  Standard_EXPORT void ErrorOnRestriction(const Standard_Boolean IsFirst,
                                          Standard_Real&         UError,
                                          Standard_Real&         VError) const;

  //! Gets the Approximation error.
  Standard_EXPORT void ErrorOnTrace(const Standard_Integer IndexOfTrace,
                                    Standard_Real&         UError,
                                    Standard_Real&         VError) const;

  Standard_EXPORT Handle(Geom_Surface) Surface() const;

  Standard_EXPORT Handle(Geom2d_Curve) Restriction(const Standard_Boolean IsFirst) const;

  Standard_EXPORT Standard_Integer NumberOfTrace() const;

  Standard_EXPORT Handle(Geom2d_Curve) Trace(const Standard_Integer IndexOfTrace) const;

protected:
private:
  Standard_EXPORT Standard_Boolean Build2d(const GeomAbs_Shape    Continuity,
                                           const Standard_Integer Degmax,
                                           const Standard_Integer Segmax);

  Standard_EXPORT Standard_Boolean BuildAll(const GeomAbs_Shape    Continuity,
                                            const Standard_Integer Degmax,
                                            const Standard_Integer Segmax);

  Standard_EXPORT Standard_Boolean BuildProduct(const GeomAbs_Shape    Continuity,
                                                const Standard_Integer Degmax,
                                                const Standard_Integer Segmax);

  Standard_EXPORT Standard_Boolean BuildKPart();

  Standard_Real                     First;
  Standard_Real                     Last;
  Standard_Real                     SFirst;
  Standard_Real                     SLast;
  Standard_Real                     Tol3d;
  Standard_Real                     BoundTol;
  Standard_Real                     Tol2d;
  Standard_Real                     TolAngular;
  Standard_Real                     SError;
  Standard_Boolean                  myForceApproxC1;
  Handle(GeomFill_LocationLaw)      myLoc;
  Handle(GeomFill_SectionLaw)       mySec;
  Handle(Geom_Surface)              mySurface;
  Handle(TColGeom2d_HArray1OfCurve) myCurve2d;
  Handle(TColStd_HArray2OfReal)     CError;
  Standard_Boolean                  done;
  Standard_Boolean                  myExchUV;
  Standard_Boolean                  isUReversed;
  Standard_Boolean                  isVReversed;
  Standard_Boolean                  myKPart;
};

#endif // _GeomFill_Sweep_HeaderFile
