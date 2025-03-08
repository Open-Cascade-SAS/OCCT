// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMeshTools_DelaunayDeflectionControlMeshAlgo_HeaderFile
#define _BRepMeshTools_DelaunayDeflectionControlMeshAlgo_HeaderFile

#include <BRepMesh_DelaunayNodeInsertionMeshAlgo.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <GeomLib.hxx>

//! Extends node insertion Delaunay meshing algo in order to control
//! deflection of generated triangles. Splits triangles failing the check.
template <class RangeSplitter, class BaseAlgo>
class BRepMesh_DelaunayDeflectionControlMeshAlgo
    : public BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter, BaseAlgo>
{
private:
  // Typedef for OCCT RTTI
  typedef BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter, BaseAlgo>
    DelaunayInsertionBaseClass;

public:
  //! Constructor.
  BRepMesh_DelaunayDeflectionControlMeshAlgo()
      : myMaxSqDeflection(-1.),
        mySqMinSize(-1.),
        myIsAllDegenerated(Standard_False),
        myCircles(NULL)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_DelaunayDeflectionControlMeshAlgo() {}

protected:
  //! Performs processing of generated mesh. Generates surface nodes and inserts them into
  //! structure.
  virtual void postProcessMesh(BRepMesh_Delaun&             theMesher,
                               const Message_ProgressRange& theRange) Standard_OVERRIDE
  {
    Message_ProgressScope aPS(theRange, "Post process mesh", 2);
    // Insert surface nodes.
    DelaunayInsertionBaseClass::postProcessMesh(theMesher, aPS.Next());
    if (!aPS.More())
    {
      return;
    }

    if (this->getParameters().ControlSurfaceDeflection
        && this->getStructure()->ElementsOfDomain().Extent() > 0)
    {
      optimizeMesh(theMesher, aPS.Next());
    }
    else
    {
      aPS.Next();
    }
  }

  //! Checks deviation of a mesh from geometrical surface.
  //! Inserts additional nodes in case of huge deviation.
  virtual void optimizeMesh(BRepMesh_Delaun& theMesher, const Message_ProgressRange& theRange)
  {
    Handle(NCollection_IncAllocator) aTmpAlloc =
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

    mySqMinSize = this->getParameters().MinSize * this->getParameters().MinSize;
    myCouplesMap =
      new IMeshData::MapOfOrientedEdges(3 * this->getStructure()->ElementsOfDomain().Extent(),
                                        aTmpAlloc);
    myControlNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);
    myCircles      = &theMesher.Circles();

    const Standard_Integer aIterationsNb = 11;
    Standard_Boolean       isInserted    = Standard_True;
    Message_ProgressScope  aPS(theRange, "Iteration", aIterationsNb);
    for (Standard_Integer aPass = 1; aPass <= aIterationsNb && isInserted && !myIsAllDegenerated;
         ++aPass)
    {
      if (!aPS.More())
      {
        return;
      }
      // Reset stop condition
      myMaxSqDeflection  = -1.;
      myIsAllDegenerated = Standard_True;
      myControlNodes->Clear();

      if (this->getStructure()->ElementsOfDomain().Extent() < 1)
      {
        break;
      }
      // Iterate on current triangles
      IMeshData::IteratorOfMapOfInteger aTriangleIt(this->getStructure()->ElementsOfDomain());
      for (; aTriangleIt.More(); aTriangleIt.Next())
      {
        const BRepMesh_Triangle& aTriangle = this->getStructure()->GetElement(aTriangleIt.Key());
        splitTriangleGeometry(aTriangle);
      }

      isInserted = this->insertNodes(myControlNodes, theMesher, aPS.Next());
    }

    myCouplesMap.Nullify();
    myControlNodes.Nullify();

    if (!(myMaxSqDeflection < 0.))
    {
      this->getDFace()->SetDeflection(Sqrt(myMaxSqDeflection));
    }
  }

