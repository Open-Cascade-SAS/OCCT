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

// Modif jcv 14/12/90 suite a la premiere revue de projet

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

void gp_Parab::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Parab gp_Parab::Mirrored (const gp_Pnt& P) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (P);
  return Prb;     
}

void gp_Parab::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Parab gp_Parab::Mirrored (const gp_Ax1& A1) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (A1);
  return Prb;     
}

void gp_Parab::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Parab gp_Parab::Mirrored (const gp_Ax2& A2) const
{
  gp_Parab Prb = *this;
  Prb.pos.Mirror (A2);
  return Prb;     
}

