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

//modif JCV 10/01/91 suite a la deuxieme revue de projet toolkit geometry

#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>

void gp_Circ2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror(P); }

gp_Circ2d gp_Circ2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Circ2d C = *this;
  C.pos.Mirror (P);
  return C; 
}

void gp_Circ2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror (A); }

gp_Circ2d gp_Circ2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Circ2d C = *this;
  C.pos.Mirror (A);
  return C; 
}

