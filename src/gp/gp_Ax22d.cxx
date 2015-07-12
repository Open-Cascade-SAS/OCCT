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

// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif mineur suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 

#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>

void gp_Ax22d::Mirror (const gp_Pnt2d& P)
{
  gp_Pnt2d Temp = point;
  Temp.Mirror (P);
  point = Temp;
  vxdir.Reverse ();
  vydir.Reverse ();
}

gp_Ax22d gp_Ax22d::Mirrored(const gp_Pnt2d& P) const
{
  gp_Ax22d Temp = *this;
  Temp.Mirror (P);
  return Temp;
}

void gp_Ax22d::Mirror (const gp_Ax2d& A1)
{
  vydir.Mirror (A1);
  vxdir.Mirror (A1);
  gp_Pnt2d Temp = point;
  Temp.Mirror (A1);
  point = Temp;
}

gp_Ax22d gp_Ax22d::Mirrored(const gp_Ax2d& A1) const
{
  gp_Ax22d Temp = *this;
  Temp.Mirror (A1);
  return Temp;
}

