// Created on: 1993-06-17
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom2d_Direction.hxx>
#include <Geom_Direction.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeDirection::Value() - no result");
  return theDirection;
}
