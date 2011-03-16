#include <GeomToStep_MakeParabola.ixx>

#include <StepGeom_Parabola.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <Geom_Parabola.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>


//=============================================================================
// Creation d'une Parabola de prostep a partir d'une Parabola de
// Geom2d
//=============================================================================

 GeomToStep_MakeParabola::GeomToStep_MakeParabola(const Handle(Geom2d_Parabola)& C)
{
  gp_Parab2d gpPar;
  gpPar = C->Parab2d();

  Handle(StepGeom_Parabola) PStep = new StepGeom_Parabola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement2d)  Ax2Step;
  Standard_Real                   focal;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(gpPar.Axis());
  Ax2Step = MkAxis2.Value();
  focal = gpPar.Focal();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  PStep->Init(name, Ax2, focal);
  theParabola = PStep;
  done = Standard_True;
}

//=============================================================================
// Creation d'une Parabola de prostep a partir d'une Parabola de
// Geom
//=============================================================================

 GeomToStep_MakeParabola::GeomToStep_MakeParabola(const Handle(Geom_Parabola)& C)
{
  gp_Parab gpPar;
  gpPar = C->Parab();

  Handle(StepGeom_Parabola) PStep = new StepGeom_Parabola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement3d)  Ax2Step;
  Standard_Real                   focal;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(gpPar.Position());
  Ax2Step = MkAxis2.Value();
  focal = gpPar.Focal();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  PStep->Init(name, Ax2, focal / UnitsMethods::LengthFactor());
  theParabola = PStep;
  done = Standard_True;
}

//=============================================================================
// return the result
//=============================================================================

const Handle(StepGeom_Parabola)& GeomToStep_MakeParabola::Value() const 
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theParabola;
}

