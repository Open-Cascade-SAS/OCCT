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

#ifndef _IGESDimen_NewGeneralNote_HeaderFile
#define _IGESDimen_NewGeneralNote_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class TCollection_HAsciiString;

//! defines NewGeneralNote, Type <213> Form <0>
//! in package IGESDimen
//! Further attributes for formatting text strings
class IGESDimen_NewGeneralNote : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_NewGeneralNote();

  //! This method is used to set the fields of the class
  //! NewGeneralNote
  //! - width                : Width of text containment area
  //! - height               : Height of text containment area
  //! - justifyCode          : Justification code
  //! - areaLoc              : Text containment area location
  //! - areaRotationAngle    : Text containment area rotation
  //! - baseLinePos          : Base line position
  //! - normalInterlineSpace : Normal interline spacing
  //! - charDisplays         : Character display type
  //! - charWidths           : Character width
  //! - charHeights          : Character height
  //! - interCharSpc         : Intercharacter spacing
  //! - interLineSpc         : Interline spacing
  //! - fontStyles           : Font style
  //! - charAngles           : Character angle
  //! - controlCodeStrings   : Control Code string
  //! - nbChars              : Number of characters in string
  //! - boxWidths            : Box width
  //! - boxHeights           : Box height
  //! - charSetCodes         : Character Set Interpretation
  //! - charSetEntities      : Character Set Font
  //! - slAngles             : Slant angle of text in radians
  //! - rotAngles            : Rotation angle of text in radians
  //! - mirrorFlags          : Type of mirroring
  //! - rotateFlags          : Rotate internal text flag
  //! - startPoints          : Text start point
  //! - texts                : Text strings
  //! raises exception if there is mismatch between the various
  //! Array Lengths.
  Standard_EXPORT void Init(
    const double                                    width,
    const double                                    height,
    const int                                       justifyCode,
    const gp_XYZ&                                   areaLoc,
    const double                                    areaRotationAngle,
    const gp_XYZ&                                   baseLinePos,
    const double                                    normalInterlineSpace,
    const occ::handle<NCollection_HArray1<int>>&    charDisplays,
    const occ::handle<NCollection_HArray1<double>>& charWidths,
    const occ::handle<NCollection_HArray1<double>>& charHeights,
    const occ::handle<NCollection_HArray1<double>>& interCharSpc,
    const occ::handle<NCollection_HArray1<double>>& interLineSpc,
    const occ::handle<NCollection_HArray1<int>>&    fontStyles,
    const occ::handle<NCollection_HArray1<double>>& charAngles,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>&
                                                                              controlCodeStrings,
    const occ::handle<NCollection_HArray1<int>>&                              nbChars,
    const occ::handle<NCollection_HArray1<double>>&                           boxWidths,
    const occ::handle<NCollection_HArray1<double>>&                           boxHeights,
    const occ::handle<NCollection_HArray1<int>>&                              charSetCodes,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& charSetEntities,
    const occ::handle<NCollection_HArray1<double>>&                           slAngles,
    const occ::handle<NCollection_HArray1<double>>&                           rotAngles,
    const occ::handle<NCollection_HArray1<int>>&                              mirrorFlags,
    const occ::handle<NCollection_HArray1<int>>&                              rotateFlags,
    const occ::handle<NCollection_HArray1<gp_XYZ>>&                           startPoints,
    const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& texts);

  //! returns width of text containment area of all strings in the note
  Standard_EXPORT double TextWidth() const;

  //! returns height of text containment area of all strings in the note
  Standard_EXPORT double TextHeight() const;

  //! returns Justification code of all strings within the note
  //! 0 = no justification
  //! 1 = right justified
  //! 2 = center justified
  //! 3 = left justified
  Standard_EXPORT int JustifyCode() const;

  //! returns Text containment area Location point
  Standard_EXPORT gp_Pnt AreaLocation() const;

  //! returns Text containment area Location point after Transformation
  Standard_EXPORT gp_Pnt TransformedAreaLocation() const;

  //! returns distance from the containment area plane
  Standard_EXPORT double ZDepthAreaLocation() const;

  //! returns rotation angle of text containment area in radians
  Standard_EXPORT double AreaRotationAngle() const;

  //! returns position of first base line
  Standard_EXPORT gp_Pnt BaseLinePosition() const;

  //! returns position of first base line after Transformation
  Standard_EXPORT gp_Pnt TransformedBaseLinePosition() const;

  //! returns distance from the Base line position plane
  Standard_EXPORT double ZDepthBaseLinePosition() const;

  //! returns Normal Interline Spacing
  Standard_EXPORT double NormalInterlineSpace() const;

  //! returns number of text HAsciiStrings
  Standard_EXPORT int NbStrings() const;

  //! returns Fixed/Variable width character display of string
  //! 0 = Fixed
  //! 1 = Variable
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int CharacterDisplay(const int Index) const;

  //! returns False if Character display width is Fixed
  //! optional method, if required
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT bool IsVariable(const int Index) const;

  //! returns Character Width of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double CharacterWidth(const int Index) const;

  //! returns Character Height of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double CharacterHeight(const int Index) const;

  //! returns Inter-character spacing of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double InterCharacterSpace(const int Index) const;

  //! returns Interline spacing of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double InterlineSpace(const int Index) const;

  //! returns FontStyle of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int FontStyle(const int Index) const;

  //! returns CharacterAngle of string
  //! Angle returned will be between 0 and 2PI
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double CharacterAngle(const int Index) const;

  //! returns ControlCodeString of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> ControlCodeString(const int Index) const;

  //! returns number of characters in string or zero
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int NbCharacters(const int Index) const;

  //! returns Box width of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double BoxWidth(const int Index) const;

  //! returns Box height of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double BoxHeight(const int Index) const;

  //! returns False if Value, True if Pointer (Entity)
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT bool IsCharSetEntity(const int Index) const;

  //! returns Character Set Interpretation (default = 1) of string
  //! returns 0 if IsCharSetEntity () is True
  //! 1 = Standard ASCII
  //! 1001 = Symbol Font1
  //! 1002 = Symbol Font2
  //! 1003 = Symbol Font3
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int CharSetCode(const int Index) const;

  //! returns Character Set Interpretation of string
  //! returns a Null Handle if IsCharSetEntity () is False
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> CharSetEntity(const int Index) const;

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

  //! returns False if MirrorFlag = 0. ie. no mirroring
  //! else returns True
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT bool IsMirrored(const int Index) const;

  //! returns Rotate internal text Flag of string
  //! 0 = text horizontal
  //! 1 = text vertical
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT int RotateFlag(const int Index) const;

  //! returns text start point of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt StartPoint(const int Index) const;

  //! returns text start point of string after Transformation
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt TransformedStartPoint(const int Index) const;

  //! returns distance from the start point plane
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT double ZDepthStartPoint(const int Index) const;

  //! returns text string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Text(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_NewGeneralNote, IGESData_IGESEntity)

