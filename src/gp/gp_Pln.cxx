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
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 
// LBO 08/93 Passage aux Ax3

#include <gp_Pln.ixx>
#include <gp.hxx>

gp_Pln::gp_Pln (const gp_Pnt& P,
		const gp_Dir& V)
{
  Standard_Real A = V.X();
  Standard_Real B = V.Y();
  Standard_Real C = V.Z();
  Standard_Real Aabs = A;
  if (Aabs < 0) Aabs = - Aabs;
  Standard_Real Babs = B;
  if (Babs < 0) Babs = - Babs;
  Standard_Real Cabs = C;
  if (Cabs < 0) Cabs = - Cabs;

  //  pour determiner l'axe X :
  //  on dit que le produit scalaire Vx.V = 0. 
  //  et on recherche le max(A,B,C) pour faire la division.
  //  l'une des coordonnees du vecteur est nulle. 

  if( Babs <= Aabs && Babs <= Cabs) {
    if (Aabs > Cabs)  pos = gp_Ax3 (P, V, gp_Dir (-C,0., A));
    else              pos = gp_Ax3 (P, V, gp_Dir ( C,0.,-A));
  }
  else if( Aabs <= Babs && Aabs <= Cabs) {
    if (Babs > Cabs)  pos = gp_Ax3 (P, V, gp_Dir (0.,-C, B));
    else              pos = gp_Ax3 (P, V, gp_Dir (0., C,-B));
  }
  else {
    if (Aabs > Babs)  pos = gp_Ax3 (P, V, gp_Dir (-B, A,0.));
    else              pos = gp_Ax3 (P, V, gp_Dir ( B,-A,0.));
  }
}

gp_Pln::gp_Pln (const Standard_Real A,
		const Standard_Real B,
		const Standard_Real C,
		const Standard_Real D)
{
  Standard_Real Aabs = A;
  if (Aabs < 0) Aabs = - Aabs;
  Standard_Real Babs = B;
  if (Babs < 0) Babs = - Babs;
  Standard_Real Cabs = C;
  if (Cabs < 0) Cabs = - Cabs;
  if (Babs <= Aabs && Babs <= Cabs) {
    if (Aabs > Cabs) pos = gp_Ax3(gp_Pnt(-D/A,  0.,  0.),
				  gp_Dir(A,B,C),
				  gp_Dir(-C,0., A));
    else             pos = gp_Ax3(gp_Pnt(  0.,  0.,-D/C),
				  gp_Dir(A,B,C),
				  gp_Dir( C,0.,-A));
  }
  else if (Aabs <= Babs && Aabs <= Cabs) {
    if (Babs > Cabs) pos = gp_Ax3(gp_Pnt(  0.,-D/B,  0.),
				  gp_Dir(A,B,C),
				  gp_Dir(0.,-C, B));
    else             pos = gp_Ax3(gp_Pnt(  0.,  0.,-D/C),
				  gp_Dir(A,B,C),
				  gp_Dir(0., C,-B));
  }
  else {
    if (Aabs > Babs) pos = gp_Ax3(gp_Pnt(-D/A,  0.,  0.),
				  gp_Dir(A,B,C),
				  gp_Dir(-B, A, 0.));
    else             pos = gp_Ax3(gp_Pnt(  0.,-D/B,  0.),
				  gp_Dir(A,B,C),
				  gp_Dir( B,-A, 0.));
  }
} 

void gp_Pln::Mirror (const gp_Pnt& P)
{ pos.Mirror(P);  }

gp_Pln gp_Pln::Mirrored (const gp_Pnt& P) const
{
  gp_Pln Pl = *this;
  Pl.pos.Mirror(P);
  return Pl;
}

void gp_Pln::Mirror (const gp_Ax1& A1)
{ pos.Mirror(A1); }

gp_Pln gp_Pln::Mirrored (const gp_Ax1& A1) const
{
  gp_Pln Pl = *this;
  Pl.pos.Mirror(A1);
  return Pl;
}

void gp_Pln::Mirror (const gp_Ax2& A2)
{ pos.Mirror(A2); }

gp_Pln gp_Pln::Mirrored (const gp_Ax2& A2) const
{
  gp_Pln Pl = *this;
  Pl.pos.Mirror(A2);
  return Pl;
}

