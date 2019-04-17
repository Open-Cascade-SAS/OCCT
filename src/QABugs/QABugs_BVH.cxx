// Created by: Eugeny MALTCHIKOV
// Created on: 2019-04-17
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_Tools.hxx>

#include <BRep_Builder.hxx>

#include <BRepBndLib.hxx>

#include <BVH_Box.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_DistanceField.hxx>
#include <BVH_Geometry.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_PairDistance.hxx>
#include <BVH_Traverse.hxx>
#include <BVH_Triangulation.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>

#include <Precision.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

//=======================================================================
//function : ShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class ShapeSelector :
  public BVH_Traverse <Standard_Real, 3, BVH_BoxSet <Standard_Real, 3, TopoDS_Shape>, Standard_Boolean>
{
public:
  //! Constructor
  ShapeSelector() {}

  //! Sets the Box for selection
  void SetBox (const Bnd_Box& theBox)
  {
    myBox = Bnd_Tools::Bnd2BVH (theBox);
  }

  //! Returns the selected shapes
  const NCollection_List<TopoDS_Shape>& Shapes () const { return myShapes; }

public:

  //! Defines the rules for node rejection by bounding box
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCornerMin,
                                       const BVH_Vec3d& theCornerMax,
                                       Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    Standard_Boolean hasOverlap;
    theIsInside = myBox.Contains (theCornerMin, theCornerMax, hasOverlap);
    return !hasOverlap;
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean AcceptMetric (const Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    return theIsInside;
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex,
                                   const Standard_Boolean& theIsInside) Standard_OVERRIDE
  {
    if (theIsInside || !myBox.IsOut (myBVHSet->Box (theIndex)))
    {
      myShapes.Append (myBVHSet->Element (theIndex));
      return Standard_True;
    }
    return Standard_False;
  }

protected:

  BVH_Box <Standard_Real, 3> myBox;         //!< Selection box
  NCollection_List <TopoDS_Shape> myShapes; //!< Selected shapes
};

//=======================================================================
//function : ShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class ShapeSelectorVoid :
  public BVH_Traverse <Standard_Real, 3, void, Standard_Boolean>
{
public:
  //! Constructor
  ShapeSelectorVoid() {}

  //! Sets the Box for selection
  void SetBox (const Bnd_Box& theBox)
  {
    myBox = Bnd_Tools::Bnd2BVH (theBox);
  }

  //! Returns the selected shapes
  const NCollection_List<TopoDS_Shape>& Shapes () const { return myShapes; }

public:

  //! Sets the Box Set
  void SetShapeBoxSet (const opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>& theBoxSet)
  {
    myBoxSet = theBoxSet;
  }

public:

  //! Defines the rules for node rejection by bounding box
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCornerMin,
                                       const BVH_Vec3d& theCornerMax,
                                       Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    Standard_Boolean hasOverlap;
    theIsInside = myBox.Contains (theCornerMin, theCornerMax, hasOverlap);
    return !hasOverlap;
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean AcceptMetric (const Standard_Boolean& theIsInside) const Standard_OVERRIDE
  {
    return theIsInside;
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex,
                                   const Standard_Boolean& theIsInside) Standard_OVERRIDE
  {
    if (theIsInside || !myBox.IsOut (myBoxSet->Box (theIndex)))
    {
      myShapes.Append (myBoxSet->Element (theIndex));
      return Standard_True;
    }
    return Standard_False;
  }

protected:

  opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>> myBoxSet; //!< ShapeBoxSet
  BVH_Box <Standard_Real, 3> myBox;         //!< Selection box
  NCollection_List <TopoDS_Shape> myShapes; //!< Selected shapes
};

