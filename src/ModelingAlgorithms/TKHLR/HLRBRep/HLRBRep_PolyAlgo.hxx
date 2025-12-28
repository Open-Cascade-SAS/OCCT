// Created on: 1992-02-18
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_PolyAlgo_HeaderFile
#define _HLRBRep_PolyAlgo_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <HLRAlgo_TriangleData.hxx>
#include <NCollection_Array1.hxx>
#include <HLRAlgo_PolyInternalSegment.hxx>
#include <NCollection_Array1.hxx>
#include <HLRAlgo_PolyInternalNode.hxx>
#include <NCollection_Array1.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <NCollection_List.hxx>
#include <HLRAlgo_PolyAlgo.hxx>
#include <HLRAlgo_PolyInternalNode.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>

class Geom_Surface;
class TopoDS_Edge;
class HLRAlgo_PolyInternalData;
class HLRAlgo_EdgeStatus;
struct HLRAlgo_TriangleData;

//! to remove Hidden lines on Shapes with Triangulations.
//! A framework to compute the shape as seen in
//! a projection plane. This is done by calculating
//! the visible and the hidden parts of the shape.
//! HLRBRep_PolyAlgo works with three types of entity:
//! -   shapes to be visualized (these shapes must
//! have already been triangulated.)
//! -   edges in these shapes (these edges are
//! defined as polygonal lines on the
//! triangulation of the shape, and are the basic
//! entities which will be visualized or hidden), and
//! -   triangles in these shapes which hide the edges.
//! HLRBRep_PolyAlgo is based on the principle
//! of comparing each edge of the shape to be
//! visualized with each of the triangles produced
//! by the triangulation of the shape, and
//! calculating the visible and the hidden parts of each edge.
//! For a given projection, HLRBRep_PolyAlgo
//! calculates a set of lines characteristic of the
//! object being represented. It is also used in
//! conjunction with the HLRBRep_PolyHLRToShape extraction
//! utilities, which reconstruct a new, simplified
//! shape from a selection of calculation results.
//! This new shape is made up of edges, which
//! represent the shape visualized in the projection.
//! HLRBRep_PolyAlgo works with a polyhedral
//! simplification of the shape whereas
//! HLRBRep_Algo takes the shape itself into
//! account. When you use HLRBRep_Algo, you
//! obtain an exact result, whereas, when you use
//! HLRBRep_PolyAlgo, you reduce computation
//! time but obtain polygonal segments.
//! An HLRBRep_PolyAlgo object provides a framework for:
//! -   defining the point of view
//! -   identifying the shape or shapes to be visualized
//! -   calculating the outlines
//! -   calculating the visible and hidden lines of the shape.
//! Warning
//! -   Superimposed lines are not eliminated by this algorithm.
//! -   There must be no unfinished objects inside the shape you wish to visualize.
//! -   Points are not treated.
//! -   Note that this is not the sort of algorithm
//! used in generating shading, which calculates
//! the visible and hidden parts of each face in a
//! shape to be visualized by comparing each
//! face in the shape with every other face in the same shape.
class HLRBRep_PolyAlgo : public Standard_Transient
{

public:
  //! Constructs an empty framework for the
  //! calculation of the visible and hidden lines of a shape in a projection.
  //! Use the functions:
  //! -   Projector to define the point of view
  //! -   Load to select the shape or shapes to be visualized
  //! -   Update to compute the visible and hidden lines of the shape.
  //! Warning
  //! The shape or shapes to be visualized must have already been triangulated.
  Standard_EXPORT HLRBRep_PolyAlgo();

  Standard_EXPORT HLRBRep_PolyAlgo(const occ::handle<HLRBRep_PolyAlgo>& A);

  Standard_EXPORT HLRBRep_PolyAlgo(const TopoDS_Shape& S);

  int NbShapes() const { return myShapes.Length(); }

  Standard_EXPORT TopoDS_Shape& Shape(const int I);

  //! remove the Shape of Index <I>.
  Standard_EXPORT void Remove(const int I);

  //! return the index of the Shape <S> and return 0 if
  //! the Shape <S> is not found.
  Standard_EXPORT int Index(const TopoDS_Shape& S) const;

  //! Loads the shape S into this framework.
  //! Warning S must have already been triangulated.
  void Load(const TopoDS_Shape& theShape) { myShapes.Append(theShape); }

  const occ::handle<HLRAlgo_PolyAlgo>& Algo() const { return myAlgo; }

  //! Sets the parameters of the view for this framework.
  //! These parameters are defined by an HLRAlgo_Projector object,
  //! which is returned by the Projector function on a Prs3d_Projector object.
  const HLRAlgo_Projector& Projector() const { return myProj; }

