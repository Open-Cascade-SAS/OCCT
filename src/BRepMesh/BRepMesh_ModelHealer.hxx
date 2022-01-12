// Created on: 2016-06-23
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

#ifndef _BRepMesh_ModelHealer_HeaderFile
#define _BRepMesh_ModelHealer_HeaderFile

#include <IMeshTools_ModelAlgo.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Model.hxx>
#include <TopoDS_Vertex.hxx>

//! Class implements functionality of model healer tool.
//! Iterates over model's faces and checks consistency of their wires, 
//! i.e.whether wires are closed and do not contain self - intersections.
//! In case if wire contains disconnected parts, ends of adjacent edges
//! forming the gaps are connected in parametric space forcibly. The notion
//! of this operation is to create correct discrete model defined relatively
//! parametric space of target face taking into account connectivity and 
//! tolerances of 3D space only. This means that there are no specific 
//! computations are made for the sake of determination of U and V tolerance.
//! Registers intersections on edges forming the face's shape and tries to
//! amplify discrete representation by decreasing of deflection for the target edge.
//! Checks can be performed in parallel mode.
class BRepMesh_ModelHealer : public IMeshTools_ModelAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ModelHealer();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ModelHealer();

  //! Functor API to discretize the given edge.
  void operator() (const Standard_Integer theEdgeIndex) const {
    process(theEdgeIndex);
  }

  //! Functor API to discretize the given edge.
  void operator() (const IMeshData::IFaceHandle& theDFace) const {
    process(theDFace);
  }

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ModelHealer, IMeshTools_ModelAlgo)

protected:

  //! Performs processing of edges of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal (
    const Handle(IMeshData_Model)& theModel,
    const IMeshTools_Parameters&   theParameters,
    const Message_ProgressRange&   theRange) Standard_OVERRIDE;

private:

  //! Checks existing discretization of the face and updates data model.
  void process(const Standard_Integer theFaceIndex) const
  {
    const IMeshData::IFaceHandle& aDFace = myModel->GetFace(theFaceIndex);
    process(aDFace);
  }

  //! Checks existing discretization of the face and updates data model.
  void process(const IMeshData::IFaceHandle& theDFace) const;

  //! Amplifies discretization of edges in case if self-intersection problem has been found.
  void amplifyEdges();

  //! Returns common vertex of two edges or null ptr in case if there is no such vertex.
  TopoDS_Vertex getCommonVertex(
    const IMeshData::IEdgeHandle& theEdge1,
    const IMeshData::IEdgeHandle& theEdge2) const;

  //! Connects pcurves of previous and current edge on the specified face 
  //! according to topological connectivity. Uses next edge in order to
  //! identify closest point in case of single vertex shared between both
  //! ends of edge (degenerative edge)
  Standard_Boolean connectClosestPoints(
    const IMeshData::IPCurveHandle& thePrevDEdge,
    const IMeshData::IPCurveHandle& theCurrDEdge,
    const IMeshData::IPCurveHandle& theNextDEdge) const;

  //! Chooses the most closest point to reference one from the given pair.
  //! Returns square distance between reference point and closest one as 
  //! well as pointer to closest point.
  Standard_Real closestPoint(
    gp_Pnt2d&  theRefPnt,
    gp_Pnt2d&  theFristPnt,
    gp_Pnt2d&  theSecondPnt,
    gp_Pnt2d*& theClosestPnt) const
  {
    // Find the most closest end-points.
    const Standard_Real aSqDist1 = theRefPnt.SquareDistance(theFristPnt);
    const Standard_Real aSqDist2 = theRefPnt.SquareDistance(theSecondPnt);
    if (aSqDist1 < aSqDist2)
    {
      theClosestPnt = &theFristPnt;
      return aSqDist1;
    }

    theClosestPnt = &theSecondPnt;
    return aSqDist2;
  }

  //! Chooses the most closest points among the given to reference one from the given pair.
  //! Returns square distance between reference point and closest one as 
  //! well as pointer to closest point.
  Standard_Real closestPoints(
    gp_Pnt2d&  theFirstPnt1,
    gp_Pnt2d&  theSecondPnt1,
    gp_Pnt2d&  theFirstPnt2,
    gp_Pnt2d&  theSecondPnt2,
    gp_Pnt2d*& theClosestPnt1,
    gp_Pnt2d*& theClosestPnt2) const
  {
    gp_Pnt2d *aCurrPrevUV1 = NULL, *aCurrPrevUV2 = NULL;
    const Standard_Real aSqDist1 = closestPoint(theFirstPnt1,  theFirstPnt2, theSecondPnt2, aCurrPrevUV1);
    const Standard_Real aSqDist2 = closestPoint(theSecondPnt1, theFirstPnt2, theSecondPnt2, aCurrPrevUV2);
    if (aSqDist1 - aSqDist2 < gp::Resolution())
    {
      theClosestPnt1 = &theFirstPnt1;
      theClosestPnt2 = aCurrPrevUV1;
      return aSqDist1;
    }

    theClosestPnt1 = &theSecondPnt1;
    theClosestPnt2 = aCurrPrevUV2;
    return aSqDist2;
  }

  //! Adjusts the given pair of points supposed to be the same.
  //! In addition, adjusts another end-point of an edge in order
  //! to perform correct matching in case of gap.
  void adjustSamePoints(
    gp_Pnt2d*& theMajorSamePnt1,
    gp_Pnt2d*& theMinorSamePnt1,
    gp_Pnt2d*& theMajorSamePnt2,
    gp_Pnt2d*& theMinorSamePnt2,
    gp_Pnt2d&  theMajorFirstPnt,
    gp_Pnt2d&  theMajorLastPnt,
    gp_Pnt2d&  theMinorFirstPnt,
    gp_Pnt2d&  theMinorLastPnt) const
  {
    if (theMajorSamePnt2 == theMajorSamePnt1)
    {
      theMajorSamePnt2 = (theMajorSamePnt2 == &theMajorFirstPnt) ? &theMajorLastPnt : &theMajorFirstPnt;
      closestPoint(*theMajorSamePnt2, theMinorFirstPnt, theMinorLastPnt, theMinorSamePnt2);
    }

    *theMajorSamePnt1 = *theMinorSamePnt1;
    *theMajorSamePnt2 = *theMinorSamePnt2;
  }

  //! Connects ends of pcurves of face's wires according to topological coherency.
  void fixFaceBoundaries(const IMeshData::IFaceHandle& theDFace) const;

  //! Returns True if check can be done in parallel.
  Standard_Boolean isParallel() const
  {
    return (myParameters.InParallel && myModel->FacesNb() > 1);
  }

  //! Collects unique edges to be updated from face map. Clears data stored in face map.
  Standard_Boolean popEdgesToUpdate(IMeshData::MapOfIEdgePtr& theEdgesToUpdate);

private:

  Handle(IMeshData_Model)                           myModel;
  IMeshTools_Parameters                             myParameters;
  Handle(IMeshData::DMapOfIFacePtrsMapOfIEdgePtrs)  myFaceIntersectingEdges;
};

#endif