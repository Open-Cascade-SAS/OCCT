// File:	Geom2dAdaptor_HCurve.cxx
// Created:	Fri Aug 25 14:48:36 1995
// Author:	Remi LEQUETTE
//		<rle@mentox>


#include <Geom2dAdaptor_HCurve.ixx>

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve()
{
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Geom2dAdaptor_Curve& AS) :
Geom2dAdaptor_GHCurve(AS)
{
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S)
{
  ChangeCurve2d().Load(S);
}

//=======================================================================
//function : Geom2dAdaptor_HCurve
//purpose  : 
//=======================================================================

Geom2dAdaptor_HCurve::Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S, 
						  const Standard_Real UFirst, 
						  const Standard_Real ULast)
{
  ChangeCurve2d().Load(S,UFirst,ULast);
}

