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

#ifndef _BRepMesh_BaseMeshAlgo_HeaderFile
#define _BRepMesh_BaseMeshAlgo_HeaderFile

#include <IMeshTools_MeshAlgo.hxx>
#include <NCollection_Shared.hxx>
#include <IMeshTools_Parameters.hxx>
#include <BRepMesh_DegreeOfFreedom.hxx>
#include <Poly_Triangulation.hxx>

class BRepMesh_DataStructureOfDelaun;

//! Class provides base functionality for algorithms building face triangulation.
//! Performs initialization of BRepMesh_DataStructureOfDelaun and nodes map structures.
class BRepMesh_BaseMeshAlgo : public IMeshTools_MeshAlgo
{
public:

  typedef NCollection_Shared<NCollection_Vector<gp_Pnt> > VectorOfPnt;

  //! Constructor.
  Standard_EXPORT BRepMesh_BaseMeshAlgo();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_BaseMeshAlgo();

  //! Performs processing of the given face.
  Standard_EXPORT virtual void Perform(
    const IMeshData::IFaceHandle& theDFace,
    const IMeshTools_Parameters&  theParameters,
    const Message_ProgressRange&  theRange = Message_ProgressRange()) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_BaseMeshAlgo, IMeshTools_MeshAlgo)

protected:

  //! Gets discrete face.
  const IMeshData::IFaceHandle& getDFace() const
  {
    return myDFace;
  }

  //! Gets meshing parameters.
  const IMeshTools_Parameters& getParameters() const
  {
    return myParameters;
  }

  //! Gets common allocator.
  const Handle(NCollection_IncAllocator)& getAllocator() const
  {
    return myAllocator;
  }

  //! Gets mesh structure.
  const Handle(BRepMesh_DataStructureOfDelaun)& getStructure() const
  {
    return myStructure;
  }

  //! Gets 3d nodes map.
  const Handle(VectorOfPnt)& getNodesMap() const
  {
    return myNodesMap;
  }

protected:

  //! Registers the given point in vertex map and adds 2d point to mesh data structure.
  //! Returns index of node in the structure.
  Standard_EXPORT virtual Standard_Integer registerNode(
    const gp_Pnt&                  thePoint,
    const gp_Pnt2d&                thePoint2d,
    const BRepMesh_DegreeOfFreedom theMovability,
    const Standard_Boolean         isForceAdd);

  //! Adds the given 2d point to mesh data structure.
  //! Returns index of node in the structure.
  Standard_EXPORT virtual Standard_Integer addNodeToStructure(
    const gp_Pnt2d&                thePoint,
    const Standard_Integer         theLocation3d,
    const BRepMesh_DegreeOfFreedom theMovability,
    const Standard_Boolean         isForceAdd);

  //! Returns 2d point associated to the given vertex.
  Standard_EXPORT virtual gp_Pnt2d getNodePoint2d(const BRepMesh_Vertex& theVertex) const;

  //! Performs initialization of data structure using existing model data.
  Standard_EXPORT virtual Standard_Boolean initDataStructure();

  //! Generates mesh for the contour stored in data structure.
  Standard_EXPORT virtual void generateMesh(const Message_ProgressRange& theRange) = 0;

private:

  //! If the given edge has another pcurve for current face coinciding with specified one,
  //! returns TopAbs_INTERNAL flag. Elsewhere returns orientation of specified pcurve.
  TopAbs_Orientation fixSeamEdgeOrientation(
    const IMeshData::IEdgeHandle&   theDEdge,
    const IMeshData::IPCurveHandle& thePCurve) const;

  //! Adds new link to the mesh data structure.
  //! Movability of the link and order of nodes depend on orientation parameter.
  Standard_Integer addLinkToMesh(
    const Standard_Integer   theFirstNodeId,
    const Standard_Integer   theLastNodeId,
    const TopAbs_Orientation theOrientation);

  //! Commits generated triangulation to TopoDS face.
  void commitSurfaceTriangulation();

  //! Collects triangles to output data.
  Handle(Poly_Triangulation) collectTriangles();

  //! Collects nodes to output data.
  void collectNodes(const Handle(Poly_Triangulation)& theTriangulation);

private:
  typedef NCollection_Shared<NCollection_DataMap<Standard_Integer, Standard_Integer> > DMapOfIntegerInteger;

  IMeshData::IFaceHandle                 myDFace;
  IMeshTools_Parameters                  myParameters;
  Handle(NCollection_IncAllocator)       myAllocator;
  Handle(BRepMesh_DataStructureOfDelaun) myStructure;
  Handle(VectorOfPnt)                    myNodesMap;
  Handle(DMapOfIntegerInteger)           myUsedNodes;
};

#endif