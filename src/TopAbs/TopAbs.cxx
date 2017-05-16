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
  static Standard_CString TopAbs_Table_PrintShapeEnum[9] =
  {
    "COMPOUND","COMPSOLID","SOLID","SHELL","FACE","WIRE","EDGE","VERTEX","SHAPE"
  };

  static Standard_CString TopAbs_Table_PrintOrientation[4] =
  {
    "FORWARD","REVERSED","INTERNAL","EXTERNAL"
  };
}

//=======================================================================
//function : ShapeTypeToString
//purpose  :
//=======================================================================
Standard_CString TopAbs::ShapeTypeToString (TopAbs_ShapeEnum theType)
{
  return TopAbs_Table_PrintShapeEnum[theType];
}

//=======================================================================
//function : ShapeTypeFromString
//purpose  :
//=======================================================================
Standard_Boolean TopAbs::ShapeTypeFromString (Standard_CString theTypeString,
                                              TopAbs_ShapeEnum& theType)
{
  TCollection_AsciiString aName (theTypeString);
  aName.UpperCase();
  for (Standard_Integer aTypeIter = 0; aTypeIter <= TopAbs_SHAPE; ++aTypeIter)
  {
    Standard_CString aTypeName = TopAbs_Table_PrintShapeEnum[aTypeIter];
    if (aName == aTypeName)
    {
      theType = TopAbs_ShapeEnum(aTypeIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : ShapeOrientationToString
//purpose  :
//=======================================================================
Standard_CString TopAbs::ShapeOrientationToString (TopAbs_Orientation theOrientation)
{
  return TopAbs_Table_PrintOrientation[theOrientation];
}

//=======================================================================
//function : TopAbs_Compose
//purpose  : Compose two orientations
//=======================================================================
TopAbs_Orientation TopAbs::Compose(const TopAbs_Orientation O1,
                                   const TopAbs_Orientation O2)
{
  // see the composition table in the file TopAbs.cdl
  static const TopAbs_Orientation TopAbs_Table_Compose[4][4] =
  {
    { TopAbs_FORWARD,  TopAbs_REVERSED, TopAbs_INTERNAL, TopAbs_EXTERNAL },
    { TopAbs_REVERSED, TopAbs_FORWARD,  TopAbs_INTERNAL, TopAbs_EXTERNAL },
    { TopAbs_INTERNAL, TopAbs_INTERNAL, TopAbs_INTERNAL, TopAbs_INTERNAL },
    { TopAbs_EXTERNAL, TopAbs_EXTERNAL, TopAbs_EXTERNAL, TopAbs_EXTERNAL }
  };
  return TopAbs_Table_Compose[(Standard_Integer)O2][(Standard_Integer)O1];
}

//=======================================================================
//function : TopAbs::Reverse
//purpose  : reverse an Orientation
//=======================================================================

TopAbs_Orientation TopAbs::Reverse(const TopAbs_Orientation Ori)
{
  static const TopAbs_Orientation TopAbs_Table_Reverse[4] =
  {
    TopAbs_REVERSED, TopAbs_FORWARD, TopAbs_INTERNAL, TopAbs_EXTERNAL
  };
  return TopAbs_Table_Reverse[(Standard_Integer)Ori];
}

//=======================================================================
//function : TopAbs::Complement
//purpose  : complement an Orientation
//=======================================================================

TopAbs_Orientation TopAbs::Complement(const TopAbs_Orientation Ori)
{
  static const TopAbs_Orientation TopAbs_Table_Complement[4] =
  {
    TopAbs_REVERSED, TopAbs_FORWARD, TopAbs_EXTERNAL, TopAbs_INTERNAL
  };
  return TopAbs_Table_Complement[(Standard_Integer)Ori];
}

//=======================================================================
//function : TopAbs_Print
//purpose  : print the name of a State on a stream.
//=======================================================================

Standard_OStream& TopAbs::Print(const TopAbs_State st,
                                Standard_OStream& s)
{
  static const Standard_CString TopAbs_Table_PrintState[4] =
  {
    "ON","IN","OUT","UNKNOWN"
  };
  return (s << TopAbs_Table_PrintState[(Standard_Integer)st]);
}
