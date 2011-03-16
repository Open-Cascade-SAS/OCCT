// File:	gp_Ax3.cxx
// Created:	Tue Aug  3 12:42:36 1993
// Author:	Laurent BOURESCHE
//		<lbo@nonox>

#include <gp_Ax3.ixx>

//=======================================================================
//function : gp_Ax3
//purpose  : 
//=======================================================================

gp_Ax3::gp_Ax3 (const gp_Pnt& P,
		const gp_Dir& V) : axis(P,V)
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
  gp_Dir D;
  
  //  pour determiner l axe X :
  //  on dit que le produit scalaire Vx.V = 0. 
  //  et on recherche le max(A,B,C) pour faire la division.
  //  l une des coordonnees du vecteur est nulle. 
  
  if     ( Babs <= Aabs && Babs <= Cabs) {
    if (Aabs > Cabs) D.SetCoord(-C,0., A);
    else             D.SetCoord( C,0.,-A);
  }
  else if( Aabs <= Babs && Aabs <= Cabs) {
    if (Babs > Cabs) D.SetCoord(0.,-C, B);
    else             D.SetCoord(0., C,-B);
  }
  else {
    if (Aabs > Babs) D.SetCoord(-B, A,0.);
    else             D.SetCoord( B,-A,0.);
  }
  vxdir = D;
  vydir = V.Crossed(vxdir);
}

void  gp_Ax3::Mirror(const gp_Pnt& P)
{
  axis.Mirror (P);
  vxdir.Reverse ();
  vydir.Reverse ();
}

gp_Ax3  gp_Ax3::Mirrored(const gp_Pnt& P)const
{
  gp_Ax3 Temp = *this;
  Temp.Mirror (P);
  return Temp;
}

void  gp_Ax3::Mirror(const gp_Ax1& A1)
{
  vydir.Mirror (A1);
  vxdir.Mirror (A1);
  axis.Mirror (A1);
}

gp_Ax3  gp_Ax3::Mirrored(const gp_Ax1& A1)const
{
  gp_Ax3 Temp = *this;
  Temp.Mirror (A1);
  return Temp;
}

void  gp_Ax3::Mirror(const gp_Ax2& A2)
{
  vydir.Mirror (A2);
  vxdir.Mirror (A2);
  axis.Mirror (A2);
}

gp_Ax3  gp_Ax3::Mirrored(const gp_Ax2& A2)const
{
  gp_Ax3 Temp = *this;
  Temp.Mirror (A2);
  return Temp;
}

