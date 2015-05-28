// Created on: 2015-04-26
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BRepExtrema_SelfIntersection.hxx>

#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=======================================================================
//function : BRepExtrema_SelfIntersection
//purpose  :
//=======================================================================
BRepExtrema_SelfIntersection::BRepExtrema_SelfIntersection (const Standard_Real theTolerance)
: myTolerance (theTolerance)
{
  myIsInit = Standard_False;
}

//=======================================================================
//function : BRepExtrema_SelfIntersection
//purpose  :
//=======================================================================
BRepExtrema_SelfIntersection::BRepExtrema_SelfIntersection (const TopoDS_Shape& theShape, const Standard_Real theTolerance)
: myTolerance (theTolerance)
{
  LoadShape (theShape);
}

//=======================================================================
//function : LoadShape
//purpose  :
//=======================================================================
Standard_Boolean BRepExtrema_SelfIntersection::LoadShape (const TopoDS_Shape& theShape)
{
  myFaceList.Clear();

  for (TopExp_Explorer anIter (theShape, TopAbs_FACE); anIter.More(); anIter.Next())
  {
    myFaceList.Append (static_cast<const TopoDS_Face&> (anIter.Current()));
  }

  if (myElementSet.IsNull())
  {
    myElementSet = new BRepExtrema_TriangleSet;
  }

  myIsInit = myElementSet->Init (myFaceList);

  if (myIsInit)
  {
    myOverlapTool.LoadTriangleSets (myElementSet,
                                    myElementSet);
  }

  return myIsInit;
}

#define ZERO_VEC BVH_Vec3d (0.0, 0.0, 0.0)

namespace
{
  // =======================================================================
  // function : ccw
  // purpose  : Check if triple is in counterclockwise order
  // =======================================================================
  Standard_Boolean ccw (const BVH_Vec3d& theVertex0,
                        const BVH_Vec3d& theVertex1,
                        const BVH_Vec3d& theVertex2,
                        const Standard_Integer theX,
                        const Standard_Integer theY)
{
  const Standard_Real aSum =
    (theVertex1[theX] - theVertex0[theX]) * (theVertex1[theY] + theVertex0[theY]) +
    (theVertex2[theX] - theVertex1[theX]) * (theVertex2[theY] + theVertex1[theY]) +
    (theVertex0[theX] - theVertex2[theX]) * (theVertex0[theY] + theVertex2[theY]);

  return aSum < 0.0;
}

  // =======================================================================
  // function : rayInsideAngle
  // purpose  : Check the given ray is inside the angle
  // =======================================================================
  Standard_Boolean rayInsideAngle (const BVH_Vec3d&   theDirec,
                                   const BVH_Vec3d&   theEdge0,
                                   const BVH_Vec3d&   theEdge1,
                                   const Standard_Integer theX,
                                   const Standard_Integer theY)
{
  const Standard_Boolean aCCW = ccw (ZERO_VEC, theEdge0, theEdge1, theX, theY);

  return ccw (ZERO_VEC, theEdge0, theDirec, theX, theY) == aCCW
      && ccw (ZERO_VEC, theDirec, theEdge1, theX, theY) == aCCW;
}

  // =======================================================================
  // function : getProjectionAxes
  // purpose  :
  // =======================================================================
  void getProjectionAxes (const BVH_Vec3d&  theNorm,
                          Standard_Integer& theAxisX,
                          Standard_Integer& theAxisY)
{
  if (fabs (theNorm[0]) > fabs (theNorm[1]))
  {
    theAxisX = fabs (theNorm[0]) > fabs (theNorm[2]) ? 1 : 0;
    theAxisY = fabs (theNorm[0]) > fabs (theNorm[2]) ? 2 : 1;
  }
  else
  {
    theAxisX = fabs (theNorm[1]) > fabs (theNorm[2]) ? 0 : 0;
    theAxisY = fabs (theNorm[1]) > fabs (theNorm[2]) ? 2 : 1;
  }
}
}

