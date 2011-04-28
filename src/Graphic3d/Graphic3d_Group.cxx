
// File		Graphic3d_Group.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL
//              11/97 ; CAL : retrait DownCast

//-Copyright	MatraDatavision 1991,1992

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
