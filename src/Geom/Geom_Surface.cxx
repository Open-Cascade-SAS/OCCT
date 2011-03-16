// File:	Geom_Surface.cxx
// Created:	Wed Mar 10 10:27:16 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Surface.cxx, JCV 17/01/91

#include <Geom_Surface.ixx>

typedef Geom_Surface         Surface;
typedef Handle(Geom_Surface) Handle(Surface);


//=======================================================================
//function : UReversed
//purpose  : 
//=======================================================================

Handle(Surface) Geom_Surface::UReversed () const
{
  Handle(Surface) S = Handle(Surface)::DownCast(Copy());
  S->UReverse();
  return S;
}


//=======================================================================
//function : VReversed
//purpose  : 
//=======================================================================

Handle(Surface) Geom_Surface::VReversed () const
{
  Handle(Surface) S = Handle(Surface)::DownCast(Copy());
  S->VReverse();
  return S;
}


//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_Surface::TransformParameters(Standard_Real& ,
				       Standard_Real& ,
				       const gp_Trsf&  ) const
{
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_Surface::ParametricTransformation(const gp_Trsf&) const
{
  gp_GTrsf2d dummy;
  return dummy;
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_Surface::UPeriod() const
{
  Standard_NoSuchObject_Raise_if
    ( !IsUPeriodic(),"Geom_Surface::UPeriod");

  Standard_Real U1, U2, V1, V2;
  Bounds(U1,U2,V1,V2);
  return ( U2 - U1);
}


//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_Surface::VPeriod() const
{
  Standard_NoSuchObject_Raise_if
    ( !IsVPeriodic(),"Geom_Surface::VPeriod");

  Standard_Real U1, U2, V1, V2;
  Bounds(U1,U2,V1,V2);
  return ( V2 - V1);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt  Geom_Surface::Value(const Standard_Real U, 
			    const Standard_Real V)const 
{
  gp_Pnt P;
  D0(U,V,P);
  return P;
}
