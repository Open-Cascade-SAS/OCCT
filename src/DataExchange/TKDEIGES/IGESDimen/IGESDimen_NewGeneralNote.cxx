// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_NewGeneralNote, IGESData_IGESEntity)

IGESDimen_NewGeneralNote::IGESDimen_NewGeneralNote() {}

void IGESDimen_NewGeneralNote::Init(
  const double                            width,
  const double                            height,
  const int                         justifyCode,
  const gp_XYZ&                                  areaLoc,
  const double                            areaRotationAngle,
  const gp_XYZ&                                  baseLinePos,
  const double                            normalInterlineSpace,
  const occ::handle<NCollection_HArray1<int>>&        charDisplays,
  const occ::handle<NCollection_HArray1<double>>&           charWidths,
  const occ::handle<NCollection_HArray1<double>>&           charHeights,
  const occ::handle<NCollection_HArray1<double>>&           interCharSpc,
  const occ::handle<NCollection_HArray1<double>>&           interLineSpc,
  const occ::handle<NCollection_HArray1<int>>&        fontStyles,
  const occ::handle<NCollection_HArray1<double>>&           charAngles,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& controlCodeStrings,
  const occ::handle<NCollection_HArray1<int>>&        nbChars,
  const occ::handle<NCollection_HArray1<double>>&           boxWidths,
  const occ::handle<NCollection_HArray1<double>>&           boxHeights,
  const occ::handle<NCollection_HArray1<int>>&        charSetCodes,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&    charSetEntities,
  const occ::handle<NCollection_HArray1<double>>&           slAngles,
  const occ::handle<NCollection_HArray1<double>>&           rotAngles,
  const occ::handle<NCollection_HArray1<int>>&        mirrorFlags,
  const occ::handle<NCollection_HArray1<int>>&        rotateFlags,
  const occ::handle<NCollection_HArray1<gp_XYZ>>&             startPoints,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& texts)
{
  int num = nbChars->Length();

  if (nbChars->Lower() != 1 || (charDisplays->Lower() != 1 || charDisplays->Length() != num)
      || (charWidths->Lower() != 1 || charWidths->Length() != num)
      || (charHeights->Lower() != 1 || charHeights->Length() != num)
      || (interCharSpc->Lower() != 1 || interCharSpc->Length() != num)
      || (interLineSpc->Lower() != 1 || interLineSpc->Length() != num)
      || (fontStyles->Lower() != 1 || fontStyles->Length() != num)
      || (charAngles->Lower() != 1 || charAngles->Length() != num)
      || (controlCodeStrings->Lower() != 1 || controlCodeStrings->Length() != num)
      || (boxWidths->Lower() != 1 || boxWidths->Length() != num)
      || (boxHeights->Lower() != 1 || boxHeights->Length() != num)
      || (charSetCodes->Lower() != 1 || charSetCodes->Length() != num)
      || (charSetEntities->Lower() != 1 || charSetEntities->Length() != num)
      || (slAngles->Lower() != 1 || slAngles->Length() != num)
      || (rotAngles->Lower() != 1 || rotAngles->Length() != num)
      || (mirrorFlags->Lower() != 1 || mirrorFlags->Length() != num)
      || (rotateFlags->Lower() != 1 || rotateFlags->Length() != num)
      || (startPoints->Lower() != 1 || startPoints->Length() != num)
      || (texts->Lower() != 1 || texts->Length() != num))
    throw Standard_DimensionMismatch("IGESDimen_GeneralNote : Init");

  theWidth                = width;
  theHeight               = height;
  theJustifyCode          = justifyCode;
  theAreaLoc              = areaLoc;
  theAreaRotationAngle    = areaRotationAngle;
  theBaseLinePos          = baseLinePos;
  theNormalInterlineSpace = normalInterlineSpace;
  theCharDisplays         = charDisplays;
  theCharWidths           = charWidths;
  theCharHeights          = charHeights;
  theInterCharSpaces      = interCharSpc;
  theInterlineSpaces      = interLineSpc;
  theFontStyles           = fontStyles;
  theCharAngles           = charAngles;
  theControlCodeStrings   = controlCodeStrings;
  theNbChars              = nbChars;
  theBoxWidths            = boxWidths;
  theBoxHeights           = boxHeights;
  theCharSetCodes         = charSetCodes;
  theCharSetEntities      = charSetEntities;
  theSlantAngles          = slAngles;
  theRotationAngles       = rotAngles;
  theMirrorFlags          = mirrorFlags;
  theRotateFlags          = rotateFlags;
  theStartPoints          = startPoints;
  theTexts                = texts;
  InitTypeAndForm(213, 0);
}

double IGESDimen_NewGeneralNote::TextWidth() const
{
  return theWidth;
}

double IGESDimen_NewGeneralNote::TextHeight() const
{
  return theHeight;
}

