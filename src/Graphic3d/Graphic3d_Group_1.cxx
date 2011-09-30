
#define XDESTROY

// File		Graphic3d_Group_1.cxx (Clear Remove Open Close)
// Created	Fevrier 1992
// Author	NW,JPB,CAL

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

void Graphic3d_Group::Destroy () {

#ifdef DESTROY
	cout << "Graphic3d_Group::Destroy ()\n";
#endif

  // tell graphics driver to clear internal resources of the group
  if (!IsEmpty () && !MyGraphicDriver.IsNull ())
    MyGraphicDriver->ClearGroup (MyCGroup);

}

void Graphic3d_Group::Remove () {

	if (IsDeleted ()) return;

	MyCGroup.IsDeleted	= 1;
	MyGraphicDriver->RemoveGroup (MyCGroup);
	MyStructure->Remove (this);
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

void Graphic3d_Group::BeginPrimitives () {

	if (IsDeleted ()) return;
	if (MyCGroup.IsOpen) return;

	MyCGroup.IsOpen	= 1;

	MyGraphicDriver->OpenGroup (MyCGroup);

}

void Graphic3d_Group::EndPrimitives () {

	if (IsDeleted ()) return;
	if (! MyCGroup.IsOpen) return;

	MyCGroup.IsOpen	= 0;

	MyGraphicDriver->CloseGroup (MyCGroup);

}

/*void Graphic3d_Group::SetTransformPersistence(  const Graphic3d_TransModeFlags& AFlag )
{
	//MyCGroup.TransformPersistenceFlag = AFlag;
}

Graphic3d_TransModeFlags Graphic3d_Group::TransformPersistence() const
{
	//return MyCGroup.TransformPersistenceFlag;
}*/
