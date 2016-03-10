// Created on: 2015-06-18
// Created by: Ilya SEVRIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef OpenGl_TextBuilder_Header
#define OpenGl_TextBuilder_Header

#include <Font_TextFormatter.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Font.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_VertexBufferEditor.hxx>
#include <OpenGl_Vec.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_Handle.hxx>


//! This class generates primitive array required for rendering textured text using OpenGl_Font instance.
class OpenGl_TextBuilder
{
public:

  //! Creates empty object.
  Standard_EXPORT OpenGl_TextBuilder();

  //! Creates texture quads for the given text.
  Standard_EXPORT void Perform (const Font_TextFormatter&                        theFormatter,
                                const Handle(OpenGl_Context)&                    theContext,
                                OpenGl_Font&                                     theFont,
                                NCollection_Vector<GLuint>&                      theTextures,
                                NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theVertsPerTexture,
                                NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theTCrdsPerTexture);

protected: //! @name class auxillary methods

  Standard_EXPORT void createGlyphs (const Font_TextFormatter&                                                      theFormatter,
                                     const Handle(OpenGl_Context)&                                                  theCtx,
                                     OpenGl_Font&                                                                   theFont,
                                     NCollection_Vector<GLuint>&                                                    theTextures,
                                     NCollection_Vector< NCollection_Handle < NCollection_Vector <OpenGl_Vec2> > >& theVertsPerTexture,
                                     NCollection_Vector< NCollection_Handle < NCollection_Vector <OpenGl_Vec2> > >& theTCrdsPerTexture);

protected: //! @name class auxillary fields

  NCollection_Vector<OpenGl_Font::Tile>  myTileRects;
  OpenGl_VertexBufferEditor<OpenGl_Vec2> myVboEditor;
};

#endif // OpenGl_TextBuilder_Header
