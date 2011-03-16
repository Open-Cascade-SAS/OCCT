
// File		Graphic3d_Group.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL
//              11/97 ; CAL : retrait DownCast

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux groupes
//		de primitives

//-Warning	Un groupe est defini dans une structure
//		Il s'agit de la plus petite entite editable

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
// Un petit commentaire sur l'utilisation du This !
//
// Un Graphic3d_Group est cree dans une structure. Graphic3d_Structure est
// donc un manager de Graphic3d_Group. Dans le constructeur de Graphic3d_Group
// est appele une methode de Graphic3d_Structure, Add, qui permet d'ajouter
// l'instance du Graphic3d_Group dans son manager. Il y a donc 2 references
// a <me> et tout va bien.
//
// This () est l'instance de la classe, c-a-d le groupe courant
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
