// Created on: 2014-10-15
// Created by: Denis Bogolepov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_Cylinder.hxx>

// =======================================================================
// function : OpenGl_Cylinder
// purpose  :
// =======================================================================
OpenGl_Cylinder::OpenGl_Cylinder()
: myBotRad (1.0f),
  myTopRad (1.0f),
  myHeight (1.0f)
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Cylinder::Init (const Standard_ShortReal theBotRad,
                                        const Standard_ShortReal theTopRad,
                                        const Standard_ShortReal theHeight,
                                        const Standard_Integer   theNbSlices,
                                        const Standard_Integer   theNbStacks)
{
  myBotRad = theBotRad;
  myTopRad = theTopRad;
  myHeight = theHeight;
  return OpenGl_Quadric::init (theNbSlices, theNbStacks);
}

// =======================================================================
// function : evalVertex
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Cylinder::evalVertex (const Standard_ShortReal theU,
                                         const Standard_ShortReal theV) const
{
  const Standard_ShortReal aU      = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aRadius = myBotRad + (myTopRad - myBotRad) * theV;
  return OpenGl_Vec3 (cosf (aU) * aRadius,
                      sinf (aU) * aRadius,
                      theV * myHeight);
}

// =======================================================================
// function : evalNormal
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Cylinder::evalNormal (const Standard_ShortReal theU,
                                         const Standard_ShortReal /*theV*/) const
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  return OpenGl_Vec3 (cosf (aU) * myHeight,
                      sinf (aU) * myHeight,
                      myBotRad - myTopRad).Normalized();
}
