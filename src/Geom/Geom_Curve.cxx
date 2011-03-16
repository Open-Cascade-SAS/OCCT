// File:	Geom_Curve.cxx
// Created:	Wed Mar 10 09:36:27 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Curve.cxx, JCV 17/01/91


#include <Geom_Curve.ixx>

typedef Geom_Curve         Curve;
typedef Handle(Geom_Curve) Handle(Curve);


//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================

Handle(Curve) Geom_Curve::Reversed () const
{
  Handle(Curve) C = Handle(Curve)::DownCast(Copy());
  C->Reverse();
  return C;
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom_Curve::Period() const
{
  Standard_NoSuchObject_Raise_if
    ( !IsPeriodic(),"Geom_Curve::Period");

  return ( LastParameter() - FirstParameter());
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt Geom_Curve::Value( const Standard_Real U) const 
{
  gp_Pnt P;
  D0( U, P);
  return P;
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Curve::TransformedParameter(const Standard_Real U,
					       const gp_Trsf&) const
{
  return U;
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Curve::ParametricTransformation(const gp_Trsf& ) const
{
  return 1.;
}


