// Created on: 1991-04-23
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Modified by model, Thu Jun 25 10:40:27 1992

#include <TopAbs.ixx>


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
//purpose  : print the name of a ShapeEnum on a stream.
//=======================================================================

Standard_OStream& TopAbs::Print(const TopAbs_ShapeEnum se,
                                       Standard_OStream& s)
{
  static const Standard_CString TopAbs_Table_PrintShapeEnum[9] =
  {
    "COMPOUND","COMPSOLID","SOLID","SHELL","FACE","WIRE","EDGE","VERTEX","SHAPE"
  };
  return (s << TopAbs_Table_PrintShapeEnum[(Standard_Integer)se]);
}

//=======================================================================
//function : TopAbs_Print
//purpose  : print the name of an Orientation on a stream
//=======================================================================

Standard_OStream& TopAbs::Print(const TopAbs_Orientation ori,
                                       Standard_OStream& s)
{
  static const Standard_CString TopAbs_Table_PrintOrientation[4] =
  {
    "FORWARD","REVERSED","INTERNAL","EXTERNAL"
  };
  return (s << TopAbs_Table_PrintOrientation[(Standard_Integer)ori]);
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
