// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef OpenGl_Text_Header
#define OpenGl_Text_Header

#include <OpenGl_Element.hxx>

#include <OpenGl_AspectText.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_TextFormatter.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

class Handle(OpenGl_PrinterContext);

//! Text rendering
class OpenGl_Text : public OpenGl_Element
{

public:

  //! Main constructor
  Standard_EXPORT OpenGl_Text (const Standard_Utf8Char* theText,
                               const OpenGl_Vec3&       thePoint,
                               const OpenGl_TextParam&  theParams);

  //! Setup new string and position
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Utf8Char*      theText,
                             const OpenGl_Vec3&            thePoint);

  //! Setup new string and parameters
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Utf8Char*      theText,
                             const OpenGl_Vec3&            thePoint,
                             const OpenGl_TextParam&       theParams);

  //! Setup new position
  Standard_EXPORT void SetPosition (const OpenGl_Vec3& thePoint);

  //! Setup new font size
  Standard_EXPORT void SetFontSize (const Handle(OpenGl_Context)& theContext,
                                    const Standard_Integer        theFontSize);

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

public: //! @name methods for compatibility with layers

  //! Empty constructor
  Standard_EXPORT OpenGl_Text();

  //! Create key for shared resource
  Standard_EXPORT static TCollection_AsciiString FontKey (const OpenGl_AspectText& theAspect,
                                                          const Standard_Integer   theHeight);

  //! Find shared resource for specified font or initialize new one
  Standard_EXPORT static Handle(OpenGl_Font) FindFont (const Handle(OpenGl_Context)& theCtx,
                                                       const OpenGl_AspectText&      theAspect,
                                                       const Standard_Integer        theHeight,
                                                       const TCollection_AsciiString theKey);

  //! Compute text width
  Standard_EXPORT static void StringSize (const Handle(OpenGl_Context)& theCtx,
                                          const NCollection_String&     theText,
                                          const OpenGl_AspectText&      theTextAspect,
                                          const OpenGl_TextParam&       theParams,
                                          Standard_ShortReal&           theWidth,
                                          Standard_ShortReal&           theAscent,
                                          Standard_ShortReal&           theDescent);

  //! Setup new string and parameters
  Standard_EXPORT void Init (const Handle(OpenGl_Context)&     theCtx,
                             const TCollection_ExtendedString& theText,
                             const OpenGl_Vec2&                thePoint,
                             const OpenGl_TextParam&           theParams);

  //! Perform rendering
  Standard_EXPORT void Render (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                               const Handle(OpenGl_Context)&        theCtx,
                               const OpenGl_AspectText&             theTextAspect) const;

protected:

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Text();

  friend class OpenGl_Trihedron;
  friend class OpenGl_GraduatedTrihedron;

private:

  //! Release cached VBO resources
  void releaseVbos (OpenGl_Context* theCtx);

  //! Setup matrix.
  void setupMatrix (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                    const Handle(OpenGl_Context)&        theCtx,
                    const OpenGl_AspectText&             theTextAspect,
                    const OpenGl_Vec3                    theDVec) const;

  //! Draw arrays of vertices.
  void drawText (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                 const Handle(OpenGl_Context)&        theCtx,
                 const OpenGl_AspectText&             theTextAspect) const;

  //! Main rendering code
  void render (const Handle(OpenGl_PrinterContext)& thePrintCtx,
               const Handle(OpenGl_Context)&        theCtx,
               const OpenGl_AspectText&             theTextAspect,
               const TEL_COLOUR&                    theColorText,
               const TEL_COLOUR&                    theColorSubs) const;

protected:

  mutable Handle(OpenGl_Font)                             myFont;
  mutable NCollection_Vector<GLuint>                      myTextures;   //!< textures' IDs
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myVertsVbo;   //!< VBOs of vertices
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myTCrdsVbo;   //!< VBOs of texture coordinates
  mutable Font_FTFont::Rect                               myBndBox;

protected:

  mutable OpenGl_Mat4d myProjMatrix;
  mutable OpenGl_Mat4d myModelMatrix;
  mutable GLint    myViewport[4];
  mutable GLdouble myWinX;
  mutable GLdouble myWinY;
  mutable GLdouble myWinZ;
  mutable GLdouble myScaleHeight;
  mutable GLdouble myExportHeight;

protected:

  OpenGl_TextParam   myParams;
  NCollection_String myString;
  OpenGl_Vec3        myPoint;
  bool               myIs2d;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Text_Header