//=======================================================================
//function : QABVH_ShapeSelect
//purpose : Test the work of BVH on the simple example of shapes selection
//=======================================================================
static Standard_Integer QABVH_ShapeSelect (Draw_Interpretor& theDI,
                                           Standard_Integer theArgc,
                                           const char** theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  // Get the shape to add its sub-shapes into BVH
  TopoDS_Shape aShape = DBRep::Get (theArgv [2]);
  if (aShape.IsNull())
  {
    std::cout << theArgv[2] << " does not exist" << std::endl;
    return 1;
  }

  // Get the shape to get the Box for selection
  TopoDS_Shape aBShape = DBRep::Get (theArgv [3]);
  if (aBShape.IsNull())
  {
    std::cout << theArgv[3] << " does not exist" << std::endl;
    return 1;
  }

  // Which selector to use
  Standard_Boolean useVoidSelector = Standard_False;
  if (theArgc > 4)
    useVoidSelector = !strcmp (theArgv[4], "-void");

  // Define BVH Builder
  opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder <Standard_Real, 3>();

  // Create the ShapeSet
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> > aShapeBoxSet =
    new BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> (aLBuilder);

  // Add elements into BVH

  // Map the shape
  TopTools_IndexedMapOfShape aMapShapes;
  TopExp::MapShapes (aShape, TopAbs_VERTEX, aMapShapes);
  TopExp::MapShapes (aShape, TopAbs_EDGE,   aMapShapes);
  TopExp::MapShapes (aShape, TopAbs_FACE,   aMapShapes);

  for (Standard_Integer iS = 1; iS <= aMapShapes.Extent(); ++iS)
  {
    const TopoDS_Shape& aS = aMapShapes(iS);

    Bnd_Box aSBox;
    BRepBndLib::Add (aS, aSBox);

    aShapeBoxSet->Add (aS, Bnd_Tools::Bnd2BVH (aSBox));
  }

  // Build BVH
  aShapeBoxSet->Build();

  TopTools_ListOfShape aSelectedShapes;

  // Prepare a Box for selection
  Bnd_Box aSelectionBox;
  BRepBndLib::Add (aBShape, aSelectionBox);

  // Perform selection
  if (!useVoidSelector)
  {
    ShapeSelector aSelector;
    aSelector.SetBox (aSelectionBox);
    aSelector.SetBVHSet (aShapeBoxSet.get());
    aSelector.Select();
    aSelectedShapes = aSelector.Shapes();
  }
  else
  {
    ShapeSelectorVoid aSelector;
    aSelector.SetBox (aSelectionBox);
    aSelector.SetShapeBoxSet (aShapeBoxSet);
    aSelector.Select (aShapeBoxSet->BVH());
    aSelectedShapes = aSelector.Shapes();
  }

  // Draw the selected shapes
  TopoDS_Compound aResult;
  BRep_Builder().MakeCompound (aResult);

  for (TopTools_ListOfShape::Iterator it (aSelectedShapes); it.More(); it.Next())
    BRep_Builder().Add (aResult, it.Value());

  DBRep::Set (theArgv[1], aResult);
  return 0;
}

//=======================================================================
//function : PairShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class PairShapesSelector :
  public BVH_PairTraverse <Standard_Real, 3, BVH_BoxSet <Standard_Real, 3, TopoDS_Shape>>
{
public:
  //! Constructor
  PairShapesSelector() {}

  //! Returns the selected pairs of shapes
  const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& Pairs () const { return myPairs; }

public:

  //! Defines the rules for node rejection
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCornerMin1,
                                       const BVH_Vec3d& theCornerMax1,
                                       const BVH_Vec3d& theCornerMin2,
                                       const BVH_Vec3d& theCornerMax2,
                                       Standard_Real&) const Standard_OVERRIDE
  {
    return BVH_Box <Standard_Real, 3>(theCornerMin1, theCornerMax1).IsOut (
           BVH_Box <Standard_Real, 3>(theCornerMin2, theCornerMax2));
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex1,
                                   const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    BVH_Box<Standard_Real, 3> aBox1 = myBVHSet1->Box (theIndex1);
    BVH_Box<Standard_Real, 3> aBox2 = myBVHSet2->Box (theIndex2);
    if (!aBox1.IsOut (aBox2))
    {
      myPairs.Append (std::pair <TopoDS_Shape, TopoDS_Shape> (myBVHSet1->Element (theIndex1),
                                                              myBVHSet2->Element (theIndex2)));
      return Standard_True;
    }
    return Standard_False;
  }

