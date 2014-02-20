// Created by: NW,JPB,CAL
// Copyright (c) 1995-1999 Matra Datavision
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
    Standard_ShortReal X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= X;
    if (Y < MyBounds.YMin) MyBounds.YMin	= Y;
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Z;
    if (X > MyBounds.XMax) MyBounds.XMax	= X;
    if (Y > MyBounds.YMax) MyBounds.YMax	= Y;
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Z;
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
    Standard_ShortReal X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= X;
    if (Y < MyBounds.YMin) MyBounds.YMin	= Y;
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Z;
    if (X > MyBounds.XMax) MyBounds.XMax	= X;
    if (Y > MyBounds.YMax) MyBounds.YMax	= Y;
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Z;
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
    Standard_ShortReal X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= X;
    if (Y < MyBounds.YMin) MyBounds.YMin	= Y;
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Z;
    if (X > MyBounds.XMax) MyBounds.XMax	= X;
    if (Y > MyBounds.YMax) MyBounds.YMax	= Y;
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Z;
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
    Standard_ShortReal X, Y, Z;
    APoint.Coord (X, Y, Z);
    if (X < MyBounds.XMin) MyBounds.XMin	= X;
    if (Y < MyBounds.YMin) MyBounds.YMin	= Y;
    if (Z < MyBounds.ZMin) MyBounds.ZMin	= Z;
    if (X > MyBounds.XMax) MyBounds.XMax	= X;
    if (Y > MyBounds.YMax) MyBounds.YMax	= Y;
    if (Z > MyBounds.ZMax) MyBounds.ZMax	= Z;
  }

  MyGraphicDriver->Text (MyCGroup, AText, APoint, AHeight, EvalMinMax);

  Update ();
}
