// File:	gce_MakeScale.cxx
// Created:	Fri Sep  4 14:28:10 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeScale.ixx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================

gce_MakeScale::
  gce_MakeScale(const gp_Pnt&       Point ,
		const Standard_Real Scale ) {
   TheScale.SetScale(Point,Scale);
 }

const gp_Trsf& gce_MakeScale::Value() const
{ 
  return TheScale; 
}

const gp_Trsf& gce_MakeScale::Operator() const 
{
  return TheScale;
}

gce_MakeScale::operator gp_Trsf() const
{
  return TheScale;
}
