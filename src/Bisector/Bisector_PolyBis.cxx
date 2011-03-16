// File:	Bisector_PolyBis.cxx
// Created:	Mon Jan 10 17:06:47 1994
// Author:	Yves FRICAUD
//		<yfr@phylox>

#include <Bisector_PolyBis.ixx>
#include <Bisector_PointOnBis.hxx>
#include <Geom2d_Curve.hxx>
#include <ElCLib.hxx>
#include <gp.hxx>

#include <Standard_ConstructionError.hxx>

//=============================================================================
//function : Bisector_PolyBis
// purpose :
//=============================================================================
Bisector_PolyBis::Bisector_PolyBis()
{
  nbPoints = 0;
}

//=============================================================================
//function : Append
// purpose :
//=============================================================================
void Bisector_PolyBis::Append (const Bisector_PointOnBis& P)
{
  nbPoints++;
  thePoints [nbPoints] = P;
}

//=============================================================================
//function : Length
// purpose :
//=============================================================================
Standard_Integer Bisector_PolyBis::Length() const
{
  return nbPoints;
}

//=============================================================================
//function : IsEmpty
// purpose :
//=============================================================================
Standard_Boolean Bisector_PolyBis::IsEmpty() const
{
  return (nbPoints == 0);
}

//=============================================================================
//function : Value
// purpose :
//=============================================================================
const Bisector_PointOnBis& Bisector_PolyBis::Value
  (const Standard_Integer Index)
const
{
  return thePoints [Index];
}

//=============================================================================
//function : First
// purpose :
//=============================================================================
const Bisector_PointOnBis& Bisector_PolyBis::First() const
{
  return thePoints[1];
}

//=============================================================================
//function : Last
// purpose :
//=============================================================================
const Bisector_PointOnBis& Bisector_PolyBis::Last() const
{
  return thePoints[nbPoints];
}

//=============================================================================
//function : Points
// purpose :
//=============================================================================
//const PointOnBis& Bisector_PolyBis::Points()
//{
//  return thePoints;
//}

//=============================================================================
//function : Interval
// purpose :
//=============================================================================
Standard_Integer Bisector_PolyBis::Interval (const Standard_Real U) const
{
  if ( Last().ParamOnBis() - U < gp::Resolution()) {
    return nbPoints - 1;
  }
  Standard_Real    dU   = (Last().ParamOnBis() - First().ParamOnBis())/(nbPoints - 1);
  if (dU <= gp::Resolution()) return 1;

  Standard_Integer IntU = Standard_Integer(Abs(U - First().ParamOnBis())/dU) ;
  IntU++;

  if (thePoints[IntU].ParamOnBis() >= U) {
    for (Standard_Integer i = IntU; i >= 1; i--) {
      if (thePoints[i].ParamOnBis() <= U) {
	IntU = i;
	break;
      }
    }
  }
  else {
    for (Standard_Integer i = IntU; i <= nbPoints - 1; i++) {
      if (thePoints[i].ParamOnBis() >= U) {
	IntU = i - 1;
	break;
      }
    }
  }
  return IntU;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Bisector_PolyBis::Transform(const gp_Trsf2d& T)
{
  for (Standard_Integer i = 1; i <= nbPoints; i ++) {
    gp_Pnt2d P = thePoints[i].Point();
    P.Transform(T) ;
    thePoints[i].Point(P);
  }
}
