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

//-Version	

//-Design	Declaration des variables specifiques aux groupes
//		de primitives

//-Warning	Un groupe est defini dans une structure
//		Il s'agit de la plus petite entite editable

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

//-Methods, in order

void Graphic3d_Group::Clear (Standard_Boolean theUpdateStructureMgr)
{
  if (IsDeleted ()) return;

  MyCGroup.ContextLine.IsSet     = 0,
  MyCGroup.ContextText.IsSet     = 0,
  MyCGroup.ContextMarker.IsSet   = 0,
  MyCGroup.ContextFillArea.IsSet = 0;

  MyCGroup.ContextLine.IsDef     = 0,
  MyCGroup.ContextText.IsDef     = 0,
  MyCGroup.ContextMarker.IsDef   = 0,
  MyCGroup.ContextFillArea.IsDef = 0;

  MyCGroup.PickId.IsDef = 0,
  MyCGroup.PickId.IsSet = 0,
  MyCGroup.PickId.Value = 0;

  MyBounds.XMin	= ShortRealLast (),
  MyBounds.YMin	= ShortRealLast (),
  MyBounds.ZMin	= ShortRealLast ();

  MyBounds.XMax	= ShortRealFirst (),
  MyBounds.YMax	= ShortRealFirst (),
  MyBounds.ZMax	= ShortRealFirst ();

  if (MyContainsFacet) MyStructure->GroupsWithFacet (-1);
  MyContainsFacet = Standard_False,
  MyIsEmpty       = Standard_True;

  MyGraphicDriver->ClearGroup (MyCGroup);

  // clear method could be used on Graphic3d_Structure destruction,
  // and its structure manager could be already destroyed, in that
  // case we don't need to update it;
  if (theUpdateStructureMgr)
    Update ();
}

void Graphic3d_Group::Destroy ()
{
  // tell graphics driver to clear internal resources of the group
  if (!IsEmpty () && !MyGraphicDriver.IsNull ())
    MyGraphicDriver->ClearGroup (MyCGroup);

}

void Graphic3d_Group::Remove ()
{
	if (IsDeleted ()) return;

	MyGraphicDriver->RemoveGroup (MyCGroup);
	MyStructure->Remove (this);
	MyCGroup.ptrGroup	= NULL;

	Update ();

	MyBounds.XMin	= ShortRealLast (),
	MyBounds.YMin	= ShortRealLast (),
	MyBounds.ZMin	= ShortRealLast ();

	MyBounds.XMax	= ShortRealFirst (),
	MyBounds.YMax	= ShortRealFirst (),
	MyBounds.ZMax	= ShortRealFirst ();

	if (MyContainsFacet) MyStructure->GroupsWithFacet (-1);
	MyContainsFacet	= Standard_False,
	MyIsEmpty	= Standard_True;
}
