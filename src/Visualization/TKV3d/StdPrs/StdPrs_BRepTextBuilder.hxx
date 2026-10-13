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

#ifndef StdPrs_BRepTextBuilder_HeaderFile
#define StdPrs_BRepTextBuilder_HeaderFile

#include <StdPrs_BRepFont.hxx>
#include <BRep_Builder.hxx>
#include <Font_TextFormatter.hxx>

//! Compatibility interface for BRep text construction.
//! New code should call StdPrs_BRepFont::RenderText().
class StdPrs_BRepTextBuilder
{
public:
  //! Build text using the positions stored in an already formatted formatter.
  //! @param[in] theFont initialized font
  //! @param[in] theFormatter formatter with completed layout
  //! @param[in] thePenLoc text baseline placement
  //! @return text compound, or a null shape on invalid input or glyph construction failure
  Standard_DEPRECATED("Use StdPrs_BRepFont::RenderText()")
  Standard_EXPORT TopoDS_Shape Perform(StdPrs_BRepFont&                       theFont,
                                       const occ::handle<Font_TextFormatter>& theFormatter,
                                       const gp_Ax3&                          thePenLoc = gp_Ax3());

  //! Build text with the requested alignment and placement.
  //! @param[in] theFont initialized font
  //! @param[in] theString UTF text
  //! @param[in] thePenLoc text baseline placement
  //! @param[in] theHAlign horizontal alignment
  //! @param[in] theVAlign vertical alignment
  //! @return text compound, or a null shape when construction fails
  Standard_DEPRECATED("Use StdPrs_BRepFont::RenderText()")
  Standard_EXPORT TopoDS_Shape
    Perform(StdPrs_BRepFont&                  theFont,
            const NCollection_String&         theString,
            const gp_Ax3&                     thePenLoc = gp_Ax3(),
            Graphic3d_HorizontalTextAlignment theHAlign = Graphic3d_HTA_LEFT,
            Graphic3d_VerticalTextAlignment   theVAlign = Graphic3d_VTA_BOTTOM);

protected:
  BRep_Builder myBuilder; //!< Builder retained for subclasses using the compatibility interface.
};

#endif // StdPrs_BRepTextBuilder_HeaderFile
