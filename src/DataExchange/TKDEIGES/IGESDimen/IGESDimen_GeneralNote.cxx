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
#include <IGESDimen_GeneralNote.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_GeneralNote, IGESData_IGESEntity)

IGESDimen_GeneralNote::IGESDimen_GeneralNote() = default;

void IGESDimen_GeneralNote::Init(
  const occ::handle<NCollection_HArray1<int>>&                                   nbChars,
  const occ::handle<NCollection_HArray1<double>>&                                widths,
  const occ::handle<NCollection_HArray1<double>>&                                heights,
  const occ::handle<NCollection_HArray1<int>>&                                   fontCodes,
  const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextFontDef>>>&    fonts,
  const occ::handle<NCollection_HArray1<double>>&                                slants,
  const occ::handle<NCollection_HArray1<double>>&                                rotations,
  const occ::handle<NCollection_HArray1<int>>&                                   mirrorFlags,
  const occ::handle<NCollection_HArray1<int>>&                                   rotFlags,
  const occ::handle<NCollection_HArray1<gp_XYZ>>&                                start,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& texts)
{
  int num = nbChars->Length();

  if (nbChars->Lower() != 1 || (widths->Lower() != 1 || widths->Length() != num)
      || (heights->Lower() != 1 || heights->Length() != num)
      || (fontCodes->Lower() != 1 || fontCodes->Length() != num)
      || (fonts->Lower() != 1 || fonts->Length() != num)
      || (slants->Lower() != 1 || slants->Length() != num)
      || (rotations->Lower() != 1 || rotations->Length() != num)
      || (mirrorFlags->Lower() != 1 || mirrorFlags->Length() != num)
      || (rotFlags->Lower() != 1 || rotFlags->Length() != num)
      || (start->Lower() != 1 || start->Length() != num)
      || (texts->Lower() != 1 || texts->Length() != num))
    throw Standard_DimensionMismatch("IGESDimen_GeneralNote : Init");

  theNbChars        = nbChars;
  theBoxWidths      = widths;
  theBoxHeights     = heights;
  theFontCodes      = fontCodes;
  theFontEntities   = fonts;
  theSlantAngles    = slants;
  theRotationAngles = rotations;
  theMirrorFlags    = mirrorFlags;
  theRotateFlags    = rotFlags;
  theStartPoints    = start;
  theTexts          = texts;
  InitTypeAndForm(212, FormNumber());
  // FormNumber for Graphical Representation
}

void IGESDimen_GeneralNote::SetFormNumber(const int form)
{
  if ((form < 0 || form > 8) && (form < 100 || form > 102) && form != 105)
    throw Standard_OutOfRange("IGESDimen_GeneralNote : SetFormNumber");
  InitTypeAndForm(212, form);
}

int IGESDimen_GeneralNote::NbStrings() const
{
  return theNbChars->Length();
}

int IGESDimen_GeneralNote::NbCharacters(const int Index) const
{
  return theNbChars->Value(Index);
}

double IGESDimen_GeneralNote::BoxWidth(const int Index) const
{
  return theBoxWidths->Value(Index);
}

double IGESDimen_GeneralNote::BoxHeight(const int Index) const
{
  return theBoxHeights->Value(Index);
}

bool IGESDimen_GeneralNote::IsFontEntity(const int Index) const
{
  return (!(theFontEntities->Value(Index)).IsNull());
}

int IGESDimen_GeneralNote::FontCode(const int Index) const
{
  return theFontCodes->Value(Index);
}

occ::handle<IGESGraph_TextFontDef> IGESDimen_GeneralNote::FontEntity(const int Index) const
{
  return theFontEntities->Value(Index);
}

double IGESDimen_GeneralNote::SlantAngle(const int Index) const
{
  return theSlantAngles->Value(Index);
}

double IGESDimen_GeneralNote::RotationAngle(const int Index) const
{
  return theRotationAngles->Value(Index);
}

int IGESDimen_GeneralNote::MirrorFlag(const int Index) const
{
  return theMirrorFlags->Value(Index);
}

int IGESDimen_GeneralNote::RotateFlag(const int Index) const
{
  return theRotateFlags->Value(Index);
}

gp_Pnt IGESDimen_GeneralNote::StartPoint(const int Index) const
{
  gp_Pnt point(theStartPoints->Value(Index));
  return point;
}

gp_Pnt IGESDimen_GeneralNote::TransformedStartPoint(const int Index) const
{
  gp_XYZ point = (theStartPoints->Value(Index));
  if (HasTransf())
    Location().Transforms(point);
  return gp_Pnt(point);
}

double IGESDimen_GeneralNote::ZDepthStartPoint(const int Index) const
{
  return ((theStartPoints->Value(Index)).Z());
}

occ::handle<TCollection_HAsciiString> IGESDimen_GeneralNote::Text(const int Index) const
{
  return theTexts->Value(Index);
}
