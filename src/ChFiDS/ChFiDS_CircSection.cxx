// File:	ChFiDS_CircSection.cxx
// Created:	Wed Mar  6 15:54:42 1996
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <ChFiDS_CircSection.ixx>

//=======================================================================
//function : ChFiDS_CircSection
//purpose  : 
//=======================================================================

ChFiDS_CircSection::ChFiDS_CircSection(){}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Circ&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myCirc = C;
  myF    = F;
  myL    = L;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Lin&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myLin  = C;
  myF    = F;
  myL    = L;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Circ&       C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myCirc;
  F = myF;
  L = myL;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Lin&        C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myLin;
  F = myF;
  L = myL;
}
