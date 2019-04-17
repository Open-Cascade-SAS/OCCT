// Created on: 2015-04-26
// Created by: Denis BOGOLEPOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Precision.hxx>

#include <BRepExtrema_OverlapTool.hxx>

//=======================================================================
//function : BRepExtrema_OverlapTool
//purpose  :
//=======================================================================
BRepExtrema_OverlapTool::BRepExtrema_OverlapTool()
: myFilter (NULL),
  myTolerance (0.0)
{
  myIsDone = Standard_False;
}

//=======================================================================
//function : BRepExtrema_OverlapTool
//purpose  :
//=======================================================================
BRepExtrema_OverlapTool::BRepExtrema_OverlapTool (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                  const Handle(BRepExtrema_TriangleSet)& theSet2)
: myFilter (NULL),
  myTolerance (0.0)
{
  LoadTriangleSets (theSet1, theSet2);
}

//=======================================================================
//function : LoadTriangleSets
//purpose  :
//=======================================================================
void BRepExtrema_OverlapTool::LoadTriangleSets (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                const Handle(BRepExtrema_TriangleSet)& theSet2)
{
  mySet1 = theSet1;
  mySet2 = theSet2;

  myIsDone = Standard_False;
}

#ifndef DBL_EPSILON
  #define DBL_EPSILON std::numeric_limits<Standard_Real>::epsilon()
#endif

namespace
{
  //! Bounding triangular prism for specified triangle.
  class BRepExtrema_BoundingPrism
  {
  public:

    //! Vertices of the prism.
    BVH_Vec3d Vertices[6];

    //! Edges of the prism.
    BVH_Vec3d Edges[3];

    //! Normal to prism caps.
    BVH_Vec3d Normal;

    //! Normals to prism edges.
    BVH_Vec3d EdgeNormals[3];

    //! Is prism initialized?
    Standard_Boolean IsInited;

  public:

    //! Creates uninitialized bounding prism.
    BRepExtrema_BoundingPrism() : IsInited (Standard_False)
    {
      //
    }

    //! Creates new bounding prism for the given triangle.
    BRepExtrema_BoundingPrism (const BVH_Vec3d&    theVertex0,
                               const BVH_Vec3d&    theVertex1,
                               const BVH_Vec3d&    theVertex2,
                               const Standard_Real theDeflect)
    {
      Init (theVertex0,
            theVertex1,
            theVertex2,
            theDeflect);
    }

    //! Calculates bounding prism for the given triangle.
    void Init (const BVH_Vec3d&    theVertex0,
               const BVH_Vec3d&    theVertex1,
               const BVH_Vec3d&    theVertex2,
               const Standard_Real theDeflect)
    {
      Edges[0] = theVertex1 - theVertex0;
      Edges[1] = theVertex2 - theVertex0;
      Edges[2] = theVertex2 - theVertex1;

      Normal = BVH_Vec3d::Cross (Edges[0], Edges[1]);

      EdgeNormals[0] = BVH_Vec3d::Cross (Edges[0], Normal);
      EdgeNormals[1] = BVH_Vec3d::Cross (Edges[1], Normal);
      EdgeNormals[2] = BVH_Vec3d::Cross (Edges[2], Normal);

      EdgeNormals[0] *= 1.0 / Max (EdgeNormals[0].Modulus(), Precision::Confusion());
      EdgeNormals[1] *= 1.0 / Max (EdgeNormals[1].Modulus(), Precision::Confusion());
      EdgeNormals[2] *= 1.0 / Max (EdgeNormals[2].Modulus(), Precision::Confusion());

      const BVH_Vec3d aDirect01 = EdgeNormals[0] - EdgeNormals[1];
      const BVH_Vec3d aDirect02 = EdgeNormals[0] + EdgeNormals[2];
      const BVH_Vec3d aDirect12 = EdgeNormals[2] - EdgeNormals[1];

      Vertices[0] = Vertices[3] = theVertex0 + aDirect01 * (theDeflect / aDirect01.Dot (EdgeNormals[0]));
      Vertices[1] = Vertices[4] = theVertex1 + aDirect02 * (theDeflect / aDirect02.Dot (EdgeNormals[2]));
      Vertices[2] = Vertices[5] = theVertex2 + aDirect12 * (theDeflect / aDirect12.Dot (EdgeNormals[2]));

      const BVH_Vec3d aNormOffset = Normal * (theDeflect / Max (Normal.Modulus(), Precision::Confusion()));

      for (Standard_Integer aVertIdx = 0; aVertIdx < 3; ++aVertIdx)
      {
        Vertices[aVertIdx + 0] += aNormOffset;
        Vertices[aVertIdx + 3] -= aNormOffset;
      }

      IsInited = Standard_True;
    }

