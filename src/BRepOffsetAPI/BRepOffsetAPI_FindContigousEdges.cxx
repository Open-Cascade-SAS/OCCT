// File:	BRepOffsetAPI_FindContigousEdges.cxx
// Created:	Fri May 5 09:45:44 1995
// Author:	Jing Cheng MEI
//		<mei@junon>

#include <BRepOffsetAPI_FindContigousEdges.ixx>


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