int IGESDimen_NewGeneralNote::JustifyCode() const
{
  return theJustifyCode;
}

gp_Pnt IGESDimen_NewGeneralNote::AreaLocation() const
{
  gp_Pnt loc(theAreaLoc);
  return loc;
}

gp_Pnt IGESDimen_NewGeneralNote::TransformedAreaLocation() const
{
  gp_XYZ tempXYZ = theAreaLoc;
  if (HasTransf())
    Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

double IGESDimen_NewGeneralNote::ZDepthAreaLocation() const
{
  return (theAreaLoc.Z());
}

double IGESDimen_NewGeneralNote::AreaRotationAngle() const
{
  return theAreaRotationAngle;
}

gp_Pnt IGESDimen_NewGeneralNote::BaseLinePosition() const
{
  gp_Pnt pos(theBaseLinePos);
  return pos;
}

gp_Pnt IGESDimen_NewGeneralNote::TransformedBaseLinePosition() const
{
  gp_XYZ tempXYZ = theBaseLinePos;
  if (HasTransf())
    Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

double IGESDimen_NewGeneralNote::ZDepthBaseLinePosition() const
{
  return (theBaseLinePos.Z());
}

double IGESDimen_NewGeneralNote::NormalInterlineSpace() const
{
  return theNormalInterlineSpace;
}

int IGESDimen_NewGeneralNote::NbStrings() const
{
  return theCharDisplays->Length();
}

int IGESDimen_NewGeneralNote::CharacterDisplay(const int Index) const
{
  return theCharDisplays->Value(Index);
}

bool IGESDimen_NewGeneralNote::IsVariable(const int Index) const
{
  return (theCharDisplays->Value(Index) == 1);
}

double IGESDimen_NewGeneralNote::CharacterWidth(const int Index) const
{
  return theCharWidths->Value(Index);
}

double IGESDimen_NewGeneralNote::CharacterHeight(const int Index) const
{
  return theCharHeights->Value(Index);
}

double IGESDimen_NewGeneralNote::InterCharacterSpace(const int Index) const
{
  return theInterCharSpaces->Value(Index);
}

double IGESDimen_NewGeneralNote::InterlineSpace(const int Index) const
{
  return theInterlineSpaces->Value(Index);
}

int IGESDimen_NewGeneralNote::FontStyle(const int Index) const
{
  return theFontStyles->Value(Index);
}

double IGESDimen_NewGeneralNote::CharacterAngle(const int Index) const
{
  return theCharAngles->Value(Index);
}

occ::handle<TCollection_HAsciiString> IGESDimen_NewGeneralNote::ControlCodeString(
  const int Index) const
{
  return theControlCodeStrings->Value(Index);
}

int IGESDimen_NewGeneralNote::NbCharacters(const int Index) const
{
  return theNbChars->Value(Index);
}

double IGESDimen_NewGeneralNote::BoxWidth(const int Index) const
{
  return theBoxWidths->Value(Index);
}

double IGESDimen_NewGeneralNote::BoxHeight(const int Index) const
{
  return theBoxHeights->Value(Index);
}

bool IGESDimen_NewGeneralNote::IsCharSetEntity(const int Index) const
{
  return (!(theCharSetEntities->Value(Index)).IsNull());
}

int IGESDimen_NewGeneralNote::CharSetCode(const int Index) const
{
  return theCharSetCodes->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESDimen_NewGeneralNote::CharSetEntity(
  const int Index) const
{
  return theCharSetEntities->Value(Index);
}

double IGESDimen_NewGeneralNote::SlantAngle(const int Index) const
{
  return theSlantAngles->Value(Index);
}

double IGESDimen_NewGeneralNote::RotationAngle(const int Index) const
{
  return theRotationAngles->Value(Index);
}

int IGESDimen_NewGeneralNote::MirrorFlag(const int Index) const
{
  return theMirrorFlags->Value(Index);
}

bool IGESDimen_NewGeneralNote::IsMirrored(const int Index) const
{
  return (theMirrorFlags->Value(Index) != 0);
}

int IGESDimen_NewGeneralNote::RotateFlag(const int Index) const
{
  return theRotateFlags->Value(Index);
}

gp_Pnt IGESDimen_NewGeneralNote::StartPoint(const int Index) const
{
  return gp_Pnt(theStartPoints->Value(Index));
}

gp_Pnt IGESDimen_NewGeneralNote::TransformedStartPoint(const int Index) const
{
  gp_XYZ tempXYZ = theStartPoints->Value(Index);
  if (HasTransf())
    Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

double IGESDimen_NewGeneralNote::ZDepthStartPoint(const int Index) const
{
  return (theStartPoints->Value(Index).Z());
}

occ::handle<TCollection_HAsciiString> IGESDimen_NewGeneralNote::Text(const int Index) const
{
  return theTexts->Value(Index);
}
