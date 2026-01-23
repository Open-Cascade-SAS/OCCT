// Created on: 1991-04-23
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <TopAbs.hxx>

#include <TCollection_AsciiString.hxx>

namespace
{
static const char* TopAbs_Table_PrintShapeEnum[9] =
  {"COMPOUND", "COMPSOLID", "SOLID", "SHELL", "FACE", "WIRE", "EDGE", "VERTEX", "SHAPE"};

static const char* TopAbs_Table_PrintOrientation[4] = {"FORWARD",
                                                       "REVERSED",
                                                       "INTERNAL",
                                                       "EXTERNAL"};
} // namespace

//=================================================================================================

const char* TopAbs::ShapeTypeToString(TopAbs_ShapeEnum theType)
{
  return TopAbs_Table_PrintShapeEnum[theType];
}

//=================================================================================================

bool TopAbs::ShapeTypeFromString(const char* theTypeString, TopAbs_ShapeEnum& theType)
{
  TCollection_AsciiString aName(theTypeString);
  aName.UpperCase();
  for (int aTypeIter = 0; aTypeIter <= TopAbs_SHAPE; ++aTypeIter)
  {
    const char* aTypeName = TopAbs_Table_PrintShapeEnum[aTypeIter];
    if (aName == aTypeName)
    {
      theType = TopAbs_ShapeEnum(aTypeIter);
      return true;
    }
  }
  return false;
}

//=================================================================================================

const char* TopAbs::ShapeOrientationToString(TopAbs_Orientation theOrientation)
{
  return TopAbs_Table_PrintOrientation[theOrientation];
}

//=================================================================================================

bool TopAbs::ShapeOrientationFromString(const char* const   theOrientationString,
                                        TopAbs_Orientation& theOrientation)
{
  TCollection_AsciiString aName(theOrientationString);
  aName.UpperCase();
  for (int anOrientationIter = 0; anOrientationIter <= TopAbs_EXTERNAL; ++anOrientationIter)
  {
    const char* anOrientationName = TopAbs_Table_PrintOrientation[anOrientationIter];
    if (aName == anOrientationName)
    {
      theOrientation = TopAbs_Orientation(anOrientationIter);
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : TopAbs_Print
// purpose  : print the name of a State on a stream.
//=======================================================================

Standard_OStream& TopAbs::Print(const TopAbs_State st, Standard_OStream& s)
{
  static const char* const TopAbs_Table_PrintState[4] = {"ON", "IN", "OUT", "UNKNOWN"};
  return (s << TopAbs_Table_PrintState[(int)st]);
}
