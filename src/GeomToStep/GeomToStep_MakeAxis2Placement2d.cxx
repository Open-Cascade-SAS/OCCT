// Created on: 1994-08-26
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



#include <GeomToStep_MakeAxis2Placement2d.ixx>
#include <StepGeom_Direction.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>

#include <StepGeom_Axis2Placement2d.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Ax2 de gp
//=============================================================================

GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
( const gp_Ax2& A)
{
  Handle(StepGeom_Axis2Placement2d) Axe;
  Handle(StepGeom_CartesianPoint) P;
  Handle(StepGeom_Direction) D;

  GeomToStep_MakeCartesianPoint MkPoint(A.Location());
  GeomToStep_MakeDirection      MkDir(A.Direction());

  P = MkPoint.Value();
  D = MkDir.Value();

  Axe = new StepGeom_Axis2Placement2d;
  Axe->SetLocation(P);
  Axe->SetRefDirection(D);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Axe->SetName(name);
  theAxis2Placement2d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Ax22d de gp
//=============================================================================

GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
( const gp_Ax22d& A)
{
  Handle(StepGeom_Axis2Placement2d) Axe;
  Handle(StepGeom_CartesianPoint) P;
  Handle(StepGeom_Direction) D1;

  GeomToStep_MakeCartesianPoint MkPoint(A.Location());
  GeomToStep_MakeDirection      MkDir(A.XDirection());

  P  = MkPoint.Value();
  D1 = MkDir.Value();

  Axe = new StepGeom_Axis2Placement2d;
  Axe->SetLocation(P);
  Axe->SetRefDirection(D1);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Axe->SetName(name);
  theAxis2Placement2d = Axe;
  done = Standard_True;
}

//=============================================================================
// Creation d' un axis2_placement_2d de prostep a partir d' un Axis2Placement
// de Geom
//=============================================================================

//GeomToStep_MakeAxis2Placement2d::GeomToStep_MakeAxis2Placement2d
//  ( const Handle(Geom_Axis2Placement)& Axis2)
//{
//  Handle(StepGeom_Axis2Placement2d) Axe;
//  Handle(StepGeom_CartesianPoint) P;
//  Handle(StepGeom_Direction) D1, D2;
//  gp_Ax2 A;
  
//  A = Axis2->Ax2();
//  P = GeomToStep_MakeCartesianPoint(A.Location());
//  D1 = GeomToStep_MakeDirection(A.Direction());
//  D2 = GeomToStep_MakeDirection(A.XDirection());
//  Axe = new StepGeom_Axis2Placement2d;
//  Axe->SetLocation(P);
//  Axe->SetAxis(D1);  
//  Axe->SetRefDirection(D2);
//  theAxis2Placement2d = Axe;
//  done = Standard_True;
//}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Axis2Placement2d) &
      GeomToStep_MakeAxis2Placement2d::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theAxis2Placement2d;
}
