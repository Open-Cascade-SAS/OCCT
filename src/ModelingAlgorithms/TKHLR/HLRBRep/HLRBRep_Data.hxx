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
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <NCollection_Array1.hxx>
#include <HLRBRep_FaceData.hxx>
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
#include <TopoDS_Shape.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_State.hxx>
#include <NCollection_List.hxx>
class BRepTopAdaptor_TopolTool;
class gp_Dir2d;
class HLRBRep_EdgeData;
class HLRBRep_FaceData;
class HLRBRep_Surface;
class IntRes2d_IntersectionPoint;
class TableauRejection;

class HLRBRep_Data : public Standard_Transient
{

public:
  //! Create an empty data structure of <NV> vertices,
  //! <NE> edges and <NF> faces.
  Standard_EXPORT HLRBRep_Data(const int NV, const int NE, const int NF);

  //! Write <DS> in me with a translation of
  //! <dv>,<de>,<df>.
  Standard_EXPORT void Write(const occ::handle<HLRBRep_Data>& DS,
                             const int                        dv,
                             const int                        de,
                             const int                        df);

  NCollection_Array1<HLRBRep_EdgeData>& EDataArray();

  NCollection_Array1<HLRBRep_FaceData>& FDataArray();

  //! Set the tolerance for the rejections during the
  //! exploration
  void Tolerance(const float tol);

  //! returns the tolerance for the rejections during
  //! the exploration
  float Tolerance() const;

  //! end of building of the Data and updating
  //! all the information linked to the projection.
  Standard_EXPORT void Update(const HLRAlgo_Projector& P);

  HLRAlgo_Projector& Projector();

  int NbVertices() const;

  int NbEdges() const;

  int NbFaces() const;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& EdgeMap();

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& FaceMap();

  //! to compare with only non rejected edges.
  Standard_EXPORT void InitBoundSort(const HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxTot,
                                     const int                                e1,
                                     const int                                e2);

  //! Begin an iteration only on visible Edges
  //! crossing the face number <FI>.
  Standard_EXPORT void InitEdge(
    const int                                                                        FI,
    NCollection_DataMap<TopoDS_Shape, BRepTopAdaptor_Tool, TopTools_ShapeMapHasher>& MST);

  Standard_EXPORT bool MoreEdge();

  Standard_EXPORT void NextEdge(const bool skip = true);

  //! Returns the current Edge
  Standard_EXPORT int Edge() const;

  //! Returns true if the current edge to be hidden
  //! belongs to the hiding face.
  bool HidingTheFace() const;

  //! Returns true if the current hiding face is not an
  //! auto-intersected one.
  bool SimpleHidingFace() const;

  //! Intersect the current Edge with the boundary of
  //! the hiding face. The interferences are given by
  //! the More, Next, and Value methods.
  Standard_EXPORT void InitInterference();

  bool MoreInterference() const;

  Standard_EXPORT void NextInterference();

  //! Returns True if the interference is rejected.
  Standard_EXPORT bool RejectedInterference();

  //! Returns True if the rejected interference is above
  //! the face.
  Standard_EXPORT bool AboveInterference();

  HLRAlgo_Interference& Interference();

  //! Returns the local description of the projection of
  //! the current LEdge at parameter <Param>.
  Standard_EXPORT void LocalLEGeometry2D(const double Param,
                                         gp_Dir2d&    Tg,
                                         gp_Dir2d&    Nm,
                                         double&      Cu);

  //! Returns the local description of the projection of
  //! the current FEdge at parameter <Param>.
  Standard_EXPORT void LocalFEGeometry2D(const int    FE,
                                         const double Param,
                                         gp_Dir2d&    Tg,
                                         gp_Dir2d&    Nm,
                                         double&      Cu);

  //! Returns the local 3D state of the intersection
  //! between the current edge and the current face at the
  //! <p1> and <p2> parameters.
  Standard_EXPORT void EdgeState(const double  p1,
                                 const double  p2,
                                 TopAbs_State& stbef,
                                 TopAbs_State& staf);

  //! Returns the true if the Edge <ED> belongs to the
  //! Hiding Face.
  bool EdgeOfTheHidingFace(const int E, const HLRBRep_EdgeData& ED) const;

  //! Returns the number of levels of hiding face above
  //! the first point of the edge <ED>. The
  //! InterferenceList is given to compute far away of
  //! the Interferences and then come back.
  Standard_EXPORT int HidingStartLevel(const int                                     E,
                                       const HLRBRep_EdgeData&                       ED,
                                       const NCollection_List<HLRAlgo_Interference>& IL);

