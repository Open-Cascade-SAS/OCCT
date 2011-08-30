// File:      Graphic3d_Group_10.cxx (Text)
// Created:   Fevrier 1992
// Author:    NW,JPB,CAL
// Copyright: OPEN CASCADE 1992

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

#include <TCollection_AsciiString.hxx>

//-Methods, in order

void Graphic3d_Group::Text (
                       const Standard_CString AText,
                       const Graphic3d_Vertex& APoint,
                       const Standard_Real AHeight,
                       const Quantity_PlaneAngle AAngle,
                       const Graphic3d_TextPath ATp,
                       const Graphic3d_HorizontalTextAlignment AHta,
                       const Graphic3d_VerticalTextAlignment AVta,
                       const Standard_Boolean EvalMinMax
                      )
{
  if (IsDeleted ()) return;

  MyIsEmpty = Standard_False;

  // Min-Max Update
  if (EvalMinMax) {
    Standard_Real X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
    if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
    if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
    if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
  }

  MyGraphicDriver->Text(MyCGroup, AText, APoint, AHeight, AAngle, ATp, AHta, AVta, EvalMinMax);

  Update ();
}

void Graphic3d_Group::Text (
                       const Standard_CString AText, 
                       const Graphic3d_Vertex& APoint, 
                       const Standard_Real AHeight, 
                       const Standard_Boolean EvalMinMax
                      )
{
  if (IsDeleted ()) return;

  MyIsEmpty = Standard_False;

  // Min-Max Update
  if (EvalMinMax) {
    Standard_Real X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
    if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
    if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
    if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
  }

  MyGraphicDriver->Text (MyCGroup, AText, APoint, AHeight, EvalMinMax);

  Update ();
}

void Graphic3d_Group::Text (
                       const TCollection_ExtendedString& AText, 
                       const Graphic3d_Vertex& APoint, 
                       const Standard_Real AHeight, 
                       const Quantity_PlaneAngle AAngle, 
                       const Graphic3d_TextPath ATp, 
                       const Graphic3d_HorizontalTextAlignment AHta, 
                       const Graphic3d_VerticalTextAlignment AVta, 
                       const Standard_Boolean EvalMinMax
                      )
{
  if (IsDeleted ()) return;

  MyIsEmpty	= Standard_False;

  // Min-Max Update
  if (EvalMinMax) {
    Standard_Real X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
    if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
    if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
    if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
  }

  MyGraphicDriver->Text(MyCGroup, AText, APoint, AHeight, AAngle, ATp, AHta, AVta, EvalMinMax);

  Update ();
}

void Graphic3d_Group::Text (
                       const TCollection_ExtendedString& AText, 
                       const Graphic3d_Vertex& APoint, 
                       const Standard_Real AHeight, 
                       const Standard_Boolean EvalMinMax
                      )
{
  if (IsDeleted ()) return;

  MyIsEmpty = Standard_False;

  // Min-Max Update
  if (EvalMinMax) {
    Standard_Real X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
    if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
    if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
    if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
  }

  MyGraphicDriver->Text (MyCGroup, AText, APoint, AHeight, EvalMinMax);

  Update ();
}
