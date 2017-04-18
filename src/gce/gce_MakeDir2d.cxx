// Created on: 1992-09-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gce_MakeDir2d.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une direction 2d (Dir2d) de gp a partir de 2 Pnt2d de gp. +
//=========================================================================
gce_MakeDir2d::gce_MakeDir2d(const gp_Pnt2d& P1,
			     const gp_Pnt2d& P2)
{
  if (P1.Distance(P2) <= gp::Resolution()) { TheError = gce_ConfusedPoints; }
  else {
    TheDir2d = gp_Dir2d(P2.XY()-P1.XY());
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const gp_XY& Coord)
{
  if (Coord.Modulus() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(Coord);
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const gp_Vec2d& V)
{
  if (V.Magnitude() <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(V);
    TheError = gce_Done;
  }
}

gce_MakeDir2d::gce_MakeDir2d(const Standard_Real Xv,
			     const Standard_Real Yv)
{
  if (Xv*Xv+Yv*Yv <= gp::Resolution()) { TheError = gce_NullVector; }
  else {
    TheDir2d = gp_Dir2d(Xv,Yv);
    TheError = gce_Done;
  }
}

const gp_Dir2d& gce_MakeDir2d::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeDir2d::Value() - no result");
  return TheDir2d;
}

const gp_Dir2d& gce_MakeDir2d::Operator() const 
{
  return Value();
}

gce_MakeDir2d::operator gp_Dir2d() const
{
  return Value();
}