//=======================================================================
//function : isRegularSharedVertex
//purpose  :
//=======================================================================
BRepExtrema_ElementFilter::FilterResult BRepExtrema_SelfIntersection::isRegularSharedVertex (const BVH_Vec3d& theSharedVert,
                                                                                             const BVH_Vec3d& theTrng0Vtxs1,
                                                                                             const BVH_Vec3d& theTrng0Vtxs2,
                                                                                             const BVH_Vec3d& theTrng1Vtxs1,
                                                                                             const BVH_Vec3d& theTrng1Vtxs2)
{
  const BVH_Vec3d aTrng0Edges[] = { (theTrng0Vtxs1 - theSharedVert).Normalized(),
                                    (theTrng0Vtxs2 - theSharedVert).Normalized() };

  const BVH_Vec3d aTrng1Edges[] = { (theTrng1Vtxs1 - theSharedVert).Normalized(),
                                    (theTrng1Vtxs2 - theSharedVert).Normalized() };

  const BVH_Vec3d aTrng0Normal = BVH_Vec3d::Cross (aTrng0Edges[0], aTrng0Edges[1]);
  const BVH_Vec3d aTrng1Normal = BVH_Vec3d::Cross (aTrng1Edges[0], aTrng1Edges[1]);

  BVH_Vec3d aCrossLine = BVH_Vec3d::Cross (aTrng0Normal,
                                           aTrng1Normal);

  Standard_Integer anX;
  Standard_Integer anY;

  if (aCrossLine.SquareModulus() < Precision::SquareConfusion()) // coplanar case
  {
    getProjectionAxes (aTrng0Normal, anX, anY);

    if (rayInsideAngle (aTrng1Edges[0], aTrng0Edges[0], aTrng0Edges[1], anX, anY)
     || rayInsideAngle (aTrng1Edges[1], aTrng0Edges[0], aTrng0Edges[1], anX, anY)
     || rayInsideAngle (aTrng0Edges[0], aTrng1Edges[0], aTrng1Edges[1], anX, anY)
     || rayInsideAngle (aTrng0Edges[1], aTrng1Edges[0], aTrng1Edges[1], anX, anY))
    {
      return BRepExtrema_ElementFilter::Overlap;
    }

    return BRepExtrema_ElementFilter::NoCheck;
  }
  else // shared line should lie outside at least one triangle
  {
    getProjectionAxes (aTrng0Normal, anX, anY);

    const Standard_Boolean aPosOutTrgn0 = !rayInsideAngle ( aCrossLine, aTrng0Edges[0], aTrng0Edges[1], anX, anY);
    const Standard_Boolean aNegOutTrgn0 = !rayInsideAngle (-aCrossLine, aTrng0Edges[0], aTrng0Edges[1], anX, anY);

    Standard_ASSERT_RAISE (aPosOutTrgn0 || aNegOutTrgn0,
      "Failed to detect if shared vertex is regular or not");

    if (aPosOutTrgn0 && aNegOutTrgn0)
    {
      return BRepExtrema_ElementFilter::NoCheck;
    }

    getProjectionAxes (aTrng1Normal, anX, anY);

    const Standard_Boolean aPosOutTrgn1 = !rayInsideAngle ( aCrossLine, aTrng1Edges[0], aTrng1Edges[1], anX, anY);
    const Standard_Boolean aNegOutTrgn1 = !rayInsideAngle (-aCrossLine, aTrng1Edges[0], aTrng1Edges[1], anX, anY);

    Standard_ASSERT_RAISE (aPosOutTrgn1 || aNegOutTrgn1,
      "Failed to detect if shared vertex is regular or not");

    if (aPosOutTrgn1 && aNegOutTrgn1)
    {
      return BRepExtrema_ElementFilter::NoCheck;
    }

    return (aPosOutTrgn0 || aPosOutTrgn1) && (aNegOutTrgn0 || aNegOutTrgn1) ?
     BRepExtrema_ElementFilter::NoCheck : BRepExtrema_ElementFilter::Overlap;
  }
}

