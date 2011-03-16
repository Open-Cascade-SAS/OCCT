// File:	Geom2d_Curve.cxx
// Created:	Wed Mar 24 19:20:49 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Curve.cxx, JCV 24/06/91


#include <Geom2d_Curve.ixx>



typedef Geom2d_Curve         Curve;
typedef Handle(Geom2d_Curve) Handle(Curve);


//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) Geom2d_Curve::Reversed () const
{
  Handle(Curve) C = Handle(Curve)::DownCast(Copy());
  C->Reverse();
  return C;
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::TransformedParameter(const Standard_Real U,
						 const gp_Trsf2d& ) const
{
  return U;
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::ParametricTransformation(const gp_Trsf2d& ) const
{
  return 1.;
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::Period() const
{
  Standard_NoSuchObject_Raise_if
    ( !IsPeriodic(),"Geom2d_Curve::Period");

  return ( LastParameter() - FirstParameter());
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d  Geom2d_Curve::Value(const Standard_Real U)const 
{
  gp_Pnt2d P;
  D0(U,P);
  return P;
}