protected:

  NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> > myPairs; //!< Selected pairs
};

//=======================================================================
//function : PairShapeSelector
//purpose : Implement the simplest shape's selector
//=======================================================================
class PairShapesSelectorVoid :
  public BVH_PairTraverse <Standard_Real, 3>
{
public:
  //! Constructor
  PairShapesSelectorVoid() {}

  //! Returns the selected pairs of shapes
  const NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >& Pairs () const { return myPairs; }

public:

  //! Sets the sets to access the elements
  void SetShapeBoxSets (const opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>& theSBSet1,
                        const opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>& theSBSet2)
  {
    mySBSet1 = theSBSet1;
    mySBSet2 = theSBSet2;
  }

public:

  //! Defines the rules for node rejection
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCornerMin1,
                                       const BVH_Vec3d& theCornerMax1,
                                       const BVH_Vec3d& theCornerMin2,
                                       const BVH_Vec3d& theCornerMax2,
                                       Standard_Real&) const Standard_OVERRIDE
  {
    return BVH_Box <Standard_Real, 3>(theCornerMin1, theCornerMax1).IsOut (
           BVH_Box <Standard_Real, 3>(theCornerMin2, theCornerMax2));
  }

  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex1,
                                   const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    BVH_Box<Standard_Real, 3> aBox1 = mySBSet1->Box (theIndex1);
    BVH_Box<Standard_Real, 3> aBox2 = mySBSet2->Box (theIndex2);
    if (!aBox1.IsOut (aBox2))
    {
      myPairs.Append (std::pair <TopoDS_Shape, TopoDS_Shape> (mySBSet1->Element (theIndex1),
                                                              mySBSet2->Element (theIndex2)));
      return Standard_True;
    }
    return Standard_False;
  }

protected:
  opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>> mySBSet1; //!< First ShapeBoxSet
  opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>> mySBSet2; //!< Second ShapeBoxSet
  NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> > myPairs; //!< Selected pairs
};