    //! Checks if two prisms are separated along the given axis.
    Standard_Boolean Separated (const BRepExtrema_BoundingPrism& thePrism, const BVH_Vec3d& theAxis) const
    {
      Standard_Real aMin1 =  DBL_MAX;
      Standard_Real aMax1 = -DBL_MAX;

      Standard_Real aMin2 =  DBL_MAX;
      Standard_Real aMax2 = -DBL_MAX;

      for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
      {
        const Standard_Real aProj1 = Vertices[aVertIdx].Dot (theAxis);

        aMin1 = Min (aMin1, aProj1);
        aMax1 = Max (aMax1, aProj1);

        const Standard_Real aProj2 = thePrism.Vertices[aVertIdx].Dot (theAxis);

        aMin2 = Min (aMin2, aProj2);
        aMax2 = Max (aMax2, aProj2);

        if (aMin1 <= aMax2 && aMax1 >= aMin2)
        {
          return Standard_False;
        }
      }

      return aMin1 > aMax2 || aMax1 < aMin2;
    }
  };

  // =======================================================================
  // function : sign
  // purpose  :
  // =======================================================================
  Standard_Real sign (const BVH_Vec3d& theVertex0,
                      const BVH_Vec3d& theVertex1,
                      const BVH_Vec3d& theVertex2,
                      const Standard_Integer theX,
                      const Standard_Integer theY)
  {
    return (theVertex0[theX] - theVertex2[theX]) * (theVertex1[theY] - theVertex2[theY]) -
           (theVertex1[theX] - theVertex2[theX]) * (theVertex0[theY] - theVertex2[theY]);
  }

  // =======================================================================
  // function : pointInTriangle
  // purpose  :
  // =======================================================================
  Standard_Boolean pointInTriangle (const BVH_Vec3d& theTestPnt,
                                    const BVH_Vec3d& theTrgVtx0,
                                    const BVH_Vec3d& theTrgVtx1,
                                    const BVH_Vec3d& theTrgVtx2,
                                    const Standard_Integer theX,
                                    const Standard_Integer theY)
  {
    const Standard_Boolean aSign0 = sign (theTestPnt, theTrgVtx0, theTrgVtx1, theX, theY) <= 0.0;
    const Standard_Boolean aSign1 = sign (theTestPnt, theTrgVtx1, theTrgVtx2, theX, theY) <= 0.0;
    const Standard_Boolean aSign2 = sign (theTestPnt, theTrgVtx2, theTrgVtx0, theX, theY) <= 0.0;

    return (aSign0 == aSign1) && (aSign1 == aSign2);
  }

