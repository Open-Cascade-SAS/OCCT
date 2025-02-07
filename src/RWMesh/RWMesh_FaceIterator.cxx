// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWMesh_FaceIterator.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>

//=================================================================================================

RWMesh_FaceIterator::RWMesh_FaceIterator(const TDF_Label&       theLabel,
                                         const TopLoc_Location& theLocation,
                                         const Standard_Boolean theToMapColors,
                                         const XCAFPrs_Style&   theStyle)
    : RWMesh_ShapeIterator(theLabel,
                           theLocation,
                           TopAbs_FACE,
                           TopAbs_SHAPE,
                           theToMapColors,
                           theStyle),
      mySLTool(1, 1e-12),
      myHasNormals(false),
      myIsMirrored(false)
{
  Next();
}

//=================================================================================================

RWMesh_FaceIterator::RWMesh_FaceIterator(const TopoDS_Shape&  theShape,
                                         const XCAFPrs_Style& theStyle)
    : RWMesh_ShapeIterator(theShape, TopAbs_FACE, TopAbs_SHAPE, theStyle),
      mySLTool(1, 1e-12),
      myHasNormals(false),
      myIsMirrored(false)
{
  Next();
}

//=================================================================================================

gp_Dir RWMesh_FaceIterator::normal(Standard_Integer theNode) const
{
  gp_Dir aNormal(gp::DZ());
  if (myPolyTriang->HasNormals())
  {
    Graphic3d_Vec3 aNormVec3;
    myPolyTriang->Normal(theNode, aNormVec3);
    if (aNormVec3.Modulus() != 0.0f)
    {
      aNormal.SetCoord(aNormVec3.x(), aNormVec3.y(), aNormVec3.z());
    }
  }
  else if (myHasNormals && myPolyTriang->HasUVNodes())
  {
    const gp_XY anUV = myPolyTriang->UVNode(theNode).XY();
    mySLTool.SetParameters(anUV.X(), anUV.Y());
    if (mySLTool.IsNormalDefined())
    {
      aNormal = mySLTool.Normal();
    }
  }
  return aNormal;
}

//=================================================================================================

void RWMesh_FaceIterator::Next()
{
  for (; myIter.More(); myIter.Next())
  {
    myFace       = TopoDS::Face(myIter.Current());
    myPolyTriang = BRep_Tool::Triangulation(myFace, myLocation);
    myTrsf       = myLocation.Transformation();
    if (myPolyTriang.IsNull() || myPolyTriang->NbTriangles() == 0)
    {
      resetFace();
      continue;
    }

    initFace();
    myIter.Next();
    return;
  }

  resetFace();
}

//=================================================================================================

void RWMesh_FaceIterator::initFace()
{
  myHasNormals = false;
  myIsMirrored = myTrsf.VectorialPart().Determinant() < 0.0;
  if (myPolyTriang->HasNormals())
  {
    myHasNormals = true;
  }
  if (myPolyTriang->HasUVNodes() && !myHasNormals)
  {
    TopoDS_Face aFaceFwd = TopoDS::Face(myFace.Oriented(TopAbs_FORWARD));
    aFaceFwd.Location(TopLoc_Location());
    TopLoc_Location aLoc;
    if (!BRep_Tool::Surface(aFaceFwd, aLoc).IsNull())
    {
      myFaceAdaptor.Initialize(aFaceFwd, false);
      mySLTool.SetSurface(myFaceAdaptor);
      myHasNormals = true;
    }
  }

  initShape();
}
