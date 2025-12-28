// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDimen_GeneralNote_HeaderFile
#define _IGESDimen_GeneralNote_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESGraph_TextFontDef;
class gp_Pnt;
class TCollection_HAsciiString;

//! defines GeneralNote, Type <212> Form <0-8, 100-200, 105>
//! in package IGESDimen
//! Used for formatting boxed text in different ways
class IGESDimen_GeneralNote : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_GeneralNote();

  //! This method is used to set the fields of the class
  //! GeneralNote
  //! - nNbChars      : number of chars strings
  //! - widths        : Box widths
  //! - heights       : Box heights
  //! - fontCodes     : Font codes, default = 1
  //! - fonts         : Text Font Definition Entities
  //! - slants        : Slant angles in radians
  //! - rotations     : Rotation angles in radians
  //! - mirrorFlags   : Mirror flags
  //! - rotFlags      : Rotation internal text flags
  //! - start         : Text start points
  //! - texts         : Text strings
  //! raises exception if there is mismatch between the various
  //! Array Lengths.
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<int>>&        nbChars,
                            const occ::handle<NCollection_HArray1<double>>&           widths,
                            const occ::handle<NCollection_HArray1<double>>&           heights,
                            const occ::handle<NCollection_HArray1<int>>&        fontCodes,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextFontDef>>>&  fonts,
                            const occ::handle<NCollection_HArray1<double>>&           slants,
                            const occ::handle<NCollection_HArray1<double>>&           rotations,
                            const occ::handle<NCollection_HArray1<int>>&        mirrorFlags,
                            const occ::handle<NCollection_HArray1<int>>&        rotFlags,
                            const occ::handle<NCollection_HArray1<gp_XYZ>>&             start,
                            const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& texts);

  //! Changes FormNumber (indicates Graphical Representation)
  //! Error if not in ranges [0-8] or [100-102] or 105
  Standard_EXPORT void SetFormNumber(const int form);

  //! returns number of text strings in General Note
  Standard_EXPORT int NbStrings() const;

  //! returns number of characters of string or zero
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int NbCharacters(const int Index) const;

  //! returns Box width of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double BoxWidth(const int Index) const;

  //! returns Box height of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double BoxHeight(const int Index) const;

  //! returns False if Value, True if Entity
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT bool IsFontEntity(const int Index) const;

  //! returns Font code (default = 1) of string
  //! returns 0 if IsFontEntity () is True
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int FontCode(const int Index) const;

  //! returns Text Font Definition Entity of string
  //! returns a Null Handle if IsFontEntity () returns False
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT occ::handle<IGESGraph_TextFontDef> FontEntity(const int Index) const;

  //! returns Slant angle of string in radians
  //! default value = PI/2
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double SlantAngle(const int Index) const;

  //! returns Rotation angle of string in radians
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double RotationAngle(const int Index) const;

  //! returns Mirror Flag of string
  //! 0 = no mirroring
  //! 1 = mirror axis is perpendicular to the text base line
  //! 2 = mirror axis is text base line
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int MirrorFlag(const int Index) const;

  //! returns Rotate internal text Flag of string
  //! 0 = text horizontal
  //! 1 = text vertical
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int RotateFlag(const int Index) const;

  //! returns text start point of Index'th string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt StartPoint(const int Index) const;

  //! returns text start point of Index'th string after Transformation
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt TransformedStartPoint(const int Index) const;

  //! returns distance from Start Point plane of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double ZDepthStartPoint(const int Index) const;

  //! returns text string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Text(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_GeneralNote, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<int>>        theNbChars;
  occ::handle<NCollection_HArray1<double>>           theBoxWidths;
  occ::handle<NCollection_HArray1<double>>           theBoxHeights;
  occ::handle<NCollection_HArray1<int>>        theFontCodes;
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextFontDef>>>  theFontEntities;
  occ::handle<NCollection_HArray1<double>>           theSlantAngles;
  occ::handle<NCollection_HArray1<double>>           theRotationAngles;
  occ::handle<NCollection_HArray1<int>>        theMirrorFlags;
  occ::handle<NCollection_HArray1<int>>        theRotateFlags;
  occ::handle<NCollection_HArray1<gp_XYZ>>             theStartPoints;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theTexts;
};

#endif // _IGESDimen_GeneralNote_HeaderFile
