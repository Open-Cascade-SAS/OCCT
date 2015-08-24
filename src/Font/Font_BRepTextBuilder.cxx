// Created on: 2015-08-10
// Created by: Ilya SEVRIKOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Font_BRepTextBuilder.hxx>

// =======================================================================
// Function : Perfrom
// Purpose  :
// =======================================================================
TopoDS_Shape Font_BRepTextBuilder::Perform (Font_BRepFont&            theFont,
                                            const Font_TextFormatter& theFormatter,
                                            const gp_Ax3&             thePenLoc)
{
  gp_Trsf          aTrsf;
  gp_XYZ           aPen;
  TopoDS_Shape     aGlyphShape;
  TopoDS_Compound  aResult;
  Standard_Mutex::Sentry aSentry (theFont.Mutex());

  myBuilder.MakeCompound (aResult);

  Standard_Integer aSymbolCounter = 0;
  Standard_Real    aScaleUnits    = theFont.Scale();
  for (NCollection_Utf8Iter anIter = theFormatter.String().Iterator(); *anIter != 0; ++anIter)
  {
    const Standard_Utf32Char aCharCurr = *anIter;
    if (aCharCurr == '\x0D' // CR  (carriage return)
     || aCharCurr == '\a'   // BEL (alarm)
     || aCharCurr == '\f'   // FF  (form feed) NP (new page)
     || aCharCurr == '\b'   // BS  (backspace)
     || aCharCurr == '\v'   // VT  (vertical tab)
     || aCharCurr == ' '
     || aCharCurr == '\t'
     || aCharCurr == '\n')
    {
      continue; // skip unsupported carriage control codes
    }

    const NCollection_Vec2<Standard_ShortReal>& aCorner = theFormatter.TopLeft (aSymbolCounter);
    aPen.SetCoord (aCorner.x() * aScaleUnits, aCorner.y() * aScaleUnits, 0.0);
    aGlyphShape = theFont.RenderGlyph (aCharCurr);
    if (!aGlyphShape.IsNull())
    {
      aTrsf.SetTranslation (gp_Vec (aPen));
      aGlyphShape.Move (aTrsf);
      myBuilder.Add (aResult, aGlyphShape);
    }

    ++aSymbolCounter;
  }

  aTrsf.SetTransformation (thePenLoc, gp_Ax3 (gp::XOY()));
  aResult.Move (aTrsf);

  return aResult;
}

// =======================================================================
// Function : Perform
// Purpose  :
// =======================================================================
TopoDS_Shape Font_BRepTextBuilder::Perform (Font_BRepFont&                          theFont,
                                            const NCollection_String&               theString,
                                            const gp_Ax3&                           thePenLoc,
                                            const Graphic3d_HorizontalTextAlignment theHAlign,
                                            const Graphic3d_VerticalTextAlignment   theVAlign)
{
  Font_TextFormatter aFormatter;

  aFormatter.Reset();
  aFormatter.SetupAlignment (theHAlign, theVAlign);

  aFormatter.Append (theString, *(reinterpret_cast<Font_FTFont*> (&theFont)));
  aFormatter.Format();

  return Perform (theFont, aFormatter, thePenLoc);
}