  void Projector(const HLRAlgo_Projector& theProj) { myProj = theProj; }

  double TolAngular() const { return myTolAngular; }

  void TolAngular(const double theTol) { myTolAngular = theTol; }

  double TolCoef() const { return myTolSta; }

  void TolCoef(const double theTol)
  {
    myTolSta = theTol;
    myTolEnd = 1.0 - theTol;
  }

  //! Launches calculation of outlines of the shape
  //! visualized by this framework. Used after setting the point of view and
  //! defining the shape or shapes to be visualized.
  Standard_EXPORT void Update();

  void InitHide() { myAlgo->InitHide(); }

  bool MoreHide() const { return myAlgo->MoreHide(); }

  void NextHide() { myAlgo->NextHide(); }

  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Hide(HLRAlgo_EdgeStatus& status,
                                                 TopoDS_Shape&       S,
                                                 bool&   reg1,
                                                 bool&   regn,
                                                 bool&   outl,
                                                 bool&   intl);

  void InitShow() { myAlgo->InitShow(); }

  bool MoreShow() const { return myAlgo->MoreShow(); }

  void NextShow() { myAlgo->NextShow(); }

  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Show(TopoDS_Shape&     S,
                                                 bool& reg1,
                                                 bool& regn,
                                                 bool& outl,
                                                 bool& intl);

  //! Make a shape with the internal outlines in each
  //! face.
  Standard_EXPORT TopoDS_Shape OutLinedShape(const TopoDS_Shape& S) const;

  bool Debug() const { return myDebug; }

  void Debug(const bool theDebug) { myDebug = theDebug; }

  DEFINE_STANDARD_RTTIEXT(HLRBRep_PolyAlgo, Standard_Transient)

private:
  Standard_EXPORT TopoDS_Shape MakeShape() const;

  Standard_EXPORT int InitShape(const TopoDS_Shape& Shape,
                                             bool&   IsoledF,
                                             bool&   IsoledE);

  Standard_EXPORT void StoreShell(const TopoDS_Shape&                                   theShape,
                                  int&                                     theIShell,
                                  NCollection_Array1<occ::handle<HLRAlgo_PolyShellData>>&    theShell,
                                  const bool                                theIsoledF,
                                  const bool                                theIsoledE,
                                  NCollection_Array1<int>&                              theES,
                                  NCollection_Array1<occ::handle<HLRAlgo_PolyData>>&         thePD,
                                  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID,
                                  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theShapeMap1,
                                  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theShapeMap2);

  Standard_EXPORT bool Normal(const int                 theINode,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNodIndices,
                                          HLRAlgo_PolyInternalNode::NodeData&    theNod1RValues,
                                          NCollection_Array1<HLRAlgo_TriangleData>&                 theTData,
                                          NCollection_Array1<HLRAlgo_PolyInternalSegment>&                 thePISeg,
                                          NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&                 thePINod,
                                          const bool                 orient) const;

  Standard_EXPORT bool
    AverageNormal(const int                 theINode,
                  HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                  NCollection_Array1<HLRAlgo_TriangleData>&                 theTData,
                  NCollection_Array1<HLRAlgo_PolyInternalSegment>&                 thePISeg,
                  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&                 thePINod,
                  double&                         theX,
                  double&                         theY,
                  double&                         theZ) const;

  bool AverageNormal(const int                 theINode,
                                 HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                 NCollection_Array1<HLRAlgo_TriangleData>&                 theTData,
                                 NCollection_Array1<HLRAlgo_PolyInternalSegment>&                 thePISeg,
                                 NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&                 thePINod,
                                 gp_XYZ&                                theNormal) const
  {
    return AverageNormal(theINode,
                         theNodeIndices,
                         theTData,
                         thePISeg,
                         thePINod,
                         theNormal.ChangeCoord(1),
                         theNormal.ChangeCoord(2),
                         theNormal.ChangeCoord(3));
  }

  Standard_EXPORT void AddNormalOnTriangle(const int theITri,
                                           const int theINode,
                                           int&      theJNode,
                                           NCollection_Array1<HLRAlgo_TriangleData>& theTData,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>& thePINod,
                                           double&         theX,
                                           double&         theY,
                                           double&         theZ,
                                           bool&      theOK) const;

  Standard_EXPORT void InitBiPointsWithConnexity(
    const int                                theIEdge,
    TopoDS_Edge&                                          theEdge,
    NCollection_List<HLRAlgo_BiPoint>&                                 theList,
    NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID,
    NCollection_List<TopoDS_Shape>&                                 theLS,
    const bool                                theIsConnex);

