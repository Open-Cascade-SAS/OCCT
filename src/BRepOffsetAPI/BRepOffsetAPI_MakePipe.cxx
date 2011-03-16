// File:	BRepOffsetAPI_MakePipe.cxx
// Created:	Tue Jul 12 10:33:07 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>

#include <BRepOffsetAPI_MakePipe.ixx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakePipe
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakePipe::BRepOffsetAPI_MakePipe(const TopoDS_Wire&  Spine ,
				   const TopoDS_Shape& Profile)
     : myPipe(Spine, Profile)
{
  Build();
}

//=======================================================================
//function : Pipe
//purpose  : 
//=======================================================================

const BRepFill_Pipe& BRepOffsetAPI_MakePipe::Pipe() const
{
  return myPipe;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakePipe::Build() 
{
  myShape = myPipe.Shape();
  Done();
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::FirstShape()
{
  return myPipe.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::LastShape()
{
  return myPipe.LastShape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_MakePipe::Generated (const TopoDS_Shape& SSpine,
					  const TopoDS_Shape& SProfile)
{
  if (SProfile.ShapeType () == TopAbs_EDGE) {
    return myPipe.Face (TopoDS::Edge (SSpine), TopoDS::Edge (SProfile));
  }
  else if (SProfile.ShapeType () == TopAbs_VERTEX) {
    return myPipe.Edge (TopoDS::Edge (SSpine), TopoDS::Vertex (SProfile));
  }

//POP pour NT
  TopoDS_Shape bid;
  return bid;
}

