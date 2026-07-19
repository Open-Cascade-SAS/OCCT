// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_TextFontDef_HeaderFile
#define _IGESGraph_TextFontDef_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESBasic_HArray1OfHArray1OfInteger;

//! defines IGES Text Font Definition Entity, Type <310>
//! in package IGESGraph
//!
//! Used to define the appearance of characters in a text font.
//! It may be used to describe a complete font or a
//! modification to a subset of characters in another font.
class IGESGraph_TextFontDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_TextFontDef();

  //! This method is used to set the fields of the class
  //! TextFontDef
  //! - aFontCode         : Font Code
  //! - aFontName         : Font Name
  //! - aSupersededFont   : Number of superseded font
  //! - aSupersededEntity : Text Definition Entity
  //! - aScale            : No. of grid units = 1 text height unit
  //! - allASCIICodes     : ASCII codes for characters
  //! - allNextCharX & Y  : Grid locations of the next
  //! character's origin (Integer vals)
  //! - allPenMotions     : No. of pen motions for the characters
  //! - allPenFlags       : Pen up/down flags,
  //! 0 = Down (default), 1 = Up
  //! - allMovePenToX & Y : Grid locations the pen will move to
  //! This method initializes the fields of the class TextFontDef.
  //! An exception is raised if the lengths of allASCIICodes,
  //! allNextChars, allPenMotions, allPenFlags and allMovePenTo
  //! are not same.
  Standard_EXPORT void Init(const int                                    aFontCode,
                            const occ::handle<TCollection_HAsciiString>& aFontName,
                            const int                                    aSupersededFont,
                            const occ::handle<IGESGraph_TextFontDef>&    aSupersededEntity,
                            const int                                    aScale,
                            const occ::handle<NCollection_HArray1<int>>& allASCIICodes,
                            const occ::handle<NCollection_HArray1<int>>& allNextCharX,
                            const occ::handle<NCollection_HArray1<int>>& allNextCharY,
                            const occ::handle<NCollection_HArray1<int>>& allPenMotions,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allPenFlags,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allMovePenToX,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& allMovePenToY);

  //! returns the font code.
  Standard_EXPORT int FontCode() const;

  //! returns the font name.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> FontName() const;

  //! True if this definition supersedes another
  //! TextFontDefinition Entity,
  //! False if it supersedes value.
  Standard_EXPORT bool IsSupersededFontEntity() const;

  //! returns the font number which this entity modifies.
  Standard_EXPORT int SupersededFontCode() const;

  //! returns the font entity which this entity modifies.
  Standard_EXPORT occ::handle<IGESGraph_TextFontDef> SupersededFontEntity() const;

  //! returns the number of grid units which equal one text height unit.
  Standard_EXPORT int Scale() const;

  //! returns the number of characters in this definition.
  Standard_EXPORT int NbCharacters() const;

  //! returns the ASCII code of Chnum'th character.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT int ASCIICode(const int Chnum) const;

  //! returns grid location of origin of character next to Chnum'th char.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT void NextCharOrigin(const int Chnum, int& NX, int& NY) const;

  //! returns number of pen motions for Chnum'th character.
  //! Exception OutOfRange is raised if Chnum <= 0 or Chnum > NbCharacters
  Standard_EXPORT int NbPenMotions(const int Chnum) const;

  //! returns pen status(True if 1, False if 0) of Motionnum'th motion
  //! of Chnum'th character.
  //! Exception raised if Chnum <= 0 or Chnum > NbCharacters or
  //! Motionnum <= 0 or Motionnum > NbPenMotions
  Standard_EXPORT bool IsPenUp(const int Chnum, const int Motionnum) const;

  Standard_EXPORT void NextPenPosition(const int Chnum,
                                       const int Motionnum,
                                       int&      IX,
                                       int&      IY) const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_TextFontDef, IGESData_IGESEntity)

private:
  int                                              theFontCode;
  occ::handle<TCollection_HAsciiString>            theFontName;
  int                                              theSupersededFontCode;
  occ::handle<IGESGraph_TextFontDef>               theSupersededFontEntity;
  int                                              theScale;
  occ::handle<NCollection_HArray1<int>>            theASCIICodes;
  occ::handle<NCollection_HArray1<int>>            theNextCharOriginX;
  occ::handle<NCollection_HArray1<int>>            theNextCharOriginY;
  occ::handle<NCollection_HArray1<int>>            theNbPenMotions;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> thePenMotions;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> thePenMovesToX;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> thePenMovesToY;
};

#endif // _IGESGraph_TextFontDef_HeaderFile
