// Created on: 2011-05-20
// Created by: Anton POLETAEV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OPENGL_PRINTERCONTEXT_H
#define _OPENGL_PRINTERCONTEXT_H

#include <Standard_DefineHandle.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Handle_Standard_Transient.hxx>
#include <OpenGl_Vec.hxx>

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

  //! Returns projection matrix.
  OpenGl_Mat4 ProjTransformation();

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
