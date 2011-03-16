// File:	GC_MakeScale.cxx
// Created:	Fri Oct  2 16:37:41 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeScale.ixx>
#include <GC_MakeScale.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================

GC_MakeScale::GC_MakeScale(const gp_Pnt&  Point ,
			     const Standard_Real     Scale ) {
  TheScale = new Geom_Transformation();
  TheScale->SetScale(Point,Scale);
}

const Handle(Geom_Transformation)& GC_MakeScale::Value() const
{ 
  return TheScale;
}

const Handle(Geom_Transformation)& GC_MakeScale::Operator() const 
{
  return TheScale;
}

GC_MakeScale::operator Handle(Geom_Transformation) () const
{
  return TheScale;
}

