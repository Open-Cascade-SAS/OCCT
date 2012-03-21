// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Graphic3d_GraphicDevice.hxx>
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

Standard_Integer TheLabelBegin, TheLabelEnd;

	// MyStructure	= AStructure;
	MyPtrStructure	= (void *) AStructure.operator->();
	MyStructure->GroupLabels (TheLabelBegin, TheLabelEnd);
	MyStructure->Add (this);

	MyContainsFacet			= Standard_False,
	MyIsEmpty			= Standard_True;

	MyCGroup.Struct	= (CALL_DEF_STRUCTURE *) (MyStructure->CStructure ());
	MyCGroup.Struct->Id	= int (MyStructure->Identification ());
	MyCGroup.IsDeleted	= 0;
	MyCGroup.IsOpen		= 0;
	MyCGroup.LabelBegin	= int (TheLabelBegin);
	MyCGroup.LabelEnd	= int (TheLabelEnd);

	MyCGroup.StructureEnd	= Structure_END;

	MyCGroup.ContextLine.IsDef	= 0,
	MyCGroup.ContextText.IsDef	= 0,
	MyCGroup.ContextMarker.IsDef	= 0,
	MyCGroup.ContextFillArea.IsDef	= 0;

	MyCGroup.ContextLine.IsSet	= 0,
	MyCGroup.ContextText.IsSet	= 0,
	MyCGroup.ContextMarker.IsSet	= 0,
	MyCGroup.ContextFillArea.IsSet	= 0;

	MyCGroup.PickId.IsDef	= 0,
	MyCGroup.PickId.IsSet	= 0,
	MyCGroup.PickId.Value	= 0;

Handle(Aspect_GraphicDriver) agd =
((MyStructure->StructureManager ())->GraphicDevice ())->GraphicDriver ();

	MyGraphicDriver	= *(Handle(Graphic3d_GraphicDriver) *) &agd;

	MyGraphicDriver->Group (MyCGroup);

        //MyCGroup.TransformPersistenceFlag = Graphic3d_TMF_None;
        //MyCGroup.Struct->TransformPersistenceFlag = Graphic3d_TMF_None;
        MyMarkWidth = 0;
	MyMarkHeight = 0;
        MyMarkArray.Nullify();
}