//=======================================================================
//function : QABVH_PairSelect
//purpose : Test the work of BVH on the simple example of pairs of shapes selection
//=======================================================================
static Standard_Integer QABVH_PairSelect (Draw_Interpretor& theDI,
                                          Standard_Integer theArgc,
                                          const char** theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  TopoDS_Shape aShape[2];
  // Get the first shape
  aShape[0] = DBRep::Get (theArgv [2]);
  if (aShape[0].IsNull())
  {
    std::cout << theArgv[2] << " does not exist" << std::endl;
    return 1;
  }

  // Get the second shape
  aShape[1] = DBRep::Get (theArgv [3]);
  if (aShape[1].IsNull())
  {
    std::cout << theArgv[3] << " does not exist" << std::endl;
    return 1;
  }

  // Which selector to use
  Standard_Boolean useVoidSelector = Standard_False;
  if (theArgc > 4)
    useVoidSelector = !strcmp (theArgv[4], "-void");

  // Define BVH Builder
  opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder <Standard_Real, 3>();

  // Create the ShapeSet
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> > aShapeBoxSet[2];

  for (Standard_Integer i = 0; i < 2; ++i)
  {
    aShapeBoxSet[i] = new BVH_BoxSet <Standard_Real, 3, TopoDS_Shape> (aLBuilder);
    // Add elements into set
    TopTools_IndexedMapOfShape aMapShapes;
    TopExp::MapShapes (aShape[i], TopAbs_VERTEX, aMapShapes);
    TopExp::MapShapes (aShape[i], TopAbs_EDGE,   aMapShapes);
    TopExp::MapShapes (aShape[i], TopAbs_FACE,   aMapShapes);

    for (Standard_Integer iS = 1; iS <= aMapShapes.Extent(); ++iS)
    {
      const TopoDS_Shape& aS = aMapShapes(iS);
  
      Bnd_Box aSBox;
      BRepBndLib::Add (aS, aSBox);
  
      aShapeBoxSet[i]->Add (aS, Bnd_Tools::Bnd2BVH (aSBox));
    }
    // Build BVH
    aShapeBoxSet[i]->Build();
  }

  NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> > aPairs;
  if (!useVoidSelector)
  {
    // Initialize selector
    PairShapesSelector aSelector;
    // Select the elements
    aSelector.SetBVHSets (aShapeBoxSet[0].get(), aShapeBoxSet[1].get());
    aSelector.Select();
    aPairs = aSelector.Pairs();
  }
  else
  {
    // Initialize selector
    PairShapesSelectorVoid aSelector;
    // Select the elements
    aSelector.SetShapeBoxSets (aShapeBoxSet[0], aShapeBoxSet[1]);
    aSelector.Select (aShapeBoxSet[0]->BVH(), aShapeBoxSet[1]->BVH());
    aPairs = aSelector.Pairs();
  }

  // Draw the selected shapes
  TopoDS_Compound aResult;
  BRep_Builder().MakeCompound (aResult);

  for (NCollection_List <std::pair <TopoDS_Shape, TopoDS_Shape> >::Iterator it (aPairs); it.More(); it.Next())
  {
    TopoDS_Compound aPair;
    BRep_Builder().MakeCompound (aPair);

    BRep_Builder().Add (aPair, it.Value().first);
    BRep_Builder().Add (aPair, it.Value().second);

    BRep_Builder().Add (aResult, aPair);
  }

  DBRep::Set (theArgv[1], aResult);
  return 0;
}


//=======================================================================
//function : Triangle
//purpose : Auxiliary structure to keep the nodes of the triangle
//=======================================================================
struct Triangle
{
  Triangle() {}
  Triangle(const BVH_Vec3d& theP1, 
           const BVH_Vec3d& theP2,
           const BVH_Vec3d& theP3)
    : _Node1 (theP1), _Node2 (theP2), _Node3 (theP3)
  {}

  BVH_Vec3d _Node1;
  BVH_Vec3d _Node2;
  BVH_Vec3d _Node3;
};

//=======================================================================
//function : TriangleTriangleSqDistance
//purpose : Computes the Triangle-Triangle square distance
//=======================================================================
static Standard_Real TriangleTriangleSqDistance (const BVH_Vec3d& theNode11,
                                                 const BVH_Vec3d& theNode12,
                                                 const BVH_Vec3d& theNode13,
                                                 const BVH_Vec3d& theNode21,
                                                 const BVH_Vec3d& theNode22,
                                                 const BVH_Vec3d& theNode23)
{
  Standard_Real aDist, aMinDist = RealLast();

  BVH_Vec3d aNodes[2][3] = { { theNode11, theNode12, theNode13 },
                             { theNode21, theNode22, theNode23 } };

  const Standard_Integer aNbSeg = 100; // number of segments on edge
  for (Standard_Integer iT = 0; iT < 2; ++iT)
  {
    // projecting points of one triangle on the opposite triangle
    for (Standard_Integer iP = 0; iP < 3; ++iP)
    {
      aDist = 
        BVH_Tools<Standard_Real, 3>::PointTriangleSquareDistance (aNodes[iT][iP],
                                                                  aNodes[(iT + 1) % 2][0],
                                                                  aNodes[(iT + 1) % 2][1],
                                                                  aNodes[(iT + 1) % 2][2]);
      if (aDist < aMinDist)
      {
        aMinDist = aDist;
        if (aMinDist == 0)
          return aMinDist;
      }
    }

    // projecting edges on the opposite triangle
    std::pair<BVH_Vec3d, BVH_Vec3d> anEdges[3] =
      { std::pair<BVH_Vec3d, BVH_Vec3d> (aNodes[iT][0], aNodes[iT][1]),
        std::pair<BVH_Vec3d, BVH_Vec3d> (aNodes[iT][1], aNodes[iT][2]),
        std::pair<BVH_Vec3d, BVH_Vec3d> (aNodes[iT][2], aNodes[iT][0]) };

    for (Standard_Integer iE = 0; iE < 3; ++iE)
    {
      const BVH_Vec3d& aPFirst = anEdges[iE].first, aPLast = anEdges[iE].second;
      BVH_Vec3d anEdge = (aPLast - aPFirst);
      Standard_Real aLength = anEdge.Modulus();
      if (aLength < Precision::Confusion())
        continue;
      anEdge /= aLength;

      Standard_Real aDelta = aLength / aNbSeg;
  
      for (Standard_Integer iP = 1; iP < aNbSeg; ++iP)
      {
        BVH_Vec3d aPE = aPFirst + anEdge.Multiplied (iP * aDelta);
        aDist = 
          BVH_Tools<Standard_Real, 3>::PointTriangleSquareDistance (aPE, 
                                                                    aNodes[(iT + 1) % 2][0],
                                                                    aNodes[(iT + 1) % 2][1],
                                                                    aNodes[(iT + 1) % 2][2]);
        if (aDist < aMinDist)
        {
          aMinDist = aDist;
          if (aMinDist == 0)
            return aMinDist;
        }
      }
    }
  }
  return aMinDist;
}

