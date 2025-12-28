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

#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_TextFontDef, IGESData_IGESEntity)

IGESGraph_TextFontDef::IGESGraph_TextFontDef() = default;

void IGESGraph_TextFontDef::Init(
  const int                                               aFontCode,
  const occ::handle<TCollection_HAsciiString>&            aFontName,
  const int                                               aSupersededFont,
  const occ::handle<IGESGraph_TextFontDef>&               aSupersededEntity,
  const int                                               aScale,
  const occ::handle<NCollection_HArray1<int>>&            allASCIICodes,
  const occ::handle<NCollection_HArray1<int>>&            allNextCharX,
  const occ::handle<NCollection_HArray1<int>>&            allNextCharY,
  const occ::handle<NCollection_HArray1<int>>&            allPenMotions,
  const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allPenFlags,
  const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allMovePenToX,
  const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allMovePenToY)
{
  int Len = allASCIICodes->Length();
  if (allASCIICodes->Lower() != 1 || (allNextCharX->Lower() != 1 || allNextCharX->Length() != Len)
      || (allNextCharY->Lower() != 1 || allNextCharY->Length() != Len)
      || (allPenMotions->Lower() != 1 || allPenMotions->Length() != Len)
      || (allPenFlags->Lower() != 1 || allPenFlags->Length() != Len)
      || (allMovePenToX->Lower() != 1 || allMovePenToX->Length() != Len)
      || (allMovePenToY->Lower() != 1 || allMovePenToY->Length() != Len))
    throw Standard_DimensionMismatch("IGESGraph_TextFontDef : Init");

  theFontCode             = aFontCode;
  theFontName             = aFontName;
  theSupersededFontCode   = aSupersededFont;
  theSupersededFontEntity = aSupersededEntity;
  theScale                = aScale;
  theASCIICodes           = allASCIICodes;
  theNextCharOriginX      = allNextCharX;
  theNextCharOriginY      = allNextCharY;
  theNbPenMotions         = allPenMotions;
  thePenMotions           = allPenFlags;
  thePenMovesToX          = allMovePenToX;
  thePenMovesToY          = allMovePenToY;
  InitTypeAndForm(310, 0);
}

int IGESGraph_TextFontDef::FontCode() const
{
  return theFontCode;
}

occ::handle<TCollection_HAsciiString> IGESGraph_TextFontDef::FontName() const
{
  return theFontName;
}

bool IGESGraph_TextFontDef::IsSupersededFontEntity() const
{
  return (!theSupersededFontEntity.IsNull());
}

int IGESGraph_TextFontDef::SupersededFontCode() const
{
  return theSupersededFontCode;
}

occ::handle<IGESGraph_TextFontDef> IGESGraph_TextFontDef::SupersededFontEntity() const
{
  return theSupersededFontEntity;
}

int IGESGraph_TextFontDef::Scale() const
{
  return theScale;
}

int IGESGraph_TextFontDef::NbCharacters() const
{
  return (theASCIICodes->Length());
}

int IGESGraph_TextFontDef::ASCIICode(const int Chnum) const
{
  return (theASCIICodes->Value(Chnum));
}

void IGESGraph_TextFontDef::NextCharOrigin(const int Chnum, int& NX, int& NY) const
{
  NX = theNextCharOriginX->Value(Chnum);
  NY = theNextCharOriginY->Value(Chnum);
}

int IGESGraph_TextFontDef::NbPenMotions(const int Chnum) const
{
  return (theNbPenMotions->Value(Chnum));
}

bool IGESGraph_TextFontDef::IsPenUp(const int Chnum, const int Motionnum) const
{
  occ::handle<NCollection_HArray1<int>> MotionArr = thePenMotions->Value(Chnum);
  int                                   PenStatus = MotionArr->Value(Motionnum);
  return (PenStatus == 1);
}

void IGESGraph_TextFontDef::NextPenPosition(const int Chnum,
                                            const int Motionnum,
                                            int&      IX,
                                            int&      IY) const
{
  IX = thePenMovesToX->Value(Chnum)->Value(Motionnum);
  IY = thePenMovesToY->Value(Chnum)->Value(Motionnum);
}
