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


#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

void gp_Sphere::Coefficients
(Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, 
 Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, 
 Standard_Real& C1, Standard_Real& C2, Standard_Real& C3,
 Standard_Real& D) const
{
  // Dans le repere local de la sphere :
  // X*X + Y*Y + Z*Z - radius * radius = 0
  gp_Trsf T;
  T.SetTransformation (pos);
  Standard_Real T11 = T.Value (1, 1);
  Standard_Real T12 = T.Value (1, 2);
  Standard_Real T13 = T.Value (1, 3);
  Standard_Real T14 = T.Value (1, 4);
  Standard_Real T21 = T.Value (2, 1);
  Standard_Real T22 = T.Value (2, 2);
  Standard_Real T23 = T.Value (2, 3);
  Standard_Real T24 = T.Value (2, 4);
  Standard_Real T31 = T.Value (3, 1);
  Standard_Real T32 = T.Value (3, 2);
  Standard_Real T33 = T.Value (3, 3);
  Standard_Real T34 = T.Value (3, 4);
  A1 = T11 * T11 + T21 * T21 + T31 * T31;
  A2 = T12 * T12 + T22 * T22 + T32 * T32;
  A3 = T13 * T13 + T23 * T23 + T33 * T33;
  B1 = T11 * T12 + T21 * T22 + T31 * T32;
  B2 = T11 * T13 + T21 * T23 + T31 * T33;
  B3 = T12 * T13 + T22 * T23 + T32 * T33;
  C1 = T11 * T14 + T21 * T24 + T31 * T34;
  C2 = T12 * T14 + T22 * T24 + T32 * T34;
  C3 = T13 * T14 + T23 * T24 + T33 * T34;
  D  = T14 * T14 + T24 * T24 + T34 * T34 - radius * radius;
}

void gp_Sphere::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Sphere gp_Sphere::Mirrored (const gp_Pnt& P) const
{
  gp_Sphere C = *this;
  C.pos.Mirror (P);
  return C;
}

void gp_Sphere::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Sphere gp_Sphere::Mirrored (const gp_Ax1& A1) const
{
  gp_Sphere C = *this;
  C.pos.Mirror (A1);
  return C;
}

void gp_Sphere::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Sphere gp_Sphere::Mirrored (const gp_Ax2& A2) const
{
  gp_Sphere C = *this;
  C.pos.Mirror (A2);
  return C;
}

