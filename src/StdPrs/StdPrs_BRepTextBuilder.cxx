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

#include <StdPrs_BRepTextBuilder.hxx>

#include <Font_TextFormatter.hxx>

// =======================================================================
// Function : Perform
// Purpose  :
// =======================================================================
TopoDS_Shape StdPrs_BRepTextBuilder::Perform (StdPrs_BRepFont&          theFont,
                                              const Handle(Font_TextFormatter)& theFormatter,
                                              const gp_Ax3&             thePenLoc)
{
  gp_Trsf          aTrsf;
  gp_XYZ           aPen;
  TopoDS_Shape     aGlyphShape;
  TopoDS_Compound  aResult;
  Standard_Mutex::Sentry aSentry (theFont.Mutex());

  myBuilder.MakeCompound (aResult);

  Standard_Real aScaleUnits    = theFont.Scale();
  for (Font_TextFormatter::Iterator aFormatterIt (*theFormatter, Font_TextFormatter::IterationFilter_ExcludeInvisible);
       aFormatterIt.More(); aFormatterIt.Next())
  {
    const NCollection_Vec2<Standard_ShortReal>& aCorner = theFormatter->BottomLeft (aFormatterIt.SymbolPosition());

    aPen.SetCoord (aCorner.x() * aScaleUnits, aCorner.y() * aScaleUnits, 0.0);
    aGlyphShape = theFont.RenderGlyph (aFormatterIt.Symbol());
    if (!aGlyphShape.IsNull())
    {
      aTrsf.SetTranslation (gp_Vec (aPen));
      aGlyphShape.Move (aTrsf);
      myBuilder.Add (aResult, aGlyphShape);
    }
  }

  aTrsf.SetTransformation (thePenLoc, gp_Ax3 (gp::XOY()));
  aResult.Move (aTrsf);

  return aResult;
}

// =======================================================================
// Function : Perform
// Purpose  :
// =======================================================================
TopoDS_Shape StdPrs_BRepTextBuilder::Perform (StdPrs_BRepFont&                        theFont,
                                              const NCollection_String&               theString,
                                              const gp_Ax3&                           thePenLoc,
                                              const Graphic3d_HorizontalTextAlignment theHAlign,
                                              const Graphic3d_VerticalTextAlignment   theVAlign)
{
  Handle(Font_TextFormatter) aFormatter = new Font_TextFormatter();

  aFormatter->Reset();
  aFormatter->SetupAlignment (theHAlign, theVAlign);

  aFormatter->Append (theString, *theFont.FTFont());
  aFormatter->Format();

  return Perform (theFont, aFormatter, thePenLoc);
}
