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

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BOPTools_ListOfCoupleOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_BaseAllocator.hxx>
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


class BOPTools_AlgoTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer ComputeVV (const TopoDS_Vertex& aV1, const gp_Pnt& aP2, const Standard_Real aTolP2);
  
  Standard_EXPORT static Standard_Integer ComputeVV (const TopoDS_Vertex& aV1, 
                                                     const TopoDS_Vertex& aV2, 
                                                     const Standard_Real theFuzz = Precision::Confusion());
  
  Standard_EXPORT static void MakeVertex (const BOPCol_ListOfShape& aLV, TopoDS_Vertex& aV);
  
  Standard_EXPORT static void MakeEdge (const IntTools_Curve& theCurve, const TopoDS_Vertex& theV1, const Standard_Real theT1, const TopoDS_Vertex& theV2, const Standard_Real theT2, const Standard_Real theTolR3D, TopoDS_Edge& theE);
  
  //! Makes 2d curve of the edge <theE> on the faces <theF1> and <theF2>.<br>
  //! <theContext> - storage for caching the geometrical tools
  Standard_EXPORT static void MakePCurve (const TopoDS_Edge& theE,
                                          const TopoDS_Face& theF1,
                                          const TopoDS_Face& theF2,
                                          const IntTools_Curve& theCurve,
                                          const Standard_Boolean thePC1,
                                          const Standard_Boolean thePC2,
                                          const Handle(IntTools_Context)& theContext = Handle(IntTools_Context)());
  
  Standard_EXPORT static void MakeContainer (const TopAbs_ShapeEnum theType, TopoDS_Shape& theShape);
  
  Standard_EXPORT static Standard_Boolean IsHole (const TopoDS_Shape& aW, const TopoDS_Shape& aF);
  
  //! Returns True if the shape theSplit has opposite
  //! direction than theShape
  //! theContext - cashed geometrical tools
  Standard_EXPORT static Standard_Boolean IsSplitToReverse (const TopoDS_Shape& theSplit, const TopoDS_Shape& theShape, Handle(IntTools_Context)& theContext);
  
  //! Returns True if normal direction of the face
  //! theShape is not the same as for the face
  //! theSplit
  //! theContext - cashed geometrical tools
  Standard_EXPORT static Standard_Boolean IsSplitToReverse (const TopoDS_Face& theSplit, const TopoDS_Face& theShape, Handle(IntTools_Context)& theContext);
  
  Standard_EXPORT static Standard_Boolean IsSplitToReverse (const TopoDS_Edge& aE1, const TopoDS_Edge& aE2, Handle(IntTools_Context)& aContext);
  
  Standard_EXPORT static Standard_Boolean AreFacesSameDomain (const TopoDS_Face& theF1,
                                         const TopoDS_Face& theF2, 
                                         Handle(IntTools_Context)& theContext,
                                         const Standard_Real theFuzz = Precision::Confusion());
  
  Standard_EXPORT static Standard_Boolean CheckSameGeom (const TopoDS_Face& theF1, const TopoDS_Face& theF2, Handle(IntTools_Context)& theContext);
  
  //! Basing on the normals directions of the faces the method
  //! Defines whether to reverse the second face or not.<br>
  //! <theContext> - storage for caching the geometrical tools
  Standard_EXPORT static Standard_Integer Sense (const TopoDS_Face& theF1,
                                                 const TopoDS_Face& theF2,
                                                 const Handle(IntTools_Context)& theContext = Handle(IntTools_Context)());
  
  //! Returns True if the face theFace contains
  //! the edge theEdge but with opposite orientation.
  //! If the method  returns True theEdgeOff is the
  //! edge founded
  Standard_EXPORT static Standard_Boolean GetEdgeOff (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace, TopoDS_Edge& theEdgeOff);
  
  //! For the face theFace and its edge theEdge
  //! finds the face suitable to produce shell.
  //! theLCEF - set of faces to search. All faces
  //! from theLCEF must share edge theEdge
  Standard_EXPORT static Standard_Boolean GetFaceOff (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace, BOPTools_ListOfCoupleOfShape& theLCEF, TopoDS_Face& theFaceOff, Handle(IntTools_Context)& theContext);
  
  //! Returns True if the face theFace is inside of the
  //! couple of faces theFace1, theFace2.
  //! The faces theFace, theFace1, theFace2  must
  //! share the edge theEdge
  //! Return values:
  //!  * 0 state is not IN
  //!  * 1 state is IN
  //!  * 2 state can not be found by the method of angles
  Standard_EXPORT static Standard_Integer IsInternalFace (const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, const TopoDS_Face& theFace1, const TopoDS_Face& theFace2, Handle(IntTools_Context)& theContext);
  
  //! Returns True if the face theFace is inside of the
  //! appropriate couple of faces (from the set theLF)    .
  //! The faces of the set theLF and theFace  must
  //! share the edge theEdge
  //!  * 0 state is not IN
  //!  * 1 state is IN
  //!  * 2 state can not be found by the method of angles
  Standard_EXPORT static Standard_Integer IsInternalFace (const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, BOPCol_ListOfShape& theLF, Handle(IntTools_Context)& theContext);
  
  //! Returns True if the face theFace is inside the
  //! solid theSolid.
  //! theMEF - Map Edge/Faces for theSolid
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  Standard_EXPORT static Standard_Boolean IsInternalFace (const TopoDS_Face& theFace, const TopoDS_Solid& theSolid, BOPCol_IndexedDataMapOfShapeListOfShape& theMEF, const Standard_Real theTol, Handle(IntTools_Context)& theContext);
  
  //! For the face theFace gets the edge theEdgeOnF
  //! that is the same as theEdge
  //! Returns True if such edge exists
  //! Returns False if there is no such edge
  Standard_EXPORT static Standard_Boolean GetEdgeOnFace (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace, TopoDS_Edge& theEdgeOnF);
  
  //! Computes the 3-D state of the point thePoint
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState (const gp_Pnt& thePoint, const TopoDS_Solid& theSolid, const Standard_Real theTol, Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the vertex theVertex
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState (const TopoDS_Vertex& theVertex, const TopoDS_Solid& theSolid, const Standard_Real theTol, Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the edge theEdge
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState (const TopoDS_Edge& theEdge, const TopoDS_Solid& theSolid, const Standard_Real theTol, Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the face theFace
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theBounds - set of edges of theFace to avoid
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeState (const TopoDS_Face& theFace, const TopoDS_Solid& theSolid, const Standard_Real theTol, BOPCol_IndexedMapOfShape& theBounds, Handle(IntTools_Context)& theContext);
  
  //! Computes the 3-D state of the shape theShape
  //! toward solid theSolid.
  //! theTol - value of precision of computation
  //! theContext- cahed geometrical tools
  //! Returns 3-D state.
  Standard_EXPORT static TopAbs_State ComputeStateByOnePoint (const TopoDS_Shape& theShape, const TopoDS_Solid& theSolid, const Standard_Real theTol, Handle(IntTools_Context)& theContext);
  
  //! For the list of faces theLS build block
  //! theLSCB in terms of connexity by edges
  //! theMapAvoid - set of edges to avoid for
  //! the treatment
  Standard_EXPORT static void MakeConnexityBlock (BOPCol_ListOfShape& theLS, BOPCol_IndexedMapOfShape& theMapAvoid, BOPCol_ListOfShape& theLSCB, const BOPCol_BaseAllocator& theAllocator);
  
  //! For the compound theS build the blocks
  //! theLCB (as list of compounds)
  //! in terms of connexity by the shapes of theType
  Standard_EXPORT static void MakeConnexityBlocks (const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType1, const TopAbs_ShapeEnum theType2, BOPCol_ListOfShape& theLCB);

  //! Correctly orients edges on the wire
  Standard_EXPORT static void OrientEdgesOnWire (TopoDS_Shape& theWire);

  //! Correctly orients faces on the shell
  Standard_EXPORT static void OrientFacesOnShell (TopoDS_Shape& theShell);
  

  //! Provides valid values of tolerances for the shape <theS>
  //! <theTolMax> is max value of the tolerance that can be
  //! accepted for correction.  If real value of the tolerance
  //! will be greater than  <aTolMax>, the correction does not
  //! perform.
  Standard_EXPORT static void CorrectTolerances
              (const TopoDS_Shape& theS, 
               const BOPCol_IndexedMapOfShape& theMapToAvoid,
               const Standard_Real theTolMax = 0.0001,
               const Standard_Boolean theRunParallel = Standard_False);

  //! Provides valid values of tolerances for the shape <theS>
  //! in  terms of BRepCheck_InvalidCurveOnSurface.
  Standard_EXPORT static void CorrectCurveOnSurface
              (const TopoDS_Shape& theS,
               const BOPCol_IndexedMapOfShape& theMapToAvoid,
               const Standard_Real theTolMax = 0.0001,
               const Standard_Boolean theRunParallel = Standard_False);

  //! Provides valid values of tolerances for the shape <theS>
  //! in  terms of BRepCheck_InvalidPointOnCurve.
  Standard_EXPORT static void CorrectPointOnCurve
              (const TopoDS_Shape& theS,
               const BOPCol_IndexedMapOfShape& theMapToAvoid,
               const Standard_Real theTolMax = 0.0001,
               const Standard_Boolean theRunParallel = Standard_False);

  //! Make a vertex using 3D-point <aP1> and 3D-tolerance value <aTol>
  Standard_EXPORT static void MakeNewVertex (const gp_Pnt& aP1, const Standard_Real aTol, TopoDS_Vertex& aNewVertex);
  

  //! Make a vertex using couple of vertices  <aV1, aV2>
  Standard_EXPORT static void MakeNewVertex (const TopoDS_Vertex& aV1, const TopoDS_Vertex& aV2, TopoDS_Vertex& aNewVertex);
  

  //! Make a vertex in place of intersection between two edges
  //! <aE1, aE2> with parameters <aP1, aP2>
  Standard_EXPORT static void MakeNewVertex (const TopoDS_Edge& aE1, const Standard_Real aP1, const TopoDS_Edge& aE2, const Standard_Real aP2, TopoDS_Vertex& aNewVertex);
  

  //! Make a vertex in place of intersection between the edge <aE1>
  //! with parameter <aP1> and the face <aF2>
  Standard_EXPORT static void MakeNewVertex (const TopoDS_Edge& aE1, const Standard_Real aP1, const TopoDS_Face& aF2, TopoDS_Vertex& aNewVertex);
  

  //! Compute a 3D-point on the edge <aEdge> at parameter <aPrm>
  Standard_EXPORT static void PointOnEdge (const TopoDS_Edge& aEdge, const Standard_Real aPrm, gp_Pnt& aP);
  

  //! Make the edge from base edge <aE1> and two vertices <aV1,aV2>
  //! at parameters <aP1,aP2>
  Standard_EXPORT static void MakeSplitEdge (const TopoDS_Edge& aE1, const TopoDS_Vertex& aV1, const Standard_Real aP1, const TopoDS_Vertex& aV2, const Standard_Real aP2, TopoDS_Edge& aNewEdge);
  

  //! Make the edge from 3D-Curve <aIC>  and two vertices <aV1,aV2>
  //! at parameters <aP1,aP2>
  Standard_EXPORT static void MakeSectEdge (const IntTools_Curve& aIC, const TopoDS_Vertex& aV1, const Standard_Real aP1, const TopoDS_Vertex& aV2, const Standard_Real aP2, TopoDS_Edge& aNewEdge);
  

  //! Update the tolerance value for vertex  <aV>
  //! taking into account the fact that <aV> lays on
  //! the curve <aIC>
  Standard_EXPORT static void UpdateVertex (const IntTools_Curve& aIC, const Standard_Real aT, const TopoDS_Vertex& aV);
  

  //! Update the tolerance value for vertex  <aV>
  //! taking into account the fact that <aV> lays on
  //! the edge <aE>
  Standard_EXPORT static void UpdateVertex (const TopoDS_Edge& aE, const Standard_Real aT, const TopoDS_Vertex& aV);
  

  //! Update the tolerance value for vertex  <aVN>
  //! taking into account the fact that <aVN> should
  //! cover tolerance zone of <aVF>
  Standard_EXPORT static void UpdateVertex (const TopoDS_Vertex& aVF, const TopoDS_Vertex& aVN);
  

  //! Correct shrunk range <aSR> taking into account 3D-curve
  //! resolution and corresp. tolerances' values of <aE1>, <aE2>
  Standard_EXPORT static void CorrectRange (const TopoDS_Edge& aE1, const TopoDS_Edge& aE2, const IntTools_Range& aSR, IntTools_Range& aNewSR);
  

  //! Correct shrunk range <aSR> taking into account 3D-curve
  //! resolution and corresp. tolerances' values of <aE>, <aF>
  Standard_EXPORT static void CorrectRange (const TopoDS_Edge& aE, const TopoDS_Face& aF, const IntTools_Range& aSR, IntTools_Range& aNewSR);
  

  //! Returns TRUE if PaveBlock <aPB> lays on the face <aF>, i.e
  //! the <PB> is IN or ON in 2D of <aF>
  Standard_EXPORT static Standard_Boolean IsBlockInOnFace (const IntTools_Range& aShR, const TopoDS_Face& aF, const TopoDS_Edge& aE, Handle(IntTools_Context)& aContext);
  

  //! Checks if it is possible to compute shrunk range for the edge <aE>
  //! Flag <theCheckSplittable> defines whether to take into account 
  //! the possiblity to split the edge or not.
  Standard_EXPORT static Standard_Boolean IsMicroEdge (const TopoDS_Edge& theEdge,
                                                       const Handle(IntTools_Context)& theContext,
                                                       const Standard_Boolean theCheckSplittable = Standard_True);
  

  //! Corrects tolerance values of the sub-shapes of the shape <theS> if needed.
  Standard_EXPORT static void CorrectShapeTolerances
              (const TopoDS_Shape& theS,
               const BOPCol_IndexedMapOfShape& theMapToAvoid,
               const Standard_Boolean theRunParallel = Standard_False);

  //! Retutns dimension of the shape <theS>.
  Standard_EXPORT static Standard_Integer Dimension (const TopoDS_Shape& theS);
  
  //! Returns true if the  shell <theShell> is open
  Standard_EXPORT static Standard_Boolean IsOpenShell (const TopoDS_Shell& theShell);
  
  //! Returns true if the solid <theSolid> is inverted
  Standard_EXPORT static Standard_Boolean IsInvertedSolid (const TopoDS_Solid& theSolid);
  

  //! Computes the necessary value of the tolerance for the edge
  Standard_EXPORT static Standard_Boolean ComputeTolerance (const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, Standard_Real& theMaxDist, Standard_Real& theMaxPar);


protected:


private:

};

#endif // _BOPTools_AlgoTools_HeaderFile
