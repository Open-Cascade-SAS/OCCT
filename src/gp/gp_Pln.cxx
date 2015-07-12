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

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 12/12/90 Modif suite a la premiere revue de projet
// LPA, JCV  07/92 passage sur C1.
// JCV 07/92 Introduction de la method Dump 
// LBO 08/93 Passage aux Ax3

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

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

