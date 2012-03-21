// Created on: 1993-05-05
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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



#include <MAT_BasicElt.ixx>
#include <MAT_SequenceOfArc.hxx>
#include <MAT_Node.hxx>

//========================================================================
// function:
// purpose :
//========================================================================
MAT_BasicElt::MAT_BasicElt(const Standard_Integer anInteger)
     : startLeftArc (0),
      endLeftArc   (0),
      index(anInteger)
{
}

//========================================================================
// function: StartArc
// purpose :
//========================================================================
Handle(MAT_Arc)  MAT_BasicElt::StartArc() const
{
  return (MAT_Arc*)startLeftArc;
}

//========================================================================
// function: EndArc
// purpose :
//========================================================================
Handle(MAT_Arc)  MAT_BasicElt::EndArc() const
{
  return (MAT_Arc*)endLeftArc;
}

//========================================================================
// function: Index
// purpose :
//========================================================================
Standard_Integer  MAT_BasicElt::Index() const
{
   return index;
}

//========================================================================
// function: GeomIndex
// purpose :
//========================================================================
Standard_Integer  MAT_BasicElt::GeomIndex() const
{
   return geomIndex;
}


//========================================================================
// function: SetStartArc
// purpose :
//========================================================================
void  MAT_BasicElt::SetStartArc(const Handle_MAT_Arc& anArc)
{
  startLeftArc = anArc.operator->();
}


//========================================================================
// function: SetEndArc
// purpose :
//========================================================================
void  MAT_BasicElt::SetEndArc(const Handle_MAT_Arc& anArc)
{
  endLeftArc = anArc.operator->();
}

//========================================================================
// function: SetIndex
// purpose :
//========================================================================
void MAT_BasicElt::SetIndex(const Standard_Integer anInteger)
{
  index = anInteger;
}

//========================================================================
// function: SetGeomIndex
// purpose :
//========================================================================
void MAT_BasicElt::SetGeomIndex(const Standard_Integer anInteger)
{
  geomIndex = anInteger;
}





