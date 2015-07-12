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
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>

void gp_Hypr2d::Coefficients (Standard_Real& A, 
			      Standard_Real& B, 
			      Standard_Real& C, 
			      Standard_Real& D, 
			      Standard_Real& E, 
			      Standard_Real& F) const 
{
  Standard_Real DMin = minorRadius * minorRadius;
  Standard_Real DMaj = majorRadius * majorRadius;
  if (DMin <= gp::Resolution() && DMaj <= gp::Resolution()) {
    A = B = C = D = E = F = 0.0;
  }
  else {
    gp_Trsf2d T;
    T.SetTransformation (pos.XAxis());
    Standard_Real T11 = T.Value (1, 1);
    Standard_Real T12 = T.Value (1, 2);
    Standard_Real T13 = T.Value (1, 3);
    if (DMin <= gp::Resolution()) {
      A = T11 * T11;   B = T12 * T12;   C = T11 * T12;
      D = T11 * T13;   E = T12 * T13;   F = T13 * T13 - DMaj;
    }
    else {
      Standard_Real T21 = T.Value (2, 1);
      Standard_Real T22 = T.Value (2, 2);
      Standard_Real T23 = T.Value (2, 3);
      A = (T11 * T11 / DMaj) - (T21 * T21 / DMin);
      B = (T12 * T12 / DMaj) - (T22 * T22 / DMin);
      C = (T11 * T12 / DMaj) - (T21 * T22 / DMin);
      D = (T11 * T13 / DMaj) - (T21 * T23 / DMin);
      E = (T12 * T13 / DMaj) - (T22 * T23 / DMin);
      F = (T13 * T13 / DMaj) - (T23 * T23 / DMin) - 1.0;
    }
  }
}

void gp_Hypr2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror(P); }

gp_Hypr2d gp_Hypr2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Hypr2d H = *this;
  H.pos.Mirror (P);
  return H; 
}

void gp_Hypr2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror(A); }

gp_Hypr2d gp_Hypr2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Hypr2d H = *this;
  H.pos.Mirror (A);
  return H; 
}

