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

//Modif JCV 12/12/90

#include <gp_Elips.ixx>

void gp_Elips::Mirror (const gp_Pnt& P)
{ pos.Mirror(P); }

gp_Elips gp_Elips::Mirrored (const gp_Pnt& P) const
{
  gp_Elips E = *this;
  E.pos.Mirror (P);
  return E; 
}

void gp_Elips::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Elips gp_Elips::Mirrored (const gp_Ax1& A1) const
{
  gp_Elips E = *this;
  E.pos.Mirror (A1);
  return E; 
}

void gp_Elips::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Elips gp_Elips::Mirrored (const gp_Ax2& A2) const
{
  gp_Elips E = *this;
  E.pos.Mirror (A2);
  return E; 
}