  // =======================================================================
  // function : segmentsIntersected
  // purpose  : Checks if two line segments are intersected
  // =======================================================================
  Standard_Boolean segmentsIntersected (const BVH_Vec2d& theOriginSeg0,
                                        const BVH_Vec2d& theOriginSeg1,
                                        const BVH_Vec2d& theDirectSeg0,
                                        const BVH_Vec2d& theDirectSeg1)
  {
    const Standard_Real aDet = -theDirectSeg1.x() * theDirectSeg0.y() +
                                theDirectSeg0.x() * theDirectSeg1.y();

    if (fabs (aDet) < DBL_EPSILON) // segments are parallel
    {
      const BVH_Vec2d aDirect = theDirectSeg0 * (1.0 / theDirectSeg0.Modulus());

      const Standard_Real aEdge0Time0 = theOriginSeg0.Dot (aDirect);
      const Standard_Real aEdge1Time0 = theOriginSeg1.Dot (aDirect);

      const Standard_Real aEdge0Time1 = aEdge0Time0 + theDirectSeg0.Dot (aDirect);
      const Standard_Real aEdge1Time1 = aEdge1Time0 + theDirectSeg1.Dot (aDirect);

      const Standard_Real aEdge0Min = Min (aEdge0Time0, aEdge0Time1);
      const Standard_Real aEdge1Min = Min (aEdge1Time0, aEdge1Time1);
      const Standard_Real aEdge0Max = Max (aEdge0Time0, aEdge0Time1);
      const Standard_Real aEdge1Max = Max (aEdge1Time0, aEdge1Time1);

      if (Max (aEdge0Min, aEdge1Min) > Min (aEdge0Max, aEdge1Max))
      {
        return Standard_False;
      }

      const BVH_Vec2d aNormal (-aDirect.y(), aDirect.x());

      return fabs (theOriginSeg0.Dot (aNormal) - theOriginSeg1.Dot (aNormal)) < DBL_EPSILON;
    }

    const BVH_Vec2d aDelta = theOriginSeg0 - theOriginSeg1;

    const Standard_Real aU = (-theDirectSeg0.y() * aDelta.x() + theDirectSeg0.x() * aDelta.y()) / aDet;
    const Standard_Real aV = ( theDirectSeg1.x() * aDelta.y() - theDirectSeg1.y() * aDelta.x()) / aDet;

    return aU >= 0.0 && aU <= 1.0 && aV >= 0.0 && aV <= 1.0;
  }

