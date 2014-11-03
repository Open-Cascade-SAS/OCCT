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

#include <OpenGl_Sphere.hxx>

// =======================================================================
// function : OpenGl_Sphere
// purpose  :
// =======================================================================
OpenGl_Sphere::OpenGl_Sphere()
: myRadius (1.0f)
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Sphere::Init (const Standard_ShortReal theRadius,
                                      const Standard_Integer   theNbSlices,
                                      const Standard_Integer   theNbStacks)
{
  myRadius = theRadius;
  return OpenGl_Quadric::init (theNbSlices, theNbStacks);
}

// =======================================================================
// function : evalVertex
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Sphere::evalVertex (const Standard_ShortReal theU,
                                       const Standard_ShortReal theV) const
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aV = static_cast<Standard_ShortReal> (theV * M_PI);
  return OpenGl_Vec3 (myRadius * cosf (aU) * sinf (aV),
                     -myRadius * sinf (aU) * sinf (aV),
                      myRadius * cosf (aV));
}

// =======================================================================
// function : evalNormal
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Sphere::evalNormal (const Standard_ShortReal theU,
                                       const Standard_ShortReal theV) const
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aV = static_cast<Standard_ShortReal> (theV * M_PI);
  return OpenGl_Vec3 (cosf (aU) * sinf (aV),
                     -sinf (aU) * sinf (aV),
                      cosf (aV));
}
