// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_IntSS_HeaderFile
#define _GeomInt_IntSS_HeaderFile

#include <IntPatch_Intersection.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <Standard_Integer.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Vector.hxx>

class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class gp_Pnt2d;
class IntPatch_RLine;
class Bnd_Box2d;
class Adaptor3d_TopolTool;
class IntPatch_WLine;

class GeomInt_IntSS
{
public:
  DEFINE_STANDARD_ALLOC

  GeomInt_IntSS();

  //! performs general intersection of two surfaces just now
  GeomInt_IntSS(const occ::handle<Geom_Surface>& S1,
                const occ::handle<Geom_Surface>& S2,
                const double         Tol,
                const bool      Approx   = true,
                const bool      ApproxS1 = false,
                const bool      ApproxS2 = false);

  //! general intersection of two surfaces
  Standard_EXPORT void Perform(const occ::handle<Geom_Surface>& S1,
                               const occ::handle<Geom_Surface>& S2,
                               const double         Tol,
                               const bool      Approx   = true,
                               const bool      ApproxS1 = false,
                               const bool      ApproxS2 = false);

  //! intersection of adapted surfaces
  void Perform(const occ::handle<GeomAdaptor_Surface>& HS1,
               const occ::handle<GeomAdaptor_Surface>& HS2,
               const double                Tol,
               const bool             Approx   = true,
               const bool             ApproxS1 = false,
               const bool             ApproxS2 = false);

  //! general intersection using a starting point
  Standard_EXPORT void Perform(const occ::handle<Geom_Surface>& S1,
                               const occ::handle<Geom_Surface>& S2,
                               const double         Tol,
                               const double         U1,
                               const double         V1,
                               const double         U2,
                               const double         V2,
                               const bool      Approx   = true,
                               const bool      ApproxS1 = false,
                               const bool      ApproxS2 = false);

  //! intersection of adapted surfaces using a starting point
  void Perform(const occ::handle<GeomAdaptor_Surface>& HS1,
               const occ::handle<GeomAdaptor_Surface>& HS2,
               const double                Tol,
               const double                U1,
               const double                V1,
               const double                U2,
               const double                V2,
               const bool             Approx   = true,
               const bool             ApproxS1 = false,
               const bool             ApproxS2 = false);

  bool IsDone() const;

  double TolReached3d() const;

  double TolReached2d() const;

  int NbLines() const;

  Standard_EXPORT const occ::handle<Geom_Curve>& Line(const int Index) const;

  Standard_EXPORT bool HasLineOnS1(const int Index) const;

  Standard_EXPORT const occ::handle<Geom2d_Curve>& LineOnS1(const int Index) const;

  Standard_EXPORT bool HasLineOnS2(const int Index) const;

  Standard_EXPORT const occ::handle<Geom2d_Curve>& LineOnS2(const int Index) const;

  int NbBoundaries() const;

  Standard_EXPORT const occ::handle<Geom_Curve>& Boundary(const int Index) const;

  int NbPoints() const;

  gp_Pnt Point(const int Index) const;

  Standard_EXPORT gp_Pnt2d Pnt2d(const int Index,
                                 const bool OnFirst) const;

  Standard_EXPORT void SetTolFixTangents(const double aTolCheck,
                                         const double aTolAngCheck);

  Standard_EXPORT void TolFixTangents(double& aTolCheck, double& aTolAngCheck);

  //! converts RLine to Geom(2d)_Curve.
  Standard_EXPORT static void TreatRLine(const occ::handle<IntPatch_RLine>&      theRL,
                                         const occ::handle<GeomAdaptor_Surface>& theHS1,
                                         const occ::handle<GeomAdaptor_Surface>& theHS2,
                                         occ::handle<Geom_Curve>&                theC3d,
                                         occ::handle<Geom2d_Curve>&              theC2d1,
                                         occ::handle<Geom2d_Curve>&              theC2d2,
                                         double&                     theTolReached);

  //! creates 2D-curve on given surface from given 3D-curve
  Standard_EXPORT static void BuildPCurves(const double         theFirst,
                                           const double         theLast,
                                           const double         theUmin,
                                           const double         theUmax,
                                           const double         theVmin,
                                           const double         theVmax,
                                           double&              theTol,
                                           const occ::handle<Geom_Surface>& theSurface,
                                           const occ::handle<Geom_Curve>&   theCurve,
                                           occ::handle<Geom2d_Curve>&       theCurve2d);

  //! creates 2D-curve on given surface from given 3D-curve
  Standard_EXPORT static void BuildPCurves(const double         f,
                                           const double         l,
                                           double&              Tol,
                                           const occ::handle<Geom_Surface>& S,
                                           const occ::handle<Geom_Curve>&   C,
                                           occ::handle<Geom2d_Curve>&       C2d);

  //! puts into theArrayOfParameters the parameters of intersection
  //! points of given theC2d1 and theC2d2 curves with the boundaries
  //! of the source surface.
  Standard_EXPORT static void TrimILineOnSurfBoundaries(const occ::handle<Geom2d_Curve>& theC2d1,
                                                        const occ::handle<Geom2d_Curve>& theC2d2,
                                                        const Bnd_Box2d&            theBound1,
                                                        const Bnd_Box2d&            theBound2,
                                                        NCollection_Vector<double>& theArrayOfParameters);

  Standard_EXPORT static occ::handle<Geom_Curve> MakeBSpline(const occ::handle<IntPatch_WLine>& WL,
                                                        const int        ideb,
                                                        const int        ifin);

  Standard_EXPORT static occ::handle<Geom2d_BSplineCurve> MakeBSpline2d(
    const occ::handle<IntPatch_WLine>& theWLine,
    const int        ideb,
    const int        ifin,
    const bool        onFirst);

protected:
  Standard_EXPORT void InternalPerform(const double    Tol,
                                       const bool Approx,
                                       const bool ApproxS1,
                                       const bool ApproxS2,
                                       const bool useStart,
                                       const double    U1,
                                       const double    V1,
                                       const double    U2,
                                       const double    V2);

  Standard_EXPORT void MakeCurve(const int             Ind,
                                 const occ::handle<Adaptor3d_TopolTool>& D1,
                                 const occ::handle<Adaptor3d_TopolTool>& D2,
                                 const double                Tol,
                                 const bool             Approx,
                                 const bool             Approx1,
                                 const bool             Approx2);

private:
  IntPatch_Intersection       myIntersector;
  GeomInt_LineConstructor     myLConstruct;
  occ::handle<GeomAdaptor_Surface> myHS1;
  occ::handle<GeomAdaptor_Surface> myHS2;
  int            myNbrestr;
  NCollection_Sequence<occ::handle<Geom_Curve>>    sline;
  NCollection_Sequence<occ::handle<Geom2d_Curve>>  slineS1;
  NCollection_Sequence<occ::handle<Geom2d_Curve>>  slineS2;
  double               myTolReached2d;
  double               myTolReached3d;
  double               myTolCheck;
  double               myTolAngCheck;
};

#include <GeomInt_IntSS.lxx>

#endif // _GeomInt_IntSS_HeaderFile
