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

#include <OpenGl_Disk.hxx>

// =======================================================================
// function : OpenGl_Disk
// purpose  :
// =======================================================================
OpenGl_Disk::OpenGl_Disk()
: myInnerRadius (1.0f),
  myOuterRadius (1.0f)
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Disk::Init (const Standard_ShortReal theInnerRadius,
                                    const Standard_ShortReal theOuterRadius,
                                    const Standard_Integer   theNbSlices,
                                    const Standard_Integer   theNbStacks)
{
  myInnerRadius = theInnerRadius;
  myOuterRadius = theOuterRadius;
  return OpenGl_Quadric::init (theNbSlices, theNbStacks);
}

// =======================================================================
// function : evalVertex
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Disk::evalVertex (const Standard_ShortReal theU,
                                     const Standard_ShortReal theV) const
{
  const Standard_ShortReal aU      = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aRadius = myInnerRadius + (myOuterRadius - myInnerRadius) * theV;
  return OpenGl_Vec3 (cosf (aU) * aRadius, sinf (aU) * aRadius, 0.0f);
}

// =======================================================================
// function : evalNormal
// purpose  :
// =======================================================================
OpenGl_Vec3 OpenGl_Disk::evalNormal (const Standard_ShortReal /*theU*/,
                                     const Standard_ShortReal /*theV*/) const
{
  return OpenGl_Vec3 (0.0f, 0.0f, -1.0f);
}
