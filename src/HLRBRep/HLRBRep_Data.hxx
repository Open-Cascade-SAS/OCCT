// Created on: 1993-01-11
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _HLRBRep_Data_HeaderFile
#define _HLRBRep_Data_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <HLRBRep_Array1OfEData.hxx>
#include <HLRBRep_Array1OfFData.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_ShortReal.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_CLProps.hxx>
#include <HLRBRep_SLProps.hxx>
#include <Standard_Real.hxx>
#include <HLRBRep_FaceIterator.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
#include <TopAbs_Orientation.hxx>
#include <HLRBRep_Intersector.hxx>
#include <HLRAlgo_Interference.hxx>
#include <Standard_Transient.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>
#include <TopAbs_State.hxx>
#include <HLRAlgo_InterferenceList.hxx>
class BRepTopAdaptor_TopolTool;
class gp_Dir2d;
class HLRBRep_EdgeData;
class HLRBRep_FaceData;
class IntRes2d_IntersectionPoint;
class TableauRejection;

class HLRBRep_Data;
DEFINE_STANDARD_HANDLE(HLRBRep_Data, Standard_Transient)

class HLRBRep_Data : public Standard_Transient
{

public:
  //! Create an  empty data structure  of <NV> vertices,
  //! <NE> edges and <NF> faces.
  Standard_EXPORT HLRBRep_Data(const Standard_Integer NV,
                               const Standard_Integer NE,
                               const Standard_Integer NF);

  //! Write <DS>    in   me  with   a     translation of
  //! <dv>,<de>,<df>.
  Standard_EXPORT void Write(const Handle(HLRBRep_Data)& DS,
                             const Standard_Integer      dv,
                             const Standard_Integer      de,
                             const Standard_Integer      df);

  HLRBRep_Array1OfEData& EDataArray();

  HLRBRep_Array1OfFData& FDataArray();

  //! Set the  tolerance for the  rejections  during the
  //! exploration
  void Tolerance(const Standard_ShortReal tol);

  //! returns  the tolerance for the  rejections  during
  //! the exploration
  Standard_ShortReal Tolerance() const;

  //! end of building  of the Data and updating
  //! all the information linked to the projection.
  Standard_EXPORT void Update(const HLRAlgo_Projector& P);

  HLRAlgo_Projector& Projector();

  Standard_Integer NbVertices() const;

  Standard_Integer NbEdges() const;

  Standard_Integer NbFaces() const;

  TopTools_IndexedMapOfShape& EdgeMap();

  TopTools_IndexedMapOfShape& FaceMap();

  //! to compare with only non rejected edges.
  Standard_EXPORT void InitBoundSort(const HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxTot,
                                     const Standard_Integer                   e1,
                                     const Standard_Integer                   e2);

  //! Begin an iteration only  on visible Edges
  //! crossing the face number <FI>.
  Standard_EXPORT void InitEdge(const Standard_Integer FI, BRepTopAdaptor_MapOfShapeTool& MST);

  Standard_EXPORT Standard_Boolean MoreEdge();

  Standard_EXPORT void NextEdge(const Standard_Boolean skip = Standard_True);

  //! Returns the  current Edge
  Standard_EXPORT Standard_Integer Edge() const;

  //! Returns true if   the  current edge to   be hidden
  //! belongs to the hiding face.
  Standard_Boolean HidingTheFace() const;

  //! Returns true if the current hiding face is not  an
  //! auto-intersected one.
  Standard_Boolean SimpleHidingFace() const;

  //! Intersect  the current  Edge  with the boundary of
  //! the hiding  face.   The interferences are given by
  //! the More, Next, and Value methods.
  Standard_EXPORT void InitInterference();

  Standard_Boolean MoreInterference() const;

  Standard_EXPORT void NextInterference();

  //! Returns  True if the  interference is rejected.
  Standard_EXPORT Standard_Boolean RejectedInterference();

  //! Returns True if the rejected interference is above
  //! the face.
  Standard_EXPORT Standard_Boolean AboveInterference();

  HLRAlgo_Interference& Interference();

  //! Returns the local description of the projection of
  //! the current LEdge  at parameter  <Param>.
  Standard_EXPORT void LocalLEGeometry2D(const Standard_Real Param,
                                         gp_Dir2d&           Tg,
                                         gp_Dir2d&           Nm,
                                         Standard_Real&      Cu);

  //! Returns the local description of the projection of
  //! the current FEdge  at parameter  <Param>.
  Standard_EXPORT void LocalFEGeometry2D(const Standard_Integer FE,
                                         const Standard_Real    Param,
                                         gp_Dir2d&              Tg,
                                         gp_Dir2d&              Nm,
                                         Standard_Real&         Cu);

  //! Returns the local  3D   state of the  intersection
  //! between the current edge and the current face at the
  //! <p1> and <p2> parameters.
  Standard_EXPORT void EdgeState(const Standard_Real p1,
                                 const Standard_Real p2,
                                 TopAbs_State&       stbef,
                                 TopAbs_State&       staf);

  //! Returns the  true if the  Edge <ED> belongs to the
  //! Hiding Face.
  Standard_Boolean EdgeOfTheHidingFace(const Standard_Integer E, const HLRBRep_EdgeData& ED) const;

  //! Returns the number of  levels of hiding face above
  //! the   first  point  of   the    edge <ED>.     The
  //! InterferenceList is  given to  compute far away of
  //! the Interferences and then come back.
  Standard_EXPORT Standard_Integer HidingStartLevel(const Standard_Integer          E,
                                                    const HLRBRep_EdgeData&         ED,
                                                    const HLRAlgo_InterferenceList& IL);