private:
  double                                                                  theWidth;
  double                                                                  theHeight;
  int                                                                     theJustifyCode;
  gp_XYZ                                                                  theAreaLoc;
  double                                                                  theAreaRotationAngle;
  gp_XYZ                                                                  theBaseLinePos;
  double                                                                  theNormalInterlineSpace;
  occ::handle<NCollection_HArray1<int>>                                   theCharDisplays;
  occ::handle<NCollection_HArray1<double>>                                theCharWidths;
  occ::handle<NCollection_HArray1<double>>                                theCharHeights;
  occ::handle<NCollection_HArray1<double>>                                theInterCharSpaces;
  occ::handle<NCollection_HArray1<double>>                                theInterlineSpaces;
  occ::handle<NCollection_HArray1<int>>                                   theFontStyles;
  occ::handle<NCollection_HArray1<double>>                                theCharAngles;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theControlCodeStrings;
  occ::handle<NCollection_HArray1<int>>                                   theNbChars;
  occ::handle<NCollection_HArray1<double>>                                theBoxWidths;
  occ::handle<NCollection_HArray1<double>>                                theBoxHeights;
  occ::handle<NCollection_HArray1<int>>                                   theCharSetCodes;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>      theCharSetEntities;
  occ::handle<NCollection_HArray1<double>>                                theSlantAngles;
  occ::handle<NCollection_HArray1<double>>                                theRotationAngles;
  occ::handle<NCollection_HArray1<int>>                                   theMirrorFlags;
  occ::handle<NCollection_HArray1<int>>                                   theRotateFlags;
  occ::handle<NCollection_HArray1<gp_XYZ>>                                theStartPoints;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theTexts;
};

#endif // _IGESDimen_NewGeneralNote_HeaderFile
