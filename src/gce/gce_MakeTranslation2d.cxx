// File:	gce_MakeTranslation2d.cxx
// Created:	Thu Sep  3 17:18:54 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeTranslation2d.ixx>

//=========================================================================
//   Creation d une translation 2d de gp de vecteur de tanslation Vec.    +
//=========================================================================

gce_MakeTranslation2d::
  gce_MakeTranslation2d(const gp_Vec2d&  Vec ) {
   TheTranslation2d.SetTranslation(Vec);
 }

//=========================================================================
//   Creation d une translation 2d de gp de vecteur de tanslation le      +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

gce_MakeTranslation2d::
  gce_MakeTranslation2d(const gp_Pnt2d&  Point1 ,
			const gp_Pnt2d&  Point2 ) {
   TheTranslation2d.SetTranslation(gp_Vec2d(Point1,Point2));
 }

const gp_Trsf2d& gce_MakeTranslation2d::Value() const
{ 
  return TheTranslation2d; 
}

const gp_Trsf2d& gce_MakeTranslation2d::Operator() const 
{
  return TheTranslation2d;
}

gce_MakeTranslation2d::operator gp_Trsf2d() const
{
  return TheTranslation2d;
}
