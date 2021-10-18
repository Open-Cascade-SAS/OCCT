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

#ifndef OpenGl_Text_HeaderFile
#define OpenGl_Text_HeaderFile

#include <OpenGl_Element.hxx>

#include <OpenGl_Aspects.hxx>
#include <OpenGl_TextBuilder.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_Text.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

#include <gp_Ax2.hxx>

//! Text rendering
class OpenGl_Text : public OpenGl_Element
{

public:

  //! Creates new text in 3D space.
  Standard_EXPORT OpenGl_Text (const Handle(Graphic3d_Text)& theTextParams);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Text();

  //! Release cached VBO resources and the previous font if height changed.
  //! Cached structures will be refilled by the next render.
  //! Call Reset after modifying text parameters.
  Standard_EXPORT void Reset (const Handle(OpenGl_Context)& theCtx);

  //! Returns text parameters
  //! @sa Reset()
  const Handle(Graphic3d_Text)& Text() const { return myText; }

  //! Sets text parameters
  //! @sa Reset()
  void SetText (const Handle(Graphic3d_Text)& theText) { myText = theText; }

  //! Return true if text is 2D
  Standard_Boolean Is2D() const { return myIs2d; }

  //! Set true if text is 2D
  void Set2D (const Standard_Boolean theEnable) { myIs2d = theEnable; }

  //! Setup new font size
  Standard_EXPORT void SetFontSize (const Handle(OpenGl_Context)& theContext,
                                    const Standard_Integer        theFontSize);

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const Standard_OVERRIDE;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage for holding data without considering overheads and allocation alignment rules.
  Standard_EXPORT virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE;

  //! Increment draw calls statistics.
  Standard_EXPORT virtual void UpdateDrawStats (Graphic3d_FrameStatsDataTmp& theStats,
                                                bool theIsDetailed) const Standard_OVERRIDE;

public: //! @name methods for compatibility with layers

  //! Empty constructor
  Standard_EXPORT OpenGl_Text();

  //! Create key for shared resource
  Standard_EXPORT static TCollection_AsciiString FontKey (const OpenGl_Aspects& theAspect,
                                                          Standard_Integer theHeight,
                                                          unsigned int theResolution,
                                                          Font_Hinting theFontHinting);

  //! Find shared resource for specified font or initialize new one
  Standard_EXPORT static Handle(OpenGl_Font) FindFont (const Handle(OpenGl_Context)& theCtx,
                                                       const OpenGl_Aspects& theAspect,
                                                       Standard_Integer theHeight,
                                                       unsigned int theResolution,
                                                       Font_Hinting theFontHinting,
                                                       const TCollection_AsciiString& theKey);

  //! Compute text width
  Standard_EXPORT static void StringSize (const Handle(OpenGl_Context)& theCtx,
                                          const NCollection_String&     theText,
                                          const OpenGl_Aspects&         theTextAspect,
                                          const Standard_ShortReal      theHeight,
                                          const unsigned int            theResolution,
                                          const Font_Hinting            theFontHinting,
                                          Standard_ShortReal&           theWidth,
                                          Standard_ShortReal&           theAscent,
                                          Standard_ShortReal&           theDescent);

  //! Perform rendering
  Standard_EXPORT void Render (const Handle(OpenGl_Context)& theCtx,
                               const OpenGl_Aspects& theTextAspect,
                               unsigned int theResolution = Graphic3d_RenderingParams::THE_DEFAULT_RESOLUTION,
                               Font_Hinting theFontHinting = Font_Hinting_Off) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

//! @name obsolete methods
public:

  //! Setup new string and position
  Standard_DEPRECATED("Deprecated method Init() with obsolete arguments, use Init() and Text() instead of it")
  Standard_EXPORT void Init (const Handle(OpenGl_Context)& theCtx,
                             const Standard_Utf8Char*      theText,
                             const OpenGl_Vec3&            thePoint);

  //! Setup new position
  Standard_DEPRECATED("Deprecated method SetPosition(), use Graphic3d_Text for it")
  Standard_EXPORT void SetPosition (const OpenGl_Vec3& thePoint);

protected:

  friend class OpenGl_Trihedron;
  friend class OpenGl_GraduatedTrihedron;

  //! Release cached VBO resources
  Standard_EXPORT void releaseVbos (OpenGl_Context* theCtx);

private:

  //! Setup matrix.
  void setupMatrix (const Handle(OpenGl_Context)& theCtx,
                    const OpenGl_Aspects& theTextAspect,
                    const OpenGl_Vec3& theDVec) const;

  //! Draw arrays of vertices.
  void drawText (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_Aspects& theTextAspect) const;

  //! Draw rectangle from bounding text box.
  void drawRect (const Handle(OpenGl_Context)& theCtx,
                 const OpenGl_Aspects& theTextAspect,
                 const OpenGl_Vec4& theColorSubs) const;

  //! Main rendering code
  void render (const Handle(OpenGl_Context)& theCtx,
               const OpenGl_Aspects& theTextAspect,
               const OpenGl_Vec4& theColorText,
               const OpenGl_Vec4& theColorSubs,
               unsigned int theResolution,
               Font_Hinting theFontHinting) const;

protected:

  Handle(Graphic3d_Text)                                  myText;     //!< text parameters
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
  mutable OpenGl_Vec3d myWinXYZ;
  mutable GLdouble myScaleHeight;

protected:

  Standard_Boolean myIs2d;
public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Text_Header
