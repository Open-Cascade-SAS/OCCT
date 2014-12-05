// Created on: 2014-10-20
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

#include <BRepExtrema_ShapeProximity.hxx>

#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=======================================================================
//function : BRepExtrema_ShapeProximity
//purpose  : Creates empty proximity tool
//=======================================================================
BRepExtrema_ShapeProximity::BRepExtrema_ShapeProximity (const Standard_Real theTolerance)
: myTolerance     (theTolerance),
  myPrimitiveSet1 (new BRepExtrema_TriangleSet),
  myPrimitiveSet2 (new BRepExtrema_TriangleSet)
{
  // Should be initialized later
  myIsDone = myIsInitS1 = myIsInitS2 = Standard_False;
}

//=======================================================================
//function : BRepExtrema_ShapeProximity
//purpose  : Creates proximity tool for the given two shapes
//=======================================================================
BRepExtrema_ShapeProximity::BRepExtrema_ShapeProximity (const TopoDS_Shape& theShape1,
                                                        const TopoDS_Shape& theShape2,
                                                        const Standard_Real theTolerance)
: myTolerance     (theTolerance),
  myPrimitiveSet1 (new BRepExtrema_TriangleSet),
  myPrimitiveSet2 (new BRepExtrema_TriangleSet)
{
  LoadShape1 (theShape1);
  LoadShape2 (theShape2);
}

//=======================================================================
//function : LoadShape1
//purpose  : Loads 1st shape into proximity tool
//=======================================================================
Standard_Boolean BRepExtrema_ShapeProximity::LoadShape1 (const TopoDS_Shape& theShape1)
{
  myFaceList1.Clear();

  for (TopExp_Explorer anIter (theShape1, TopAbs_FACE); anIter.More(); anIter.Next())
  {
    myFaceList1.Append (static_cast<const TopoDS_Face&> (anIter.Current()));
  }

  myIsDone = Standard_False;

  return myIsInitS1 = myPrimitiveSet1->Init (myFaceList1);
}

//=======================================================================
//function : LoadShape2
//purpose  : Loads 2nd shape into proximity tool
//=======================================================================
Standard_Boolean BRepExtrema_ShapeProximity::LoadShape2 (const TopoDS_Shape& theShape2)
{
  myFaceList2.Clear();

  for (TopExp_Explorer anIter (theShape2, TopAbs_FACE); anIter.More(); anIter.Next())
  {
    myFaceList2.Append (static_cast<const TopoDS_Face&> (anIter.Current()));
  }

  myIsDone = Standard_False;

  return myIsInitS2 = myPrimitiveSet2->Init (myFaceList2);
}

namespace
{
  //! Tool class to describe stack item in traverse function.
  struct BRepExtrema_StackItem
  {
    Standard_Integer Node1;
    Standard_Integer Node2;

    BRepExtrema_StackItem (const Standard_Integer theNode1 = 0,
                           const Standard_Integer theNode2 = 0)
    : Node1 (theNode1),
      Node2 (theNode2)
    {
      //
    }
  };

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
  // function : Separated
  // purpose  : Checks if triangles can be separated along the given axis
  //            (projects vertices on this axis and performs interval test)
  // =======================================================================
  inline Standard_Boolean SeparateTriangles (const BVH_Vec3d& theTrg1Vert0,
                                             const BVH_Vec3d& theTrg1Vert1,
                                             const BVH_Vec3d& theTrg1Vert2,
                                             const BVH_Vec3d& theTrg2Vert0,
                                             const BVH_Vec3d& theTrg2Vert1,
                                             const BVH_Vec3d& theTrg2Vert2,
                                             const BVH_Vec3d& theSplitAxis)
  {
    const Standard_Real aA1 = theTrg1Vert0.Dot (theSplitAxis);
    const Standard_Real aB1 = theTrg1Vert1.Dot (theSplitAxis);
    const Standard_Real aC1 = theTrg1Vert2.Dot (theSplitAxis);

    const Standard_Real aA2 = theTrg2Vert0.Dot (theSplitAxis);
    const Standard_Real aB2 = theTrg2Vert1.Dot (theSplitAxis);
    const Standard_Real aC2 = theTrg2Vert2.Dot (theSplitAxis);

    const Standard_Real aMin1 = Min (aA1, Min (aB1, aC1));
    const Standard_Real aMax1 = Max (aA1, Max (aB1, aC1));

    if (aMax1 < Min (aA2, Min (aB2, aC2)))
    {
      return Standard_True;
    }

    return aMin1 > Max (aA2, Max (aB2, aC2));
  }

