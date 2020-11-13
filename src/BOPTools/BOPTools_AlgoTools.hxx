// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPTools_AlgoTools_HeaderFile
#define _BOPTools_AlgoTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <BOPTools_ListOfCoupleOfShape.hxx>
#include <BOPTools_ListOfConnexityBlock.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Precision.hxx>
class TopoDS_Vertex;
class gp_Pnt;
class IntTools_Curve;
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Shape;
class IntTools_Context;
class TopoDS_Solid;
class IntTools_Range;
class TopoDS_Shell;
class Message_Report;

//! Provides tools used in Boolean Operations algorithm:
//! - Vertices intersection;
//! - Vertex construction;
//! - Edge construction;
//! - Classification algorithms;
//! - Making connexity blocks;
//! - Shape validation.
class BOPTools_AlgoTools
{
public:

  DEFINE_STANDARD_ALLOC

public: //! @name Constants

  //! Additional tolerance (delta tolerance) is used in Boolean Operations
  //! to ensure that the tolerance of new/old entities obtained
  //! by intersection of two shapes is slightly bigger than the actual
  //! distances to these shapes. It helps to avoid numerical instability
  //! which may occur when comparing distances and tolerances.
  static Standard_Real DTolerance() { return 1.e-12; }

public: //! @name Intersection of the vertices

  //! Intersects the vertex <theV1> with the point <theP> with tolerance <theTolP>.
  //! Returns the error status:
  //! - 0 - no error, meaning that the vertex intersects the point;
  //! - 1 - the distance between vertex and point is grater than the sum of tolerances.
  Standard_EXPORT static Standard_Integer ComputeVV(const TopoDS_Vertex& theV,
                                                    const gp_Pnt& theP,
                                                    const Standard_Real theTolP);

  //! Intersects the given vertices with given fuzzy value.
  //! Returns the error status:
  //! - 0 - no error, meaning that the vertices interferes with given tolerance;
  //! - 1 - the distance between vertices is grater than the sum of their tolerances.
  Standard_EXPORT static Standard_Integer ComputeVV(const TopoDS_Vertex& theV1,
                                                    const TopoDS_Vertex& theV2,
                                                    const Standard_Real theFuzz = Precision::Confusion());

public: //! @name Vertices construction

  //! Makes the vertex in the middle of given vertices with
  //! the tolerance covering all tolerance spheres of vertices.
  Standard_EXPORT static void MakeVertex(const TopTools_ListOfShape& theLV,
                                         TopoDS_Vertex& theV);

  //! Make a vertex using 3D-point <aP1> and 3D-tolerance value <aTol>
  Standard_EXPORT static void MakeNewVertex(const gp_Pnt& aP1,
                                            const Standard_Real aTol,
                                            TopoDS_Vertex& aNewVertex);

  //! Make a vertex using couple of vertices  <aV1, aV2>
  Standard_EXPORT static void MakeNewVertex(const TopoDS_Vertex& aV1,
                                            const TopoDS_Vertex& aV2,
                                            TopoDS_Vertex& aNewVertex);

  //! Make a vertex in place of intersection between two edges
  //! <aE1, aE2> with parameters <aP1, aP2>
  Standard_EXPORT static void MakeNewVertex(const TopoDS_Edge& aE1,
                                            const Standard_Real aP1,
                                            const TopoDS_Edge& aE2,
                                            const Standard_Real aP2,
                                            TopoDS_Vertex& aNewVertex);

  //! Make a vertex in place of intersection between the edge <aE1>
  //! with parameter <aP1> and the face <aF2>
  Standard_EXPORT static void MakeNewVertex(const TopoDS_Edge& aE1,
                                            const Standard_Real aP1,
                                            const TopoDS_Face& aF2,
                                            TopoDS_Vertex& aNewVertex);


public: //! @name Updating the vertex

  //! Update the tolerance value for vertex  <aV>
  //! taking into account the fact that <aV> lays on
  //! the curve <aIC>
  Standard_EXPORT static void UpdateVertex(const IntTools_Curve& aIC,
                                           const Standard_Real aT,
                                           const TopoDS_Vertex& aV);

