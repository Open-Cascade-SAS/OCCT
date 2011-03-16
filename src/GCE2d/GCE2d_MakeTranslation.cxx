// File:	GCE2d_MakeTranslation.cxx
// Created:	Fri Oct  2 16:38:15 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeTranslation.ixx>

//=========================================================================
//   Creation d une translation 3d de Geom2d de vecteur de tanslation Vec.  +
//=========================================================================

GCE2d_MakeTranslation::GCE2d_MakeTranslation(const gp_Vec2d&  Vec ) {
  TheTranslation = new Geom2d_Transformation();
  TheTranslation->SetTranslation(Vec);
}
     
//=========================================================================
//   Creation d une translation 3d de Geom2d de vecteur de tanslation le    +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

GCE2d_MakeTranslation::GCE2d_MakeTranslation(const gp_Pnt2d&  Point1 ,
					     const gp_Pnt2d&  Point2 ) {
  TheTranslation = new Geom2d_Transformation();
  TheTranslation->SetTranslation(Point1,Point2);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeTranslation::Value() const
{ 
  return TheTranslation;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

GCE2d_MakeTranslation::operator Handle(Geom2d_Transformation) () const
{
  return TheTranslation;
}