  // =======================================================================
  // function : TrianglesIntersected
  // purpose  : Checks if two triangles are intersected
  //            (test uses SAT - Separating Axis Theorem)
  // =======================================================================
  Standard_Boolean TrianglesIntersected (const BVH_Vec3d& theTrg1Vert0,
                                         const BVH_Vec3d& theTrg1Vert1,
                                         const BVH_Vec3d& theTrg1Vert2,
                                         const BVH_Vec3d& theTrg2Vert0,
                                         const BVH_Vec3d& theTrg2Vert1,
                                         const BVH_Vec3d& theTrg2Vert2)
  {
    const BVH_Vec3d aEdges1[3] = { theTrg1Vert1 - theTrg1Vert0,
                                   theTrg1Vert2 - theTrg1Vert0,
                                   theTrg1Vert2 - theTrg1Vert1 };

    const BVH_Vec3d aTrg1Normal = BVH_Vec3d::Cross (aEdges1[0], aEdges1[1]);

    if (SeparateTriangles (theTrg1Vert0,
                           theTrg1Vert1,
                           theTrg1Vert2,
                           theTrg2Vert0,
                           theTrg2Vert1,
                           theTrg2Vert2,
                           aTrg1Normal))
    {
      return Standard_False;
    }

    const BVH_Vec3d aEdges2[3] = { theTrg2Vert1 - theTrg2Vert0,
                                   theTrg2Vert2 - theTrg2Vert0,
                                   theTrg2Vert2 - theTrg2Vert1 };

    const BVH_Vec3d aTrg2Normal = BVH_Vec3d::Cross (aEdges2[0], aEdges2[1]);

    if (SeparateTriangles (theTrg1Vert0,
                           theTrg1Vert1,
                           theTrg1Vert2,
                           theTrg2Vert0,
                           theTrg2Vert1,
                           theTrg2Vert2,
                           aTrg2Normal))
    {
      return Standard_False;
    }

    for (Standard_Integer anIdx1 = 0; anIdx1 < 3; ++anIdx1)
    {
      for (Standard_Integer anIdx2 = 0; anIdx2 < 3; ++anIdx2)
      {
        const BVH_Vec3d aSplitAxis = BVH_Vec3d::Cross (aEdges1[anIdx1], aEdges2[anIdx2]);

        if (SeparateTriangles (theTrg1Vert0,
                               theTrg1Vert1,
                               theTrg1Vert2,
                               theTrg2Vert0,
                               theTrg2Vert1,
                               theTrg2Vert2,
                               aSplitAxis))
        {
          return Standard_False;
        }
      }
    }

    return Standard_True;
  }

