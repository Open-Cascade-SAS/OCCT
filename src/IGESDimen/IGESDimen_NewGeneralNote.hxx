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
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;
class TCollection_HAsciiString;


class IGESDimen_NewGeneralNote;
DEFINE_STANDARD_HANDLE(IGESDimen_NewGeneralNote, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Real width, const Standard_Real height, const Standard_Integer justifyCode, const gp_XYZ& areaLoc, const Standard_Real areaRotationAngle, const gp_XYZ& baseLinePos, const Standard_Real normalInterlineSpace, const Handle(TColStd_HArray1OfInteger)& charDisplays, const Handle(TColStd_HArray1OfReal)& charWidths, const Handle(TColStd_HArray1OfReal)& charHeights, const Handle(TColStd_HArray1OfReal)& interCharSpc, const Handle(TColStd_HArray1OfReal)& interLineSpc, const Handle(TColStd_HArray1OfInteger)& fontStyles, const Handle(TColStd_HArray1OfReal)& charAngles, const Handle(Interface_HArray1OfHAsciiString)& controlCodeStrings, const Handle(TColStd_HArray1OfInteger)& nbChars, const Handle(TColStd_HArray1OfReal)& boxWidths, const Handle(TColStd_HArray1OfReal)& boxHeights, const Handle(TColStd_HArray1OfInteger)& charSetCodes, const Handle(IGESData_HArray1OfIGESEntity)& charSetEntities, const Handle(TColStd_HArray1OfReal)& slAngles, const Handle(TColStd_HArray1OfReal)& rotAngles, const Handle(TColStd_HArray1OfInteger)& mirrorFlags, const Handle(TColStd_HArray1OfInteger)& rotateFlags, const Handle(TColgp_HArray1OfXYZ)& startPoints, const Handle(Interface_HArray1OfHAsciiString)& texts);
  
  //! returns width of text containment area of all strings in the note
  Standard_EXPORT Standard_Real TextWidth() const;
  
  //! returns height of text containment area of all strings in the note
  Standard_EXPORT Standard_Real TextHeight() const;
  
  //! returns Justification code of all strings within the note
  //! 0 = no justification
  //! 1 = right justified
  //! 2 = center justified
  //! 3 = left justified
  Standard_EXPORT Standard_Integer JustifyCode() const;
  
  //! returns Text containment area Location point
  Standard_EXPORT gp_Pnt AreaLocation() const;
  
  //! returns Text containment area Location point after Transformation
  Standard_EXPORT gp_Pnt TransformedAreaLocation() const;
  
  //! returns distance from the containment area plane
  Standard_EXPORT Standard_Real ZDepthAreaLocation() const;
  
  //! returns rotation angle of text containment area in radians
  Standard_EXPORT Standard_Real AreaRotationAngle() const;
  
  //! returns position of first base line
  Standard_EXPORT gp_Pnt BaseLinePosition() const;
  
  //! returns position of first base line after Transformation
  Standard_EXPORT gp_Pnt TransformedBaseLinePosition() const;
  
  //! returns distance from the Base line position plane
  Standard_EXPORT Standard_Real ZDepthBaseLinePosition() const;
  
  //! returns Normal Interline Spacing
  Standard_EXPORT Standard_Real NormalInterlineSpace() const;
  
  //! returns number of text HAsciiStrings
  Standard_EXPORT Standard_Integer NbStrings() const;
  
  //! returns Fixed/Variable width character display of string
  //! 0 = Fixed
  //! 1 = Variable
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer CharacterDisplay (const Standard_Integer Index) const;
  
  //! returns False if Character display width is Fixed
  //! optional method, if required
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Boolean IsVariable (const Standard_Integer Index) const;
  
  //! returns Character Width of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real CharacterWidth (const Standard_Integer Index) const;
  
  //! returns Character Height of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real CharacterHeight (const Standard_Integer Index) const;
  
  //! returns Inter-character spacing of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real InterCharacterSpace (const Standard_Integer Index) const;
  
  //! returns Interline spacing of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real InterlineSpace (const Standard_Integer Index) const;
  
  //! returns FontStyle of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer FontStyle (const Standard_Integer Index) const;
  
  //! returns CharacterAngle of string
  //! Angle returned will be between 0 and 2PI
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real CharacterAngle (const Standard_Integer Index) const;
  
  //! returns ControlCodeString of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Handle(TCollection_HAsciiString) ControlCodeString (const Standard_Integer Index) const;
  
  //! returns number of characters in string or zero
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer NbCharacters (const Standard_Integer Index) const;
  
  //! returns Box width of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real BoxWidth (const Standard_Integer Index) const;
  
  //! returns Box height of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real BoxHeight (const Standard_Integer Index) const;
  
  //! returns False if Value, True if Pointer (Entity)
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Boolean IsCharSetEntity (const Standard_Integer Index) const;
  
  //! returns Character Set Interpretation (default = 1) of string
  //! returns 0 if IsCharSetEntity () is True
  //! 1 = Standard ASCII
  //! 1001 = Symbol Font1
  //! 1002 = Symbol Font2
  //! 1003 = Symbol Font3
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer CharSetCode (const Standard_Integer Index) const;
  
  //! returns Character Set Interpretation of string
  //! returns a Null Handle if IsCharSetEntity () is False
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Handle(IGESData_IGESEntity) CharSetEntity (const Standard_Integer Index) const;
  
  //! returns Slant angle of string in radians
  //! default value = PI/2
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real SlantAngle (const Standard_Integer Index) const;
  
  //! returns Rotation angle of string in radians
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real RotationAngle (const Standard_Integer Index) const;
  
  //! returns Mirror Flag of string
  //! 0 = no mirroring
  //! 1 = mirror axis is perpendicular to the text base line
  //! 2 = mirror axis is text base line
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer MirrorFlag (const Standard_Integer Index) const;
  
  //! returns False if MirrorFlag = 0. ie. no mirroring
  //! else returns True
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Boolean IsMirrored (const Standard_Integer Index) const;
  
  //! returns Rotate internal text Flag of string
  //! 0 = text horizontal
  //! 1 = text vertical
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Integer RotateFlag (const Standard_Integer Index) const;
  
  //! returns text start point of string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt StartPoint (const Standard_Integer Index) const;
  
  //! returns text start point of string after Transformation
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT gp_Pnt TransformedStartPoint (const Standard_Integer Index) const;
  
  //! returns distance from the start point plane
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Standard_Real ZDepthStartPoint (const Standard_Integer Index) const;
  
  //! returns text string
  //! raises exception if Index <= 0 or Index > NbStrings()
  Standard_EXPORT Handle(TCollection_HAsciiString) Text (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_NewGeneralNote,IGESData_IGESEntity)

