// Created on: 1994-09-02
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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



#include <GeomToStep_MakeEllipse.ixx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement2d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_Ellipse.hxx>
#include <gp_Elips2d.hxx>
#include <StepGeom_Ellipse.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d'une ellipse de prostep a partir d'une ellipse 3d de gp
//=============================================================================

GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const gp_Elips& E)
{
#include <GeomToStep_MakeEllipse_gen.pxx>
}


//=============================================================================
// Creation d'une ellipse de prostep a partir d'une ellipse de
// Geom
//=============================================================================

GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const Handle(Geom_Ellipse)& Cer)
{
  gp_Elips E;
  E = Cer->Elips();
#include <GeomToStep_MakeEllipse_gen.pxx>
}


//=============================================================================
// Creation d'une ellipse 2d de prostep a partir d'une ellipse de
// Geom2d
//=============================================================================

GeomToStep_MakeEllipse::GeomToStep_MakeEllipse( const Handle(Geom2d_Ellipse)& Cer)
{
  gp_Elips2d E2d;
  E2d = Cer->Elips2d();

  Handle(StepGeom_Ellipse) EStep = new StepGeom_Ellipse;
  StepGeom_Axis2Placement Ax2;
  Handle(StepGeom_Axis2Placement2d) Ax2Step;
  Standard_Real majorR, minorR;
  
  GeomToStep_MakeAxis2Placement2d MkAxis2(E2d.Axis());
  Ax2Step = MkAxis2.Value();
  majorR = E2d.MajorRadius();
  minorR = E2d.MinorRadius();
  Ax2.SetValue(Ax2Step);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  EStep->Init(name, Ax2, majorR, minorR);
  theEllipse = EStep;
  done = Standard_True;

}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Ellipse) &
      GeomToStep_MakeEllipse::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theEllipse;
}