  // =======================================================================
  // function : PrismsIntersected
  // purpose  : Checks if two triangular prisms are intersected
  //            (test uses SAT - Separating Axis Theorem)
  // =======================================================================
  Standard_Boolean PrismsIntersected (const BRepExtrema_BoundingPrism& thePrism1,
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
  // function : OverlapBoxes
  // purpose  : Checks if two boxes (AABBs) are overlapped
  // =======================================================================
  inline Standard_Boolean OverlapBoxes (const BVH_Vec3d&    theBoxMin1,
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
  TColStd_PackedMapOfInteger& getSetOfFaces (BRepExtrema_OverlappedSubShapes& theShapes,
                                             const Standard_Integer           theFaceIdx)
  {
    if (!theShapes.IsBound (theFaceIdx))
    {
      theShapes.Bind (theFaceIdx, TColStd_PackedMapOfInteger());
    }

    return theShapes.ChangeFind (theFaceIdx);
  }
}

//=======================================================================
//function : IntersectLeavesExact
//purpose  : Narrow-phase of overlap test (exact intersection)
//=======================================================================
void BRepExtrema_ShapeProximity::IntersectLeavesExact (const BVH_Vec4i& theLeaf1,
                                                       const BVH_Vec4i& theLeaf2)
{
  for (Standard_Integer aTrgIdx1 = theLeaf1.y(); aTrgIdx1 <= theLeaf1.z(); ++aTrgIdx1)
  {
    const Standard_Integer aFaceIdx1 = myPrimitiveSet1->GetFaceID (aTrgIdx1);

    BVH_Vec3d aTrg1Vert1;
    BVH_Vec3d aTrg1Vert2;
    BVH_Vec3d aTrg1Vert3;

    myPrimitiveSet1->GetVertices (aTrgIdx1,
                                  aTrg1Vert1,
                                  aTrg1Vert2,
                                  aTrg1Vert3);

    const Standard_Boolean aIsInSet = myOverlapSubShapes1.IsBound (aFaceIdx1);

    for (Standard_Integer aTrgIdx2 = theLeaf2.y(); aTrgIdx2 <= theLeaf2.z(); ++aTrgIdx2)
    {
      const Standard_Integer aFaceIdx2 = myPrimitiveSet2->GetFaceID (aTrgIdx2);

      if (!aIsInSet || !myOverlapSubShapes1.Find (aFaceIdx1).Contains (aFaceIdx2))
      {
        BVH_Vec3d aTrg2Vert1;
        BVH_Vec3d aTrg2Vert2;
        BVH_Vec3d aTrg2Vert3;

        myPrimitiveSet2->GetVertices (aTrgIdx2, aTrg2Vert1, aTrg2Vert2, aTrg2Vert3);

        if (TrianglesIntersected (aTrg1Vert1,
                                  aTrg1Vert2,
                                  aTrg1Vert3,
                                  aTrg2Vert1,
                                  aTrg2Vert2,
                                  aTrg2Vert3))
        {
          getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
          getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);
        }
      }
    }
  }
}

//=======================================================================
//function : IntersectLeavesToler
//purpose  : Narrow-phase of overlap test (with non-zero tolerance)
//=======================================================================
void BRepExtrema_ShapeProximity::IntersectLeavesToler (const BVH_Vec4i& theLeaf1,
                                                       const BVH_Vec4i& theLeaf2)
{
  for (Standard_Integer aTrgIdx1 = theLeaf1.y(); aTrgIdx1 <= theLeaf1.z(); ++aTrgIdx1)
  {
    const Standard_Integer aFaceIdx1 = myPrimitiveSet1->GetFaceID (aTrgIdx1);

    BVH_Vec3d aTrg1Vert1;
    BVH_Vec3d aTrg1Vert2;
    BVH_Vec3d aTrg1Vert3;

    myPrimitiveSet1->GetVertices (aTrgIdx1,
                                  aTrg1Vert1,
                                  aTrg1Vert2,
                                  aTrg1Vert3);

    BRepExtrema_BoundingPrism aPrism1; // not initialized

    const Standard_Boolean aIsInSet = myOverlapSubShapes1.IsBound (aFaceIdx1);

    for (Standard_Integer aTrgIdx2 = theLeaf2.y(); aTrgIdx2 <= theLeaf2.z(); ++aTrgIdx2)
    {
      const Standard_Integer aFaceIdx2 = myPrimitiveSet2->GetFaceID (aTrgIdx2);

      if (!aIsInSet || !myOverlapSubShapes1.Find (aFaceIdx1).Contains (aFaceIdx2))
      {
        if (!aPrism1.IsInited)
        {
          aPrism1.Init (aTrg1Vert1, aTrg1Vert2, aTrg1Vert3, myTolerance);
        }

        BVH_Vec3d aTrg2Vert1;
        BVH_Vec3d aTrg2Vert2;
        BVH_Vec3d aTrg2Vert3;

        myPrimitiveSet2->GetVertices (aTrgIdx2,
                                      aTrg2Vert1,
                                      aTrg2Vert2,
                                      aTrg2Vert3);

        BRepExtrema_BoundingPrism aPrism2 (aTrg2Vert1,
                                           aTrg2Vert2,
                                           aTrg2Vert3,
                                           myTolerance);

        if (PrismsIntersected (aPrism1, aPrism2))
        {
          getSetOfFaces (myOverlapSubShapes1, aFaceIdx1).Add (aFaceIdx2);
          getSetOfFaces (myOverlapSubShapes2, aFaceIdx2).Add (aFaceIdx1);
        }
      }
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : Performs search for overlapped faces
//=======================================================================
void BRepExtrema_ShapeProximity::Perform()
{
  if (myIsDone || !myIsInitS1 || !myIsInitS2)
  {
    return;
  }

  BRepExtrema_StackItem aStack[96];

  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aBVH1 = myPrimitiveSet1->BVH();
  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aBVH2 = myPrimitiveSet2->BVH();

  if (aBVH1.IsNull() || aBVH2.IsNull())
  {
    return;
  }

  BRepExtrema_StackItem aNodes; // current pair of nodes

  Standard_Integer aHead = -1; // stack head position

  for (;;)
  {
    BVH_Vec4i aNodeData1 = aBVH1->NodeInfoBuffer()[aNodes.Node1];
    BVH_Vec4i aNodeData2 = aBVH2->NodeInfoBuffer()[aNodes.Node2];

    if (aNodeData1.x() != 0 && aNodeData2.x() != 0) // leaves
    {
      if (myTolerance == 0.0)
      {
        IntersectLeavesExact (aNodeData1, aNodeData2);
      }
      else
      {
        IntersectLeavesToler (aNodeData1, aNodeData2);
      }

      if (aHead < 0)
        break;

      aNodes = aStack[aHead--];
    }
    else
    {
      BRepExtrema_StackItem aPairsToProcess[4];

      Standard_Integer aNbPairs = 0;

      if (aNodeData1.x() == 0) // inner node
      {
        const BVH_Vec3d& aMinPntLft1 = aBVH1->MinPoint (aNodeData1.y());
        const BVH_Vec3d& aMaxPntLft1 = aBVH1->MaxPoint (aNodeData1.y());
        const BVH_Vec3d& aMinPntRgh1 = aBVH1->MinPoint (aNodeData1.z());
        const BVH_Vec3d& aMaxPntRgh1 = aBVH1->MaxPoint (aNodeData1.z());

        if (aNodeData2.x() == 0) // inner node
        {
          const BVH_Vec3d& aMinPntLft2 = aBVH2->MinPoint (aNodeData2.y());
          const BVH_Vec3d& aMaxPntLft2 = aBVH2->MaxPoint (aNodeData2.y());
          const BVH_Vec3d& aMinPntRgh2 = aBVH2->MinPoint (aNodeData2.z());
          const BVH_Vec3d& aMaxPntRgh2 = aBVH2->MaxPoint (aNodeData2.z());

          if (OverlapBoxes (aMinPntLft1, aMaxPntLft1, aMinPntLft2, aMaxPntLft2, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.y(), aNodeData2.y());
          }
          if (OverlapBoxes (aMinPntLft1, aMaxPntLft1, aMinPntRgh2, aMaxPntRgh2, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.y(), aNodeData2.z());
          }
          if (OverlapBoxes (aMinPntRgh1, aMaxPntRgh1, aMinPntLft2, aMaxPntLft2, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.z(), aNodeData2.y());
          }
          if (OverlapBoxes (aMinPntRgh1, aMaxPntRgh1, aMinPntRgh2, aMaxPntRgh2, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.z(), aNodeData2.z());
          }
        }
        else
        {
          const BVH_Vec3d& aMinPntLeaf = aBVH2->MinPoint (aNodes.Node2);
          const BVH_Vec3d& aMaxPntLeaf = aBVH2->MaxPoint (aNodes.Node2);

          if (OverlapBoxes (aMinPntLft1, aMaxPntLft1, aMinPntLeaf, aMaxPntLeaf, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.y(), aNodes.Node2);
          }
          if (OverlapBoxes (aMinPntRgh1, aMaxPntRgh1, aMinPntLeaf, aMaxPntLeaf, myTolerance))
          {
            aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodeData1.z(), aNodes.Node2);
          }
        }
      }
      else
      {
        const BVH_Vec3d& aMinPntLeaf = aBVH1->MinPoint (aNodes.Node1);
        const BVH_Vec3d& aMaxPntLeaf = aBVH1->MaxPoint (aNodes.Node1);

        const BVH_Vec3d& aMinPntLft2 = aBVH2->MinPoint (aNodeData2.y());
        const BVH_Vec3d& aMaxPntLft2 = aBVH2->MaxPoint (aNodeData2.y());
        const BVH_Vec3d& aMinPntRgh2 = aBVH2->MinPoint (aNodeData2.z());
        const BVH_Vec3d& aMaxPntRgh2 = aBVH2->MaxPoint (aNodeData2.z());

        if (OverlapBoxes (aMinPntLft2, aMaxPntLft2, aMinPntLeaf, aMaxPntLeaf, myTolerance))
        {
          aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodes.Node1, aNodeData2.y());
        }
        if (OverlapBoxes (aMinPntRgh2, aMaxPntRgh2, aMinPntLeaf, aMaxPntLeaf, myTolerance))
        {
          aPairsToProcess[aNbPairs++] = BRepExtrema_StackItem (aNodes.Node1, aNodeData2.z());
        }
      }

      if (aNbPairs > 0)
      {
        aNodes = aPairsToProcess[0];

        for (Standard_Integer anIdx = 1; anIdx < aNbPairs; ++anIdx)
        {
          aStack[++aHead] = aPairsToProcess[anIdx];
        }
      }
      else
      {
        if (aHead < 0)
          break;

        aNodes = aStack[aHead--];
      }
    }
  }

  myIsDone = Standard_True;
}