  //! Returns   the  state   of  the   Edge  <ED>  after
  //! classification.
  Standard_EXPORT TopAbs_State Compare(const Standard_Integer E, const HLRBRep_EdgeData& ED);

  //! Simple classification of part of edge [p1,  p2].
  //! Returns OUT if at least 1 of Nbp points of edge is out; otherwise returns IN.
  //! It is used to check "suspicion" hidden part of edge.
  Standard_EXPORT TopAbs_State SimplClassify(const Standard_Integer  E,
                                             const HLRBRep_EdgeData& ED,
                                             const Standard_Integer  Nbp,
                                             const Standard_Real     p1,
                                             const Standard_Real     p2);

  //! Classification of an edge.
  Standard_EXPORT TopAbs_State Classify(const Standard_Integer  E,
                                        const HLRBRep_EdgeData& ED,
                                        const Standard_Boolean  LevelFlag,
                                        Standard_Integer&       Level,
                                        const Standard_Real     param);

  //! Returns true if the current face is bad.
  Standard_EXPORT Standard_Boolean IsBadFace() const;

  Standard_EXPORT void Destroy();

  ~HLRBRep_Data() { Destroy(); }

  DEFINE_STANDARD_RTTIEXT(HLRBRep_Data, Standard_Transient)

private:
  //! Orient the   OutLines  ( left  must  be  inside in
  //! projection ). Returns True if the face of a closed
  //! shell has been inverted;
  Standard_EXPORT Standard_Boolean OrientOutLine(const Standard_Integer I, HLRBRep_FaceData& FD);

  //! Orient the Edges which  are not  Internal OutLine,
  //! not Double and not IsoLine.
  Standard_EXPORT void OrientOthEdge(const Standard_Integer I, HLRBRep_FaceData& FD);

  //! Returns  True  if the  intersection is  rejected.
  Standard_EXPORT Standard_Boolean RejectedPoint(const IntRes2d_IntersectionPoint& PInter,
                                                 const TopAbs_Orientation          BoundOri,
                                                 const Standard_Integer            NumSeg);

  //! Returns True if there is a common vertex between myLE and myFE depending on <head1> and
  //! <head2>.
  Standard_EXPORT Standard_Boolean SameVertex(const Standard_Boolean head1,
                                              const Standard_Boolean head2);

private:
  Standard_Integer                   myNbVertices;
  Standard_Integer                   myNbEdges;
  Standard_Integer                   myNbFaces;
  TopTools_IndexedMapOfShape         myEMap;
  TopTools_IndexedMapOfShape         myFMap;
  HLRBRep_Array1OfEData              myEData;
  HLRBRep_Array1OfFData              myFData;
  TColStd_Array1OfInteger            myEdgeIndices;
  Standard_ShortReal                 myToler;
  HLRAlgo_Projector                  myProj;
  HLRBRep_CLProps                    myLLProps;
  HLRBRep_CLProps                    myFLProps;
  HLRBRep_SLProps                    mySLProps;
  Standard_Real                      myBigSize;
  HLRBRep_FaceIterator               myFaceItr1;
  HLRBRep_FaceIterator               myFaceItr2;
  Standard_Integer                   iFace;
  HLRBRep_FaceData*                  iFaceData;
  Standard_Address                   iFaceGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices* iFaceMinMax;
  GeomAbs_SurfaceType                iFaceType;
  Standard_Boolean                   iFaceBack;
  Standard_Boolean                   iFaceSimp;
  Standard_Boolean                   iFaceSmpl;
  Standard_Boolean                   iFaceTest;
  Standard_Integer                   myHideCount;
  Standard_Real                      myDeca[16];
  Standard_Real                      mySurD[16];
  Standard_Integer                   myCurSortEd;
  Standard_Integer                   myNbrSortEd;
  Standard_Integer                   myLE;
  Standard_Boolean                   myLEOutLine;
  Standard_Boolean                   myLEInternal;
  Standard_Boolean                   myLEDouble;
  Standard_Boolean                   myLEIsoLine;
  HLRBRep_EdgeData*                  myLEData;
  const HLRBRep_Curve*               myLEGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices* myLEMinMax;
  GeomAbs_CurveType                  myLEType;
  Standard_ShortReal                 myLETol;
  Standard_Integer                   myFE;
  TopAbs_Orientation                 myFEOri;
  Standard_Boolean                   myFEOutLine;
  Standard_Boolean                   myFEInternal;
  Standard_Boolean                   myFEDouble;
  HLRBRep_EdgeData*                  myFEData;
  HLRBRep_Curve*                     myFEGeom;
  GeomAbs_CurveType                  myFEType;
  Standard_ShortReal                 myFETol;
  HLRBRep_Intersector                myIntersector;
  Handle(BRepTopAdaptor_TopolTool)   myClassifier;
  Standard_Boolean                   mySameVertex;
  Standard_Boolean                   myIntersected;
  Standard_Integer                   myNbPoints;
  Standard_Integer                   myNbSegments;
  Standard_Integer                   iInterf;
  HLRAlgo_Interference               myIntf;
  Standard_Boolean                   myAboveIntf;
  TableauRejection*                  myReject;
};

#include <HLRBRep_Data.lxx>

#endif // _HLRBRep_Data_HeaderFile
