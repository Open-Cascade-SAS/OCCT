// Created on: 2022-08-08
// Created by: Kseniya NOSULKO
// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _BRepExtrema_ProximityDistTool_HeaderFile
#define _BRepExtrema_ProximityDistTool_HeaderFile

#include <BRepExtrema_ElementFilter.hxx>
#include <BRepExtrema_MapOfIntegerPackedMapOfInteger.hxx>
#include <BRepExtrema_TriangleSet.hxx>
#include <BVH_Distance.hxx>
#include <BVH_Tools.hxx>
#include <Poly_Triangulation.hxx>

//! Tool class for computation the proximity distance from first 
//! primitive set to second one that is the maximal from minimum 
//! perpendicular distances. If no perpendicular distance is found, the 
//! minimum distance will be returned.
//! This tool is not intended to be used independently, and is integrated
//! in other classes, implementing algorithms based on shape tessellation
//! (BRepExtrema_ProximityValueTool).
//! 
//! Please note that algorithm results are approximate and depend greatly
//! on the quality of input tessellation(s).
class BRepExtrema_ProximityDistTool : public BVH_Distance <Standard_Real, 3, BVH_Vec3d, 
                                                           BRepExtrema_TriangleSet>
{
public:

  typedef typename BVH_Tools<Standard_Real, 3>::BVH_PrjStateInTriangle BVH_PrjState;

  enum ProxPnt_Status
  {
    ProxPnt_Status_BORDER,
    ProxPnt_Status_MIDDLE,
    ProxPnt_Status_UNKNOWN
  };

public:

  //! Struct with information about projection point state from 2nd BVH,
  //! providing proximity point of 2nd shape
  struct PrjState
  {
    PrjState()
    : myTrgIdx (0),
      myPrjState (BVH_PrjState::BVH_PrjStateInTriangle_INNER),
      myNumberOfFirstNode (0),
      myNumberOfLastNode (0)
    {}

    PrjState (const Standard_Integer theTrgIdx,
              const BVH_PrjState     thePrjState,
              const Standard_Integer theNumberOfFirstNode,
              const Standard_Integer theNumberOfLastNode)
    : myTrgIdx (theTrgIdx),
      myPrjState (thePrjState),
      myNumberOfFirstNode (theNumberOfFirstNode),
      myNumberOfLastNode (theNumberOfLastNode)
    {}

    Standard_Integer GetTrgIdx() const { return myTrgIdx; }

    BVH_PrjState GetPrjState() const { return myPrjState; }

    Standard_Integer GetNumberOfFirstNode() const { return myNumberOfFirstNode; }

    Standard_Integer GetNumberOfLastNode() const { return myNumberOfLastNode; }

  private:

    Standard_Integer myTrgIdx; //!< Index of triangle on which the projection is located
    BVH_PrjState myPrjState; //!< Position of a projection on the triangle (vertex, edge, inner)
    Standard_Integer myNumberOfFirstNode; //!< The 1st vtx of the triangle edge on which the projection is located
    Standard_Integer myNumberOfLastNode; //!< The 2nd vtx of the triangle edge on which the projection is located
  };

public:

  //! Creates new unitialized tool.
  Standard_EXPORT BRepExtrema_ProximityDistTool();

  //! Creates new tool for the given element sets.
  Standard_EXPORT BRepExtrema_ProximityDistTool (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                                 const Standard_Integer theNbSamples1,
                                                 const BVH_Array3d& theAddVertices1,
                                                 const NCollection_Vector<ProxPnt_Status>& theAddStatus1,
                                                 const Handle(BRepExtrema_TriangleSet)& theSet2,
                                                 const BRepExtrema_ShapeList& theShapeList1,
                                                 const BRepExtrema_ShapeList& theShapeList2);

public:

  //! Loads the given element sets into the tool.
  Standard_EXPORT void LoadTriangleSets (const Handle(BRepExtrema_TriangleSet)& theSet1,
                                         const Handle(BRepExtrema_TriangleSet)& theSet2);

  //! Loads the given list of subshapes into the tool.
  Standard_EXPORT void LoadShapeLists (const BRepExtrema_ShapeList& theShapeList1,
                                       const BRepExtrema_ShapeList& theShapeList2);

  //! Loads given additional vertices and their statuses.
  void LoadAdditionalPointsFirstSet (const BVH_Array3d& theAddVertices1,
                                     const NCollection_Vector<ProxPnt_Status>& theAddStatus1);

  //! Performs searching of the proximity distance.
  Standard_EXPORT void Perform();

public: //! @name Reject/Accept implementations

  //! Defines the rules for node rejection by bounding box.
  Standard_EXPORT virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCornerMin,
                                                       const BVH_Vec3d& theCornerMax,
                                                       Standard_Real& theMetric) const Standard_OVERRIDE;

  //! Defines the rules for leaf acceptance.
  Standard_EXPORT virtual Standard_Boolean Accept (const Standard_Integer theSgmIdx,
                                                   const Standard_Real&) Standard_OVERRIDE;