  Standard_EXPORT void Interpolation(NCollection_List<HLRAlgo_BiPoint>&                   List,
                                     double&                          X1,
                                     double&                          Y1,
                                     double&                          Z1,
                                     double&                          X2,
                                     double&                          Y2,
                                     double&                          Z2,
                                     double&                          XTI1,
                                     double&                          YTI1,
                                     double&                          ZTI1,
                                     double&                          XTI2,
                                     double&                          YTI2,
                                     double&                          ZTI2,
                                     const int                  e,
                                     double&                          U1,
                                     double&                          U2,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  theNodeIndices1,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod11RValues,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  theNodeIndices2,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod12RValues,
                                     const int                  i1p1,
                                     const int                  i1p2,
                                     const int                  i1,
                                     const occ::handle<HLRAlgo_PolyInternalData>& pid1,
                                     NCollection_Array1<HLRAlgo_TriangleData>*&                 TData1,
                                     NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg1,
                                     NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod1) const;

  Standard_EXPORT void Interpolation(NCollection_List<HLRAlgo_BiPoint>&                   List,
                                     double&                          X1,
                                     double&                          Y1,
                                     double&                          Z1,
                                     double&                          X2,
                                     double&                          Y2,
                                     double&                          Z2,
                                     double&                          XTI1,
                                     double&                          YTI1,
                                     double&                          ZTI1,
                                     double&                          XTI2,
                                     double&                          YTI2,
                                     double&                          ZTI2,
                                     const int                  e,
                                     double&                          U1,
                                     double&                          U2,
                                     const GeomAbs_Shape                     rg,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  Nod11Indices,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod11RValues,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  Nod12Indices,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod12RValues,
                                     const int                  i1p1,
                                     const int                  i1p2,
                                     const int                  i1,
                                     const occ::handle<HLRAlgo_PolyInternalData>& pid1,
                                     NCollection_Array1<HLRAlgo_TriangleData>*&                 TData1,
                                     NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg1,
                                     NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod1,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  Nod21Indices,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod21RValues,
                                     HLRAlgo_PolyInternalNode::NodeIndices&  Nod22Indices,
                                     HLRAlgo_PolyInternalNode::NodeData&     Nod22RValues,
                                     const int                  i2p1,
                                     const int                  i2p2,
                                     const int                  i2,
                                     const occ::handle<HLRAlgo_PolyInternalData>& pid2,
                                     NCollection_Array1<HLRAlgo_TriangleData>*&                 TData2,
                                     NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg2,
                                     NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod2) const;

  Standard_EXPORT bool Interpolation(const double                 U1,
                                                 const double                 U2,
                                                 HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
                                                 HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
                                                 double&                      X3,
                                                 double&                      Y3,
                                                 double&                      Z3,
                                                 double&                      XT3,
                                                 double&                      YT3,
                                                 double&                      ZT3,
                                                 double&                      coef3,
                                                 double&                      U3,
                                                 bool&                   mP3P1) const;

  Standard_EXPORT void MoveOrInsertPoint(NCollection_List<HLRAlgo_BiPoint>&                   List,
                                         double&                          X1,
                                         double&                          Y1,
                                         double&                          Z1,
                                         double&                          X2,
                                         double&                          Y2,
                                         double&                          Z2,
                                         double&                          XTI1,
                                         double&                          YTI1,
                                         double&                          ZTI1,
                                         double&                          XTI2,
                                         double&                          YTI2,
                                         double&                          ZTI2,
                                         const int                  e,
                                         double&                          U1,
                                         double&                          U2,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod11Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod11RValues,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod12Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod12RValues,
                                         const int                  i1p1,
                                         const int                  i1p2,
                                         const int                  i1,
                                         const occ::handle<HLRAlgo_PolyInternalData>& pid1,
                                         NCollection_Array1<HLRAlgo_TriangleData>*&                 TData1,
                                         NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg1,
                                         NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod1,
                                         const double                     X3,
                                         const double                     Y3,
                                         const double                     Z3,
                                         const double                     XT3,
                                         const double                     YT3,
                                         const double                     ZT3,
                                         const double                     coef3,
                                         const double                     U3,
                                         const bool                  insP3,
                                         const bool                  mP3P1,
                                         const int                  flag) const;

