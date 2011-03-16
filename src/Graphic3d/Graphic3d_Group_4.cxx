
// File		Graphic3d_Group_4.cxx (PickId)
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

Standard_Integer Graphic3d_Group::PickId () const {

	return (Standard_Integer (MyCGroup.PickId.Value));

}

void Graphic3d_Group::RemovePickId () {

	if (IsDeleted ()) return;

	MyCGroup.PickId.IsDef	= 0;
	MyCGroup.PickId.Value	= 0;

	MyGraphicDriver->PickId (MyCGroup);

	MyCGroup.PickId.IsSet	= 0;

}

void Graphic3d_Group::SetPickId (const Standard_Integer Id) {

	if (IsDeleted ()) return;

	if (Id <= 0)
		Graphic3d_PickIdDefinitionError::Raise ("Bad value for PickId");

	MyCGroup.PickId.IsDef	= 1;
	MyCGroup.PickId.Value	= int (Id);

	MyGraphicDriver->PickId (MyCGroup);

	MyCGroup.PickId.IsSet	= 1;

}
