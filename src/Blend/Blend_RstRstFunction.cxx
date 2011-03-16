// File:	Blend_RstRstFunction.cxx
// Created:	Fri Feb  7 15:53:16 1997
// Author:	Laurent BOURESCHE
//		<lbo@pomalox.paris1.matra-dtv.fr>


#include <Blend_RstRstFunction.ixx>
#include <Standard_NotImplemented.hxx>

const gp_Pnt& Blend_RstRstFunction::Pnt1() const
{
  return PointOnRst1();
}

const gp_Pnt& Blend_RstRstFunction::Pnt2() const
{
  return PointOnRst2();
}

Standard_Real Blend_RstRstFunction::GetMinimalDistance() const
{
  Standard_NotImplemented::Raise("Blend_RstRstFunction::GetMinimalDistance");
  return RealLast();
}




