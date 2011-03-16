// File:	GCE2d_MakeScale.cxx
// Created:	Fri Oct  2 16:37:41 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GCE2d_MakeScale.ixx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================

GCE2d_MakeScale::GCE2d_MakeScale(const gp_Pnt2d&     Point ,
				 const Standard_Real Scale ) {
  TheScale = new Geom2d_Transformation();
  TheScale->SetScale(Point,Scale);
}

const Handle(Geom2d_Transformation)& GCE2d_MakeScale::Value() const
{ 
  return TheScale;
}

const Handle(Geom2d_Transformation)& GCE2d_MakeScale::Operator() const 
{
  return TheScale;
}

GCE2d_MakeScale::operator Handle(Geom2d_Transformation) () const
{
  return TheScale;
}

