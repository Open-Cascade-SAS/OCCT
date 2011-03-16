// File:	gce_MakeRotation.cxx
// Created:	Thu Sep  3 17:18:54 1992
// Author:	Remi GILET
//		<reg@sdsun1>

#include <gce_MakeRotation.ixx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================

gce_MakeRotation::
  gce_MakeRotation(const gp_Lin&  Line  ,
		   const Standard_Real     Angle ) {
   TheRotation.SetRotation(gp_Ax1(Line.Position()),Angle);
 }

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a un      +
//   axe Axis.                                                            +
//=========================================================================

gce_MakeRotation::
 gce_MakeRotation(const gp_Ax1&  Axis  ,
		  const Standard_Real     Angle ) {
   TheRotation.SetRotation(Axis,Angle);
 }

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite issue du point Point et de direction Direc.                   +
//=========================================================================

gce_MakeRotation::
 gce_MakeRotation(const gp_Pnt&  Point ,
		  const gp_Dir&  Direc ,
		  const Standard_Real     Angle ) {
   TheRotation.SetRotation(gp_Ax1(Point,Direc),Angle);
 }

const gp_Trsf& gce_MakeRotation::Value() const
{ 
  return TheRotation; 
}

const gp_Trsf& gce_MakeRotation::Operator() const 
{
  return TheRotation;
}

gce_MakeRotation::operator gp_Trsf() const
{
  return TheRotation;
}