public:

  //! Returns true if the node is on the boarder.
  Standard_EXPORT static Standard_Boolean IsNodeOnBorder (const Standard_Integer theNodeIdx,
                                                          const Handle (Poly_Triangulation)& theTr);

  //! Returns true if the edge is on the boarder.
  Standard_EXPORT static Standard_Boolean IsEdgeOnBorder (const Standard_Integer theTrgIdx,
                                                          const Standard_Integer theFirstEdgeNodeIdx,
                                                          const Standard_Integer theSecondEdgeNodeIdx,
                                                          const Handle (Poly_Triangulation)& theTr);

public:

  //! Returns points on triangles sets, which provide the proximity distance.
  void ProximityPoints (BVH_Vec3d& thePoint1, BVH_Vec3d& thePoint2) const
  {
    thePoint1 = myPnt1;
    thePoint2 = myPnt2;
  }

  //! Returns status of points on triangles sets, which provide the proximity distance.
  void ProximityPointsStatus (ProxPnt_Status& thePointStatus1, ProxPnt_Status& thePointStatus2) const
  {
    thePointStatus1 = myPntStatus1;
    thePointStatus2 = myPntStatus2;
  }

  //! Returns the computed distance
  Standard_Real ProximityDistance() const { return myProxDist; }

protected:

  //! Computes the distance between object and BVH tree.
  Standard_EXPORT Standard_Real ComputeDistance();

  //! Defines the status of proximity points.
  Standard_EXPORT void DefineStatusProxPnt();

private:

  //! Goes throught vertices from the 1st set.
  void goThroughtSet1 (const BVH_Array3d& aVertices1,
                       const Standard_Boolean theIsAdditionalSet);

  //! Defines the status of proximity point from 1st BVH.
  void defineStatusProxPnt1();

  //! Defines the status of proximity point from 2nd BVH.
  void defineStatusProxPnt2();

protected:

  Standard_Real myMinDistance; //!< Minimal distance from point to BVH, could be not equal to myDistance
  BVH_Vec3d myMinDistPoint; //!< Point on BVH providing the minimal distance

  BVH_Vec3d myExtremaPoint; //!< Point on BVH providing the extrema

  Standard_Real myProxDist; //!< Proximity distance

  //! Proximity points
  BVH_Vec3d myPnt1, myPnt2;

  //! Proximity points' status
  ProxPnt_Status myPntStatus1, myPntStatus2;

private:

  //! Set of all mesh elements (triangles) of the 1st shape.
  Handle(BRepExtrema_TriangleSet) mySet1;
  //! Set of all mesh elements (triangles) of the 2nd shape.
  Handle(BRepExtrema_TriangleSet) mySet2;

  //! List of subshapes of the 1st shape.
  BRepExtrema_ShapeList myShapeList1;
  //! List of subshapes of the 2nd shape.
  BRepExtrema_ShapeList myShapeList2;

  Standard_Integer myNbSamples1; //!< Number of samples points on the first shape

  //! Is vertex corresponding to proximity point of 1st shape from additional set
  Standard_Integer myIsProxVtx1FromAddSet;
  BVH_Array3d myAddVertices1; //!< Additional vertices on the 1st shape
  NCollection_Vector<ProxPnt_Status> myAddStatus1; //!< Status of additional vertices on the 1st shape

  //! Vertex index from 1st BVH corresponding to proximity point of 1st shape
  Standard_Integer myProxVtxIdx1;

  //! Information of projection point state from 2nd BVH providing proximity point of 2nd shape
  PrjState myProxPrjState;

  //! Information of projection point state from 2nd BVH providing the extrema
  PrjState myExtPrjState;

  //! Information of projection point state from 2nd BVH providing the minimal distance
  PrjState myMinPrjState;

};

#endif // _BRepExtrema_ProximityDistTool_HeaderFile
