// File:	BRep_Curve3D.cxx
// Created:	Tue Jul  6 18:23:00 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRep_Curve3D.ixx>

//=======================================================================
//function : BRep_Curve3D
//purpose  : 
//=======================================================================

BRep_Curve3D::BRep_Curve3D(const Handle(Geom_Curve)& C,
			   const TopLoc_Location& L) :
                           BRep_GCurve(L,
			   C.IsNull() ? RealFirst() : C->FirstParameter(),
			   C.IsNull() ? RealLast()  : C->LastParameter()),
			   myCurve(C)
{
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BRep_Curve3D::D0(const Standard_Real U, gp_Pnt& P) const
{
  // shoud be D0 NYI
  P = myCurve->Value(U);
}

//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_Curve3D::IsCurve3D()const 
{
  return Standard_True;
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_Curve3D::Curve3D()const 
{
  return myCurve;
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

void BRep_Curve3D::Curve3D(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_Curve3D::Copy() const
{
  Handle(BRep_Curve3D) C = new BRep_Curve3D(myCurve,Location());

  C->SetRange(First(), Last());
  return C;
}
