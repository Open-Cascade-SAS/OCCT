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

#ifndef Font_BRepTextBuilder_Header
#define Font_BRepTextBuilder_Header

#include <Font_BRepFont.hxx>
#include <Font_TextFormatter.hxx>
#include <gp_Ax3.hxx>

//! Represents class for applying text formatting.
class Font_BRepTextBuilder
{
public:
  //! Render text as BRep shape.
  //! @param theString text in UTF-8 encoding
  //! @param thePenLoc start position and orientation on the baseline
  //! @param theFormatter formatter which defines alignment for the text
  //! @return result shape with pen transformation applied as shape location
  Standard_EXPORT TopoDS_Shape Perform (Font_BRepFont&            theFont,
                                        const Font_TextFormatter& theFormatter,
                                        const gp_Ax3&             thePenLoc = gp_Ax3());
  //! Render text as BRep shape.
  //! @param theString text in UTF-8 encoding
  //! @param thePenLoc start position and orientation on the baseline
  //! @param theHAlign horizontal alignment of the text
  //! @param theVAlign vertical alignment of the text
  //! @return result shape with pen transformation applied as shape location
  Standard_EXPORT TopoDS_Shape Perform (Font_BRepFont&                          theFont,
                                        const NCollection_String&               theString,
                                        const gp_Ax3&                           thePenLoc = gp_Ax3(),
                                        const Graphic3d_HorizontalTextAlignment theHAlign = Graphic3d_HTA_LEFT,
                                        const Graphic3d_VerticalTextAlignment   theVAlign = Graphic3d_VTA_BOTTOM);

protected:
  BRep_Builder myBuilder;
};

#endif // Font_BRepTextBuilder_Header
