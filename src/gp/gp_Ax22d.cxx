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

// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif mineur suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 

#include <gp_Ax22d.ixx>

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

