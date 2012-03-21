// Created on: 1993-06-16
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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
