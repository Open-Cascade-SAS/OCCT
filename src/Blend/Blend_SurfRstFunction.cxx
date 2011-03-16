// File:	Blend_SurfRstFunction.cxx
// Created:	Fri Feb  7 15:53:16 1997
// Author:	Laurent BOURESCHE
//		<lbo@pomalox.paris1.matra-dtv.fr>


#include <Blend_SurfRstFunction.ixx>
#include <Standard_NotImplemented.hxx>

const gp_Pnt& Blend_SurfRstFunction::Pnt1() const
{
  return PointOnS();
}

const gp_Pnt& Blend_SurfRstFunction::Pnt2() const
{
  return PointOnRst();
}

Standard_Real Blend_SurfRstFunction::GetMinimalDistance() const
{
  Standard_NotImplemented::Raise("Blend_SurfRstFunction::GetMinimalDistance");
  return RealLast();
}
