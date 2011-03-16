// File:	GeomToStep_MakePlane.cxx
// Created:	Thu Jun 17 11:49:59 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakePlane.ixx>
#include <GeomToStep_MakePlane.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Dir.hxx>
#include <Geom_Plane.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un plane de prostep a partir d' un Pln de gp
//=============================================================================

GeomToStep_MakePlane::GeomToStep_MakePlane( const gp_Pln& P)
{
  Handle(StepGeom_Plane) Plan = new StepGeom_Plane;
  Handle(StepGeom_Axis2Placement3d) aPosition;

  GeomToStep_MakeAxis2Placement3d MkAxis2(P.Position());
  aPosition = MkAxis2.Value();
  Plan->SetPosition(aPosition);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Plan->SetName(name);
  thePlane = Plan;
  done = Standard_True;
}

//=============================================================================
// Creation d' un plane de prostep a partir d' un Plane de Geom
//=============================================================================

GeomToStep_MakePlane::GeomToStep_MakePlane( const Handle(Geom_Plane)& Gpln)
{
  gp_Pln P;
  Handle(StepGeom_Plane) Plan = new StepGeom_Plane;
  Handle(StepGeom_Axis2Placement3d) aPosition;

  P = Gpln->Pln();
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(P.Position());
  aPosition = MkAxis2.Value();
  Plan->SetPosition(aPosition);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Plan->SetName(name);
  thePlane = Plan;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Plane) &
      GeomToStep_MakePlane::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return thePlane;
}