protected:




private:


  Standard_Real theWidth;
  Standard_Real theHeight;
  Standard_Integer theJustifyCode;
  gp_XYZ theAreaLoc;
  Standard_Real theAreaRotationAngle;
  gp_XYZ theBaseLinePos;
  Standard_Real theNormalInterlineSpace;
  Handle(TColStd_HArray1OfInteger) theCharDisplays;
  Handle(TColStd_HArray1OfReal) theCharWidths;
  Handle(TColStd_HArray1OfReal) theCharHeights;
  Handle(TColStd_HArray1OfReal) theInterCharSpaces;
  Handle(TColStd_HArray1OfReal) theInterlineSpaces;
  Handle(TColStd_HArray1OfInteger) theFontStyles;
  Handle(TColStd_HArray1OfReal) theCharAngles;
  Handle(Interface_HArray1OfHAsciiString) theControlCodeStrings;
  Handle(TColStd_HArray1OfInteger) theNbChars;
  Handle(TColStd_HArray1OfReal) theBoxWidths;
  Handle(TColStd_HArray1OfReal) theBoxHeights;
  Handle(TColStd_HArray1OfInteger) theCharSetCodes;
  Handle(IGESData_HArray1OfIGESEntity) theCharSetEntities;
  Handle(TColStd_HArray1OfReal) theSlantAngles;
  Handle(TColStd_HArray1OfReal) theRotationAngles;
  Handle(TColStd_HArray1OfInteger) theMirrorFlags;
  Handle(TColStd_HArray1OfInteger) theRotateFlags;
  Handle(TColgp_HArray1OfXYZ) theStartPoints;
  Handle(Interface_HArray1OfHAsciiString) theTexts;


};







#endif // _IGESDimen_NewGeneralNote_HeaderFile
