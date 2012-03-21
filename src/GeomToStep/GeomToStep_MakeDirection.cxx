// Created on: 1993-06-17
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


#include <GeomToStep_MakeDirection.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <Geom_Direction.hxx>
#include <Geom2d_Direction.hxx>
#include <StepGeom_Direction.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une direction de prostep a partir d' une Dir de gp
//=============================================================================

GeomToStep_MakeDirection::GeomToStep_MakeDirection( const gp_Dir& D)
{
  Handle(StepGeom_Direction) Dir = new StepGeom_Direction;
  Handle(TColStd_HArray1OfReal) aDirRatios = new TColStd_HArray1OfReal(1,3);
  Standard_Real X, Y, Z;

  D.Coord(X, Y, Z);
  aDirRatios->SetValue(1,X);
  aDirRatios->SetValue(2,Y);
  aDirRatios->SetValue(3,Z);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Dir->Init(name, aDirRatios);
  theDirection = Dir;
  done = Standard_True;
}
//=============================================================================
// Creation d' une direction de prostep a partir d' une Dir2d de gp
//=============================================================================

GeomToStep_MakeDirection::GeomToStep_MakeDirection( const gp_Dir2d& D)
{
  Handle(StepGeom_Direction) Dir = new StepGeom_Direction;
  Handle(TColStd_HArray1OfReal) aDirRatios = new TColStd_HArray1OfReal(1,2);
  Standard_Real X, Y;

  D.Coord(X, Y);
  aDirRatios->SetValue(1,X);
  aDirRatios->SetValue(2,Y);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Dir->Init(name, aDirRatios);
  theDirection = Dir;
  done = Standard_True;
}

//=============================================================================
// Creation d' une direction de prostep a partir d' une Direction de Geom
//=============================================================================

GeomToStep_MakeDirection::GeomToStep_MakeDirection
  ( const Handle(Geom_Direction)& Direc)
{
  gp_Dir D;
  Handle(StepGeom_Direction) Dir = new StepGeom_Direction;
  Handle(TColStd_HArray1OfReal) aDirRatios = new TColStd_HArray1OfReal(1,3);
  Standard_Real X, Y, Z;

  D=Direc->Dir();
  D.Coord(X, Y, Z);
  aDirRatios->SetValue(1,X);
  aDirRatios->SetValue(2,Y);
  aDirRatios->SetValue(3,Z);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Dir->Init(name, aDirRatios);
  theDirection = Dir;
  done = Standard_True;

}
//=============================================================================
// Creation d' une direction de prostep a partir d' une Direction de Geom2d
//=============================================================================

GeomToStep_MakeDirection::GeomToStep_MakeDirection
  ( const Handle(Geom2d_Direction)& Direc)
{
  gp_Dir2d D;
  Handle(StepGeom_Direction) Dir = new StepGeom_Direction;
  Handle(TColStd_HArray1OfReal) aDirRatios = new TColStd_HArray1OfReal(1,2);
  Standard_Real X, Y;

  D=Direc->Dir2d();
  D.Coord(X, Y);
  aDirRatios->SetValue(1,X);
  aDirRatios->SetValue(2,Y);
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Dir->Init(name, aDirRatios);
  theDirection = Dir;
  done = Standard_True;

}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Direction) &
      GeomToStep_MakeDirection::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theDirection;
}
