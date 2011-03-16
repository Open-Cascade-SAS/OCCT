// File:	GeomToStep_MakeCircle.cxx
// Created:	Wed Jun 16 18:05:06 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeCircle.ixx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <Geom_Circle.hxx>
#include <StepGeom_Circle.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un cercle de prostep a partir d' un cercle 3d de gp
//=============================================================================

GeomToStep_MakeCircle::GeomToStep_MakeCircle( const gp_Circ& C)
{
#include <GeomToStep_MakeCircle_gen.pxx>
}


//=============================================================================
// Creation d' un cercle de prostep a partir d' un cercle de
// Geom
//=============================================================================

GeomToStep_MakeCircle::GeomToStep_MakeCircle( const Handle(Geom_Circle)& Cer)
{
  gp_Circ C;
  C = Cer->Circ();
#include <GeomToStep_MakeCircle_gen.pxx>
}


//=============================================================================
// Creation d' un cercle 2d de prostep a partir d' un cercle de
// Geom2d
//=============================================================================

GeomToStep_MakeCircle::GeomToStep_MakeCircle( const Handle(Geom2d_Circle)& Cer)
{
  gp_Circ2d C2d;
  C2d = Cer->Circ2d();

  Handle(StepGeom_Circle) CStep = new StepGeom_Circle;
  StepGeom_Axis2Placement Ax2;
  Handle(StepGeom_Axis2Placement2d) Ax2Step;
  Standard_Real Rayon;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(C2d.Position());
  Ax2Step = MkAxis2.Value();
  Rayon = C2d.Radius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  CStep->Init(name, Ax2, Rayon);
  theCircle = CStep;
  done = Standard_True;

}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Circle) &
      GeomToStep_MakeCircle::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theCircle;
}