//=======================================================================
//function : MeshMeshDistance
//purpose : Class to compute the distance between two meshes
//=======================================================================
class MeshMeshDistance : public BVH_PairDistance<Standard_Real, 3, BVH_BoxSet<Standard_Real, 3, Triangle>>
{
public:
  //! Constructor
  MeshMeshDistance() {}

public:
  //! Defines the rules for leaf acceptance
  virtual Standard_Boolean Accept (const Standard_Integer theIndex1,
                                   const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    const Triangle& aTri1 = myBVHSet1->Element (theIndex1);
    const Triangle& aTri2 = myBVHSet2->Element (theIndex2);

    Standard_Real aDistance = TriangleTriangleSqDistance (aTri1._Node1, aTri1._Node2, aTri1._Node3,
                                                          aTri2._Node1, aTri2._Node2, aTri2._Node3);
    if (aDistance < myDistance)
    {
      myDistance = aDistance;
      return Standard_True;
    }
    return Standard_False;
  }
};

//=======================================================================
//function : QABVH_PairDistance
//purpose : Computes the distance between two meshes of the given shapes
//=======================================================================
static Standard_Integer QABVH_PairDistance (Draw_Interpretor& theDI,
                                            Standard_Integer theArgc,
                                            const char** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  TopoDS_Shape aShape[2];
  // Get the first shape
  aShape[0] = DBRep::Get (theArgv [1]);
  if (aShape[0].IsNull())
  {
    std::cout << theArgv[1] << " does not exist" << std::endl;
    return 1;
  }

  // Get the second shape
  aShape[1] = DBRep::Get (theArgv [2]);
  if (aShape[1].IsNull())
  {
    std::cout << theArgv[2] << " does not exist" << std::endl;
    return 1;
  }

  // Define BVH Builder
  opencascade::handle <BVH_LinearBuilder <Standard_Real, 3> > aLBuilder =
      new BVH_LinearBuilder <Standard_Real, 3>();

  // Create the ShapeSet
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, Triangle> > aTriangleBoxSet[2];

  for (Standard_Integer i = 0; i < 2; ++i)
  {
    aTriangleBoxSet[i] = new BVH_BoxSet <Standard_Real, 3, Triangle> (aLBuilder);

    TopTools_IndexedMapOfShape aMapShapes;
    TopExp::MapShapes (aShape[i], TopAbs_FACE,   aMapShapes);

    for (Standard_Integer iS = 1; iS <= aMapShapes.Extent(); ++iS)
    {
      const TopoDS_Face& aF = TopoDS::Face (aMapShapes(iS));
      TopLoc_Location aLoc;
      const Handle(Poly_Triangulation)& aTriangulation = BRep_Tool::Triangulation(aF, aLoc);

      const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
      const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

      const int aNbTriangles = aTriangles.Length();
      for (int iT = 1; iT <= aNbTriangles; ++iT)
      {
        const Poly_Triangle& aTriangle = aTriangles (iT);
        // Nodes indices
        Standard_Integer id1, id2, id3;
        aTriangle.Get (id1, id2, id3);

        const gp_Pnt& aP1 = aNodes(id1).Transformed(aLoc.Transformation());
        const gp_Pnt& aP2 = aNodes(id2).Transformed(aLoc.Transformation());
        const gp_Pnt& aP3 = aNodes(id3).Transformed(aLoc.Transformation());

        BVH_Vec3d aBVHP1 (aP1.X(), aP1.Y(), aP1.Z());
        BVH_Vec3d aBVHP2 (aP2.X(), aP2.Y(), aP2.Z());
        BVH_Vec3d aBVHP3 (aP3.X(), aP3.Y(), aP3.Z());

        BVH_Box<Standard_Real, 3> aBox;
        aBox.Add (aBVHP1);
        aBox.Add (aBVHP2);
        aBox.Add (aBVHP3);

        aTriangleBoxSet[i]->Add (Triangle (aBVHP1, aBVHP2, aBVHP3), aBox);
      }
    }
    // Build BVH
    aTriangleBoxSet[i]->Build();
  }

  // Initialize selector
  MeshMeshDistance aDistTool;
  // Select the elements
  aDistTool.SetBVHSets (aTriangleBoxSet[0].get(), aTriangleBoxSet[1].get());
  Standard_Real aSqDist = aDistTool.ComputeDistance();
  if (!aDistTool.IsDone())
    std::cout << "Not Done" << std::endl;
  else
    theDI << "Distance " << sqrt (aSqDist) << "\n";

  return 0;
}

