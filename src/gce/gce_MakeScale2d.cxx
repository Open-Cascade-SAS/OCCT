// File:	gce_MakeScale2d.cxx
// Created:	Thu Sep  3 17:18:54 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeScale2d.ixx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================

gce_MakeScale2d::
  gce_MakeScale2d(const gp_Pnt2d&     Point ,
		  const Standard_Real Scale ) {
  TheScale2d.SetScale(Point,Scale);
}

const gp_Trsf2d& gce_MakeScale2d::Value() const
{ 
  return TheScale2d; 
}

const gp_Trsf2d& gce_MakeScale2d::Operator() const 
{
  return TheScale2d;
}

gce_MakeScale2d::operator gp_Trsf2d() const
{
  return TheScale2d;
}
