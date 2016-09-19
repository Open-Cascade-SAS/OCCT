// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Graphic3d_TransformPers.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TransformPers, Standard_Transient)

// =======================================================================
// function : FromDeprecatedParams
// purpose  :
// =======================================================================
Handle(Graphic3d_TransformPers) Graphic3d_TransformPers::FromDeprecatedParams (Graphic3d_TransModeFlags theFlag,
                                                                               const gp_Pnt&            thePoint)
{
  Handle(Graphic3d_TransformPers) aTrsfPers;
  if (Graphic3d_TransformPers::IsZoomOrRotate (theFlag))
  {
    aTrsfPers = new Graphic3d_TransformPers (theFlag, thePoint);
  }
  else if (Graphic3d_TransformPers::IsTrihedronOr2d (theFlag))
  {
    Standard_Integer aCorner = Aspect_TOTP_CENTER;
    const Graphic3d_Vec2i anOffset (Standard_Integer(thePoint.Z()), Standard_Integer(thePoint.Z()));
    if (thePoint.X() > 0.0)
    {
      aCorner |= Aspect_TOTP_RIGHT;
    }
    else if (thePoint.X() < 0.0)
    {
      aCorner |= Aspect_TOTP_LEFT;
    }
    if (thePoint.Y() > 0.0)
    {
      aCorner |= Aspect_TOTP_TOP;
    }
    else if (thePoint.Y() < 0.0)
    {
      aCorner |= Aspect_TOTP_BOTTOM;
    }
    aTrsfPers = new Graphic3d_TransformPers (theFlag, Aspect_TypeOfTriedronPosition(aCorner), anOffset);
  }
  return aTrsfPers;
}
