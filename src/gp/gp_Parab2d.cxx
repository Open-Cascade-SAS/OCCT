// Copyright (c) 1995-1999 Matra Datavision
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

#define No_Standard_OutOfRange


#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>

gp_Parab2d::gp_Parab2d (const gp_Ax2d& theDirectrix,
                        const gp_Pnt2d& theFocus,
                        const Standard_Boolean theSense)
{
  const gp_Pnt2d &aDirLoc = theDirectrix.Location();
  const gp_Dir2d &aDirVec = theDirectrix.Direction();

  const gp_Vec2d aFVec(aDirLoc, theFocus);

  const gp_Pnt2d anOrigin(aDirLoc.XY()+aDirVec.XY()*(aFVec.Dot(aDirVec)));
  const gp_Pnt2d anApex(0.5*(anOrigin.XY()+theFocus.XY()));

  focalLength = 0.5*anOrigin.Distance(theFocus);

  gp_Dir2d aXDir = (focalLength > 0.0) ? gp_Dir2d(theFocus.XY()-anOrigin.XY()) :
                        theDirectrix.Rotated(aDirLoc,
                                             theSense ? -M_PI_2 : M_PI_2).Direction();

  pos = gp_Ax22d(anApex, aXDir, aDirVec);
}

void gp_Parab2d::Coefficients
(Standard_Real& A, Standard_Real& B, Standard_Real& C,
 Standard_Real& D, Standard_Real& E, Standard_Real& F) const
{
  Standard_Real P = 2.0 * focalLength;
  gp_Trsf2d T;
  T.SetTransformation (pos.XAxis());
  Standard_Real T11 = T.Value (1, 1);
  Standard_Real T12 = T.Value (1, 2);
  Standard_Real T13 = T.Value (1, 3);
  Standard_Real T21 = T.Value (2, 1);
  Standard_Real T22 = T.Value (2, 2);
  Standard_Real T23 = T.Value (2, 3);
  A = T21 * T21;
  B = T22 * T22;
  C = T21 * T22;
  D = (T21 * T23) - (P * T11);
  E = (T22 * T23) - (P * T12);
  F = (T23 * T23) - (2.0 * P * T13);
}

void gp_Parab2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror (P); }

gp_Parab2d gp_Parab2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Parab2d Prb = *this;
  Prb.pos.Mirror (P);
  return Prb;     
}

void gp_Parab2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror (A); }

gp_Parab2d gp_Parab2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Parab2d Prb = *this;
  Prb.pos.Mirror (A);
  return Prb;     
}

