// Created on: 2011-05-20
// Created by: Anton POLETAEV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#ifndef _OPENGL_PRINTERCONTEXT_H
#define _OPENGL_PRINTERCONTEXT_H

#include <OpenGl_GlCore11.hxx>

#include <MMgt_TShared.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_MMgt_TShared.hxx>
#include <NCollection_DataMap.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <OpenGl_Workspace.hxx>

class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class OpenGl_PrinterContext;

DEFINE_STANDARD_HANDLE(OpenGl_PrinterContext,MMgt_TShared)

//! Class provides specific information for redrawing view to offscreen buffer
//! on printing. The information is: projection matrixes for tiling,
//! scaling factors for text/markers and layer viewport dimensions.
//! The OpenGl_PrinterContext class allows to have only one global instance
//! that can be accessed by GetPrinterContext() during printing operation. 
//! The class instance can be created only by call_togl_print().
class OpenGl_PrinterContext : public MMgt_TShared
{

public:

  //! Get the PrinterContext instance assigned for OpenGl context.
  //! Return NULL, if there is no current printing operation and
  //! there is no assigned instance for "theCtx" OpenGl context.
  static OpenGl_PrinterContext* GetPrinterContext(GLCONTEXT theCtx);

  //! Get view projection transformation matrix.
  const TColStd_Array2OfReal& GetProjTransformation () 
  {
    return myProjTransform; 
  }

  //! Get view projection transformation matrix.
  void GetProjTransformation (GLfloat theMatrix[16]); 

  //! Get text/markers scale factor
  void GetScale (GLfloat& theScaleX, GLfloat& theScaleY)
  {
    theScaleX = myScaleX;
    theScaleY = myScaleY;
  }

  //! Get layer viewport dimensions
  void GetLayerViewport (GLsizei& theViewportX,
                         GLsizei& theViewportY)
  {
    theViewportX = myLayerViewportX;
    theViewportY = myLayerViewportY;
  }

private:

  //! Constructor
  OpenGl_PrinterContext (GLCONTEXT theCtx);

  //! Destructor
  virtual ~OpenGl_PrinterContext ();

  //! Deactivate current printing context.
  //! Useful when you need to redraw in usual mode the same OpenGl context
  //! that you used for printing right after printing, before the 
  //! OpenGl_PrinterContext instance destroyed.
  void Deactivate ();

  //! Set view projection transformation matrix for printing/tiling purposes
  //! theProjTransform parameter should be an 4x4 array.
  bool SetProjTransformation (TColStd_Array2OfReal& theProjTransform);

  //! Set text scale factor
  void SetScale (GLfloat theScaleX, GLfloat theScaleY)
  {
    myScaleX = theScaleX;
    myScaleY = theScaleY;
  }

  //! Set layer viewport dimensions
  void SetLayerViewport (GLsizei theViewportX,
                         GLsizei theViewportY) 
  {
    myLayerViewportX = theViewportX; 
    myLayerViewportY = theViewportY;
  }

private:

  static OpenGl_PrinterContext* g_PrinterContext;
  static GLCONTEXT g_ContextId;
  TColStd_Array2OfReal myProjTransform;
  GLfloat              myScaleX;
  GLfloat              myScaleY;
  GLsizei              myLayerViewportX;
  GLsizei              myLayerViewportY;
  GLCONTEXT            myCtx;

  // the printer context could be created only in method call_togl_print
  /*friend Standard_Boolean call_togl_print (CALL_DEF_VIEW *, CALL_DEF_LAYER *,
                                           CALL_DEF_LAYER *, 
                                           const Aspect_Drawable, const int,
                                           const char*, const int, const float);*/
  friend Standard_Boolean OpenGl_Workspace::Print (const Graphic3d_CView& ACView,
                                                  const Aspect_CLayer2d& ACUnderLayer, 
                                                  const Aspect_CLayer2d& ACOverLayer,
                                                  const Aspect_Handle    hPrintDC,
                                                  const Standard_Boolean showBackground,
                                                  const Standard_CString filename,
                                                  const Aspect_PrintAlgo printAlgorithm,
                                                  const Standard_Real theScaleFactor);
};

#endif
