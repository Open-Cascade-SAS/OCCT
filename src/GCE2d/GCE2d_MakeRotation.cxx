// File:	GCE2d_MakeRotation.cxx
// Created:	Fri Oct  2 16:37:27 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeRotation.ixx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================

GCE2d_MakeRotation::GCE2d_MakeRotation(const gp_Pnt2d&     Point  ,
				       const Standard_Real Angle ) {
  TheRotation = new Geom2d_Transformation();
  TheRotation->SetRotation(Point,Angle);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeRotation::Value() const
{ 
  return TheRotation;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeRotation::Operator() const 
{
  return TheRotation;
}

GCE2d_MakeRotation::operator Handle(Geom2d_Transformation) () const
{
  return TheRotation;
}




