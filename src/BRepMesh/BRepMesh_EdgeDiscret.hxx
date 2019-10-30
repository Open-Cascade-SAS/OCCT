// Created on: 2016-04-19
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

#ifndef _BRepMesh_EdgeDiscret_HeaderFile
#define _BRepMesh_EdgeDiscret_HeaderFile

#include <IMeshTools_ModelAlgo.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Types.hxx>

class IMeshTools_CurveTessellator;

//! Class implements functionality of edge discret tool.
//! Performs check of the edges for existing Poly_PolygonOnTriangulation.
//! In case if it fits specified deflection, restores data structure using
//! it, else clears edges from outdated data.
class BRepMesh_EdgeDiscret : public IMeshTools_ModelAlgo
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_EdgeDiscret ();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_EdgeDiscret ();

  //! Creates instance of free edge tessellator.
  Standard_EXPORT static Handle(IMeshTools_CurveTessellator) CreateEdgeTessellator(
    const IMeshData::IEdgeHandle& theDEdge,
    const IMeshTools_Parameters&  theParameters,
    const Standard_Integer        theMinPointsNb = 2);

  //! Creates instance of edge tessellator.
  Standard_EXPORT static Handle(IMeshTools_CurveTessellator) CreateEdgeTessellator(
    const IMeshData::IEdgeHandle& theDEdge,
    const TopAbs_Orientation      theOrientation,
    const IMeshData::IFaceHandle& theDFace,
    const IMeshTools_Parameters&  theParameters,
    const Standard_Integer        theMinPointsNb = 2);

  //! Creates instance of tessellation extractor.
  Standard_EXPORT static Handle(IMeshTools_CurveTessellator) CreateEdgeTessellationExtractor(
    const IMeshData::IEdgeHandle& theDEdge,
    const IMeshData::IFaceHandle& theDFace);

  //! Functor API to discretize the given edge.
  void operator() (const Standard_Integer theEdgeIndex) const {
    process (theEdgeIndex);
  }

  //! Updates 3d discrete edge model using the given tessellation tool.
  Standard_EXPORT static void Tessellate3d(
    const IMeshData::IEdgeHandle&              theDEdge,
    const Handle(IMeshTools_CurveTessellator)& theTessellator,
    const Standard_Boolean                     theUpdateEnds);

  //! Updates 2d discrete edge model using tessellation of 3D curve.
  Standard_EXPORT static void Tessellate2d(
    const IMeshData::IEdgeHandle& theDEdge,
    const Standard_Boolean        theUpdateEnds);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_EdgeDiscret, IMeshTools_ModelAlgo)

protected:

  //! Performs processing of edges of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal (
    const Handle (IMeshData_Model)& theModel,
    const IMeshTools_Parameters&    theParameters,
    const Message_ProgressRange&    theRange) Standard_OVERRIDE;

private:

  //! Checks existing discretization of the edge and updates data model.
  void process (const Standard_Integer theEdgeIndex) const;

  //! Checks existing polygon on triangulation does it fit edge deflection or not.
  //! @return deflection of polygon or RealLast () in case if edge has no polygon 
  //! or it was dropped.
  Standard_Real checkExistingPolygonAndUpdateStatus(
    const IMeshData::IEdgeHandle&   theDEdge,
    const IMeshData::IPCurveHandle& thePCurve) const;

private:

  Handle (IMeshData_Model) myModel;
  IMeshTools_Parameters    myParameters;
};

#endif