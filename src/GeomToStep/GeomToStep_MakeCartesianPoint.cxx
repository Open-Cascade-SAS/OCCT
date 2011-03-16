// File:	GeomToStep_MakeCartesianPoint.cxx
// Created:	Tue Jun 15 11:41:21 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeCartesianPoint.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un cartesian_point de prostep a partir d' un point3d de gp
//=============================================================================

GeomToStep_MakeCartesianPoint::GeomToStep_MakeCartesianPoint( const gp_Pnt& P)
{
  Handle(StepGeom_CartesianPoint) Pstep = new StepGeom_CartesianPoint;
//  Handle(TColStd_HArray1OfReal) Acoord = new TColStd_HArray1OfReal(1,3);
  Standard_Real X, Y, Z;
  
  P.Coord(X, Y, Z);
//  Acoord->SetValue(1,X);
//  Acoord->SetValue(2,Y);
//  Acoord->SetValue(3,Z);
//  Pstep->SetCoordinates(Acoord);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
//  Pstep->SetName(name);
  Standard_Real fact = UnitsMethods::LengthFactor();
  Pstep->Init3D (name,X/fact,Y/fact,Z/fact);
  theCartesianPoint = Pstep;
  done = Standard_True;
}
//=============================================================================
// Creation d' un cartesian_point de prostep a partir d' un point 2d de gp
//=============================================================================

GeomToStep_MakeCartesianPoint::GeomToStep_MakeCartesianPoint( const gp_Pnt2d& P)
{
  Handle(StepGeom_CartesianPoint) Pstep = new StepGeom_CartesianPoint;
//  Handle(TColStd_HArray1OfReal) Acoord = new TColStd_HArray1OfReal(1,2);
  Standard_Real X, Y;
  
  P.Coord(X, Y);
//  Acoord->SetValue(1,X);
//  Acoord->SetValue(2,Y);
//  Pstep->SetCoordinates(Acoord);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
//  Pstep->SetName(name);
  Pstep->Init2D (name,X,Y);
  theCartesianPoint = Pstep;
  done = Standard_True;
}

//=============================================================================
// Creation d' un cartesian_point de prostep a partir d' un point 3d de Geom
//=============================================================================

GeomToStep_MakeCartesianPoint::
  GeomToStep_MakeCartesianPoint( const Handle(Geom_CartesianPoint)& P)

{
  Handle(StepGeom_CartesianPoint) Pstep = new StepGeom_CartesianPoint;
//  Handle(TColStd_HArray1OfReal) Acoord = new TColStd_HArray1OfReal(1,3);
  Standard_Real X, Y, Z;
  
  P->Coord(X, Y, Z);
//  Acoord->SetValue(1,X);
//  Acoord->SetValue(2,Y);
//  Acoord->SetValue(3,Z);
//  Pstep->SetCoordinates(Acoord);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
//  Pstep->SetName(name);
  Standard_Real fact = UnitsMethods::LengthFactor();
  Pstep->Init3D (name,X/fact,Y/fact,Z/fact);
  theCartesianPoint = Pstep;
  done = Standard_True;
}

//=============================================================================
// Creation d' un cartesian_point de prostep a partir d' un point 2d de Geom2d
//=============================================================================

GeomToStep_MakeCartesianPoint::
  GeomToStep_MakeCartesianPoint( const Handle(Geom2d_CartesianPoint)& P)

{
  Handle(StepGeom_CartesianPoint) Pstep = new StepGeom_CartesianPoint;
//  Handle(TColStd_HArray1OfReal) Acoord = new TColStd_HArray1OfReal(1,2);
  Standard_Real X, Y;
  
  P->Coord(X, Y);
//  Acoord->SetValue(1,X);
//  Acoord->SetValue(2,Y);
//  Pstep->SetCoordinates(Acoord);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
//  Pstep->SetName(name);
  Pstep->Init2D (name,X,Y);
  theCartesianPoint = Pstep;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_CartesianPoint) &
      GeomToStep_MakeCartesianPoint::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theCartesianPoint;
}