  //! Update the tolerance value for vertex  <aV>
  //! taking into account the fact that <aV> lays on
  //! the edge <aE>
  Standard_EXPORT static void UpdateVertex(const TopoDS_Edge& aE,
                                           const Standard_Real aT,
                                           const TopoDS_Vertex& aV);

  //! Update the tolerance value for vertex  <aVN>
  //! taking into account the fact that <aVN> should
  //! cover tolerance zone of <aVF>
  Standard_EXPORT static void UpdateVertex(const TopoDS_Vertex& aVF,
                                           const TopoDS_Vertex& aVN);


public: //! @name Edge construction

  //! Makes the edge based on the given curve with given bounding vertices.
  Standard_EXPORT static void MakeEdge(const IntTools_Curve& theCurve,
                                       const TopoDS_Vertex& theV1,
                                       const Standard_Real theT1,
                                       const TopoDS_Vertex& theV2,
                                       const Standard_Real theT2,
                                       const Standard_Real theTolR3D,
                                       TopoDS_Edge& theE);

  //! Makes a copy of <theEdge> with vertices.
  Standard_EXPORT static TopoDS_Edge CopyEdge(const TopoDS_Edge& theEdge);

  //! Make the edge from base edge <aE1> and two vertices <aV1,aV2>
  //! at parameters <aP1,aP2>
  Standard_EXPORT static void MakeSplitEdge(const TopoDS_Edge& aE1,
                                            const TopoDS_Vertex& aV1,
                                            const Standard_Real aP1,
                                            const TopoDS_Vertex& aV2,
                                            const Standard_Real aP2,
                                            TopoDS_Edge& aNewEdge);

  //! Make the edge from 3D-Curve <aIC>  and two vertices <aV1,aV2>
  //! at parameters <aP1,aP2>
  Standard_EXPORT static void MakeSectEdge(const IntTools_Curve& aIC,
                                           const TopoDS_Vertex& aV1,
                                           const Standard_Real aP1,
                                           const TopoDS_Vertex& aV2,
                                           const Standard_Real aP2,
                                           TopoDS_Edge& aNewEdge);


public: //! @name Point/Edge/Face classification relatively solid

