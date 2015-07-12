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

// JCV 08/01/91 Modifs introduction des classes XY, Mat2d dans le package gp

#define No_Standard_OutOfRange


#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_OutOfRange.hxx>

void gp_Pnt2d::Transform (const gp_Trsf2d& T)
{
  if (T.Form () == gp_Identity) { }
  else if (T.Form () == gp_Translation)
    { coord.Add (T.TranslationPart ()); }
  else if (T.Form () == gp_Scale) {
    coord.Multiply (T.ScaleFactor ());
    coord.Add      (T.TranslationPart ());
  }
  else if (T.Form () == gp_PntMirror) {
    coord.Reverse ();
    coord.Add     (T.TranslationPart ());
  }
  else { T.Transforms(coord); }
}

void gp_Pnt2d::Mirror (const gp_Pnt2d& P)
{
  coord.Reverse ();
  gp_XY XY = P.coord;
  XY.Multiply (2.0);
  coord.Add (XY);
}

gp_Pnt2d gp_Pnt2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Pnt2d Pres = *this;
  Pres.Mirror (P);
  return Pres;
}

void gp_Pnt2d::Mirror (const gp_Ax2d& A)
{
  gp_Trsf2d T;
  T.SetMirror  (A);
  T.Transforms (coord);
}

gp_Pnt2d gp_Pnt2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Pnt2d P = *this;
  P.Mirror (A);
  return P;
}

