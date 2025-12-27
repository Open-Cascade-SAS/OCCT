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
#include <MAT_Arc.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_BasicElt, Standard_Transient)

//=================================================================================================

MAT_BasicElt::MAT_BasicElt(const int anInteger)
    : startLeftArc(0),
      endLeftArc(0),
      index(anInteger),
      geomIndex(0)
{
}

//=================================================================================================

occ::handle<MAT_Arc> MAT_BasicElt::StartArc() const
{
  return (MAT_Arc*)startLeftArc;
}

//=================================================================================================

occ::handle<MAT_Arc> MAT_BasicElt::EndArc() const
{
  return (MAT_Arc*)endLeftArc;
}

//=================================================================================================

int MAT_BasicElt::Index() const
{
  return index;
}

//=================================================================================================

int MAT_BasicElt::GeomIndex() const
{
  return geomIndex;
}

//=================================================================================================

void MAT_BasicElt::SetStartArc(const occ::handle<MAT_Arc>& anArc)
{
  startLeftArc = anArc.get();
}

//=================================================================================================

void MAT_BasicElt::SetEndArc(const occ::handle<MAT_Arc>& anArc)
{
  endLeftArc = anArc.get();
}

//=================================================================================================

void MAT_BasicElt::SetIndex(const int anInteger)
{
  index = anInteger;
}

//=================================================================================================

void MAT_BasicElt::SetGeomIndex(const int anInteger)
{
  geomIndex = anInteger;
}