//=======================================================================
//function : isRegularSharedEdge
//purpose  :
//=======================================================================
BRepExtrema_ElementFilter::FilterResult BRepExtrema_SelfIntersection::isRegularSharedEdge (const BVH_Vec3d& theTrng0Vtxs0,
                                                                                           const BVH_Vec3d& theTrng0Vtxs1,
                                                                                           const BVH_Vec3d& theTrng0Vtxs2,
                                                                                           const BVH_Vec3d& theTrng1Vtxs2)
{
  const BVH_Vec3d aSharedEdge = (theTrng0Vtxs1 - theTrng0Vtxs0).Normalized();

  const BVH_Vec3d aUniqueEdges[] = { (theTrng0Vtxs2 - theTrng0Vtxs0).Normalized(),
                                     (theTrng1Vtxs2 - theTrng0Vtxs0).Normalized() };

  const BVH_Vec3d aTrng0Normal = BVH_Vec3d::Cross (aSharedEdge, aUniqueEdges[0]);
  const BVH_Vec3d aTrng1Normal = BVH_Vec3d::Cross (aSharedEdge, aUniqueEdges[1]);

  BVH_Vec3d aCrossLine = BVH_Vec3d::Cross (aTrng0Normal,
                                           aTrng1Normal);

  if (aCrossLine.SquareModulus() > Precision::SquareConfusion()) // non-coplanar case
  {
    return BRepExtrema_ElementFilter::NoCheck;
  }

  Standard_Integer anX;
  Standard_Integer anY;

  getProjectionAxes (aTrng0Normal, anX, anY);

  return ccw (ZERO_VEC, aSharedEdge, aUniqueEdges[0], anX, anY) !=
         ccw (ZERO_VEC, aSharedEdge, aUniqueEdges[1], anX, anY) ? BRepExtrema_ElementFilter::NoCheck
                                                            : BRepExtrema_ElementFilter::Overlap;
}

//=======================================================================
//function : PreCheckElements
//purpose  :
//=======================================================================
BRepExtrema_ElementFilter::FilterResult BRepExtrema_SelfIntersection::PreCheckElements (const Standard_Integer theIndex1,
                                                                                        const Standard_Integer theIndex2)
{
  if (myElementSet->GetFaceID (theIndex1) == myElementSet->GetFaceID (theIndex2))
  {
    return BRepExtrema_ElementFilter::NoCheck; // triangles are from the same face
  }

  BVH_Vec3d aTrng0Vtxs[3];
  BVH_Vec3d aTrng1Vtxs[3];

  myElementSet->GetVertices (theIndex1,
                             aTrng0Vtxs[0],
                             aTrng0Vtxs[1],
                             aTrng0Vtxs[2]);

  myElementSet->GetVertices (theIndex2,
                             aTrng1Vtxs[0],
                             aTrng1Vtxs[1],
                             aTrng1Vtxs[2]);

  std::vector<std::pair<Standard_Integer, Standard_Integer> > aSharedVtxs;

  for (Standard_Integer aVertIdx1 = 0; aVertIdx1 < 3; ++aVertIdx1)
  {
    for (Standard_Integer aVertIdx2 = 0; aVertIdx2 < 3; ++aVertIdx2)
    {
      if ((aTrng0Vtxs[aVertIdx1] - aTrng1Vtxs[aVertIdx2]).SquareModulus() < Precision::SquareConfusion())
      {
        aSharedVtxs.push_back (std::pair<Standard_Integer, Standard_Integer> (aVertIdx1, aVertIdx2));

        break; // go to next vertex of the 1st triangle
      }
    }
  }

  if (aSharedVtxs.size() == 2) // check shared edge
  {
    return isRegularSharedEdge (aTrng0Vtxs[aSharedVtxs[0].first],
                                aTrng0Vtxs[aSharedVtxs[1].first],
                                aTrng0Vtxs[3 - aSharedVtxs[0]. first - aSharedVtxs[1]. first],
                                aTrng1Vtxs[3 - aSharedVtxs[0].second - aSharedVtxs[1].second]);
  }
  else if (aSharedVtxs.size() == 1) // check shared vertex
  {
    std::swap (*aTrng0Vtxs, aTrng0Vtxs[aSharedVtxs.front(). first]);
    std::swap (*aTrng1Vtxs, aTrng1Vtxs[aSharedVtxs.front().second]);

    return isRegularSharedVertex (aTrng0Vtxs[0],
                                  aTrng0Vtxs[1],
                                  aTrng0Vtxs[2],
                                  aTrng1Vtxs[1],
                                  aTrng1Vtxs[2]);
  }

  return BRepExtrema_ElementFilter::DoCheck;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void BRepExtrema_SelfIntersection::Perform()
{
  if (!myIsInit || myOverlapTool.IsDone())
  {
    return;
  }

  myOverlapTool.SetElementFilter (this);

  myOverlapTool.Perform (myTolerance);
}
