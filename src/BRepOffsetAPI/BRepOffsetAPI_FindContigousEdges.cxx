// Created on: 1995-05-05
// Created by: Jing Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepOffsetAPI_FindContigousEdges.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepOffsetAPI_FindContigousEdges::BRepOffsetAPI_FindContigousEdges(const Standard_Real tolerance,
						       const Standard_Boolean option)
{
  mySewing = new BRepBuilderAPI_Sewing;
  Init(tolerance, option);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_FindContigousEdges::Init(const Standard_Real tolerance,
				      const Standard_Boolean option)
{
  mySewing->Init(tolerance, option, Standard_False,Standard_True);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepOffsetAPI_FindContigousEdges::Add(const TopoDS_Shape& aShape)
{
  mySewing->Add(aShape);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepOffsetAPI_FindContigousEdges::Perform()
{
  mySewing->Perform();
}


//=======================================================================
//function : NbContigousEdges
//purpose  : 
//=======================================================================

Standard_Integer BRepOffsetAPI_FindContigousEdges::NbContigousEdges() const
{
  return mySewing->NbContigousEdges();
}

//=======================================================================
//function : ContigousEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepOffsetAPI_FindContigousEdges::ContigousEdge(const Standard_Integer index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbContigousEdges(), "BRepOffsetAPI_FindContigousEdges::ContigousEdge");
  return mySewing->ContigousEdge(index);
}
//=======================================================================
//function : ContigousEdgeCouple
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_FindContigousEdges::ContigousEdgeCouple(const Standard_Integer index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbContigousEdges(), "BRepOffsetAPI_FindContigousEdges::ContigousEdgeCouple");
  return mySewing->ContigousEdgeCouple(index);
}

//=======================================================================
//function : SectionToBoundary
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepOffsetAPI_FindContigousEdges::SectionToBoundary(const TopoDS_Edge& section) const
{
  Standard_NoSuchObject_Raise_if(!mySewing->IsSectionBound(section), "BRepOffsetAPI_FindContigousEdges::SectionToBoundary");
  return mySewing->SectionToBoundary(section);
}

//=======================================================================
//function : NbDegeneratedShapes
//purpose  : 
//=======================================================================

Standard_Integer BRepOffsetAPI_FindContigousEdges::NbDegeneratedShapes() const
{
  return mySewing->NbDegeneratedShapes();
}

//=======================================================================
//function : DegeneratedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepOffsetAPI_FindContigousEdges::DegeneratedShape(const Standard_Integer index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbDegeneratedShapes(), "BRepOffsetAPI_FindContigousEdges::DegereratedShape");
  return mySewing->DegeneratedShape(index);
}

//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffsetAPI_FindContigousEdges::IsDegenerated(const TopoDS_Shape& aShape) const
{
  return mySewing->IsDegenerated(aShape);
}

//=======================================================================
//function : IsModified
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffsetAPI_FindContigousEdges::IsModified(const TopoDS_Shape& aShape) const
{
  return mySewing->IsModified(aShape);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepOffsetAPI_FindContigousEdges::Modified(const TopoDS_Shape& aShape) const
{
  Standard_NoSuchObject_Raise_if(!IsModified(aShape),"BRepOffsetAPI_FindContigousEdges::Modified");
  return mySewing->Modified(aShape);
}



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void BRepOffsetAPI_FindContigousEdges::Dump() const
{
  mySewing->Dump();
}

