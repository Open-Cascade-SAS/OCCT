// Created on: 1994-10-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRep_FacesFiller_HeaderFile
#define _TopOpeBRep_FacesFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopOpeBRep_PFacesIntersector.hxx>
#include <TopOpeBRepDS_PDataStructure.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRep_PLineInter.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
class TopOpeBRepDS_HDataStructure;
class TopOpeBRep_FFDumper;
class TopoDS_Shape;
class TopOpeBRep_FacesIntersector;
class TopOpeBRep_PointClassifier;
class TopOpeBRep_LineInter;
class TopOpeBRep_VPointInter;
class TopOpeBRep_VPointInterClassifier;
class TopOpeBRep_VPointInterIterator;
class TopOpeBRepDS_Transition;
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_Point;
class TopoDS_Face;
class gp_Pnt;
class TopOpeBRepDS_DataStructure;


//! Fills a DataStructure from TopOpeBRepDS with the result
//! of Face/Face instersection described by FacesIntersector from TopOpeBRep.
//! if the faces have same Domain, record it in the DS.
//! else record lines and  points and attach list of interferences
//! to the faces, the lines and the edges.
class TopOpeBRep_FacesFiller 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_FacesFiller();
  
  //! Stores in <DS> the intersections of <S1> and <S2>.
  Standard_EXPORT void Insert (const TopoDS_Shape& F1, const TopoDS_Shape& F2, TopOpeBRep_FacesIntersector& FACINT, const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  Standard_EXPORT void ProcessSectionEdges();
  
  Standard_EXPORT TopOpeBRep_PointClassifier& ChangePointClassifier();
  
  //! return field myPShapeClassifier.
  Standard_EXPORT TopOpeBRepTool_PShapeClassifier PShapeClassifier() const;
  
  //! set field myPShapeClassifier.
  Standard_EXPORT void SetPShapeClassifier (const TopOpeBRepTool_PShapeClassifier& PSC);
  
  Standard_EXPORT void LoadLine (TopOpeBRep_LineInter& L);
  
  Standard_EXPORT Standard_Boolean CheckLine (TopOpeBRep_LineInter& L) const;
  
  //! compute position of VPoints of lines
  Standard_EXPORT void VP_Position (TopOpeBRep_FacesIntersector& FACINT);
  
  //! compute position of VPoints of line L
  Standard_EXPORT void VP_Position (TopOpeBRep_LineInter& L);
  
  //! compute position of VPoints of non-restriction line L.
  Standard_EXPORT void VP_PositionOnL (TopOpeBRep_LineInter& L);
  
  //! compute position of VPoints of restriction line L.
  Standard_EXPORT void VP_PositionOnR (TopOpeBRep_LineInter& L);
  
  //! compute position of VP with current faces,
  //! according to VP.ShapeIndex() .
  Standard_EXPORT void VP_Position (TopOpeBRep_VPointInter& VP, TopOpeBRep_VPointInterClassifier& VPC);
  
  //! Process current intersection line (set by LoadLine)
  Standard_EXPORT void ProcessLine();
  
  Standard_EXPORT void ResetDSC();
  
  //! Process current restriction line, adding restriction edge
  //! and computing face/edge interference.
  Standard_EXPORT void ProcessRLine();
  
  //! VP processing for restriction  line  and line sharing
  //! same domain with  section edges :
  //! - if restriction :
  //! Adds restriction edges as  section edges and compute
  //! face/edge  interference.
  //! - if  same domain :
  //! If line share same domain  with section edges, compute
  //! parts of line IN/IN the two faces, and compute curve/point
  //! interference for VP boundaries.
  Standard_EXPORT void FillLineVPonR();
  
  Standard_EXPORT void FillLine();
  
  //! compute 3d curve, pcurves and face/curve interferences
  //! for current NDSC. Add them to the DS.
  Standard_EXPORT void AddShapesLine();
  
  //! Get map <mapES > of restriction edges having parts IN one
  //! of the 2 faces.
  Standard_EXPORT void GetESL (TopTools_ListOfShape& LES);
  
  //! calling the followings ProcessVPIonR and ProcessVPonR.
  Standard_EXPORT void ProcessVPR (TopOpeBRep_FacesFiller& FF, const TopOpeBRep_VPointInter& VP);
  
  //! processing ProcessVPonR for VPI.
  Standard_EXPORT void ProcessVPIonR (TopOpeBRep_VPointInterIterator& VPI, const TopOpeBRepDS_Transition& trans1, const TopoDS_Shape& F1, const Standard_Integer ShapeIndex);
  
  //! adds <VP>'s   geometric   point (if not   stored)  and
  //! computes (curve or edge)/(point or vertex) interference.
  Standard_EXPORT void ProcessVPonR (const TopOpeBRep_VPointInter& VP, const TopOpeBRepDS_Transition& trans1, const TopoDS_Shape& F1, const Standard_Integer ShapeIndex);
  
  //! VP processing on closing arc.
  Standard_EXPORT void ProcessVPonclosingR (const TopOpeBRep_VPointInter& VP, const TopoDS_Shape& F1, const Standard_Integer ShapeIndex, const TopOpeBRepDS_Transition& transEdge, const TopOpeBRepDS_Kind PVKind, const Standard_Integer PVIndex, const Standard_Boolean EPIfound, const Handle(TopOpeBRepDS_Interference)& IEPI);
  
  //! VP processing on degenerated arc.
  Standard_EXPORT Standard_Boolean ProcessVPondgE (const TopOpeBRep_VPointInter& VP, const Standard_Integer ShapeIndex, TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex, Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI, Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI);
  
  //! processing ProcessVPnotonR for VPI.
  Standard_EXPORT void ProcessVPInotonR (TopOpeBRep_VPointInterIterator& VPI);
  
  //! adds <VP>'s  geometrical point to the DS (if not stored)
  //! and computes curve point interference.
  Standard_EXPORT void ProcessVPnotonR (const TopOpeBRep_VPointInter& VP);
  

  //! Get the geometry of a DS point <DSP>.
  //! Search for it with ScanInterfList (previous method).
  //! if found, set <G> to the geometry of the interference found.
  //! else, add the point <DSP> in the <DS> and set <G> to the
  //! value of the new geometry such created.
  //! returns the value of ScanInterfList().
  Standard_EXPORT Standard_Boolean GetGeometry (TopOpeBRepDS_ListIteratorOfListOfInterference& IT, const TopOpeBRep_VPointInter& VP, Standard_Integer& G, TopOpeBRepDS_Kind& K);
  
  Standard_EXPORT Standard_Integer MakeGeometry (const TopOpeBRep_VPointInter& VP, const Standard_Integer ShapeIndex, TopOpeBRepDS_Kind& K);
  

  //! Add interference <I> to list myDSCIL.
  //! on a given line, at first call, add a new DS curve.
  Standard_EXPORT void StoreCurveInterference (const Handle(TopOpeBRepDS_Interference)& I);
  
  //! search for G = geometry of Point which is identical to <DSP>
  //! among the DS Points created in the CURRENT face/face
  //! intersection ( current Insert() call).
  Standard_EXPORT Standard_Boolean GetFFGeometry (const TopOpeBRepDS_Point& DSP, TopOpeBRepDS_Kind& K, Standard_Integer& G) const;
  
  //! search for G = geometry of Point which is identical to <VP>
  //! among the DS Points created in the CURRENT face/face
  //! intersection ( current Insert() call).
  Standard_EXPORT Standard_Boolean GetFFGeometry (const TopOpeBRep_VPointInter& VP, TopOpeBRepDS_Kind& K, Standard_Integer& G) const;
  
  Standard_EXPORT TopOpeBRep_FacesIntersector& ChangeFacesIntersector();
  
  Standard_EXPORT Handle(TopOpeBRepDS_HDataStructure) HDataStructure();
  
  Standard_EXPORT TopOpeBRepDS_DataStructure& ChangeDataStructure();
  
  Standard_EXPORT const TopoDS_Face& Face (const Standard_Integer I) const;
  
  Standard_EXPORT const TopOpeBRepDS_Transition& FaceFaceTransition (const TopOpeBRep_LineInter& L, const Standard_Integer I) const;
  
  Standard_EXPORT const TopOpeBRepDS_Transition& FaceFaceTransition (const Standard_Integer I) const;
  
  Standard_EXPORT TopOpeBRep_PFacesIntersector PFacesIntersectorDummy() const;
  
  Standard_EXPORT TopOpeBRepDS_PDataStructure PDataStructureDummy() const;
  
  Standard_EXPORT TopOpeBRep_PLineInter PLineInterDummy() const;
  
  Standard_EXPORT void SetTraceIndex (const Standard_Integer exF1, const Standard_Integer exF2);
  
  Standard_EXPORT void GetTraceIndex (Standard_Integer& exF1, Standard_Integer& exF2) const;
  
  //! Computes <pmin> and <pmax> the upper and lower bounds of <L>
  //! enclosing all vpoints.
  Standard_EXPORT static void Lminmax (const TopOpeBRep_LineInter& L, Standard_Real& pmin, Standard_Real& pmax);
  
  //! Returns <True> if <L> shares a same geometric domain with
  //! at least one of the section edges of <ERL>.
  Standard_EXPORT static Standard_Boolean LSameDomainERL (const TopOpeBRep_LineInter& L, const TopTools_ListOfShape& ERL);
  
  //! Computes the transition <T> of the VPoint <iVP> on the edge
  //! of <SI12>. Returns <False> if the status is unknown.
  Standard_EXPORT static Standard_Boolean IsVPtransLok (const TopOpeBRep_LineInter& L, const Standard_Integer iVP, const Standard_Integer SI12, TopOpeBRepDS_Transition& T);
  
  //! Computes   transition   on line for  VP<iVP>   on edge
  //! restriction of <SI>.  If <isINOUT>  :  returns <true> if
  //! transition computed is IN/OUT else : returns <true> if
  //! transition computed is OUT/IN.
  Standard_EXPORT static Standard_Boolean TransvpOK (const TopOpeBRep_LineInter& L, const Standard_Integer iVP, const Standard_Integer SI, const Standard_Boolean isINOUT);
  
  //! Returns parameter u of vp on the restriction edge.
  Standard_EXPORT static Standard_Real VPParamOnER (const TopOpeBRep_VPointInter& vp, const TopOpeBRep_LineInter& Lrest);
  
  Standard_EXPORT static Standard_Boolean EqualpPonR (const TopOpeBRep_LineInter& Lrest, const TopOpeBRep_VPointInter& VP1, const TopOpeBRep_VPointInter& VP2);
  




protected:





private:

  
  Standard_EXPORT Standard_Boolean KeepRLine (const TopOpeBRep_LineInter& Lrest, const Standard_Boolean checkkeep) const;
  
  Standard_EXPORT TopOpeBRepDS_Transition GetEdgeTrans (const TopOpeBRep_VPointInter& VP, const TopOpeBRepDS_Kind PVKind, const Standard_Integer PVIndex, const Standard_Integer Sind, const TopoDS_Face& F);
  
  //! If <VP>'s index != 3, calls a classifier to determine
  //! <VP>'s state on the face.
  Standard_EXPORT TopAbs_State StateVPonFace (const TopOpeBRep_VPointInter& VP) const;
  
  //! <VP> is of geometry <P>.
  //! Looks after a   VPoint on RESTRICTION <Lrest>  with
  //! geometric value  <P>. If true, updates states ON for <VP>.
  Standard_EXPORT Standard_Boolean PequalVPonR (const gp_Pnt& P3D, const Standard_Integer VPshapeindex, TopOpeBRep_VPointInter& VP, TopOpeBRep_LineInter& Lrest) const;
  
  //! Classifies (VPf, VPl) middle point on restriction edge
  Standard_EXPORT TopAbs_State StBipVPonF (const TopOpeBRep_VPointInter& IVPf, const TopOpeBRep_VPointInter& IVPl, const TopOpeBRep_LineInter& Lrest, const Standard_Boolean isonedge1) const;


  TopoDS_Face myF1;
  TopoDS_Face myF2;
  TopAbs_Orientation myF1ori;
  TopAbs_Orientation myF2ori;
  TopOpeBRep_PFacesIntersector myFacesIntersector;
  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopOpeBRepDS_PDataStructure myDS;
  Standard_Integer myFFfirstDSP;
  TopOpeBRep_PLineInter myLine;
  Standard_Boolean myLineOK;
  Standard_Boolean myLineINL;
  TopOpeBRepDS_Transition myLineTonF1;
  TopOpeBRepDS_Transition myLineTonF2;
  Standard_Boolean myLineIsonEdge;
  TopTools_ListOfShape myERL;
  TopTools_DataMapOfShapeListOfShape myDataforDegenEd;
  Standard_Boolean myLastVPison0;
  Standard_Integer mykeptVPnbr;
  Standard_Integer myDSCIndex;
  TopOpeBRepDS_ListOfInterference myDSCIL;
  TopOpeBRep_PointClassifier myPointClassifier;
  TopOpeBRepTool_PShapeClassifier myPShapeClassifier;
  Standard_Integer myexF1;
  Standard_Integer myexF2;
  Handle(TopOpeBRep_FFDumper) myHFFD;


};







#endif // _TopOpeBRep_FacesFiller_HeaderFile
