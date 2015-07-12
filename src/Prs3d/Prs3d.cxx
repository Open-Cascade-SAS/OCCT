// Created on: 1993-08-27
// Created by: Jean-Louis FRENKEL
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


#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : MatchSegment
//purpose  :
//=======================================================================
Standard_Boolean Prs3d::MatchSegment 
                 (const Quantity_Length X,
                  const Quantity_Length Y,
                  const Quantity_Length Z,
                  const Quantity_Length aDistance,
                  const gp_Pnt& P1,
                  const gp_Pnt& P2,
                  Quantity_Length& dist)
{
  Standard_Real X1,Y1,Z1,X2,Y2,Z2;
  P1.Coord(X1,Y1,Z1); P2.Coord(X2,Y2,Z2);
  Standard_Real DX = X2-X1; 
  Standard_Real DY = Y2-Y1; 
  Standard_Real DZ = Z2-Z1;
  Standard_Real Dist = DX*DX + DY*DY + DZ*DZ;
  if (Dist == 0.) return Standard_False;
  
  Standard_Real Lambda = ((X-X1)*DX + (Y-Y1)*DY + (Z-Z1)*DZ)/Dist;
  if ( Lambda < 0. || Lambda > 1. ) return Standard_False;
  dist =  Abs(X-X1-Lambda*DX) +
          Abs(Y-Y1-Lambda*DY) +
          Abs(Z-Z1-Lambda*DZ);
  return (dist < aDistance);
}

//=======================================================================
//function : GetDeflection
//purpose  :
//=======================================================================
Standard_Real Prs3d::GetDeflection (const TopoDS_Shape&         theShape,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
#define MAX2(X, Y)    (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z) (MAX2 (MAX2 (X, Y), Z))

  Standard_Real aDeflection = theDrawer->MaximalChordialDeviation();
  if (theDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    Bnd_Box aBndBox;
    BRepBndLib::Add (theShape, aBndBox, Standard_False);
    if (!aBndBox.IsVoid())
    {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      aBndBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      aDeflection = MAX3 (aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * theDrawer->DeviationCoefficient() * 4.0;
      // we store computed relative deflection of shape as absolute deviation coefficient
      // in case relative type to use it later on for sub-shapes.
      theDrawer->SetMaximalChordialDeviation (aDeflection);
    }
  }
  return aDeflection;
}
