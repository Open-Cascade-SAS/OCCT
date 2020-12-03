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
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <Standard_Integer.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <gp_Pnt.hxx>
#include <GeomInt_VectorOfReal.hxx>

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
    GeomInt_IntSS(const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real Tol, const Standard_Boolean Approx = Standard_True, const Standard_Boolean ApproxS1 = Standard_False, const Standard_Boolean ApproxS2 = Standard_False);
  
  //! general intersection of two surfaces
  Standard_EXPORT void Perform (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real Tol, const Standard_Boolean Approx = Standard_True, const Standard_Boolean ApproxS1 = Standard_False, const Standard_Boolean ApproxS2 = Standard_False);
  
  //! intersection of adapted surfaces
    void Perform (const Handle(GeomAdaptor_Surface)& HS1, const Handle(GeomAdaptor_Surface)& HS2, const Standard_Real Tol, const Standard_Boolean Approx = Standard_True, const Standard_Boolean ApproxS1 = Standard_False, const Standard_Boolean ApproxS2 = Standard_False);
  
  //! general intersection using a starting point
  Standard_EXPORT void Perform (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real Tol, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Boolean Approx = Standard_True, const Standard_Boolean ApproxS1 = Standard_False, const Standard_Boolean ApproxS2 = Standard_False);
  
  //! intersection of adapted surfaces using a starting point
    void Perform (const Handle(GeomAdaptor_Surface)& HS1, const Handle(GeomAdaptor_Surface)& HS2, const Standard_Real Tol, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Boolean Approx = Standard_True, const Standard_Boolean ApproxS1 = Standard_False, const Standard_Boolean ApproxS2 = Standard_False);
  
    Standard_Boolean IsDone() const;
  
    Standard_Real TolReached3d() const;
  
    Standard_Real TolReached2d() const;
  
    Standard_Integer NbLines() const;
  
  Standard_EXPORT const Handle(Geom_Curve)& Line (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Boolean HasLineOnS1 (const Standard_Integer Index) const;
  
  Standard_EXPORT const Handle(Geom2d_Curve)& LineOnS1 (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Boolean HasLineOnS2 (const Standard_Integer Index) const;
  
  Standard_EXPORT const Handle(Geom2d_Curve)& LineOnS2 (const Standard_Integer Index) const;
  
    Standard_Integer NbBoundaries() const;
  
  Standard_EXPORT const Handle(Geom_Curve)& Boundary (const Standard_Integer Index) const;
  
    Standard_Integer NbPoints() const;
  
    gp_Pnt Point (const Standard_Integer Index) const;
  
  Standard_EXPORT gp_Pnt2d Pnt2d (const Standard_Integer Index, const Standard_Boolean OnFirst) const;
  
  Standard_EXPORT void SetTolFixTangents (const Standard_Real aTolCheck, const Standard_Real aTolAngCheck);
  
  Standard_EXPORT void TolFixTangents (Standard_Real& aTolCheck, Standard_Real& aTolAngCheck);
  
  //! converts RLine to Geom(2d)_Curve.
  Standard_EXPORT static void TreatRLine (const Handle(IntPatch_RLine)& theRL, const Handle(GeomAdaptor_Surface)& theHS1, const Handle(GeomAdaptor_Surface)& theHS2, Handle(Geom_Curve)& theC3d, Handle(Geom2d_Curve)& theC2d1, Handle(Geom2d_Curve)& theC2d2, Standard_Real& theTolReached);
  
  //! creates 2D-curve on given surface from given 3D-curve
  Standard_EXPORT static void BuildPCurves (const Standard_Real f, const Standard_Real l, Standard_Real& Tol, const Handle(Geom_Surface)& S, const Handle(Geom_Curve)& C, Handle(Geom2d_Curve)& C2d);
  
  //! puts into theArrayOfParameters the parameters of intersection
  //! points of given theC2d1 and theC2d2 curves with the boundaries
  //! of the source surface.
  Standard_EXPORT static void TrimILineOnSurfBoundaries (const Handle(Geom2d_Curve)& theC2d1, const Handle(Geom2d_Curve)& theC2d2, const Bnd_Box2d& theBound1, const Bnd_Box2d& theBound2, GeomInt_VectorOfReal& theArrayOfParameters);

  Standard_EXPORT static Handle(Geom_Curve) MakeBSpline (const Handle(IntPatch_WLine)& WL, const Standard_Integer ideb, const Standard_Integer ifin);

  Standard_EXPORT static Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine, const Standard_Integer ideb, const Standard_Integer ifin, const Standard_Boolean onFirst);


protected:

  
  Standard_EXPORT void InternalPerform (const Standard_Real Tol, const Standard_Boolean Approx, const Standard_Boolean ApproxS1, const Standard_Boolean ApproxS2, const Standard_Boolean useStart, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  Standard_EXPORT void MakeCurve (const Standard_Integer Ind, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real Tol, const Standard_Boolean Approx, const Standard_Boolean Approx1, const Standard_Boolean Approx2);




private:



  IntPatch_Intersection myIntersector;
  GeomInt_LineConstructor myLConstruct;
  Handle(GeomAdaptor_Surface) myHS1;
  Handle(GeomAdaptor_Surface) myHS2;
  Standard_Integer myNbrestr;
  TColGeom_SequenceOfCurve sline;
  TColGeom2d_SequenceOfCurve slineS1;
  TColGeom2d_SequenceOfCurve slineS2;
  Standard_Real myTolReached2d;
  Standard_Real myTolReached3d;
  Standard_Real myTolCheck;
  Standard_Real myTolAngCheck;


};


#include <GeomInt_IntSS.lxx>





#endif // _GeomInt_IntSS_HeaderFile
