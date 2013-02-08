// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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
  Standard_EXPORT OpenGl_Text (const TCollection_ExtendedString& theText,
                               const OpenGl_Vec3&                thePoint,
                               const OpenGl_TextParam&           theParams);

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
  Standard_EXPORT virtual void Release (const Handle(OpenGl_Context)&   theContext);

public: //! @name methods for compatibility with layers

  //! Empty constructor
  OpenGl_Text();

  //! Create key for shared resource
  static TCollection_AsciiString FontKey (const OpenGl_AspectText& theAspect,
                                          const Standard_Integer   theHeight);

  //! Find shared resource for specified font or initialize new one
  static Handle(OpenGl_Font) FindFont (const Handle(OpenGl_Context)& theCtx,
                                       const OpenGl_AspectText&      theAspect,
                                       const Standard_Integer        theHeight,
                                       const TCollection_AsciiString theKey);

  //! Compute text width
  static void StringSize (const Handle(OpenGl_Context)& theCtx,
                          const NCollection_String&     theText,
                          const OpenGl_AspectText&      theTextAspect,
                          const OpenGl_TextParam&       theParams,
                          Standard_ShortReal&           theWidth,
                          Standard_ShortReal&           theAscent,
                          Standard_ShortReal&           theDescent);

  //! Setup new string and parameters
  void Init (const Handle(OpenGl_Context)&     theCtx,
             const TCollection_ExtendedString& theText,
             const OpenGl_Vec2&                thePoint,
             const OpenGl_TextParam&           theParams);

  //! Perform rendering
  void Render (const Handle(OpenGl_PrinterContext)& thePrintCtx,
               const Handle(OpenGl_Context)&        theCtx,
               const OpenGl_AspectText&             theTextAspect) const;

protected:

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_Text();

  friend class OpenGl_Trihedron;
  friend class OpenGl_GraduatedTrihedron;

private:

  //! Release cached VBO resources
  void releaseVbos (const Handle(OpenGl_Context)& theCtx);

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
  mutable NCollection_Vector<Handle(OpenGl_Vec2Array)>    myVertsArray; //!< arrays of vertices (for compatibility mode)
  mutable NCollection_Vector<Handle(OpenGl_Vec2Array)>    myTCrdsArray; //!< arrays of vertices (for compatibility mode)
  mutable Font_FTFont::Rect                               myBndBox;

protected:

  mutable GLdouble myProjMatrix[16];
  mutable GLdouble myModelMatrix[16];
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
