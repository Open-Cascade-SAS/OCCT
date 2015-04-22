// Created on: 2014-11-14
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectBasics_PickResult_HeaderFile
#define _SelectBasics_PickResult_HeaderFile

#include <Standard.hxx>
#include <NCollection_Vec4.hxx>

//! This structure provides unified access to the results of
//! Matches() method in all sensitive entities.
struct SelectBasics_PickResult
{
public:

  SelectBasics_PickResult()
  : myDepth (DBL_MAX),
    myDistToCenter (DBL_MAX) {}

  SelectBasics_PickResult (const Standard_Real theDepth,
                           const Standard_Real theDistToCenter)
  : myDepth (theDepth),
    myDistToCenter (theDistToCenter) {}

public:
  inline Standard_Real Depth() const
  {
    return myDepth;
  }

  inline Standard_Real DistToGeomCenter() const
  {
    return myDistToCenter;
  }

private:
  //!< Depth to detected point
  Standard_Real                   myDepth;
  //!< Distance from 3d projection user-picked selection point to entity's geometry center
  Standard_Real                   myDistToCenter;
};

#endif // _SelectBasics_PickResult_HeaderFile
