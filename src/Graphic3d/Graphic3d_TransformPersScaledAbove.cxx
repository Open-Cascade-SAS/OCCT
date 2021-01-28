// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <Graphic3d_TransformPersScaledAbove.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TransformPersScaledAbove, Graphic3d_TransformPers)

// =======================================================================
// function : Graphic3d_TransformPersScaledAbove
// purpose  :
// =======================================================================
Graphic3d_TransformPersScaledAbove::Graphic3d_TransformPersScaledAbove (const Standard_Real theScale,
                                                                        const gp_Pnt& thePnt)
: Graphic3d_TransformPers (Graphic3d_TMF_ZoomPers, thePnt),
  myScale (theScale)
{
}

// =======================================================================
// function : persistentScale
// purpose  :
// =======================================================================
Standard_Real Graphic3d_TransformPersScaledAbove::persistentScale (const Handle(Graphic3d_Camera)& theCamera,
                                                                   const Standard_Integer theViewportWidth,
                                                                   const Standard_Integer theViewportHeight) const
{
  Standard_Real aScale = base_type::persistentScale (theCamera, theViewportWidth, theViewportHeight);
  if (aScale < myScale)
  {
    // do not apply zoom persistent, the model is zoomed
    return myScale;
  }
  return aScale;
}