//=======================================================================
//function : QABVH_TriangleSet
//purpose : Auxiliary class to contain triangulation of a face
//=======================================================================
class QABVH_TriangleSet : public BVH_Triangulation<Standard_Real, 3>
{
public:
  QABVH_TriangleSet()
    : BVH_Triangulation<Standard_Real, 3>()
  {}

public:

  //! Creates the triangulation from a face
  void Build (const TopoDS_Face& theFace)
  {
    TopLoc_Location aLoc;
    const Handle(Poly_Triangulation)& aTriangulation = BRep_Tool::Triangulation(theFace, aLoc);

    const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
    const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

    const int aNbTriangles = aTriangles.Length();
    for (int iT = 1; iT <= aNbTriangles; ++iT)
    {
      const Poly_Triangle& aTriangle = aTriangles (iT);
      // Nodes indices
      Standard_Integer id1, id2, id3;
      aTriangle.Get (id1, id2, id3);

      const gp_Pnt& aP1 = aNodes(id1).Transformed(aLoc.Transformation());
      const gp_Pnt& aP2 = aNodes(id2).Transformed(aLoc.Transformation());
      const gp_Pnt& aP3 = aNodes(id3).Transformed(aLoc.Transformation());

      BVH_Vec3d aBVHP1 (aP1.X(), aP1.Y(), aP1.Z());
      BVH_Vec3d aBVHP2 (aP2.X(), aP2.Y(), aP2.Z());
      BVH_Vec3d aBVHP3 (aP3.X(), aP3.Y(), aP3.Z());

      Standard_Integer id = static_cast<Standard_Integer>(Vertices.size()) - 1;
      Vertices.push_back (aBVHP1);
      Vertices.push_back (aBVHP2);
      Vertices.push_back (aBVHP3);

      Elements.push_back (BVH_Vec4i (id + 1, id + 2, id + 3, iT));
    }

    MarkDirty();
    BVH();
  }
};

