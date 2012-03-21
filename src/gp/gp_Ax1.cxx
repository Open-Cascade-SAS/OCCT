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
// JCV 12/12/90 modif introduction des classes XYZ et Mat dans le package
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 

#define No_Standard_OutOfRange

#include <gp_Ax1.ixx>
#include <gp.hxx>

Standard_Boolean gp_Ax1::IsCoaxial
(const gp_Ax1& Other, 
 const Standard_Real AngularTolerance,
 const Standard_Real LinearTolerance) const
{
  gp_XYZ XYZ1 = loc.XYZ();
  XYZ1.Subtract (Other.loc.XYZ());
  XYZ1.Cross (Other.vdir.XYZ());
  Standard_Real D1 = XYZ1.Modulus();
  gp_XYZ XYZ2 = Other.loc.XYZ();
  XYZ2.Subtract (loc.XYZ());
  XYZ2.Cross (vdir.XYZ());
  Standard_Real D2 = XYZ2.Modulus();
  return (vdir.IsEqual (Other.vdir, AngularTolerance) &&
          D1 <= LinearTolerance && D2 <= LinearTolerance);
}

void gp_Ax1::Mirror (const gp_Pnt& P)
{
  loc.Mirror(P);
  vdir.Reverse();
}

gp_Ax1 gp_Ax1::Mirrored (const gp_Pnt& P) const
{
  gp_Ax1 A1 = *this;    
  A1.Mirror (P);
  return A1;
}

void gp_Ax1::Mirror (const gp_Ax1& A1)
{
  loc.Mirror(A1);
  vdir.Mirror(A1.vdir);
}

gp_Ax1 gp_Ax1::Mirrored (const gp_Ax1& A1) const
{
  gp_Ax1 A = *this;
  A.Mirror (A1);
  return A;
}

void gp_Ax1::Mirror (const gp_Ax2& A2)
{
  loc.Mirror  (A2);
  vdir.Mirror (A2);
}

gp_Ax1 gp_Ax1::Mirrored (const gp_Ax2& A2) const
{
  gp_Ax1 A1 = *this;
  A1.Mirror (A2);
  return A1;
}