  //! Returns the state of the Edge <ED> after
  //! classification.
  Standard_EXPORT TopAbs_State Compare(const int E, const HLRBRep_EdgeData& ED);

  //! Simple classification of part of edge [p1, p2].
  //! Returns OUT if at least 1 of Nbp points of edge is out; otherwise returns IN.
  //! It is used to check "suspicion" hidden part of edge.
  Standard_EXPORT TopAbs_State SimplClassify(const int               E,
                                             const HLRBRep_EdgeData& ED,
                                             const int               Nbp,
                                             const double            p1,
                                             const double            p2);

  //! Classification of an edge.
  Standard_EXPORT TopAbs_State Classify(const int               E,
                                        const HLRBRep_EdgeData& ED,
                                        const bool              LevelFlag,
                                        int&                    Level,
                                        const double            param);

  //! Returns true if the current face is bad.
  Standard_EXPORT bool IsBadFace() const;

  Standard_EXPORT void Destroy();

  ~HLRBRep_Data() { Destroy(); }

  DEFINE_STANDARD_RTTIEXT(HLRBRep_Data, Standard_Transient)

private:
  //! Orient the OutLines (left must be inside projection).
  //! Returns True if the face of a closed shell has been
  //! inverted.
  Standard_EXPORT bool OrientOutLine(const int I, HLRBRep_FaceData& FD);

  //! Orient the Edges which are not Internal OutLine,
  //! not Double and not IsoLine.
  Standard_EXPORT void OrientOthEdge(const int I, HLRBRep_FaceData& FD);

  //! Returns True if the intersection is rejected.
  Standard_EXPORT bool RejectedPoint(const IntRes2d_IntersectionPoint& PInter,
                                     const TopAbs_Orientation          BoundOri,
                                     const int                         NumSeg);

  //! Returns True if there is a common vertex between
  //! myLE and myFE depending on <head1> and <head2>.
  Standard_EXPORT bool SameVertex(const bool head1, const bool head2);

private:
  int                                                           myNbVertices;
  int                                                           myNbEdges;
  int                                                           myNbFaces;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myEMap;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myFMap;
  NCollection_Array1<HLRBRep_EdgeData>                          myEData;
  NCollection_Array1<HLRBRep_FaceData>                          myFData;
  NCollection_Array1<int>                                       myEdgeIndices;
  float                                                         myToler;
  HLRAlgo_Projector                                             myProj;
  HLRBRep_CLProps                                               myLLProps;
  HLRBRep_CLProps                                               myFLProps;
  HLRBRep_SLProps                                               mySLProps;
  double                                                        myBigSize;
  HLRBRep_FaceIterator                                          myFaceItr1;
  HLRBRep_FaceIterator                                          myFaceItr2;
  int                                                           iFace;
  HLRBRep_FaceData*                                             iFaceData;
  HLRBRep_Surface*                                              iFaceGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices*                            iFaceMinMax;
  GeomAbs_SurfaceType                                           iFaceType;
  bool                                                          iFaceBack;
  bool                                                          iFaceSimp;
  bool                                                          iFaceSmpl;
  bool                                                          iFaceTest;
  int                                                           myHideCount;
  double                                                        myDeca[16];
  double                                                        mySurD[16];
  int                                                           myCurSortEd;
  int                                                           myNbrSortEd;
  int                                                           myLE;
  bool                                                          myLEOutLine;
  bool                                                          myLEInternal;
  bool                                                          myLEDouble;
  bool                                                          myLEIsoLine;
  HLRBRep_EdgeData*                                             myLEData;
  const HLRBRep_Curve*                                          myLEGeom;
  HLRAlgo_EdgesBlock::MinMaxIndices*                            myLEMinMax;
  GeomAbs_CurveType                                             myLEType;
  float                                                         myLETol;
  int                                                           myFE;
  TopAbs_Orientation                                            myFEOri;
  bool                                                          myFEOutLine;
  bool                                                          myFEInternal;
  bool                                                          myFEDouble;
  HLRBRep_EdgeData*                                             myFEData;
  HLRBRep_Curve*                                                myFEGeom;
  GeomAbs_CurveType                                             myFEType;
  float                                                         myFETol;
  HLRBRep_Intersector                                           myIntersector;
  occ::handle<BRepTopAdaptor_TopolTool>                         myClassifier;
  bool                                                          mySameVertex;
  bool                                                          myIntersected;
  int                                                           myNbPoints;
  int                                                           myNbSegments;
  int                                                           iInterf;
  HLRAlgo_Interference                                          myIntf;
  bool                                                          myAboveIntf;
  TableauRejection*                                             myReject;
};

#include <HLRBRep_Data.lxx>

#endif // _HLRBRep_Data_HeaderFile
