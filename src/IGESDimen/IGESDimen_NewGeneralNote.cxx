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
#include <IGESData_IGESEntity.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_NewGeneralNote,IGESData_IGESEntity)

IGESDimen_NewGeneralNote::IGESDimen_NewGeneralNote ()    {  }


    void  IGESDimen_NewGeneralNote::Init
  (const Standard_Real    width,                const Standard_Real height,
   const Standard_Integer justifyCode,          const gp_XYZ& areaLoc,
   const Standard_Real    areaRotationAngle,    const gp_XYZ& baseLinePos,
   const Standard_Real    normalInterlineSpace,
   const Handle(TColStd_HArray1OfInteger)&        charDisplays,
   const Handle(TColStd_HArray1OfReal)&           charWidths,
   const Handle(TColStd_HArray1OfReal)&           charHeights,
   const Handle(TColStd_HArray1OfReal)&           interCharSpc,
   const Handle(TColStd_HArray1OfReal)&           interLineSpc,
   const Handle(TColStd_HArray1OfInteger)&        fontStyles,
   const Handle(TColStd_HArray1OfReal)&           charAngles,
   const Handle(Interface_HArray1OfHAsciiString)& controlCodeStrings,
   const Handle(TColStd_HArray1OfInteger)&        nbChars,
   const Handle(TColStd_HArray1OfReal)&           boxWidths,
   const Handle(TColStd_HArray1OfReal)&           boxHeights,
   const Handle(TColStd_HArray1OfInteger)&        charSetCodes,
   const Handle(IGESData_HArray1OfIGESEntity)&    charSetEntities,
   const Handle(TColStd_HArray1OfReal)&           slAngles,
   const Handle(TColStd_HArray1OfReal)&           rotAngles,
   const Handle(TColStd_HArray1OfInteger)&        mirrorFlags,
   const Handle(TColStd_HArray1OfInteger)&        rotateFlags,
   const Handle(TColgp_HArray1OfXYZ)&             startPoints,
   const Handle(Interface_HArray1OfHAsciiString)& texts)
{
  Standard_Integer num = nbChars->Length();

  if ( nbChars->Lower()         != 1 ||
      (charDisplays->Lower()    != 1 || charDisplays->Length()    != num) ||
      (charWidths->Lower()      != 1 || charWidths->Length()      != num) ||
      (charHeights->Lower()     != 1 || charHeights->Length()     != num) ||
      (interCharSpc->Lower()    != 1 || interCharSpc->Length()    != num) ||
      (interLineSpc->Lower()    != 1 || interLineSpc->Length()    != num) ||
      (fontStyles->Lower()      != 1 || fontStyles->Length()      != num) ||
      (charAngles->Lower()      != 1 || charAngles->Length()      != num) ||
      (controlCodeStrings->Lower() != 1 || controlCodeStrings->Length() != num)||
      (boxWidths->Lower()       != 1 || boxWidths->Length()       != num) ||
      (boxHeights->Lower()      != 1 || boxHeights->Length()      != num) ||
      (charSetCodes->Lower()    != 1 || charSetCodes->Length()    != num) ||
      (charSetEntities->Lower() != 1 || charSetEntities->Length() != num) ||
      (slAngles->Lower()        != 1 || slAngles->Length()        != num) ||
      (rotAngles->Lower()       != 1 || rotAngles->Length()       != num) ||
      (mirrorFlags->Lower()     != 1 || mirrorFlags->Length()     != num) ||
      (rotateFlags->Lower()     != 1 || rotateFlags->Length()     != num) ||
      (startPoints->Lower()     != 1 || startPoints->Length()     != num) ||
      (texts->Lower()           != 1 || texts->Length() != num))
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
  InitTypeAndForm(213,0);
}

    Standard_Real  IGESDimen_NewGeneralNote::TextWidth () const 
{
  return theWidth;
}

    Standard_Real  IGESDimen_NewGeneralNote::TextHeight () const 
{
  return theHeight;
}

    Standard_Integer  IGESDimen_NewGeneralNote::JustifyCode () const 
{
  return theJustifyCode;
}

    gp_Pnt  IGESDimen_NewGeneralNote::AreaLocation () const 
{
  gp_Pnt loc(theAreaLoc);
  return loc;
}

    gp_Pnt  IGESDimen_NewGeneralNote::TransformedAreaLocation () const 
{
  gp_XYZ tempXYZ = theAreaLoc;
  if (HasTransf()) Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

    Standard_Real  IGESDimen_NewGeneralNote::ZDepthAreaLocation () const 
{
  return (theAreaLoc.Z());
}

    Standard_Real  IGESDimen_NewGeneralNote::AreaRotationAngle () const 
{
  return theAreaRotationAngle;
}

    gp_Pnt  IGESDimen_NewGeneralNote::BaseLinePosition () const 
{
  gp_Pnt pos(theBaseLinePos);
  return pos;
}

    gp_Pnt  IGESDimen_NewGeneralNote::TransformedBaseLinePosition () const 
{
  gp_XYZ tempXYZ = theBaseLinePos;
  if (HasTransf()) Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

    Standard_Real  IGESDimen_NewGeneralNote::ZDepthBaseLinePosition () const 
{
  return (theBaseLinePos.Z());
}

    Standard_Real  IGESDimen_NewGeneralNote::NormalInterlineSpace () const 
{
  return theNormalInterlineSpace;
}

    Standard_Integer  IGESDimen_NewGeneralNote::NbStrings () const 
{
  return theCharDisplays->Length();
}

    Standard_Integer  IGESDimen_NewGeneralNote::CharacterDisplay
  (const Standard_Integer Index) const 
{
  return theCharDisplays->Value(Index);
}

    Standard_Boolean  IGESDimen_NewGeneralNote::IsVariable
  (const Standard_Integer Index) const 
{
  return (theCharDisplays->Value(Index) == 1); 
}

    Standard_Real  IGESDimen_NewGeneralNote::CharacterWidth
  (const Standard_Integer Index) const 
{
  return theCharWidths->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::CharacterHeight
  (const Standard_Integer Index) const 
{
  return theCharHeights->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::InterCharacterSpace
  (const Standard_Integer Index) const 
{
  return theInterCharSpaces->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::InterlineSpace
  (const Standard_Integer Index) const 
{
  return theInterlineSpaces->Value(Index);
}

    Standard_Integer  IGESDimen_NewGeneralNote::FontStyle
  (const Standard_Integer Index) const 
{
  return theFontStyles->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::CharacterAngle
  (const Standard_Integer Index) const 
{
  return theCharAngles->Value(Index);
}

    Handle(TCollection_HAsciiString)  IGESDimen_NewGeneralNote::ControlCodeString
  (const Standard_Integer Index) const 
{
  return theControlCodeStrings->Value(Index);
}

    Standard_Integer  IGESDimen_NewGeneralNote::NbCharacters
  (const Standard_Integer Index) const 
{
  return theNbChars->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::BoxWidth
  (const Standard_Integer Index) const 
{
  return theBoxWidths->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::BoxHeight
  (const Standard_Integer Index) const 
{
  return theBoxHeights->Value(Index);
}

    Standard_Boolean  IGESDimen_NewGeneralNote::IsCharSetEntity
  (const Standard_Integer Index) const 
{
  return (! (theCharSetEntities->Value(Index)).IsNull());
}

    Standard_Integer  IGESDimen_NewGeneralNote::CharSetCode
  (const Standard_Integer Index) const 
{
  return theCharSetCodes->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESDimen_NewGeneralNote::CharSetEntity
  (const Standard_Integer Index) const 
{
  return theCharSetEntities->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::SlantAngle
  (const Standard_Integer Index) const 
{
  return theSlantAngles->Value(Index);
}

    Standard_Real  IGESDimen_NewGeneralNote::RotationAngle
  (const Standard_Integer Index) const 
{
  return theRotationAngles->Value(Index);
}

    Standard_Integer  IGESDimen_NewGeneralNote::MirrorFlag
  (const Standard_Integer Index) const 
{
  return theMirrorFlags->Value(Index);
}

    Standard_Boolean  IGESDimen_NewGeneralNote::IsMirrored
  (const Standard_Integer Index) const 
{
  return (theMirrorFlags->Value(Index) != 0);
}

    Standard_Integer  IGESDimen_NewGeneralNote::RotateFlag
  (const Standard_Integer Index) const 
{
  return theRotateFlags->Value(Index);
}

    gp_Pnt  IGESDimen_NewGeneralNote::StartPoint
  (const Standard_Integer Index) const 
{
  return gp_Pnt(theStartPoints->Value(Index));
}

    gp_Pnt  IGESDimen_NewGeneralNote::TransformedStartPoint
  (const Standard_Integer Index) const 
{
  gp_XYZ tempXYZ = theStartPoints->Value(Index);
  if (HasTransf()) Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

    Standard_Real  IGESDimen_NewGeneralNote::ZDepthStartPoint
  (const Standard_Integer Index) const 
{
  return (theStartPoints->Value(Index).Z());
}

    Handle(TCollection_HAsciiString)  IGESDimen_NewGeneralNote::Text
  (const Standard_Integer Index) const 
{
  return theTexts->Value(Index);
}
