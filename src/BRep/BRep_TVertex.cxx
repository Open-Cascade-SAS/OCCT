// Created on: 1992-08-25
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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


#include <BRep_TVertex.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_TVertex,TopoDS_TVertex)

//=======================================================================
//function : BRep_TVertex
//purpose  : 
//=======================================================================
BRep_TVertex::BRep_TVertex() :
       TopoDS_TVertex(),
       myTolerance(RealEpsilon())
{
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TVertex::EmptyCopy() const
{
  Handle(BRep_TVertex) TV = 
    new BRep_TVertex();
  TV->Pnt(myPnt);
  TV->Tolerance(myTolerance);
  return TV;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void BRep_TVertex::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TopoDS_TVertex)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPnt)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTolerance)
  for (BRep_ListIteratorOfListOfPointRepresentation itr(myPoints); itr.More(); itr.Next())
  {
    const Handle(BRep_PointRepresentation)& aPointRepresentation = itr.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aPointRepresentation.get())
  }
}
