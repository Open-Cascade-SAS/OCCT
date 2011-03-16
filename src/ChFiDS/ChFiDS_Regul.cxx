// File:	ChFiDS_Regul.cxx
// Created:	Tue Mar 21 11:15:18 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <ChFiDS_Regul.ixx>

//=======================================================================
//function : ChFiDS_Regul
//purpose  : 
//=======================================================================

ChFiDS_Regul::ChFiDS_Regul()
{
}


//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetCurve(const Standard_Integer IC)
{
  icurv = Abs(IC);
}


//=======================================================================
//function : SetS1
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetS1(const Standard_Integer IS1, 
			const Standard_Boolean IsFace)
{
  if(IsFace) is1 = Abs(IS1);
  else is1 = -Abs(IS1);
}


//=======================================================================
//function : SetS2
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetS2(const Standard_Integer IS2, 
			const Standard_Boolean IsFace)
{
  if(IsFace) is2 = Abs(IS2);
  else is2 = -Abs(IS2);
}


//=======================================================================
//function : IsSurface1
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Regul::IsSurface1() const 
{
  return (is1<0);
}


//=======================================================================
//function : IsSurface2
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Regul::IsSurface2() const 
{
  return (is2<0);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::Curve() const 
{
  return icurv;
}


//=======================================================================
//function : S1
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::S1() const 
{
  return Abs(is1);
}


//=======================================================================
//function : S2
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::S2() const 
{
  return Abs(is2);
}


