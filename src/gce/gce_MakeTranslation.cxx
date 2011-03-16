// File:	gce_MakeTranslation.cxx
// Created:	Thu Sep  3 17:18:54 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeTranslation.ixx>

//=========================================================================
//   Creation d une translation 3d de gp de vecteur de tanslation Vec.    +
//=========================================================================

gce_MakeTranslation::
  gce_MakeTranslation(const gp_Vec&  Vec ) {
   TheTranslation.SetTranslation(Vec);
 }

//=========================================================================
//   Creation d une translation 3d de gp de vecteur de tanslation le      +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

gce_MakeTranslation::
  gce_MakeTranslation(const gp_Pnt&  Point1 ,
		      const gp_Pnt&  Point2 ) {
   TheTranslation.SetTranslation(gp_Vec(Point1,Point2));
 }

const gp_Trsf& gce_MakeTranslation::Value() const
{ 
  return TheTranslation; 
}

const gp_Trsf& gce_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

gce_MakeTranslation::operator gp_Trsf() const
{
  return TheTranslation;
}
