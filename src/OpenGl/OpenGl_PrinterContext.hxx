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

#include <Standard_DefineHandle.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Handle_Standard_Transient.hxx>

//! Class provides specific information for redrawing view to offscreen buffer
//! on printing. The information is: projection matrixes for tiling,
//! scaling factors for text/markers and layer viewport dimensions.
class OpenGl_PrinterContext : public Standard_Transient
{

public:

  //! Constructor
  OpenGl_PrinterContext();

  //! Destructor
  virtual ~OpenGl_PrinterContext();

  //! Get view projection transformation matrix.
  inline const TColStd_Array2OfReal& GetProjTransformation() const
  {
    return myProjTransform; 
  }

  //! Set view projection transformation matrix for printing/tiling purposes
  //! theProjTransform parameter should be an 4x4 array.
  bool SetProjTransformation (const TColStd_Array2OfReal& theProjTransform);

  //! Setup view projection transformation matrix (glLoadMatrixf).
  void LoadProjTransformation();

  //! Get text/markers scale factor
  inline void GetScale (Standard_ShortReal& theScaleX,
                        Standard_ShortReal& theScaleY) const
  {
    theScaleX = myScaleX;
    theScaleY = myScaleY;
  }

  //! Set text scale factor
  void SetScale (const Standard_ShortReal theScaleX,
                 const Standard_ShortReal theScaleY);

  //! Get layer viewport dimensions
  inline void GetLayerViewport (Standard_Integer& theViewportX,
                                Standard_Integer& theViewportY) const
  {
    theViewportX = myLayerViewportX;
    theViewportY = myLayerViewportY;
  }

  //! Set layer viewport dimensions
  void SetLayerViewport (const Standard_Integer theViewportX,
                         const Standard_Integer theViewportY);

private:

  TColStd_Array2OfReal myProjTransform;
  Standard_ShortReal   myProjMatrixGl[16];
  Standard_ShortReal   myScaleX;
  Standard_ShortReal   myScaleY;
  Standard_Integer     myLayerViewportX;
  Standard_Integer     myLayerViewportY;

public:

  DEFINE_STANDARD_RTTI(OpenGl_PrinterContext) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_PrinterContext, Standard_Transient)

#endif