private:
  //! Contains geometrical data related to node of triangle.
  struct TriangleNodeInfo
  {
    TriangleNodeInfo()
        : isFrontierLink(Standard_False)
    {
    }

    gp_XY            Point2d;
    gp_XYZ           Point;
    Standard_Boolean isFrontierLink;
  };

  //! Functor computing deflection of a point from surface.
  class NormalDeviation
  {
  public:
    NormalDeviation(const gp_Pnt& theRefPnt, const gp_Vec& theNormal)
        : myRefPnt(theRefPnt),
          myNormal(theNormal)
    {
    }

    Standard_Real SquareDeviation(const gp_Pnt& thePoint) const
    {
      const Standard_Real aDeflection = Abs(myNormal.Dot(gp_Vec(myRefPnt, thePoint)));
      return aDeflection * aDeflection;
    }

  private:
    NormalDeviation(const NormalDeviation& theOther);

    void operator=(const NormalDeviation& theOther);

  private:
    const gp_Pnt& myRefPnt;
    const gp_Vec& myNormal;
  };

  //! Functor computing deflection of a point on triangle link from surface.
  class LineDeviation
  {
  public:
    LineDeviation(const gp_Pnt& thePnt1, const gp_Pnt& thePnt2)
        : myPnt1(thePnt1),
          myPnt2(thePnt2)
    {
    }

    Standard_Real SquareDeviation(const gp_Pnt& thePoint) const
    {
      return BRepMesh_GeomTool::SquareDeflectionOfSegment(myPnt1, myPnt2, thePoint);
    }

  private:
    LineDeviation(const LineDeviation& theOther);

    void operator=(const LineDeviation& theOther);

  private:
    const gp_Pnt& myPnt1;
    const gp_Pnt& myPnt2;
  };

  //! Returns nodes info of the given triangle.
  void getTriangleInfo(const BRepMesh_Triangle& theTriangle,
                       const Standard_Integer (&theNodesIndices)[3],
                       TriangleNodeInfo (&theInfo)[3]) const
  {
    const Standard_Integer(&e)[3] = theTriangle.myEdges;
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      const BRepMesh_Vertex& aVertex = this->getStructure()->GetNode(theNodesIndices[i]);
      theInfo[i].Point2d = this->getRangeSplitter().Scale(aVertex.Coord(), Standard_False).XY();
      theInfo[i].Point   = this->getNodesMap()->Value(aVertex.Location3d()).XYZ();
      theInfo[i].isFrontierLink =
        (this->getStructure()->GetLink(e[i]).Movability() == BRepMesh_Frontier);
    }
  }

  // Check geometry of the given triangle. If triangle does not suit specified deflection, inserts
  // new point.
  void splitTriangleGeometry(const BRepMesh_Triangle& theTriangle)
  {
    if (theTriangle.Movability() != BRepMesh_Deleted)
    {
      Standard_Integer aNodexIndices[3];
      this->getStructure()->ElementNodes(theTriangle, aNodexIndices);

      TriangleNodeInfo aNodesInfo[3];
      getTriangleInfo(theTriangle, aNodexIndices, aNodesInfo);

      gp_Vec aNormal;
      gp_Vec aLinkVec[3];
      if (computeTriangleGeometry(aNodesInfo, aLinkVec, aNormal))
      {
        myIsAllDegenerated = Standard_False;

        const gp_XY aCenter2d =
          (aNodesInfo[0].Point2d + aNodesInfo[1].Point2d + aNodesInfo[2].Point2d) / 3.;

        usePoint(aCenter2d, NormalDeviation(aNodesInfo[0].Point, aNormal));
        splitLinks(aNodesInfo, aNodexIndices);
      }
    }
  }

  //! Updates array of links vectors.
  //! @return False on degenerative triangle.
  Standard_Boolean computeTriangleGeometry(const TriangleNodeInfo (&theNodesInfo)[3],
                                           gp_Vec (&theLinks)[3],
                                           gp_Vec& theNormal)
  {
    if (checkTriangleForDegenerativityAndGetLinks(theNodesInfo, theLinks))
    {
      if (checkTriangleArea2d(theNodesInfo))
      {
        if (computeNormal(theLinks[0], theLinks[1], theNormal))
        {
          return Standard_True;
        }
      }
    }

    return Standard_False;
  }

  //! Updates array of links vectors.
  //! @return False on degenerative triangle.
  Standard_Boolean checkTriangleForDegenerativityAndGetLinks(
    const TriangleNodeInfo (&theNodesInfo)[3],
    gp_Vec (&theLinks)[3])
  {
    const Standard_Real MinimalSqLength3d = 1.e-12;
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      theLinks[i] = theNodesInfo[(i + 1) % 3].Point - theNodesInfo[i].Point;
      if (theLinks[i].SquareMagnitude() < MinimalSqLength3d)
      {
        return Standard_False;
      }
    }

    return Standard_True;
  }

  //! Checks area of triangle in parametric space for degenerativity.
  //! @return False on degenerative triangle.
  Standard_Boolean checkTriangleArea2d(const TriangleNodeInfo (&theNodesInfo)[3])
  {
    const gp_Vec2d aLink2d1(theNodesInfo[0].Point2d, theNodesInfo[1].Point2d);
    const gp_Vec2d aLink2d2(theNodesInfo[1].Point2d, theNodesInfo[2].Point2d);

    const Standard_Real MinimalArea2d = 1.e-9;
    return (Abs(aLink2d1 ^ aLink2d2) > MinimalArea2d);
  }

  //! Computes normal using two link vectors.
  //! @return True on success, False in case of normal of null magnitude.
  Standard_Boolean computeNormal(const gp_Vec& theLink1, const gp_Vec& theLink2, gp_Vec& theNormal)
  {
    const gp_Vec aNormal(theLink1 ^ theLink2);
    if (aNormal.SquareMagnitude() > gp::Resolution())
    {
      theNormal = aNormal.Normalized();
      return Standard_True;
    }

    return Standard_False;
  }

  //! Computes deflection of midpoints of triangles links.
  //! @return True if point fits specified deflection.
  void splitLinks(const TriangleNodeInfo (&theNodesInfo)[3],
                  const Standard_Integer (&theNodesIndices)[3])
  {
    // Check deflection at triangle links
    for (Standard_Integer i = 0; i < 3; ++i)
    {
      if (theNodesInfo[i].isFrontierLink)
      {
        continue;
      }

      const Standard_Integer j = (i + 1) % 3;
      // Check if this link was already processed
      Standard_Integer aFirstVertex, aLastVertex;
      if (theNodesIndices[i] < theNodesIndices[j])
      {
        aFirstVertex = theNodesIndices[i];
        aLastVertex  = theNodesIndices[j];
      }
      else
      {
        aFirstVertex = theNodesIndices[j];
        aLastVertex  = theNodesIndices[i];
      }

      if (myCouplesMap->Add(BRepMesh_OrientedEdge(aFirstVertex, aLastVertex)))
      {
        const gp_XY aMidPnt2d = (theNodesInfo[i].Point2d + theNodesInfo[j].Point2d) / 2.;

        if (!usePoint(aMidPnt2d, LineDeviation(theNodesInfo[i].Point, theNodesInfo[j].Point)))
        {
          if (!rejectSplitLinksForMinSize(theNodesInfo[i], theNodesInfo[j], aMidPnt2d))
          {
            if (!checkLinkEndsForAngularDeviation(theNodesInfo[i], theNodesInfo[j], aMidPnt2d))
            {
              myControlNodes->Append(aMidPnt2d);
            }
          }
        }
      }
    }
  }

  //! Checks that two links produced as the result of a split of
  //! the given link by the middle point fit MinSize requirement.
  Standard_Boolean rejectSplitLinksForMinSize(const TriangleNodeInfo& theNodeInfo1,
                                              const TriangleNodeInfo& theNodeInfo2,
                                              const gp_XY&            theMidPoint)
  {
    const gp_Pnt aPnt = getPoint3d(theMidPoint);
    return ((theNodeInfo1.Point - aPnt.XYZ()).SquareModulus() < mySqMinSize
            || (theNodeInfo2.Point - aPnt.XYZ()).SquareModulus() < mySqMinSize);
  }

  //! Checks the given point (located between the given nodes)
  //! for specified angular deviation.
  Standard_Boolean checkLinkEndsForAngularDeviation(const TriangleNodeInfo& theNodeInfo1,
                                                    const TriangleNodeInfo& theNodeInfo2,
                                                    const gp_XY& /*theMidPoint*/)
  {
    gp_Dir                      aNorm1, aNorm2;
    const Handle(Geom_Surface)& aSurf = this->getDFace()->GetSurface()->Surface().Surface();

    if ((GeomLib::NormEstim(aSurf, theNodeInfo1.Point2d, Precision::Confusion(), aNorm1) == 0)
        && (GeomLib::NormEstim(aSurf, theNodeInfo2.Point2d, Precision::Confusion(), aNorm2) == 0))
    {
      Standard_Real anAngle = aNorm1.Angle(aNorm2);
      if (anAngle > this->getParameters().AngleInterior)
      {
        return Standard_False;
      }
    }
#if 0
    else if (GeomLib::NormEstim(aSurf, theMidPoint, Precision::Confusion(), aNorm1) != 0)
    {
      // It is better to consider the singular point as a node of triangulation.
      // However, it leads to hangs up meshing some faces (including faces with
      // degenerated edges). E.g. tests "mesh standard_incmesh Q6".
      // So, this code fragment is better to implement in the future.
      return Standard_False;
    }
#endif

    return Standard_True;
  }

  //! Returns 3d point corresponding to the given one in 2d space.
  gp_Pnt getPoint3d(const gp_XY& thePnt2d)
  {
    gp_Pnt aPnt;
    this->getDFace()->GetSurface()->D0(thePnt2d.X(), thePnt2d.Y(), aPnt);
    return aPnt;
  }

  //! Computes deflection of the given point and caches it for
  //! insertion in case if it overflows deflection.
  //! @return True if point has been cached for insertion.
  template <class DeflectionFunctor>
  Standard_Boolean usePoint(const gp_XY& thePnt2d, const DeflectionFunctor& theDeflectionFunctor)
  {
    const gp_Pnt aPnt = getPoint3d(thePnt2d);
    if (!checkDeflectionOfPointAndUpdateCache(thePnt2d,
                                              aPnt,
                                              theDeflectionFunctor.SquareDeviation(aPnt)))
    {
      myControlNodes->Append(thePnt2d);
      return Standard_True;
    }

    return Standard_False;
  }

  //! Checks the given point for specified linear deflection.
  //! Updates value of total mesh defleciton.
  Standard_Boolean checkDeflectionOfPointAndUpdateCache(const gp_XY&        thePnt2d,
                                                        const gp_Pnt&       thePnt3d,
                                                        const Standard_Real theSqDeflection)
  {
    if (theSqDeflection > myMaxSqDeflection)
    {
      myMaxSqDeflection = theSqDeflection;
    }

    const Standard_Real aSqDeflection =
      this->getDFace()->GetDeflection() * this->getDFace()->GetDeflection();
    if (theSqDeflection < aSqDeflection)
    {
      return Standard_True;
    }

    return rejectByMinSize(thePnt2d, thePnt3d);
  }

  //! Checks distance between the given node and nodes of triangles
  //! shot by it for MinSize criteria.
  //! This check is expected to roughly estimate and prevent
  //! generation of triangles with sides smaller than MinSize.
  Standard_Boolean rejectByMinSize(const gp_XY& thePnt2d, const gp_Pnt& thePnt3d)
  {
    IMeshData::MapOfInteger   aUsedNodes;
    IMeshData::ListOfInteger& aCirclesList =
      const_cast<BRepMesh_CircleTool&>(*myCircles)
        .Select(this->getRangeSplitter().Scale(thePnt2d, Standard_True).XY());

    IMeshData::ListOfInteger::Iterator aCircleIt(aCirclesList);
    for (; aCircleIt.More(); aCircleIt.Next())
    {
      const BRepMesh_Triangle& aTriangle = this->getStructure()->GetElement(aCircleIt.Value());

      Standard_Integer aNodes[3];
      this->getStructure()->ElementNodes(aTriangle, aNodes);

      for (Standard_Integer i = 0; i < 3; ++i)
      {
        if (!aUsedNodes.Contains(aNodes[i]))
        {
          aUsedNodes.Add(aNodes[i]);
          const BRepMesh_Vertex& aVertex = this->getStructure()->GetNode(aNodes[i]);
          const gp_Pnt&          aPoint  = this->getNodesMap()->Value(aVertex.Location3d());

          if (thePnt3d.SquareDistance(aPoint) < mySqMinSize)
          {
            return Standard_True;
          }
        }
      }
    }

    return Standard_False;
  }

private:
  Standard_Real                         myMaxSqDeflection;
  Standard_Real                         mySqMinSize;
  Standard_Boolean                      myIsAllDegenerated;
  Handle(IMeshData::MapOfOrientedEdges) myCouplesMap;
  Handle(IMeshData::ListOfPnt2d)        myControlNodes;
  const BRepMesh_CircleTool*            myCircles;
};

#endif
