// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 01/10/90 Changement de nom du package vgeom -> gp
// JCV 07/12/90 Modifs introduction des classes XYZ, Mat dans le package gp

#define No_Standard_OutOfRange

#include <gp_Pnt.ixx>

void gp_Pnt::Transform (const gp_Trsf& T)
{
  if (T.Form() == gp_Identity) { }
  else if (T.Form() == gp_Translation) { coord.Add (T.TranslationPart ()); }
  else if (T.Form() == gp_Scale) {
    coord.Multiply (T.ScaleFactor ());
    coord.Add      (T.TranslationPart ());
  }
  else if(T.Form() == gp_PntMirror) {
    coord.Reverse ();
    coord.Add     (T.TranslationPart ());
  }
  else { T.Transforms(coord); }
}

void gp_Pnt::Mirror (const gp_Pnt& P)
{
  coord.Reverse ();
  gp_XYZ XYZ = P.coord;
  XYZ.Multiply (2.0);
  coord.Add      (XYZ);
}

gp_Pnt gp_Pnt::Mirrored (const gp_Pnt& P) const
{
  gp_Pnt Pres = *this;
  Pres.Mirror (P);
  return Pres;
}

void gp_Pnt::Mirror (const gp_Ax1& A1)
{
  gp_Trsf T;
  T.SetMirror  (A1);
  T.Transforms (coord);
}

gp_Pnt gp_Pnt::Mirrored (const gp_Ax1& A1) const
{
  gp_Pnt P = *this;
  P.Mirror (A1);
  return P;
}

void gp_Pnt::Mirror (const gp_Ax2& A2)
{
  gp_Trsf T;
  T.SetMirror  (A2);
  T.Transforms (coord);
}

gp_Pnt gp_Pnt::Mirrored (const gp_Ax2& A2) const
{
  gp_Pnt P = *this;
  P.Mirror (A2);
  return P;
}

