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
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_TextFontDef,IGESData_IGESEntity)

IGESGraph_TextFontDef::IGESGraph_TextFontDef ()    {  }


    void IGESGraph_TextFontDef::Init
  (const Standard_Integer                             aFontCode,
   const Handle(TCollection_HAsciiString)&            aFontName,
   const Standard_Integer                             aSupersededFont,
   const Handle(IGESGraph_TextFontDef)&               aSupersededEntity,
   const Standard_Integer                             aScale,
   const Handle(TColStd_HArray1OfInteger)&            allASCIICodes,
   const Handle(TColStd_HArray1OfInteger)&            allNextCharX,
   const Handle(TColStd_HArray1OfInteger)&            allNextCharY,
   const Handle(TColStd_HArray1OfInteger)&            allPenMotions,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& allPenFlags,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& allMovePenToX,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& allMovePenToY)
{
  Standard_Integer Len  = allASCIICodes->Length();
  if (allASCIICodes->Lower()  != 1 ||
      (allNextCharX->Lower()  != 1 || allNextCharX->Length()  != Len) ||
      (allNextCharY->Lower()  != 1 || allNextCharY->Length()  != Len) ||
      (allPenMotions->Lower() != 1 || allPenMotions->Length() != Len) ||
      (allPenFlags->Lower()   != 1 || allPenFlags->Length()   != Len) ||
      (allMovePenToX->Lower() != 1 || allMovePenToX->Length() != Len) ||
      (allMovePenToY->Lower() != 1 || allMovePenToY->Length() != Len) )
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
  InitTypeAndForm(310,0);
}

    Standard_Integer IGESGraph_TextFontDef::FontCode () const
{
  return theFontCode;
}

    Handle(TCollection_HAsciiString) IGESGraph_TextFontDef::FontName () const
{
  return theFontName;
}

    Standard_Boolean IGESGraph_TextFontDef::IsSupersededFontEntity () const
{
  return (! theSupersededFontEntity.IsNull());
}

    Standard_Integer IGESGraph_TextFontDef::SupersededFontCode () const
{   
  return theSupersededFontCode;
}

    Handle(IGESGraph_TextFontDef) IGESGraph_TextFontDef::SupersededFontEntity () const
{   
  return theSupersededFontEntity;
}

    Standard_Integer IGESGraph_TextFontDef::Scale () const
{
  return theScale;
}

    Standard_Integer IGESGraph_TextFontDef::NbCharacters () const
{
  return ( theASCIICodes->Length() );
}

    Standard_Integer IGESGraph_TextFontDef::ASCIICode
  (const Standard_Integer Chnum) const
{
  return ( theASCIICodes->Value(Chnum) ); 
}

    void IGESGraph_TextFontDef::NextCharOrigin
  (const Standard_Integer Chnum,
   Standard_Integer& NX, Standard_Integer& NY) const
{
  NX = theNextCharOriginX->Value(Chnum);
  NY = theNextCharOriginY->Value(Chnum);
}

    Standard_Integer IGESGraph_TextFontDef::NbPenMotions
  (const Standard_Integer Chnum) const
{
  return ( theNbPenMotions->Value(Chnum) ); 
}

    Standard_Boolean IGESGraph_TextFontDef::IsPenUp
  (const Standard_Integer Chnum, const Standard_Integer Motionnum) const
{
  Handle(TColStd_HArray1OfInteger) MotionArr = thePenMotions->Value(Chnum);
  Standard_Integer PenStatus =     MotionArr->Value(Motionnum);
  return ( PenStatus == 1 );
}

    void IGESGraph_TextFontDef::NextPenPosition
  (const Standard_Integer Chnum, const Standard_Integer Motionnum,
   Standard_Integer& IX, Standard_Integer& IY) const
{
  IX = thePenMovesToX->Value(Chnum)->Value(Motionnum);
  IY = thePenMovesToY->Value(Chnum)->Value(Motionnum);
}
