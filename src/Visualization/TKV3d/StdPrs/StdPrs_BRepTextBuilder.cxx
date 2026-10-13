// Copyright (c) 2026 OPEN CASCADE SAS
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

//==================================================================================================

TopoDS_Shape StdPrs_BRepTextBuilder::Perform(StdPrs_BRepFont&                       theFont,
                                             const occ::handle<Font_TextFormatter>& theFormatter,
                                             const gp_Ax3&                          thePenLoc)
{
  return !theFormatter.IsNull() ? theFont.RenderText(*theFormatter, thePenLoc) : TopoDS_Shape();
}

//==================================================================================================

TopoDS_Shape StdPrs_BRepTextBuilder::Perform(StdPrs_BRepFont&                        theFont,
                                             const NCollection_String&               theString,
                                             const gp_Ax3&                           thePenLoc,
                                             const Graphic3d_HorizontalTextAlignment theHAlign,
                                             const Graphic3d_VerticalTextAlignment   theVAlign)
{
  StdPrs_BRepFont::TextOptions anOptions;
  anOptions.Pen                   = thePenLoc;
  anOptions.HorizontalAlignment   = theHAlign;
  anOptions.VerticalAlignment     = theVAlign;
  anOptions.ToConcatenateContours = theFont.myToConcatenateContours;
  return theFont.RenderText(theString, anOptions);
}
