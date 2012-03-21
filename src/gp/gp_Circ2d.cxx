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

//modif JCV 10/01/91 suite a la deuxieme revue de projet toolkit geometry

#include <gp_Circ2d.ixx>

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

