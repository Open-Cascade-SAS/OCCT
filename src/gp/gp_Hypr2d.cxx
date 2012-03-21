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


#define No_Standard_OutOfRange

#include <gp_Hypr2d.ixx>

void gp_Hypr2d::Coefficients (Standard_Real& A, 
			      Standard_Real& B, 
			      Standard_Real& C, 
			      Standard_Real& D, 
			      Standard_Real& E, 
			      Standard_Real& F) const 
{
  Standard_Real DMin = minorRadius * minorRadius;
  Standard_Real DMaj = majorRadius * majorRadius;
  if (DMin <= gp::Resolution() && DMaj <= gp::Resolution()) {
    A = B = C = D = E = F = 0.0;
  }
  else {
    gp_Trsf2d T;
    T.SetTransformation (pos.XAxis());
    Standard_Real T11 = T.Value (1, 1);
    Standard_Real T12 = T.Value (1, 2);
    Standard_Real T13 = T.Value (1, 3);
    if (DMin <= gp::Resolution()) {
      A = T11 * T11;   B = T12 * T12;   C = T11 * T12;
      D = T11 * T13;   E = T12 * T13;   F = T13 * T13 - DMaj;
    }
    else {
      Standard_Real T21 = T.Value (2, 1);
      Standard_Real T22 = T.Value (2, 2);
      Standard_Real T23 = T.Value (2, 3);
      A = (T11 * T11 / DMaj) - (T21 * T21 / DMin);
      B = (T12 * T12 / DMaj) - (T22 * T22 / DMin);
      C = (T11 * T12 / DMaj) - (T21 * T22 / DMin);
      D = (T11 * T13 / DMaj) - (T21 * T23 / DMin);
      E = (T12 * T13 / DMaj) - (T22 * T23 / DMin);
      F = (T13 * T13 / DMaj) - (T23 * T23 / DMin) - 1.0;
    }
  }
}

void gp_Hypr2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror(P); }

gp_Hypr2d gp_Hypr2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Hypr2d H = *this;
  H.pos.Mirror (P);
  return H; 
}

void gp_Hypr2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror(A); }

gp_Hypr2d gp_Hypr2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Hypr2d H = *this;
  H.pos.Mirror (A);
  return H; 
}

