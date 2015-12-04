// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Plane.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Plane,IGESData_IGESEntity)

IGESGeom_Plane::IGESGeom_Plane ()     {  }


    void IGESGeom_Plane::Init
  (const Standard_Real A, const Standard_Real B, 
   const Standard_Real C, const Standard_Real D, 
   const Handle(IGESData_IGESEntity)& aCurve, 
   const gp_XYZ& attach, const Standard_Real aSize)
{
  theA = A;
  theB = B;
  theC = C;
  theD = D;
  theCurve  = aCurve;
  theAttach = attach;
  theSize   = aSize;
  InitTypeAndForm(108,FormNumber());
// FormNumber : 0 No Curve. +1 Bound. -1 Hole
}

    void  IGESGeom_Plane::SetFormNumber (const Standard_Integer form)
{
  Standard_Integer fn = 0;
  if (form < 0) fn = -1;
  if (form > 0) fn = 1;
  InitTypeAndForm(108,fn);
}

    void IGESGeom_Plane::Equation
  (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D) const
{
  A = theA;
  B = theB;
  C = theC;
  D = theD;
}

    Standard_Boolean IGESGeom_Plane::HasBoundingCurve () const
{
  return (!theCurve.IsNull());
}

    Standard_Boolean IGESGeom_Plane::HasBoundingCurveHole () const
{
  return ((FormNumber() == -1) && (!theCurve.IsNull()));
}

    Handle(IGESData_IGESEntity) IGESGeom_Plane::BoundingCurve () const
{
  return theCurve;
}

    Standard_Boolean IGESGeom_Plane::HasSymbolAttach () const
{
  return (theSize > 0);
}

    gp_Pnt IGESGeom_Plane::SymbolAttach () const
{
  gp_Pnt attach(theAttach);
  return attach;
}

    gp_Pnt IGESGeom_Plane::TransformedSymbolAttach () const
{
  if (theSize > 0 && HasTransf()) 
    {
      gp_XYZ Symbol = theAttach;
      Location().Transforms(Symbol);
      return gp_Pnt(Symbol);
    }
  else return gp_Pnt(0, 0, 0);
}

    Standard_Real IGESGeom_Plane::SymbolSize () const
{
  return theSize;
}

    void  IGESGeom_Plane::TransformedEquation
  (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D) const
{
  //eqn of plane AX + BY + CZ = D

  Standard_Real x1,y1,z1,x2,y2,z2,x3,y3,z3;

  //case 1 intersection of the plane with the XY plane.
  x1 = 0.0;
  y1 = 0.0;
  z1 = theD / theC;

  //case 2 intersection of the plane with the XZ plane.
  x2 = 0.0;
  y2 = theD / theB;
  z2 = 0.0;

  //case 3 intersection of the plane with the YZ plane.
  x3 = theD / theA;
  y3 = 0.0;
  z3 = 0.0;
  gp_XYZ P1(x1,y1,z1);
  gp_XYZ P2(x2,y2,z2);
  gp_XYZ P3(x3,y3,z3);
  Location().Transforms(P1);
  Location().Transforms(P2);
  Location().Transforms(P3);
  x1 = P1.X();
  y1 = P1.Y();
  z1 = P1.Z();
  x2 = P2.X();
  y2 = P2.Y();
  z2 = P2.Z();
  x3 = P3.X();
  y3 = P3.Y();
  z3 = P3.Z();

/*
  General eqn of plane can also be written as
  a(x1 -x2) + b(y1 - y2) + c(z1 - z2) = 0
  a(x3 - x2) + b(y3 - y2) + c(z3 -z2) = 0 
  Applying Cramer's Rule :
  a                       b                       c               
  -------------   =   ---------------     =    ---------------   =   k
  |y3-y2  z3-z2|      |z3-z2   x3-x2|          |x3-x2   y3-y2|
  |y1-y2  z1-z2|      |z1-z2   x1-x2|          |x1-x2   y1-y2|

  .
  . .  a = c1*k , b = c2*k , c = c3*k
  hence c1(x - x2) + c2(y - y2) + c3(z - z2) = 0

*/
  Standard_Real c1,c2,c3;

  c1 = (y1*(-z3 + z2) + y2*(-z1 + z3) + y3*(z1 - z2));
  c2 = (x1*(z3 - z2) + x2*(-z3 + z1) + x3*(-z1 + z2));
  c3 = (x1*(-y3 + y2) + x2*(-y1 + y3) + x3*(y1 - y2));

  A = c1;
  B = c2;
  C = c3;
  D = c1*x2 + c2*y2 + c3*z3;
}