//=======================================================================
//function : QABVH_Geometry
//purpose : Auxiliary class to contain triangulation of a shape
//=======================================================================
class QABVH_Geometry : public BVH_Geometry<Standard_Real, 3>
{
public:
  QABVH_Geometry()
    : BVH_Geometry<Standard_Real, 3>()
  {}

public:

  //! Creates the triangulation from a face
  void Build (const TopoDS_Shape& theShape)
  {
    TopExp_Explorer anExp (theShape, TopAbs_FACE);
    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Face& aF = TopoDS::Face (anExp.Current());
      Handle(QABVH_TriangleSet) aTriSet = new QABVH_TriangleSet();
      aTriSet->Build (aF);
      myObjects.Append (aTriSet);
    }

    MarkDirty();
    BVH();
  }
};

//=======================================================================
//function : QABVH_DistanceField
//purpose : Computes the distance field on a given shape
//=======================================================================
static Standard_Integer QABVH_DistanceField (Draw_Interpretor& theDI,
                                             Standard_Integer theArgc,
                                             const char** theArgv)
{
  if (theArgc < 2)
  {
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  TopoDS_Shape aShape;
  // Get the first shape
  aShape = DBRep::Get (theArgv [1]);
  if (aShape.IsNull())
  {
    std::cout << theArgv[1] << " does not exist" << std::endl;
    return 1;
  }

  Standard_Integer aNbDim = 10;
  if (theArgc > 2)
  {
    aNbDim = Draw::Atoi (theArgv[2]);
  }

  QABVH_Geometry aGeometry;
  aGeometry.Build(aShape);

  BVH_DistanceField<Standard_Real, 3> aDField(aNbDim, Standard_True);
  aDField.Build (aGeometry);

  for (Standard_Integer iX = 0; iX < aDField.DimensionX(); ++iX)
  {
    for (Standard_Integer iY = 0; iY < aDField.DimensionY(); ++iY)
    {
      for (Standard_Integer iZ = 0; iZ < aDField.DimensionZ(); ++iZ)
      {
        Standard_Real aDist = aDField.Voxel (iX, iY, iZ);
        std::cout << "(" << iX << ", " << iY << ", " << iZ << "): " << aDist << std::endl;
      }
    }
  }
  return 0;
}

//=======================================================================
//function : Commands_BVH
//purpose : BVH commands
//=======================================================================
void QABugs::Commands_BVH (Draw_Interpretor& theCommands)
{
  const char *group = "QABugs";

  theCommands.Add ("QABVH_ShapeSelect",
                   "Tests the work of BHV_BoxSet algorithm on the simple example of selection of shapes which boxes interfere with given box.\n"
                   "Usage: QABVH_ShapeSelect result shape box (defined as a solid) [-void]\n"
                   "\tResult should contain all sub-shapes of the shape interfering with given box",
                   __FILE__, QABVH_ShapeSelect, group);

  theCommands.Add ("QABVH_PairSelect",
                   "Tests the work of BHV_BoxSet algorithm on the simple example of selection of pairs of shapes with interfering bounding boxes.\n"
                   "Usage: QABVH_PairSelect result shape1 shape2 [-void]\n"
                   "\tResult should contain all interfering pairs (compound of pairs)",
                   __FILE__, QABVH_PairSelect, group);

  theCommands.Add ("QABVH_PairDistance",
                   "Computes the distance between the meshes of the given shapes.\n"
                   "Usage: QABVH_PairDistance shape1 shape2\n"
                   "\tThe given shapes should contain triangulation\n",
                   __FILE__, QABVH_PairDistance, group);

  theCommands.Add ("QABVH_DistanceField",
                   "Computes the distance field for a shape with triangulation\n"
                   "Usage: QABVH_DistanceField shape [nbSplit]\n",
                   __FILE__, QABVH_DistanceField, group);

}
