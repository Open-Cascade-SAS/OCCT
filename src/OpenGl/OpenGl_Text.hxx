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
#include <OpenGl_TextBuilder.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

#include <gp_Ax2.hxx>

//! Text rendering
class OpenGl_Text : public OpenGl_Element
{

public:

  //! Main constructor
  Standard_EXPORT OpenGl_Text (const Standard_Utf8Char* theText,
                               const OpenGl_Vec3&       thePoint,
                               const OpenGl_TextParam&  theParams);

  //! Creates new text in 3D space.
  Standard_EXPORT OpenGl_Text (const Standard_Utf8Char* theText,
                               const gp_Ax2&            theOrientation,
                               const OpenGl_TextParam&  theParams,
                               const bool               theHasOwnAnchor = true);

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
                                                          const Standard_Integer   theHeight,
                                                          const unsigned int       theResolution);

  //! Find shared resource for specified font or initialize new one
  Standard_EXPORT static Handle(OpenGl_Font) FindFont (const Handle(OpenGl_Context)& theCtx,
                                                       const OpenGl_AspectText&      theAspect,
                                                       const Standard_Integer        theHeight,
                                                       const unsigned int            theResolution,
                                                       const TCollection_AsciiString theKey);

  //! Compute text width
  Standard_EXPORT static void StringSize (const Handle(OpenGl_Context)& theCtx,
                                          const NCollection_String&     theText,
                                          const OpenGl_AspectText&      theTextAspect,
                                          const OpenGl_TextParam&       theParams,
                                          const unsigned int            theResolution,
                                          Standard_ShortReal&           theWidth,
                                          Standard_ShortReal&           theAscent,
                                          Standard_ShortReal&           theDescent);

  //! Setup new string and parameters
  Standard_EXPORT void Init (const Handle(OpenGl_Context)&     theCtx,
                             const TCollection_ExtendedString& theText,
                             const OpenGl_Vec2&                thePoint,
                             const OpenGl_TextParam&           theParams);

  //! Perform rendering
  Standard_EXPORT void Render (const Handle(OpenGl_Context)& theCtx,
                               const OpenGl_AspectText&      theTextAspect,
                               const unsigned int            theResolution = Graphic3d_RenderingParams::THE_DEFAULT_RESOLUTION) const;

protected:

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Text();

  friend class OpenGl_Trihedron;
  friend class OpenGl_GraduatedTrihedron;

private:

  //! Release cached VBO resources
  void releaseVbos (OpenGl_Context* theCtx);

  //! Setup matrix.
  void setupMatrix (const Handle(OpenGl_Context)& theCtx,
                    const OpenGl_AspectText&      theTextAspect,
                    const OpenGl_Vec3             theDVec) const;

  //! Draw arrays of vertices.
  void drawText (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_AspectText&      theTextAspect) const;

  //! Draw rectangle from bounding text box.
  void drawRect (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_AspectText&      theTextAspect,
                 const OpenGl_Vec4&            theColorSubs) const;

  //! Main rendering code
  void render (const Handle(OpenGl_Context)& theCtx,
               const OpenGl_AspectText&      theTextAspect,
               const OpenGl_Vec4&            theColorText,
               const OpenGl_Vec4&            theColorSubs,
               const unsigned int            theResolution) const;

protected:

  mutable Handle(OpenGl_Font)                             myFont;
  mutable NCollection_Vector<GLuint>                      myTextures;   //!< textures' IDs
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myVertsVbo;   //!< VBOs of vertices
  mutable NCollection_Vector<Handle(OpenGl_VertexBuffer)> myTCrdsVbo;   //!< VBOs of texture coordinates
  mutable Handle(OpenGl_VertexBuffer)                     myBndVertsVbo;//!< VBOs of vertices for bounding box
  mutable Font_Rect                                       myBndBox;

protected:

  mutable OpenGl_Mat4d myProjMatrix;
  mutable OpenGl_Mat4d myModelMatrix;
  mutable OpenGl_Mat4d myOrientationMatrix;
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
  gp_Ax2             myOrientation; //!< Text orientation in 3D space.
  bool               myHasPlane;    //!< Check if text have orientation in 3D space.
  bool               myHasAnchorPoint; //!< Shows if it has own attach point

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Text_Header
