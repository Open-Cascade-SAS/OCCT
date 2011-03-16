// File:	gce_MakeLin.cxx
// Created:	Wed Sep  2 11:35:00 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeLin.ixx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================
//   Creation d une ligne 3d de gp a partir d un Ax1 de gp.               +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Ax1& A1)
{
  TheLin = gp_Lin(A1);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp a partir de son origine P (Pnt de gp)  +
//   et d une direction V (Dir de gp).                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P,
			 const gp_Dir& V)
{
  TheLin = gp_Lin(P,V);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp passant par les deux points <P1> et    +
//   <P2>.                                                                +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P1 ,
			 const gp_Pnt& P2 ) 
{
  if (P1.Distance(P2) >= gp::Resolution()) {
    TheLin = gp_Lin(P1,gp_Dir(P2.XYZ()-P1.XYZ()));
    TheError = gce_Done;
  }
  else { TheError = gce_ConfusedPoints; }
}

//=========================================================================
//   Creation d une ligne 3d de gp parallele a une autre <Lin> et passant +
//   par le point <P>.                                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Lin& Lin ,
			 const gp_Pnt& P   )
{
  TheLin = gp_Lin(P,Lin.Direction());
  TheError = gce_Done;
}

const gp_Lin& gce_MakeLin::Value() const
{ 
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
  return TheLin;
}

const gp_Lin& gce_MakeLin::Operator() const 
{
  return Value();
}

gce_MakeLin::operator gp_Lin() const
{
  return Value();
}

