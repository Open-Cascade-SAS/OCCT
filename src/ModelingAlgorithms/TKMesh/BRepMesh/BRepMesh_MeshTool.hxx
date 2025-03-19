// Created on: 2016-08-22
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

#ifndef _BRepMesh_MeshTool_HeaderFile
#define _BRepMesh_MeshTool_HeaderFile

#include <Standard_Transient.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_CircleTool.hxx>
#include <gp_Lin2d.hxx>
#include <IMeshData_Types.hxx>
#include <BRepMesh_Edge.hxx>

#include <stack>

//! Auxiliary tool providing API for manipulation with BRepMesh_DataStructureOfDelaun.
class BRepMesh_MeshTool : public Standard_Transient
{
public:
  //! Helper functor intended to separate points to left and right from the constraint.
  class NodeClassifier
  {
  public:
    NodeClassifier(const BRepMesh_Edge&                          theConstraint,
                   const Handle(BRepMesh_DataStructureOfDelaun)& theStructure)
        : myStructure(theStructure)
    {
      const BRepMesh_Vertex& aVertex1 = myStructure->GetNode(theConstraint.FirstNode());
      const BRepMesh_Vertex& aVertex2 = myStructure->GetNode(theConstraint.LastNode());

      myConstraint.SetLocation(aVertex1.Coord());
      myConstraint.SetDirection(gp_Vec2d(aVertex1.Coord(), aVertex2.Coord()));
      mySign = myConstraint.Direction().X() > 0;
    }

    Standard_Boolean IsAbove(const Standard_Integer theNodeIndex) const
    {
      const BRepMesh_Vertex& aVertex = myStructure->GetNode(theNodeIndex);
      const gp_Vec2d         aNodeVec(myConstraint.Location(), aVertex.Coord());
      if (aNodeVec.SquareMagnitude() > gp::Resolution())
      {
        const Standard_Real aCross = aNodeVec.Crossed(myConstraint.Direction());
        if (Abs(aCross) > gp::Resolution())
        {
          return mySign ? aCross < 0. : aCross > 0.;
        }
      }

      return Standard_False;
    }

  private:
    NodeClassifier(const NodeClassifier& theOther);

    void operator=(const NodeClassifier& theOther);

  private:
    const Handle(BRepMesh_DataStructureOfDelaun)& myStructure;
    gp_Lin2d                                      myConstraint;
    Standard_Boolean                              mySign;
  };

  //! Constructor.
  //! Initializes tool by the given data structure.
  Standard_EXPORT BRepMesh_MeshTool(const Handle(BRepMesh_DataStructureOfDelaun)& theStructure);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_MeshTool();

  //! Returns data structure manipulated by this tool.
  const Handle(BRepMesh_DataStructureOfDelaun)& GetStructure() const { return myStructure; }

  //! Dumps triangles to specified file.
  void DumpTriangles(const Standard_CString theFileName, IMeshData::MapOfInteger* theTriangles);

  //! Adds new triangle with specified nodes to mesh.
  //! Legalizes triangle in case if it violates circle criteria.
  void AddAndLegalizeTriangle(const Standard_Integer thePoint1,
                              const Standard_Integer thePoint2,
                              const Standard_Integer thePoint3)
  {
    Standard_Integer aEdges[3];
    AddTriangle(thePoint1, thePoint2, thePoint3, aEdges);

    Legalize(aEdges[0]);
    Legalize(aEdges[1]);
    Legalize(aEdges[2]);
  }

  //! Adds new triangle with specified nodes to mesh.
  void AddTriangle(const Standard_Integer thePoint1,
                   const Standard_Integer thePoint2,
                   const Standard_Integer thePoint3,
                   Standard_Integer (&theEdges)[3])
  {
    Standard_Boolean aOri[3];
    AddLink(thePoint1, thePoint2, theEdges[0], aOri[0]);
    AddLink(thePoint2, thePoint3, theEdges[1], aOri[1]);
    AddLink(thePoint3, thePoint1, theEdges[2], aOri[2]);

    myStructure->AddElement(BRepMesh_Triangle(theEdges, aOri, BRepMesh_Free));
  }