  // =======================================================================
  // function : trianglesIntersected
  // purpose  : Checks if two triangles are intersected
  //            ("A Fast Triangle-Triangle Intersection Test" by T. Moller)
  // =======================================================================
  Standard_Boolean trianglesIntersected (const BVH_Vec3d& theTrng0Vert0,
                                         const BVH_Vec3d& theTrng0Vert1,
                                         const BVH_Vec3d& theTrng0Vert2,
                                         const BVH_Vec3d& theTrng1Vert0,
                                         const BVH_Vec3d& theTrng1Vert1,
                                         const BVH_Vec3d& theTrng1Vert2)
  {
    const BVH_Vec3d aTrng1Normal = BVH_Vec3d::Cross (theTrng1Vert1 - theTrng1Vert0,
                                                     theTrng1Vert2 - theTrng1Vert0).Normalized();

    const Standard_Real aTrng1PlaneDist = aTrng1Normal.Dot (-theTrng1Vert0);

    Standard_Real aDistTrng0Vert0 = aTrng1Normal.Dot (theTrng0Vert0) + aTrng1PlaneDist;
    Standard_Real aDistTrng0Vert1 = aTrng1Normal.Dot (theTrng0Vert1) + aTrng1PlaneDist;
    Standard_Real aDistTrng0Vert2 = aTrng1Normal.Dot (theTrng0Vert2) + aTrng1PlaneDist;

    if ((aDistTrng0Vert0 < 0.0 && aDistTrng0Vert1 < 0.0 && aDistTrng0Vert2 < 0.0)
     || (aDistTrng0Vert0 > 0.0 && aDistTrng0Vert1 > 0.0 && aDistTrng0Vert2 > 0.0))
    {
      return Standard_False; // 1st triangle lies on one side of the 2nd triangle
    }

    if (fabs (aDistTrng0Vert0) > Precision::Confusion()
     || fabs (aDistTrng0Vert1) > Precision::Confusion()
     || fabs (aDistTrng0Vert2) > Precision::Confusion()) // general 3D case
    {
      const BVH_Vec3d aTrng0Normal = BVH_Vec3d::Cross (theTrng0Vert1 - theTrng0Vert0,
                                                       theTrng0Vert2 - theTrng0Vert0).Normalized();

      const Standard_Real aTrng0PlaneDist = aTrng0Normal.Dot (-theTrng0Vert0);

      Standard_Real aDistTrng1Vert0 = aTrng0Normal.Dot (theTrng1Vert0) + aTrng0PlaneDist;
      Standard_Real aDistTrng1Vert1 = aTrng0Normal.Dot (theTrng1Vert1) + aTrng0PlaneDist;
      Standard_Real aDistTrng1Vert2 = aTrng0Normal.Dot (theTrng1Vert2) + aTrng0PlaneDist;

      if ((aDistTrng1Vert0 < 0.0 && aDistTrng1Vert1 < 0.0 && aDistTrng1Vert2 < 0.0)
       || (aDistTrng1Vert0 > 0.0 && aDistTrng1Vert1 > 0.0 && aDistTrng1Vert2 > 0.0))
      {
        return Standard_False; // 2nd triangle lies on one side of the 1st triangle
      }

      const BVH_Vec3d aCrossLine = BVH_Vec3d::Cross (aTrng0Normal,
                                                     aTrng1Normal);

      Standard_Real aProjTrng0Vert0 = theTrng0Vert0.Dot (aCrossLine);
      Standard_Real aProjTrng0Vert1 = theTrng0Vert1.Dot (aCrossLine);
      Standard_Real aProjTrng0Vert2 = theTrng0Vert2.Dot (aCrossLine);

      if (aDistTrng0Vert0 * aDistTrng0Vert1 > 0.0)
      {
        std::swap (aDistTrng0Vert1, aDistTrng0Vert2);
        std::swap (aProjTrng0Vert1, aProjTrng0Vert2);
      }
      else if (aDistTrng0Vert1 * aDistTrng0Vert2 > 0.0)
      {
        std::swap (aDistTrng0Vert1, aDistTrng0Vert0);
        std::swap (aProjTrng0Vert1, aProjTrng0Vert0);
      }

      Standard_Real aTime1 = fabs (aDistTrng0Vert0) <= DBL_EPSILON ? aProjTrng0Vert0 :
        aProjTrng0Vert0 + (aProjTrng0Vert1 - aProjTrng0Vert0) * aDistTrng0Vert0 / (aDistTrng0Vert0 - aDistTrng0Vert1);
      Standard_Real aTime2 = fabs (aDistTrng0Vert2) <= DBL_EPSILON ? aProjTrng0Vert2 :
        aProjTrng0Vert2 + (aProjTrng0Vert1 - aProjTrng0Vert2) * aDistTrng0Vert2 / (aDistTrng0Vert2 - aDistTrng0Vert1);

      const Standard_Real aTimeMin1 = Min (aTime1, aTime2);
      const Standard_Real aTimeMax1 = Max (aTime1, aTime2);

      Standard_Real aProjTrng1Vert0 = theTrng1Vert0.Dot (aCrossLine);
      Standard_Real aProjTrng1Vert1 = theTrng1Vert1.Dot (aCrossLine);
      Standard_Real aProjTrng1Vert2 = theTrng1Vert2.Dot (aCrossLine);

      if (aDistTrng1Vert0 * aDistTrng1Vert1 > 0.0)
      {
        std::swap (aDistTrng1Vert1, aDistTrng1Vert2);
        std::swap (aProjTrng1Vert1, aProjTrng1Vert2);
      }
      else if (aDistTrng1Vert1 * aDistTrng1Vert2 > 0.0)
      {
        std::swap (aDistTrng1Vert1, aDistTrng1Vert0);
        std::swap (aProjTrng1Vert1, aProjTrng1Vert0);
      }

      aTime1 = fabs (aDistTrng1Vert0) <= DBL_EPSILON ? aProjTrng1Vert0 :
        aProjTrng1Vert0 + (aProjTrng1Vert1 - aProjTrng1Vert0) * aDistTrng1Vert0 / (aDistTrng1Vert0 - aDistTrng1Vert1);
      aTime2 = fabs (aDistTrng1Vert2) <= DBL_EPSILON ? aProjTrng1Vert2 :
        aProjTrng1Vert2 + (aProjTrng1Vert1 - aProjTrng1Vert2) * aDistTrng1Vert2 / (aDistTrng1Vert2 - aDistTrng1Vert1);

      const Standard_Real aTimeMin2 = Min (aTime1, aTime2);
      const Standard_Real aTimeMax2 = Max (aTime1, aTime2);

      aTime1 = Max (aTimeMin1, aTimeMin2);
      aTime2 = Min (aTimeMax1, aTimeMax2);

      return aTime1 <= aTime2; // intervals intersected --> triangles overlapped
    }
    else // triangles are co-planar
    {
      Standard_Integer anX;
      Standard_Integer anY;

      if (fabs (aTrng1Normal[0]) > fabs (aTrng1Normal[1]))
      {
        anX = fabs (aTrng1Normal[0]) > fabs (aTrng1Normal[2]) ? 1 : 0;
        anY = fabs (aTrng1Normal[0]) > fabs (aTrng1Normal[2]) ? 2 : 1;
      }
      else
      {
        anX = fabs (aTrng1Normal[1]) > fabs (aTrng1Normal[2]) ? 0 : 0;
        anY = fabs (aTrng1Normal[1]) > fabs (aTrng1Normal[2]) ? 2 : 1;
      }

      const BVH_Vec2d aOriginSeg0 [] = {BVH_Vec2d (theTrng0Vert0[anX], theTrng0Vert0[anY]),
                                        BVH_Vec2d (theTrng0Vert1[anX], theTrng0Vert1[anY]),
                                        BVH_Vec2d (theTrng0Vert2[anX], theTrng0Vert2[anY]) };

      const BVH_Vec2d aDirectSeg0 [] = {aOriginSeg0[1] - aOriginSeg0[0],
                                        aOriginSeg0[2] - aOriginSeg0[1],
                                        aOriginSeg0[0] - aOriginSeg0[2] };

      const BVH_Vec2d aOriginSeg1 [] = {BVH_Vec2d (theTrng1Vert0[anX], theTrng1Vert0[anY]),
                                        BVH_Vec2d (theTrng1Vert1[anX], theTrng1Vert1[anY]),
                                        BVH_Vec2d (theTrng1Vert2[anX], theTrng1Vert2[anY]) };

      const BVH_Vec2d aDirectSeg1 [] = {aOriginSeg1[1] - aOriginSeg1[0],
                                        aOriginSeg1[2] - aOriginSeg1[1],
                                        aOriginSeg1[0] - aOriginSeg1[2] };

      for (Standard_Integer aTrg0Edge = 0; aTrg0Edge < 3; ++aTrg0Edge)
      {
        for (Standard_Integer aTrg1Edge = 0; aTrg1Edge < 3; ++aTrg1Edge)
        {
          if (segmentsIntersected (aOriginSeg0[aTrg0Edge],
                                   aOriginSeg1[aTrg1Edge],
                                   aDirectSeg0[aTrg0Edge],
                                   aDirectSeg1[aTrg1Edge]))
          {
            return Standard_True; // edges intersected --> triangles overlapped
          }
        }
      }

      if (pointInTriangle (theTrng1Vert0,
                           theTrng0Vert0,
                           theTrng0Vert1,
                           theTrng0Vert2,
                           anX,
                           anY))
      {
        return Standard_True; // 1st triangle inside 2nd --> triangles overlapped
      }

      if (pointInTriangle (theTrng0Vert0,
                           theTrng1Vert0,
                           theTrng1Vert1,
                           theTrng1Vert2,
                           anX,
                           anY))
      {
        return Standard_True; // 2nd triangle inside 1st --> triangles overlapped
      }
    }

    return Standard_False;
  }

