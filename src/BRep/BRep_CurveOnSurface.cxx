// File:	BRep_CurveOnSurface.cxx
// Created:	Tue Jul  6 18:24:27 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_CurveOnSurface.ixx>
#include <Precision.hxx>


//=======================================================================
//function : BRep_CurveOnSurface
//purpose  : 
//=======================================================================

BRep_CurveOnSurface::BRep_CurveOnSurface(const Handle(Geom2d_Curve)& PC, 
					 const Handle(Geom_Surface)& S, 
					 const TopLoc_Location& L) :
       BRep_GCurve(L,PC->FirstParameter(),PC->LastParameter()),
       myPCurve(PC),
       mySurface(S)
{
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BRep_CurveOnSurface::D0(const Standard_Real U, gp_Pnt& P) const
{
  // shoud be D0 NYI
  gp_Pnt2d P2d = myPCurve->Value(U);
  P = mySurface->Value(P2d.X(),P2d.Y());
  P.Transform(myLocation.Transformation());
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnSurface::IsCurveOnSurface()const 
{
  return Standard_True;
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnSurface::IsCurveOnSurface
  (const Handle(Geom_Surface)& S, const TopLoc_Location& L)const 
{
  return (S == mySurface) && (L == myLocation);
}



//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveOnSurface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_CurveOnSurface::PCurve()const 
{
  return myPCurve;
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_CurveOnSurface::PCurve(const Handle(Geom2d_Curve)& C)
{
  myPCurve = C;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_CurveOnSurface::Copy() const
{
  Handle(BRep_CurveOnSurface) C = new BRep_CurveOnSurface(myPCurve,
							  mySurface,
							  Location());
 
  C->SetRange(First(),Last());
  C->SetUVPoints(myUV1,myUV2);

  return C;
}


//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void  BRep_CurveOnSurface::Update()
{
  Standard_Real f = First();
  Standard_Real l = Last();
  Standard_Boolean isneg = Precision::IsNegativeInfinite(f);
  Standard_Boolean ispos = Precision::IsPositiveInfinite(l);
  if (!isneg) {
    myPCurve->D0(f,myUV1);
  }
  if (!ispos) {
    myPCurve->D0(l,myUV2);
  }
}


