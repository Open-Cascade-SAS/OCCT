// File:	GC_MakeRotation.cxx
// Created:	Fri Oct  2 16:37:27 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GC_MakeRotation.ixx>
#include <GC_MakeRotation.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite Line.                                                         +
//=========================================================================

GC_MakeRotation::GC_MakeRotation(const gp_Lin&  Line  ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(gp_Ax1(Line.Location(),Line.Direction()),Angle);
}

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a un      +
//   axe Axis.                                                            +
//=========================================================================

GC_MakeRotation::GC_MakeRotation(const gp_Ax1&  Axis  ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(Axis,Angle);
}

//=========================================================================
//   Creation d une rotation 3d de gp d angle Angle par rapport a une     +
//   droite issue du point Point et de direction Direc.                   +
//=========================================================================

GC_MakeRotation::GC_MakeRotation(const gp_Pnt&  Point ,
				   const gp_Dir&  Direc ,
				   const Standard_Real     Angle ) {
  TheRotation = new Geom_Transformation();
  TheRotation->SetRotation(gp_Ax1(Point,Direc),Angle);
}

const Handle(Geom_Transformation)& GC_MakeRotation::Value() const
{ 
  return TheRotation;
}

const Handle(Geom_Transformation)& GC_MakeRotation::Operator() const 
{
  return TheRotation;
}

GC_MakeRotation::operator Handle(Geom_Transformation) () const
{
  return TheRotation;
}




