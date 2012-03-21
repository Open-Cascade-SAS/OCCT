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

// JCV 10/01/91 modifs suite a la deuxieme revue de projet
// AGV 03/04/07 bug correction: "pos" origin too far when A is very small

#define No_Standard_OutOfRange

#include <gp_Lin2d.ixx>
#include <gp.hxx>

//=======================================================================
//function : gp_Lin2d
//purpose  : 
//=======================================================================

gp_Lin2d::gp_Lin2d (const Standard_Real A,
                    const Standard_Real B,
                    const Standard_Real C)
{
  const Standard_Real Norm2 = A * A + B * B;
  Standard_ConstructionError_Raise_if (Norm2 <= gp::Resolution(), " ");
  const gp_Pnt2d P (-A*C/Norm2, -B*C/Norm2);
  const gp_Dir2d V (-B, A);

//   gp_Pnt2d P;
//   Standard_Real Norm = sqrt(A * A + B * B);
//   Standard_ConstructionError_Raise_if (Norm <= gp::Resolution(), " ");
//   Standard_Real A1 = A/Norm;
//   Standard_Real B1 = B/Norm;
//   Standard_Real C1 = C/Norm;
//   gp_Dir2d V = gp_Dir2d (-B1, A1);
//   Standard_Real AA1 = A1;
//   if (AA1 < 0) AA1 = - AA1;
//   if (AA1 > gp::Resolution()) P.SetCoord (-C1 / A1, 0.0);
//   else                        P.SetCoord (0.0, -C1 / B1);

  pos = gp_Ax2d(P, V);
}

//=======================================================================
//function : Mirror
//purpose  : 
//=======================================================================

void gp_Lin2d::Mirror (const gp_Pnt2d& P)
{ pos.Mirror(P);  }

//=======================================================================
//function : Mirrored
//purpose  : 
//=======================================================================

gp_Lin2d gp_Lin2d::Mirrored (const gp_Pnt2d& P)  const
{
  gp_Lin2d L = *this;    
  L.pos.Mirror(P);
  return L;
}

//=======================================================================
//function : Mirror
//purpose  : 
//=======================================================================

void gp_Lin2d::Mirror (const gp_Ax2d& A)
{ pos.Mirror(A); }

//=======================================================================
//function : Mirrored
//purpose  : 
//=======================================================================

gp_Lin2d gp_Lin2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Lin2d L = *this;
  L.pos.Mirror(A);
  return L;
}