  // =======================================================================
  // function : prismsIntersected
  // purpose  : Checks if two triangular prisms are intersected
  //            (test uses SAT - Separating Axis Theorem)
  // =======================================================================
  Standard_Boolean prismsIntersected (const BRepExtrema_BoundingPrism& thePrism1,
                                      const BRepExtrema_BoundingPrism& thePrism2)
  {
    if (thePrism1.Separated (thePrism2, thePrism1.Normal))
    {
      return Standard_False;
    }

    if (thePrism1.Separated (thePrism2, thePrism2.Normal))
    {
      return Standard_False;
    }

    for (Standard_Integer anIdx = 0; anIdx < 3; ++anIdx)
    {
      if (thePrism1.Separated (thePrism2, thePrism1.EdgeNormals[anIdx]))
      {
        return Standard_False;
      }
    }

    for (Standard_Integer anIdx = 0; anIdx < 3; ++anIdx)
    {
      if (thePrism1.Separated (thePrism2, thePrism2.EdgeNormals[anIdx]))
      {
        return Standard_False;
      }
    }

    for (Standard_Integer anIdx1 = 0; anIdx1 < 4; ++anIdx1)
    {
      const BVH_Vec3d& aEdge1 = (anIdx1 == 3) ? thePrism1.Normal : thePrism1.Edges[anIdx1];

      for (Standard_Integer anIdx2 = 0; anIdx2 < 4; ++anIdx2)
      {
        const BVH_Vec3d& aEdge2 = (anIdx2 == 3) ? thePrism2.Normal : thePrism2.Edges[anIdx2];

        if (thePrism1.Separated (thePrism2, BVH_Vec3d::Cross (aEdge1, aEdge2)))
        {
          return Standard_False;
        }
      }
    }

    return Standard_True;
  }

