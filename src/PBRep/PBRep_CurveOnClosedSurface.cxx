// File:	PBRep_CurveOnClosedSurface.cxx
// Created:	Mon Jul 26 10:28:54 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_CurveOnClosedSurface.ixx>


//=======================================================================
//function : PBRep_CurveOnClosedSurface
//purpose  : 
//=======================================================================

PBRep_CurveOnClosedSurface::PBRep_CurveOnClosedSurface
  (const Handle(PGeom2d_Curve)& PC1,
   const Handle(PGeom2d_Curve)& PC2,
   const Standard_Real          CF,
   const Standard_Real          CL,
   const Handle(PGeom_Surface)& S,
   const PTopLoc_Location& L,
   const GeomAbs_Shape C) :
  PBRep_CurveOnSurface(PC1, CF, CL, S, L),
  myPCurve2(PC2),
  myContinuity(C)
{
}


//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PBRep_CurveOnClosedSurface::PCurve2()const 
{
  return myPCurve2;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape  PBRep_CurveOnClosedSurface::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnClosedSurface::IsCurveOnClosedSurface()const 
{
  return Standard_True;
}

//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnClosedSurface::IsRegularity()const 
{
  return Standard_True;
}

//=======================================================================
//function : SetUVPoints2
//purpose  : 
//=======================================================================

void PBRep_CurveOnClosedSurface::SetUVPoints2(const gp_Pnt2d& Pnt1,
					      const gp_Pnt2d& Pnt2) 
{
  myUV21 = Pnt1;
  myUV22 = Pnt2;
}

//=======================================================================
//function : FirstUV2
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnClosedSurface::FirstUV2() const 
{
  return myUV21;
}

//=======================================================================
//function : LastUV2
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnClosedSurface::LastUV2() const 
{
  return myUV22;
}

