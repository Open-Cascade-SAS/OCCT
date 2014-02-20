// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

//              11/97 ; CAL : retrait DownCast


//-Version

//-Design	Declaration of variables specific to groups
//		of primitives

//-Warning	A group is definedv in a structure
//		This is the smallest editable entity

//-References

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Group.ixx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransModeFlags.hxx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_Group::Graphic3d_Group (const Handle(Graphic3d_Structure)& AStructure):
MyListOfPArray()
{
  MyBounds.XMin	= ShortRealLast (),
  MyBounds.YMin	= ShortRealLast (),
  MyBounds.ZMin	= ShortRealLast ();

  MyBounds.XMax	= ShortRealFirst (),
  MyBounds.YMax	= ShortRealFirst (),
  MyBounds.ZMax	= ShortRealFirst ();

//
// A small commentary on the usage of This !
//
// Graphic3d_Group is created in a structure. Graphic3d_Structure is a
// manager of Graphic3d_Group. In the constructor of Graphic3d_Group
// a method Add of Graphic3d_Structure is called. It allows adding
// the instance of Graphic3d_Group in its manager. So there are 2 references
// to <me> and everything works well.
//
// This () is the instance of the class, the current groupe
//Handle(Graphic3d_Group) me	= Handle(Graphic3d_Group)::DownCast (This ());

  MyPtrStructure = (void *) AStructure.operator->();
  MyStructure->Add (this);

  MyContainsFacet     = Standard_False,
  MyIsEmpty           = Standard_True;

  MyCGroup.Struct     = MyStructure->CStructure();
  MyCGroup.Struct->Id = int (MyStructure->Identification ());
  MyCGroup.ptrGroup	= NULL;

  MyCGroup.ContextLine.IsDef     = 0,
  MyCGroup.ContextText.IsDef     = 0,
  MyCGroup.ContextMarker.IsDef   = 0,
  MyCGroup.ContextFillArea.IsDef = 0;

  MyCGroup.ContextLine.IsSet     = 0,
  MyCGroup.ContextText.IsSet     = 0,
  MyCGroup.ContextMarker.IsSet   = 0,
  MyCGroup.ContextFillArea.IsSet = 0;

  MyCGroup.PickId.IsDef = 0,
  MyCGroup.PickId.IsSet = 0,
  MyCGroup.PickId.Value = 0;

  MyGraphicDriver = (MyStructure->StructureManager())->GraphicDriver();


  MyGraphicDriver->Group (MyCGroup);

  MyMarkWidth = 0;
  MyMarkHeight = 0;
  MyMarkArray.Nullify();
}
