// Created on: 1993-05-05
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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


#include <MAT_Arc.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Node.hxx>
#include <MAT_SequenceOfArc.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_BasicElt,Standard_Transient)

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
void  MAT_BasicElt::SetStartArc(const Handle(MAT_Arc)& anArc)
{
  startLeftArc = anArc.operator->();
}


//========================================================================
// function: SetEndArc
// purpose :
//========================================================================
void  MAT_BasicElt::SetEndArc(const Handle(MAT_Arc)& anArc)
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





