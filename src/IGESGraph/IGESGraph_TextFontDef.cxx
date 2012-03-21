// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESGraph_TextFontDef.ixx>


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
      (allMovePenToX->Lower() != 1 || allMovePenToX->Length() != Len) )
    Standard_DimensionMismatch::Raise("IGESGraph_TextFontDef : Init");

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
