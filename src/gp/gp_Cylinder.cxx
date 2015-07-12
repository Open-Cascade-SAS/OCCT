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

// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 
// LBO 08/93 passage aux Ax3

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

void gp_Cylinder::Coefficients
(Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, 
 Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, 
 Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, Standard_Real& D) const
{
  // Dans le repere local du cylindre :
  // X**2 + Y**2 - radius = 0.0
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

  A1 = T11 * T11 + T21 * T21;
  A2 = T12 * T12 + T22 * T22;
  A3 = T13 * T13 + T23 * T23;
  B1 = T11 * T12 + T21 * T22;
  B2 = T11 * T13 + T21 * T23;
  B3 = T12 * T13 + T22 * T23;
  C1 = T11 * T14 + T21 * T24;
  C2 = T12 * T14 + T22 * T24;
  C3 = T13 * T14 + T23 * T24;
  D  = T14 * T14 + T24 * T24 - radius * radius;  
}

void gp_Cylinder::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Cylinder gp_Cylinder::Mirrored (const gp_Pnt& P) const
{
  gp_Cylinder C = *this;
  C.pos.Mirror (P);
  return C;
}

void gp_Cylinder::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Cylinder gp_Cylinder::Mirrored (const gp_Ax1& A1) const
{
  gp_Cylinder C = *this;
  C.pos.Mirror (A1);
  return C;
}

void gp_Cylinder::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Cylinder gp_Cylinder::Mirrored (const gp_Ax2& A2) const
{
  gp_Cylinder C = *this;
  C.pos.Mirror (A2);
  return C;
}

