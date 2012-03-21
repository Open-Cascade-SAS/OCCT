// Created on: 1994-01-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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