  // =======================================================================
  // function : overlapBoxes
  // purpose  : Checks if two boxes (AABBs) are overlapped
  // =======================================================================
  inline Standard_Boolean overlapBoxes (const BVH_Vec3d&    theBoxMin1,
                                        const BVH_Vec3d&    theBoxMax1,
                                        const BVH_Vec3d&    theBoxMin2,
                                        const BVH_Vec3d&    theBoxMax2,
                                        const Standard_Real theTolerance)
  {
    // Check for overlap
    return !(theBoxMin1.x() > theBoxMax2.x() + theTolerance ||
             theBoxMax1.x() < theBoxMin2.x() - theTolerance ||
             theBoxMin1.y() > theBoxMax2.y() + theTolerance ||
             theBoxMax1.y() < theBoxMin2.y() - theTolerance ||
             theBoxMin1.z() > theBoxMax2.z() + theTolerance ||
             theBoxMax1.z() < theBoxMin2.z() - theTolerance);
  }

  //=======================================================================
  //function : getSetOfFaces
  //purpose  :
  //=======================================================================
  TColStd_PackedMapOfInteger& getSetOfFaces (
    BRepExtrema_MapOfIntegerPackedMapOfInteger& theFaces, const Standard_Integer theFaceIdx)
  {
    if (!theFaces.IsBound (theFaceIdx))
    {
      theFaces.Bind (theFaceIdx, TColStd_PackedMapOfInteger());
    }

    return theFaces.ChangeFind (theFaceIdx);
  }
}

//=======================================================================
//function : intersectTrianglesExact
//purpose  :
//=======================================================================
void BRepExtrema_OverlapTool::intersectTrianglesExact (const Standard_Integer theTrgIdx1,
                                                       const Standard_Integer theTrgIdx2)
{
  const Standard_Integer aFaceIdx1 = mySet1->GetFaceID (theTrgIdx1);

  BVH_Vec3d aTrg1Vert1;
  BVH_Vec3d aTrg1Vert2;
  BVH_Vec3d aTrg1Vert3;

  mySet1->GetVertices (theTrgIdx1,
                       aTrg1Vert1,
                       aTrg1Vert2,
                       aTrg1Vert3);

  const Standard_Boolean aIsInSet = myOverlapSubShapes1.IsBound (aFaceIdx1);

  const Standard_Integer aFaceIdx2 = mySet2->GetFaceID (theTrgIdx2);

  if (aIsInSet && myOverlapSubShapes1.Find (aFaceIdx1).Contains (aFaceIdx2))
  {
    return;
  }

  BRepExtrema_ElementFilter::FilterResult aResult = myFilter == NULL ?
    BRepExtrema_ElementFilter::DoCheck : myFilter->PreCheckElements (theTrgIdx1, theTrgIdx2);

  if (aResult == BRepExtrema_ElementFilter::Overlap)
  {
    getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
    getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
    if (mySet1 == mySet2)
    {
      myOverlapTriangles1.Add (theTrgIdx1);
      myOverlapTriangles1.Add (theTrgIdx2);
    }
    else
    {
      myOverlapTriangles1.Add (theTrgIdx1);
      myOverlapTriangles2.Add (theTrgIdx2);
    }
#endif
  }
  else if (aResult == BRepExtrema_ElementFilter::DoCheck)
  {
    BVH_Vec3d aTrg2Vert1;
    BVH_Vec3d aTrg2Vert2;
    BVH_Vec3d aTrg2Vert3;

    mySet2->GetVertices (theTrgIdx2, aTrg2Vert1, aTrg2Vert2, aTrg2Vert3);

    if (trianglesIntersected (aTrg1Vert1,
                              aTrg1Vert2,
                              aTrg1Vert3,
                              aTrg2Vert1,
                              aTrg2Vert2,
                              aTrg2Vert3))
    {
      getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
      getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
      if (mySet1 == mySet2)
      {
        myOverlapTriangles1.Add (theTrgIdx1);
        myOverlapTriangles1.Add (theTrgIdx2);
      }
      else
      {
        myOverlapTriangles1.Add (theTrgIdx1);
        myOverlapTriangles2.Add (theTrgIdx2);
      }
#endif
    }
  }
}

