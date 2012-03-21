// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <GeomToStep_MakeHyperbola.ixx>

#include <StepGeom_Hyperbola.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <Geom_Hyperbola.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>


//=============================================================================
// Creation d'une hyperbola de prostep a partir d'une hyperbola de
// Geom2d
//=============================================================================

GeomToStep_MakeHyperbola::GeomToStep_MakeHyperbola(const Handle(Geom2d_Hyperbola)& C)
{
  gp_Hypr2d gpHyp;
  gpHyp = C->Hypr2d();

  Handle(StepGeom_Hyperbola) HStep = new StepGeom_Hyperbola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement2d)  Ax2Step;
  Standard_Real                   majorR, minorR;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(gpHyp.Axis());
  Ax2Step = MkAxis2.Value();
  majorR = gpHyp.MajorRadius();
  minorR = gpHyp.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  HStep->Init(name, Ax2,majorR,minorR);
  theHyperbola = HStep;
  done = Standard_True;
}

//=============================================================================
// Creation d'une hyperbola de prostep a partir d'une hyperbola de
// Geom
//=============================================================================

 GeomToStep_MakeHyperbola::GeomToStep_MakeHyperbola(const Handle(Geom_Hyperbola)& C)
{
  gp_Hypr gpHyp;
  gpHyp = C->Hypr();

  Handle(StepGeom_Hyperbola) HStep = new StepGeom_Hyperbola;
  StepGeom_Axis2Placement            Ax2;
  Handle(StepGeom_Axis2Placement3d)  Ax2Step;
  Standard_Real                   majorR, minorR;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(gpHyp.Position());
  Ax2Step = MkAxis2.Value();
  majorR = gpHyp.MajorRadius();
  minorR = gpHyp.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Standard_Real fact = UnitsMethods::LengthFactor();
  HStep->Init(name, Ax2,majorR/fact,minorR/fact);
  theHyperbola = HStep;
  done = Standard_True;
}

//=============================================================================
// return the result
//=============================================================================

const Handle(StepGeom_Hyperbola)& GeomToStep_MakeHyperbola::Value() const 
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theHyperbola;
}

