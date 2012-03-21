// Created on: 1996-11-26
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepBlend_AppSurface.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <StdFail_NotDone.hxx>


BRepBlend_AppSurface::BRepBlend_AppSurface(
			       Handle(Approx_SweepFunction)& Func,
			       const Standard_Real First, 
			       const Standard_Real Last,
			       const Standard_Real Tol3d, 
			       const Standard_Real Tol2d,
			       const Standard_Real TolAngular,
			       const GeomAbs_Shape Continuity,
			       const Standard_Integer Degmax, 
			       const Standard_Integer Segmax) :
			       approx(Func)
{
   Standard_Integer Nb2d = Func->Nb2dCurves();
   Standard_Integer NbPolSect, NbKnotSect, udeg;
   GeomAbs_Shape   continuity = Continuity;

// (1) Verification de la possibilite de derivation
  if (continuity != GeomAbs_C0) {
    if (Nb2d == 0) Nb2d =1;
    Func->SectionShape(NbPolSect, NbKnotSect, udeg);
    TColStd_Array1OfReal W  (1, NbPolSect);
    TColgp_Array1OfPnt   P  (1, NbPolSect);
    TColgp_Array1OfPnt2d P2d(1, Nb2d);
    TColgp_Array1OfVec   V  (1, NbPolSect);
    TColgp_Array1OfVec2d V2d(1, Nb2d);
    Standard_Boolean Ok;
    if (continuity == GeomAbs_C2) {
        Ok = Func->D2( First, First, Last, P, V, V, P2d, V2d, V2d, W, W, W);
	if (!Ok) { continuity = GeomAbs_C1; }
      }
    if (continuity == GeomAbs_C1) {
      Ok = (Func->D1(First, First, Last, P, V, P2d, V2d, W, W));
      if (!Ok) { continuity = GeomAbs_C0; }
    }
  }
  

// (2) Approximation
   approx.Perform(First, Last, 
		  Tol3d,  Tol3d,  Tol2d,  TolAngular, 
		  continuity, Degmax, Segmax);	  
 }

void BRepBlend_AppSurface::SurfShape (Standard_Integer& UDegree,
				  Standard_Integer& VDegree,
				  Standard_Integer& NbUPoles,
				  Standard_Integer& NbVPoles,
				  Standard_Integer& NbUKnots,
				  Standard_Integer& NbVKnots) const
{
  approx.SurfShape(UDegree, VDegree, 
		   NbUPoles, NbVPoles, 
		   NbUKnots,NbVKnots); 
}


void BRepBlend_AppSurface::Surface(TColgp_Array2OfPnt& TPoles,
			       TColStd_Array2OfReal& TWeights,
			       TColStd_Array1OfReal& TUKnots,
			       TColStd_Array1OfReal& TVKnots,
			       TColStd_Array1OfInteger& TUMults,
			       TColStd_Array1OfInteger& TVMults) const

{
  approx.Surface(TPoles, TWeights, TUKnots,TVKnots, TUMults,TVMults);
}


Standard_Real BRepBlend_AppSurface::MaxErrorOnSurf() const 
{
  return approx.MaxErrorOnSurf();
}


void BRepBlend_AppSurface::Curves2dShape(Standard_Integer& Degree,
				   Standard_Integer& NbPoles,
				   Standard_Integer& NbKnots) const
{
  approx.Curves2dShape( Degree, NbPoles, NbKnots);
}

void BRepBlend_AppSurface::Curve2d(const Standard_Integer Index,
			       TColgp_Array1OfPnt2d& TPoles,
			       TColStd_Array1OfReal& TKnots,
			       TColStd_Array1OfInteger& TMults) const
{
  approx.Curve2d(Index, TPoles, TKnots, TMults);
}

Standard_Real BRepBlend_AppSurface::Max2dError(const Standard_Integer Index) const 
{
  return approx.Max2dError(Index);
}


Standard_Real BRepBlend_AppSurface::TolCurveOnSurf(const Standard_Integer Index) const 
{
  return approx.TolCurveOnSurf(Index);
}

inline void  BRepBlend_AppSurface::TolReached (Standard_Real& Tol3d,
					 Standard_Real& Tol2d) const
{
  Tol3d = approx.MaxErrorOnSurf();
  Tol2d = 0;
  for (Standard_Integer ii=1; ii<=approx.NbCurves2d(); ii++) {
    Tol2d = Max(Tol2d, approx.Max2dError(ii));
  }
}

void BRepBlend_AppSurface::Dump(Standard_OStream& o) const 
{
  approx.Dump(o);
}