  Standard_EXPORT void MoveOrInsertPoint(NCollection_List<HLRAlgo_BiPoint>&                   List,
                                         double&                          X1,
                                         double&                          Y1,
                                         double&                          Z1,
                                         double&                          X2,
                                         double&                          Y2,
                                         double&                          Z2,
                                         double&                          XTI1,
                                         double&                          YTI1,
                                         double&                          ZTI1,
                                         double&                          XTI2,
                                         double&                          YTI2,
                                         double&                          ZTI2,
                                         const int                  e,
                                         double&                          U1,
                                         double&                          U2,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod11Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod11RValues,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod12Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod12RValues,
                                         const int                  i1p1,
                                         const int                  i1p2,
                                         const int                  i1,
                                         const occ::handle<HLRAlgo_PolyInternalData>& pid1,
                                         NCollection_Array1<HLRAlgo_TriangleData>*&                 TData1,
                                         NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg1,
                                         NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod1,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod21Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod21RValues,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod22Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod22RValues,
                                         const int                  i2p1,
                                         const int                  i2p2,
                                         const int                  i2,
                                         const occ::handle<HLRAlgo_PolyInternalData>& pid2,
                                         NCollection_Array1<HLRAlgo_TriangleData>*&                 TData2,
                                         NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg2,
                                         NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod2,
                                         const double                     X3,
                                         const double                     Y3,
                                         const double                     Z3,
                                         const double                     XT3,
                                         const double                     YT3,
                                         const double                     ZT3,
                                         const double                     coef3,
                                         const double                     U3,
                                         const bool                  insP3,
                                         const bool                  mP3P1,
                                         const int                  flag) const;

  Standard_EXPORT void MoveOrInsertPoint(NCollection_List<HLRAlgo_BiPoint>&                   List,
                                         double&                          X1,
                                         double&                          Y1,
                                         double&                          Z1,
                                         double&                          X2,
                                         double&                          Y2,
                                         double&                          Z2,
                                         double&                          XTI1,
                                         double&                          YTI1,
                                         double&                          ZTI1,
                                         double&                          XTI2,
                                         double&                          YTI2,
                                         double&                          ZTI2,
                                         const int                  e,
                                         double&                          U1,
                                         double&                          U2,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod11Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod11RValues,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod12Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod12RValues,
                                         const int                  i1p1,
                                         const int                  i1p2,
                                         const int                  i1,
                                         const occ::handle<HLRAlgo_PolyInternalData>& pid1,
                                         NCollection_Array1<HLRAlgo_TriangleData>*&                 TData1,
                                         NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg1,
                                         NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod1,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod21Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod21RValues,
                                         HLRAlgo_PolyInternalNode::NodeIndices&  Nod22Indices,
                                         HLRAlgo_PolyInternalNode::NodeData&     Nod22RValues,
                                         const int                  i2p1,
                                         const int                  i2p2,
                                         const int                  i2,
                                         const occ::handle<HLRAlgo_PolyInternalData>& pid2,
                                         NCollection_Array1<HLRAlgo_TriangleData>*&                 TData2,
                                         NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                 PISeg2,
                                         NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&                 PINod2,
                                         const double                     X3,
                                         const double                     Y3,
                                         const double                     Z3,
                                         const double                     XT3,
                                         const double                     YT3,
                                         const double                     ZT3,
                                         const double                     coef3,
                                         const double                     U3,
                                         const bool                  insP3,
                                         const bool                  mP3P1,
                                         const double                     X4,
                                         const double                     Y4,
                                         const double                     Z4,
                                         const double                     XT4,
                                         const double                     YT4,
                                         const double                     ZT4,
                                         const double                     coef4,
                                         const double                     U4,
                                         const bool                  insP4,
                                         const bool                  mP4P1,
                                         const int                  flag) const;

  Standard_EXPORT void InsertOnOutLine(
    NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID);

  Standard_EXPORT void CheckFrBackTriangles(
    NCollection_List<HLRAlgo_BiPoint>&                                 theList,
    NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID);

  Standard_EXPORT void FindEdgeOnTriangle(const HLRAlgo_TriangleData& theTriangle,
                                          const int      ip1,
                                          const int      ip2,
                                          int&           jtrouv,
                                          bool&           isDirect) const;

  Standard_EXPORT void ChangeNode(const int                 theIp1,
                                  const int                 theIp2,
                                  HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                  HLRAlgo_PolyInternalNode::NodeData&    theNod1RValues,
                                  HLRAlgo_PolyInternalNode::NodeIndices& theNod2Indices,
                                  HLRAlgo_PolyInternalNode::NodeData&    theNod2RValues,
                                  const double                    theCoef1,
                                  const double                    theX3,
                                  const double                    theY3,
                                  const double                    theZ3,
                                  const bool                 theIsFirst,
                                  NCollection_Array1<HLRAlgo_TriangleData>&                 theTData,
                                  NCollection_Array1<HLRAlgo_PolyInternalSegment>&                 thePISeg,
                                  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&                 thePINod) const;

