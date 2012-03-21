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

// JCV 08/01/91 modif introduction des classes XY et Mat dans le package
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 

#define No_Standard_OutOfRange

#include <gp_Ax2d.ixx>
#include <gp_XY.hxx>

Standard_Boolean gp_Ax2d::IsCoaxial (const gp_Ax2d& Other, 
				     const Standard_Real AngularTolerance,
				     const Standard_Real LinearTolerance) const
{
  gp_XY XY1 = loc.XY();
  XY1.Subtract (Other.loc.XY());
  Standard_Real D1 = XY1.Crossed (Other.vdir.XY());
  if (D1 < 0) D1 = - D1;
  gp_XY XY2 = Other.loc.XY();
  XY2.Subtract (loc.XY());
  Standard_Real D2 = XY2.Crossed (vdir.XY());
  if (D2 < 0) D2 = - D2;
  return (vdir.IsParallel (Other.vdir, AngularTolerance) &&
	  D1 <= LinearTolerance && D2 <= LinearTolerance);
}

void gp_Ax2d::Scale (const gp_Pnt2d& P,
		     const Standard_Real S)
{
  loc.Scale(P, S);
  if (S < 0.0)  vdir.Reverse();
}

void gp_Ax2d::Mirror (const gp_Pnt2d& P)
{
  loc.Mirror(P);
  vdir.Reverse();
}

gp_Ax2d gp_Ax2d::Mirrored (const gp_Pnt2d& P) const
{
  gp_Ax2d A = *this;    
  A.Mirror (P);
  return A;
}

void gp_Ax2d::Mirror (const gp_Ax2d& A)
{
  loc.Mirror (A);
  vdir.Mirror (A.vdir); 
}

gp_Ax2d gp_Ax2d::Mirrored (const gp_Ax2d& A) const
{
  gp_Ax2d AA = *this;
  AA.Mirror (A);
  return AA;
}

