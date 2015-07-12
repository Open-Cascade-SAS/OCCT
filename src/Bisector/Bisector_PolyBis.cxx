// Created on: 1994-01-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Bisector_PointOnBis.hxx>
#include <Bisector_PolyBis.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <gp.hxx>
#include <gp_Trsf2d.hxx>
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