//=======================================================================
//function : intersectTrianglesToler
//purpose  :
//=======================================================================
void BRepExtrema_OverlapTool::intersectTrianglesToler (const Standard_Integer theTrgIdx1,
                                                       const Standard_Integer theTrgIdx2,
                                                       const Standard_Real theToler)
{
  const Standard_Integer aFaceIdx1 = mySet1->GetFaceID (theTrgIdx1);

  BVH_Vec3d aTrg1Vert1;
  BVH_Vec3d aTrg1Vert2;
  BVH_Vec3d aTrg1Vert3;

  mySet1->GetVertices (theTrgIdx1,
                       aTrg1Vert1,
                       aTrg1Vert2,
                       aTrg1Vert3);

  BRepExtrema_BoundingPrism aPrism1; // not initialized

  const Standard_Boolean aIsInSet = myOverlapSubShapes1.IsBound (aFaceIdx1);

  const Standard_Integer aFaceIdx2 = mySet2->GetFaceID (theTrgIdx2);

  if (aIsInSet && myOverlapSubShapes1.Find (aFaceIdx1).Contains (aFaceIdx2))
  {
    return;
  }

  BRepExtrema_ElementFilter::FilterResult aResult = myFilter == NULL ?
    BRepExtrema_ElementFilter::DoCheck : myFilter->PreCheckElements (theTrgIdx1, theTrgIdx2);

  if (aResult == BRepExtrema_ElementFilter::Overlap)
  {
    getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
    getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);

#ifdef OVERLAP_TOOL_OUTPUT_TRIANGLES
    if (mySet1 == mySet2)
    {
      myOverlapTriangles1.Add (theTrgIdx1);
      myOverlapTriangles1.Add (theTrgIdx2);
    }
    else
    {
      myOverlapTriangles1.Add (theTrgIdx1);
      myOverlapTriangles2.Add (theTrgIdx2);
    }
#endif
  }
  else if (aResult == BRepExtrema_ElementFilter::DoCheck)
  {
    if (!aPrism1.IsInited)
    {
      aPrism1.Init (aTrg1Vert1, aTrg1Vert2, aTrg1Vert3, theToler);
    }

    BVH_Vec3d aTrg2Vert1;
    BVH_Vec3d aTrg2Vert2;
    BVH_Vec3d aTrg2Vert3;

    mySet2->GetVertices (theTrgIdx2,
                         aTrg2Vert1,
                         aTrg2Vert2,
                         aTrg2Vert3);

    BRepExtrema_BoundingPrism aPrism2 (aTrg2Vert1,
                                       aTrg2Vert2,
                                       aTrg2Vert3,
                                       theToler);

    if (prismsIntersected (aPrism1, aPrism2))
    {
      getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
      getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : Performs search for overlapped faces
//=======================================================================
void BRepExtrema_OverlapTool::Perform (const Standard_Real theTolerance)
{
  myTolerance = theTolerance;

  myIsDone = (this->Select(mySet1->BVH(), mySet2->BVH()) > 0);
}

//=======================================================================
//function : Branch rejection
//purpose  : 
//=======================================================================
Standard_Boolean BRepExtrema_OverlapTool::RejectNode (const BVH_Vec3d& theCornerMin1,
                                                      const BVH_Vec3d& theCornerMax1,
                                                      const BVH_Vec3d& theCornerMin2,
                                                      const BVH_Vec3d& theCornerMax2,
                                                      Standard_Real&) const
{
  return !overlapBoxes (theCornerMin1, theCornerMax1, theCornerMin2, theCornerMax2, myTolerance);
}

//=======================================================================
//function : Leaf acceptance
//purpose  : 
//=======================================================================
Standard_Boolean BRepExtrema_OverlapTool::Accept (const Standard_Integer theTrgIdx1,
                                                  const Standard_Integer theTrgIdx2)
{
  if (myTolerance == 0.0)
  {
    intersectTrianglesExact (theTrgIdx1, theTrgIdx2);
  }
  else
  {
    intersectTrianglesToler (theTrgIdx1, theTrgIdx2, myTolerance);
  }
  return Standard_True;
}