  //! Adds new link to mesh.
  //! Updates link index and link orientation parameters.
  void AddLink(const Standard_Integer theFirstNode,
               const Standard_Integer theLastNode,
               Standard_Integer&      theLinkIndex,
               Standard_Boolean&      theLinkOri)
  {
    const Standard_Integer aLinkIt =
      myStructure->AddLink(BRepMesh_Edge(theFirstNode, theLastNode, BRepMesh_Free));

    theLinkIndex = Abs(aLinkIt);
    theLinkOri   = (aLinkIt > 0);
  }

  //! Performs legalization of triangles connected to the specified link.
  Standard_EXPORT void Legalize(const Standard_Integer theLinkIndex);

  //! Erases all elements connected to the specified artificial node.
  //! In addition, erases the artificial node itself.
  Standard_EXPORT void EraseItemsConnectedTo(const Standard_Integer theNodeIndex);

  //! Cleans frontier links from triangles to the right.
  Standard_EXPORT void CleanFrontierLinks();

  //! Erases the given set of triangles.
  //! Fills map of loop edges forming the contour surrounding the erased triangles.
  void EraseTriangles(const IMeshData::MapOfInteger&  theTriangles,
                      IMeshData::MapOfIntegerInteger& theLoopEdges);

  //! Erases triangle with the given index and adds the free edges into the map.
  //! When an edge is suppressed more than one time it is destroyed.
  Standard_EXPORT void EraseTriangle(const Standard_Integer          theTriangleIndex,
                                     IMeshData::MapOfIntegerInteger& theLoopEdges);

  //! Erases all links that have no elements connected to them.
  Standard_EXPORT void EraseFreeLinks();

  //! Erases links from the specified map that have no elements connected to them.
  Standard_EXPORT void EraseFreeLinks(const IMeshData::MapOfIntegerInteger& theLinks);

  //! Gives the list of edges with type defined by input parameter.
  Standard_EXPORT Handle(IMeshData::MapOfInteger) GetEdgesByType(
    const BRepMesh_DegreeOfFreedom theEdgeType) const;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_MeshTool, Standard_Transient)

private:
  //! Returns True if the given point lies within circumcircle of the given triangle.
  Standard_Boolean checkCircle(const Standard_Integer (&aNodes)[3], const Standard_Integer thePoint)
  {
    const BRepMesh_Vertex& aVertex0 = myStructure->GetNode(aNodes[0]);
    const BRepMesh_Vertex& aVertex1 = myStructure->GetNode(aNodes[1]);
    const BRepMesh_Vertex& aVertex2 = myStructure->GetNode(aNodes[2]);

    gp_XY                  aLocation;
    Standard_Real          aRadius;
    const Standard_Boolean isOk = BRepMesh_CircleTool::MakeCircle(aVertex0.Coord(),
                                                                  aVertex1.Coord(),
                                                                  aVertex2.Coord(),
                                                                  aLocation,
                                                                  aRadius);

    if (isOk)
    {
      const BRepMesh_Vertex& aVertex = myStructure->GetNode(thePoint);
      const Standard_Real    aDist =
        (aVertex.Coord() - aLocation).SquareModulus() - (aRadius * aRadius);
      return (aDist < Precision::SquareConfusion());
    }

    return Standard_False;
  }

  //! Adds new triangle with the given nodes and updates
  //! links stack by ones are not in used map.
  void addTriangleAndUpdateStack(const Standard_Integer         theNode0,
                                 const Standard_Integer         theNode1,
                                 const Standard_Integer         theNode2,
                                 const IMeshData::MapOfInteger& theUsedLinks,
                                 std::stack<Standard_Integer>&  theStack)
  {
    Standard_Integer aEdges[3];
    AddTriangle(theNode0, theNode1, theNode2, aEdges);

    for (Standard_Integer i = 0; i < 3; ++i)
    {
      if (!theUsedLinks.Contains(aEdges[i]))
      {
        theStack.push(aEdges[i]);
      }
    }
  }

  //! Iteratively erases triangles and their neighbours consisting
  //! of free links using the given link as starting front.
  //! Only triangles around the constraint's saddle nodes will be removed.
  void collectTrianglesOnFreeLinksAroundNodesOf(const BRepMesh_Edge&     theConstraint,
                                                const Standard_Integer   theStartLink,
                                                IMeshData::MapOfInteger& theTriangles);

private:
  Handle(BRepMesh_DataStructureOfDelaun) myStructure;
};

#endif