  Standard_EXPORT void UpdateAroundNode(const int                 theINode,
                                        HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                        NCollection_Array1<HLRAlgo_TriangleData>&                 theTData,
                                        NCollection_Array1<HLRAlgo_PolyInternalSegment>&                 thePISeg,
                                        NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&                 thePINod) const;

  Standard_EXPORT void OrientTriangle(const int                 iTri,
                                      HLRAlgo_TriangleData&                  theTriangle,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices1,
                                      HLRAlgo_PolyInternalNode::NodeData&    Nod1RValues,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices2,
                                      HLRAlgo_PolyInternalNode::NodeData&    Nod2RValues,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices3,
                                      HLRAlgo_PolyInternalNode::NodeData&    Nod3RValues) const;

  Standard_EXPORT bool Triangles(const int                 ip1,
                                             const int                 ip2,
                                             HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
                                             NCollection_Array1<HLRAlgo_PolyInternalSegment>*&                PISeg,
                                             int&                      iTri1,
                                             int&                      iTri2) const;

  Standard_EXPORT bool NewNode(HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
                                           HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
                                           double&                      coef1,
                                           bool&                   moveP1) const;

  Standard_EXPORT void UVNode(HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
                              HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
                              const double                 coef1,
                              double&                      U3,
                              double&                      V3) const;

  Standard_EXPORT void CheckDegeneratedSegment(
    HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
    HLRAlgo_PolyInternalNode::NodeData&    Nod1RValues,
    HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices,
    HLRAlgo_PolyInternalNode::NodeData&    Nod2RValues) const;

  Standard_EXPORT void UpdateOutLines(NCollection_List<HLRAlgo_BiPoint>&                                 theList,
                                      NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID);

  Standard_EXPORT void UpdateEdgesBiPoints(
    NCollection_List<HLRAlgo_BiPoint>&                                       theList,
    const NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID,
    const bool                                      theIsClosed);

  Standard_EXPORT void UpdatePolyData(NCollection_Array1<occ::handle<HLRAlgo_PolyData>>&         thePD,
                                      NCollection_Array1<occ::handle<HLRAlgo_PolyInternalData>>& thePID,
                                      const bool theClosed);

  Standard_EXPORT void TMultiply(double&         X,
                                 double&         Y,
                                 double&         Z,
                                 const bool VecPartOnly = false) const;

  void TMultiply(gp_XYZ& thePoint, const bool VecPartOnly = false) const
  {
    TMultiply(thePoint.ChangeCoord(1),
              thePoint.ChangeCoord(2),
              thePoint.ChangeCoord(3),
              VecPartOnly);
  }

  Standard_EXPORT void TTMultiply(double&         X,
                                  double&         Y,
                                  double&         Z,
                                  const bool VecPartOnly = false) const;

  void TTMultiply(gp_XYZ& thePoint, const bool VecPartOnly = false) const
  {
    TTMultiply(thePoint.ChangeCoord(1),
               thePoint.ChangeCoord(2),
               thePoint.ChangeCoord(3),
               VecPartOnly);
  }

  Standard_EXPORT void TIMultiply(double&         X,
                                  double&         Y,
                                  double&         Z,
                                  const bool VecPartOnly = false) const;

  void TIMultiply(gp_XYZ& thePoint, const bool VecPartOnly = false) const
  {
    TIMultiply(thePoint.ChangeCoord(1),
               thePoint.ChangeCoord(2),
               thePoint.ChangeCoord(3),
               VecPartOnly);
  }

private:
  HLRAlgo_Projector          myProj;
  double              TMat[3][3];
  double              TLoc[3];
  double              TTMa[3][3];
  double              TTLo[3];
  double              TIMa[3][3];
  double              TILo[3];
  NCollection_Sequence<TopoDS_Shape>   myShapes;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myEMap;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myFMap;
  occ::handle<HLRAlgo_PolyAlgo>   myAlgo;
  bool           myDebug;
  double              myTolSta;
  double              myTolEnd;
  double              myTolAngular;
  occ::handle<Geom_Surface>       myGSurf;
  BRepAdaptor_Surface        myBSurf;
  BRepAdaptor_Curve          myBCurv;
  BRepAdaptor_Curve2d        myPC;
};

#endif // _HLRBRep_PolyAlgo_HeaderFile
