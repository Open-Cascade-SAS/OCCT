// File:	PBRep_CurveOnSurface.cxx
// Created:	Mon Jul 26 10:28:56 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_CurveOnSurface.ixx>


//=======================================================================
//function : PBRep_CurveOnSurface
//purpose  : 
//=======================================================================

PBRep_CurveOnSurface::PBRep_CurveOnSurface(const Handle(PGeom2d_Curve)& PC,
					   const Standard_Real          CF,
					   const Standard_Real          CL,
					   const Handle(PGeom_Surface)& S,
					   const PTopLoc_Location&      L) :
       PBRep_GCurve(L, CF, CL),
       myPCurve(PC),
       mySurface(S)
{
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PBRep_CurveOnSurface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PBRep_CurveOnSurface::PCurve()const 
{
  return myPCurve;
}



//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveOnSurface::IsCurveOnSurface()const 
{
  return Standard_True;
}


//=======================================================================
//function : SetUVPoints
//purpose  : 
//=======================================================================

void PBRep_CurveOnSurface::SetUVPoints(const gp_Pnt2d& Pnt1,
				       const gp_Pnt2d& Pnt2) 
{
  myUV1 = Pnt1;
  myUV2 = Pnt2;
}

//=======================================================================
//function : FirstUV
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnSurface::FirstUV() const 
{
  return myUV1;
}

//=======================================================================
//function : LastUV
//purpose  : 
//=======================================================================

gp_Pnt2d PBRep_CurveOnSurface::LastUV() const 
{
  return myUV2;
}

