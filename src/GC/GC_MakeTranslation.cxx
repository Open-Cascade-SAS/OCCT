// File:	GC_MakeTranslation.cxx
// Created:	Fri Oct  2 16:38:15 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeTranslation.ixx>
#include <GC_MakeTranslation.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une translation 3d de Geom de vecteur de tanslation Vec.  +
//=========================================================================

GC_MakeTranslation::GC_MakeTranslation(const gp_Vec&  Vec ) {
  TheTranslation = new Geom_Transformation();
  TheTranslation->SetTranslation(Vec);
}
     
//=========================================================================
//   Creation d une translation 3d de Geom de vecteur de tanslation le    +
//   vecteur reliant Point1 a Point2.                                     +
//=========================================================================

GC_MakeTranslation::GC_MakeTranslation(const gp_Pnt&  Point1 ,
					 const gp_Pnt&  Point2 ) {
  TheTranslation = new Geom_Transformation();
  TheTranslation->SetTranslation(Point1,Point2);
}

const Handle(Geom_Transformation)& GC_MakeTranslation::Value() const
{ 
  return TheTranslation;
}

const Handle(Geom_Transformation)& GC_MakeTranslation::Operator() const 
{
  return TheTranslation;
}

GC_MakeTranslation::operator Handle(Geom_Transformation) () const
{
  return TheTranslation;
}