  //! Computes the 3-D state of the point thePoint
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState(const gp_Pnt& thePoint,
                                                   const TopoDS_Solid& theSolid,
                                                   const Standard_Real theTol,
                                                   const Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the vertex theVertex
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState(const TopoDS_Vertex& theVertex,
                                                   const TopoDS_Solid& theSolid,
                                                   const Standard_Real theTol,
                                                   const Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the edge theEdge
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState(const TopoDS_Edge& theEdge,
                                                  const TopoDS_Solid& theSolid,
                                                  const Standard_Real theTol,
                                                  const Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the face theFace
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theBounds - set of edges of <theSolid> to avoid
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState(const TopoDS_Face& theFace,
                                                   const TopoDS_Solid& theSolid,
                                                   const Standard_Real theTol,
                                                   const TopTools_IndexedMapOfShape& theBounds,
                                                   const Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the shape theShape
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeStateByOnePoint(const TopoDS_Shape& theShape,
                                                             const TopoDS_Solid& theSolid,
                                                             const Standard_Real theTol,
                                                             const Handle(IntTools_Context)& theContext);


public: //! @name Face classification relatively solid

  //! For the face theFace and its edge theEdge
  //! finds the face suitable to produce shell.
  //! theLCEF - set of faces to search. All faces
  //! from theLCEF must share edge theEdge
  Standard_EXPORT static Standard_Boolean GetFaceOff(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Face& theFace,
                                                     BOPTools_ListOfCoupleOfShape& theLCEF,
                                                     TopoDS_Face& theFaceOff,
                                                     const Handle(IntTools_Context)& theContext);

  //! Returns True if the face theFace is inside of the
  //! couple of faces theFace1, theFace2.
  //! The faces theFace, theFace1, theFace2  must
  //! share the edge theEdge
  //! Return values:
  //!  * 0 state is not IN
  //!  * 1 state is IN
  //!  * 2 state can not be found by the method of angles
  Standard_EXPORT static Standard_Integer IsInternalFace(const TopoDS_Face& theFace,
                                                         const TopoDS_Edge& theEdge,
                                                         const TopoDS_Face& theFace1,
                                                         const TopoDS_Face& theFace2,
                                                         const Handle(IntTools_Context)& theContext);

  //! Returns True if the face theFace is inside of the
  //! appropriate couple of faces (from the set theLF)    .
  //! The faces of the set theLF and theFace  must
  //! share the edge theEdge
  //!  * 0 state is not IN
  //!  * 1 state is IN
  //!  * 2 state can not be found by the method of angles
  Standard_EXPORT static Standard_Integer IsInternalFace(const TopoDS_Face& theFace,
                                                         const TopoDS_Edge& theEdge,
                                                         TopTools_ListOfShape& theLF,
                                                         const Handle(IntTools_Context)& theContext);

  //! Returns True if the face theFace is inside the
  //! solid theSolid.
  //! theMEF - Map Edge/Faces for theSolid
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  Standard_EXPORT static Standard_Boolean IsInternalFace(const TopoDS_Face& theFace,
                                                         const TopoDS_Solid& theSolid,
                                                         TopTools_IndexedDataMapOfShapeListOfShape& theMEF,
                                                         const Standard_Real theTol,
                                                         const Handle(IntTools_Context)& theContext);


public: //! @name PCurve construction

  //! Makes 2d curve of the edge <theE> on the faces <theF1> and <theF2>.<br>
  //! <theContext> - storage for caching the geometrical tools
  Standard_EXPORT static void MakePCurve (const TopoDS_Edge& theE,
                                          const TopoDS_Face& theF1,
                                          const TopoDS_Face& theF2,
                                          const IntTools_Curve& theCurve,
                                          const Standard_Boolean thePC1,
                                          const Standard_Boolean thePC2,
                                          const Handle(IntTools_Context)& theContext = Handle(IntTools_Context)());


public: //! @name Wire classification relatively face

  //! Checks if the wire is a hole for the face.
  Standard_EXPORT static Standard_Boolean IsHole(const TopoDS_Shape& theW,
                                                 const TopoDS_Shape& theF);


public: //! @name Choosing correct orientation for the split shape

  //! Checks if the direction of the split shape is opposite to
  //! the direction of the original shape.
  //! The method is an overload for (Edge,Edge) and (Face,Face) corresponding
  //! methods and checks only these types of shapes.
  //! For faces the method checks if normal directions are opposite.
  //! For edges the method checks if tangent vectors are opposite.
  //!
  //! In case the directions do not coincide, it returns TRUE, meaning
  //! that split shape has to be reversed to match the direction of the
  //! original shape.
  //!
  //! If requested (<theError> is not null), the method returns the status of the operation:
  //! - 0 - no error;
  //! - Error from (Edge,Edge) or (Face,Face) corresponding method
  //! - 100 - bad types.
  //! In case of any error the method always returns FALSE.
  //!
  //! @param theSplit [in] Split shape
  //! @param theShape [in] Original shape
  //! @param theContext [in] cached geometrical tools
  //! @param theError [out] Error Status of the operation
  Standard_EXPORT static Standard_Boolean IsSplitToReverse(const TopoDS_Shape& theSplit,
                                                           const TopoDS_Shape& theShape,
                                                           const Handle(IntTools_Context)& theContext,
                                                           Standard_Integer *theError = NULL);

  //! Add-on for the *IsSplitToReverse()* to check for its errors
  //! and in case of any add the *BOPAlgo_AlertUnableToOrientTheShape*
  //! warning to the report.
  Standard_EXPORT static Standard_Boolean IsSplitToReverseWithWarn(const TopoDS_Shape& theSplit,
                                                                   const TopoDS_Shape& theShape,
                                                                   const Handle(IntTools_Context)& theContext,
                                                                   const Handle(Message_Report)& theReport = NULL);

  //! Checks if the normal direction of the split face is opposite to
  //! the normal direction of the original face.
  //! The normal directions for both faces are taken in the same point -
  //! point inside the split face is projected onto the original face.
  //! Returns TRUE if the normals do not coincide, meaning the necessity
  //! to revert the orientation of the split face to match the direction
  //! of the original face.
  //!
  //! If requested (<theError> is not null), the method returns the status of the operation:
  //! - 0 - no error;
  //! - 1 - unable to find the point inside split face;
  //! - 2 - unable to compute the normal for the split face;
  //! - 3 - unable to project the point inside the split face on the original face;
  //! - 4 - unable to compute the normal for the original face.
  //! In case of any error the method always returns FALSE.
  //!
  //! @param theSplit [in] Split face
  //! @param theShape [in] Original face
  //! @param theContext [in] cached geometrical tools
  //! @param theError [out] Error Status of the operation
  Standard_EXPORT static Standard_Boolean IsSplitToReverse(const TopoDS_Face& theSplit,
                                                           const TopoDS_Face& theShape,
                                                           const Handle(IntTools_Context)& theContext,
                                                           Standard_Integer *theError = NULL);

  //! Checks if the tangent vector of the split edge is opposite to
  //! the tangent vector of the original edge.
  //! The tangent vectors for both edges are computed in the same point -
  //! point inside the split edge is projected onto the original edge.
  //! Returns TRUE if the tangent vectors do not coincide, meaning the necessity
  //! to revert the orientation of the split edge to match the direction
  //! of the original edge.
  //!
  //! If requested (<theError> is not null), the method returns the status of the operation:
  //! - 0 - no error;
  //! - 1 - degenerated edges are given;
  //! - 2 - unable to compute the tangent vector for the split edge;
  //! - 3 - unable to project the point inside the split edge on the original edge;
  //! - 4 - unable to compute the tangent vector for the original edge;
  //! In case of any error the method always returns FALSE.
  //!
  //! @param theSplit [in] Split edge
  //! @param theShape [in] Original edge
  //! @param theContext [in] cached geometrical tools
  //! @param theError [out] Error Status of the operation
  Standard_EXPORT static Standard_Boolean IsSplitToReverse(const TopoDS_Edge& theSplit,
                                                           const TopoDS_Edge& theShape,
                                                           const Handle(IntTools_Context)& theContext,
                                                           Standard_Integer *theError = NULL);

  //! Checks if the normals direction of the given faces computed near
  //! the shared edge coincide.
  //! Returns the status of operation:
  //! * 0 - in case of error (shared edge not found or directions are not collinear)
  //! * 1 - normal directions coincide;
  //! * -1 - normal directions are opposite.
  Standard_EXPORT static Standard_Integer Sense(const TopoDS_Face& theF1,
                                                const TopoDS_Face& theF2,
                                                const Handle(IntTools_Context)& theContext);

public: //! @name Making connexity blocks

  //! For the list of faces theLS build block
  //! theLSCB in terms of connexity by edges
  //! theMapAvoid - set of edges to avoid for
  //! the treatment
  Standard_EXPORT static void MakeConnexityBlock(TopTools_ListOfShape& theLS,
                                                 TopTools_IndexedMapOfShape& theMapAvoid,
                                                 TopTools_ListOfShape& theLSCB,
                                                 const Handle(NCollection_BaseAllocator)& theAllocator);

  //! For the compound <theS> builds the blocks (compounds) of
  //! elements of type <theElementType> connected through the shapes
  //! of the type <theConnectionType>.
  //! The blocks are stored into the list <theLCB>.
  Standard_EXPORT static void MakeConnexityBlocks(const TopoDS_Shape& theS,
                                                  const TopAbs_ShapeEnum theConnectionType,
                                                  const TopAbs_ShapeEnum theElementType,
                                                  TopTools_ListOfShape& theLCB);

  //! For the compound <theS> builds the blocks (compounds) of
  //! elements of type <theElementType> connected through the shapes
  //! of the type <theConnectionType>.
  //! The blocks are stored into the list of lists <theLCB>.
  //! Returns also the connection map <theConnectionMap>, filled during operation.
  Standard_EXPORT static void MakeConnexityBlocks(const TopoDS_Shape& theS,
                                                  const TopAbs_ShapeEnum theConnectionType,
                                                  const TopAbs_ShapeEnum theElementType,
                                                  TopTools_ListOfListOfShape& theLCB,
                                                  TopTools_IndexedDataMapOfShapeListOfShape& theConnectionMap);

  //! Makes connexity blocks of elements of the given type with the given type of the
  //! connecting elements. The blocks are checked on regularity (multi-connectivity)
  //! and stored to the list of blocks <theLCB>.
  Standard_EXPORT static void MakeConnexityBlocks(const TopTools_ListOfShape& theLS,
                                                  const TopAbs_ShapeEnum theConnectionType,
                                                  const TopAbs_ShapeEnum theElementType,
                                                  BOPTools_ListOfConnexityBlock& theLCB);

public: //! @name Orienting elements in container

  //! Correctly orients edges on the wire
  Standard_EXPORT static void OrientEdgesOnWire(TopoDS_Shape& theWire);

  //! Correctly orients faces on the shell
  Standard_EXPORT static void OrientFacesOnShell(TopoDS_Shape& theShell);


public: //! @name Methods for shape validation (correction)

  //! Provides valid values of tolerances for the shape <theS>
  //! <theTolMax> is max value of the tolerance that can be
  //! accepted for correction.  If real value of the tolerance
  //! will be greater than  <aTolMax>, the correction does not
  //! perform.
  Standard_EXPORT static void CorrectTolerances(const TopoDS_Shape& theS, 
                                                const TopTools_IndexedMapOfShape& theMapToAvoid,
                                                const Standard_Real theTolMax = 0.0001,
                                                const Standard_Boolean theRunParallel = Standard_False);

  //! Provides valid values of tolerances for the shape <theS>
  //! in  terms of BRepCheck_InvalidCurveOnSurface.
  Standard_EXPORT static void CorrectCurveOnSurface(const TopoDS_Shape& theS,
                                                    const TopTools_IndexedMapOfShape& theMapToAvoid,
                                                    const Standard_Real theTolMax = 0.0001,
                                                    const Standard_Boolean theRunParallel = Standard_False);

  //! Provides valid values of tolerances for the shape <theS>
  //! in  terms of BRepCheck_InvalidPointOnCurve.
  Standard_EXPORT static void CorrectPointOnCurve(const TopoDS_Shape& theS,
                                                  const TopTools_IndexedMapOfShape& theMapToAvoid,
                                                  const Standard_Real theTolMax = 0.0001,
                                                  const Standard_Boolean theRunParallel = Standard_False);

  //! Corrects tolerance values of the sub-shapes of the shape <theS> if needed.
  Standard_EXPORT static void CorrectShapeTolerances(const TopoDS_Shape& theS,
                                                     const TopTools_IndexedMapOfShape& theMapToAvoid,
                                                     const Standard_Boolean theRunParallel = Standard_False);


public: //! Checking if the faces are coinciding

  //! Checks if the given faces are same-domain, i.e. coincide.
  Standard_EXPORT static Standard_Boolean AreFacesSameDomain(const TopoDS_Face& theF1,
                                                             const TopoDS_Face& theF2, 
                                                             const Handle(IntTools_Context)& theContext,
                                                             const Standard_Real theFuzz = Precision::Confusion());

public: //! @name Looking for the edge in the face

  //! Returns True if the face theFace contains
  //! the edge theEdge but with opposite orientation.
  //! If the method  returns True theEdgeOff is the
  //! edge founded
  Standard_EXPORT static Standard_Boolean GetEdgeOff(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Face& theFace,
                                                     TopoDS_Edge& theEdgeOff);

  //! For the face theFace gets the edge theEdgeOnF
  //! that is the same as theEdge
  //! Returns True if such edge exists
  //! Returns False if there is no such edge
  Standard_EXPORT static Standard_Boolean GetEdgeOnFace(const TopoDS_Edge& theEdge,
                                                        const TopoDS_Face& theFace,
                                                        TopoDS_Edge& theEdgeOnF);


public: //! @name Correction of the edges range

  //! Correct shrunk range <aSR> taking into account 3D-curve
  //! resolution and corresponding tolerance values of <aE1>, <aE2>
  Standard_EXPORT static void CorrectRange(const TopoDS_Edge& aE1,
                                           const TopoDS_Edge& aE2,
                                           const IntTools_Range& aSR,
                                           IntTools_Range& aNewSR);
  

  //! Correct shrunk range <aSR> taking into account 3D-curve
  //! resolution and corresponding tolerance values of <aE>, <aF>
  Standard_EXPORT static void CorrectRange(const TopoDS_Edge& aE,
                                           const TopoDS_Face& aF,
                                           const IntTools_Range& aSR,
                                           IntTools_Range& aNewSR);

public: //! @name Checking edge on micro status

  //! Checks if it is possible to compute shrunk range for the edge <aE>
  //! Flag <theCheckSplittable> defines whether to take into account 
  //! the possibility to split the edge or not.
  Standard_EXPORT static Standard_Boolean IsMicroEdge(const TopoDS_Edge& theEdge,
                                                      const Handle(IntTools_Context)& theContext,
                                                      const Standard_Boolean theCheckSplittable = Standard_True);

public: //! @name Solid classification

  //! Returns true if the solid <theSolid> is inverted
  Standard_EXPORT static Standard_Boolean IsInvertedSolid(const TopoDS_Solid& theSolid);

public: //! @name Edge/Face Deviation computation

  //! Computes the necessary value of the tolerance for the edge
  Standard_EXPORT static Standard_Boolean ComputeTolerance(const TopoDS_Face& theFace,
                                                           const TopoDS_Edge& theEdge,
                                                           Standard_Real& theMaxDist,
                                                           Standard_Real& theMaxPar);

public: //! @name Other methods

  //! Makes empty container of requested type
  Standard_EXPORT static void MakeContainer(const TopAbs_ShapeEnum theType,
                                            TopoDS_Shape& theShape);

  //! Compute a 3D-point on the edge <aEdge> at parameter <aPrm>
  Standard_EXPORT static void PointOnEdge(const TopoDS_Edge& aEdge,
                                          const Standard_Real aPrm,
                                          gp_Pnt& aP);

  //! Returns TRUE if PaveBlock <aPB> lays on the face <aF>, i.e
  //! the <PB> is IN or ON in 2D of <aF>
  Standard_EXPORT static Standard_Boolean IsBlockInOnFace(const IntTools_Range& aShR,
                                                          const TopoDS_Face& aF,
                                                          const TopoDS_Edge& aE,
                                                          const Handle(IntTools_Context)& aContext);

  //! Returns the min and max dimensions of the shape <theS>.
  Standard_EXPORT static void Dimensions (const TopoDS_Shape& theS,
                                          Standard_Integer& theDMin,
                                          Standard_Integer& theDMax);

  //! Returns dimension of the shape <theS>.
  //! If the shape contains elements of different dimension, -1 is returned.
  Standard_EXPORT static Standard_Integer Dimension(const TopoDS_Shape& theS);

  //! Collects in the output list recursively all non-compound sub-shapes of the first level
  //! of the given shape theS. The optional map theMap is used to avoid the duplicates in the
  //! output list, so it will also contain all non-compound sub-shapes.
  Standard_EXPORT static void TreatCompound (const TopoDS_Shape& theS,
                                             TopTools_ListOfShape& theList,
                                             TopTools_MapOfShape* theMap = NULL);

  //! Returns true if the  shell <theShell> is open
  Standard_EXPORT static Standard_Boolean IsOpenShell(const TopoDS_Shell& theShell);

};

#endif // _BOPTools_AlgoTools_HeaderFile
