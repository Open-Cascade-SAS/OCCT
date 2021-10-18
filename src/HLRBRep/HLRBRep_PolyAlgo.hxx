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
#include <HLRAlgo_Array1OfTData.hxx>
#include <HLRAlgo_Array1OfPISeg.hxx>
#include <HLRAlgo_Array1OfPINod.hxx>
#include <HLRAlgo_ListOfBPoint.hxx>
#include <HLRAlgo_PolyAlgo.hxx>
#include <HLRAlgo_PolyInternalNode.hxx>
#include <HLRAlgo_Projector.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

class Geom_Surface;
class TopoDS_Edge;
class HLRAlgo_PolyInternalData;
class HLRAlgo_EdgeStatus;
struct HLRAlgo_TriangleData;

class HLRBRep_PolyAlgo;
DEFINE_STANDARD_HANDLE(HLRBRep_PolyAlgo, Standard_Transient)

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
  //! -   Load to select the shape or shapes to be  visualized
  //! -   Update to compute the visible and hidden lines of the shape.
  //! Warning
  //! The shape or shapes to be visualized must have already been triangulated.
  Standard_EXPORT HLRBRep_PolyAlgo();
  
  Standard_EXPORT HLRBRep_PolyAlgo(const Handle(HLRBRep_PolyAlgo)& A);
  
  Standard_EXPORT HLRBRep_PolyAlgo(const TopoDS_Shape& S);
  
  Standard_Integer NbShapes() const { return myShapes.Length(); }

  Standard_EXPORT TopoDS_Shape& Shape (const Standard_Integer I);
  
  //! remove the Shape of Index <I>.
  Standard_EXPORT void Remove (const Standard_Integer I);
  
  //! return the index of the Shape <S> and  return 0 if
  //! the Shape <S> is not found.
  Standard_EXPORT Standard_Integer Index (const TopoDS_Shape& S) const;
  
  //! Loads the shape S into this framework.
  //! Warning S must have already been triangulated.
  void Load (const TopoDS_Shape& theShape) { myShapes.Append (theShape); }

  const Handle(HLRAlgo_PolyAlgo)& Algo() const { return myAlgo; }

  //! Sets the parameters of the view for this framework.
  //! These parameters are defined by an HLRAlgo_Projector object,
  //! which is returned by the Projector function on a Prs3d_Projector object.
  const HLRAlgo_Projector& Projector() const { return myProj; }

  void Projector (const HLRAlgo_Projector& theProj) { myProj = theProj; }
  
  Standard_Real TolAngular() const { return myTolAngular; }

  void TolAngular (const Standard_Real theTol) { myTolAngular = theTol; }

  Standard_Real TolCoef() const { return myTolSta; }
  
  void TolCoef (const Standard_Real theTol)
  {
    myTolSta = theTol;
    myTolEnd = 1.0 - theTol;
  }

  //! Launches calculation of outlines of the shape
  //! visualized by this framework. Used after setting the point of view and
  //! defining the shape or shapes to be visualized.
  Standard_EXPORT void Update();

  void InitHide() { myAlgo->InitHide(); }

  Standard_Boolean MoreHide() const { return myAlgo->MoreHide(); }

  void NextHide() { myAlgo->NextHide(); }

  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Hide (
    HLRAlgo_EdgeStatus& status,
    TopoDS_Shape& S,
    Standard_Boolean& reg1,
    Standard_Boolean& regn,
    Standard_Boolean& outl,
    Standard_Boolean& intl);

  void InitShow() { myAlgo->InitShow(); }

  Standard_Boolean MoreShow() const { return myAlgo->MoreShow(); }

  void NextShow() { myAlgo->NextShow(); }

  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Show (TopoDS_Shape& S, Standard_Boolean& reg1, Standard_Boolean& regn, Standard_Boolean& outl, Standard_Boolean& intl);
  
  //! Make a shape  with  the internal outlines in  each
  //! face.
  Standard_EXPORT TopoDS_Shape OutLinedShape (const TopoDS_Shape& S) const;

  Standard_Boolean Debug() const { return myDebug; }

  void Debug (const Standard_Boolean theDebug) { myDebug = theDebug; }

  DEFINE_STANDARD_RTTIEXT(HLRBRep_PolyAlgo,Standard_Transient)

private:
  
  Standard_EXPORT TopoDS_Shape MakeShape() const;
  
  Standard_EXPORT Standard_Integer InitShape (const TopoDS_Shape& Shape, Standard_Boolean& IsoledF, Standard_Boolean& IsoledE);

  Standard_EXPORT void StoreShell (const TopoDS_Shape& theShape,
                                   Standard_Integer& theIShell,
                                   NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& theShell,
                                   const Standard_Boolean theIsoledF,
                                   const Standard_Boolean theIsoledE,
                                   TColStd_Array1OfInteger& theES,
                                   NCollection_Array1<Handle(HLRAlgo_PolyData)>& thePD,
                                   NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                   TopTools_MapOfShape& theShapeMap1,
                                   TopTools_MapOfShape& theShapeMap2);

  Standard_EXPORT Standard_Boolean Normal (const Standard_Integer theINode,
                                           HLRAlgo_PolyInternalNode::NodeIndices& theNodIndices,
                                           HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                           HLRAlgo_Array1OfTData& theTData,
                                           HLRAlgo_Array1OfPISeg& thePISeg,
                                           HLRAlgo_Array1OfPINod& thePINod,
                                           const Standard_Boolean orient) const;

  Standard_EXPORT Standard_Boolean AverageNormal (const Standard_Integer theINode,
                                                  HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                                  HLRAlgo_Array1OfTData& theTData,
                                                  HLRAlgo_Array1OfPISeg& thePISeg,
                                                  HLRAlgo_Array1OfPINod& thePINod,
                                                  Standard_Real& theX,
                                                  Standard_Real& theY,
                                                  Standard_Real& theZ) const;

  Standard_Boolean AverageNormal (const Standard_Integer theINode,
                                  HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                  HLRAlgo_Array1OfTData& theTData,
                                  HLRAlgo_Array1OfPISeg& thePISeg,
                                  HLRAlgo_Array1OfPINod& thePINod,
                                  gp_XYZ& theNormal) const
  {
    return AverageNormal (theINode, theNodeIndices, theTData, thePISeg, thePINod,
                          theNormal.ChangeCoord(1), theNormal.ChangeCoord(2), theNormal.ChangeCoord(3));
  }

  Standard_EXPORT void AddNormalOnTriangle (const Standard_Integer theITri,
                                            const Standard_Integer theINode,
                                            Standard_Integer& theJNode,
                                            HLRAlgo_Array1OfTData& theTData,
                                            HLRAlgo_Array1OfPINod& thePINod,
                                            Standard_Real& theX,
                                            Standard_Real& theY,
                                            Standard_Real& theZ,
                                            Standard_Boolean& theOK) const;

  Standard_EXPORT void InitBiPointsWithConnexity (const Standard_Integer theIEdge,
                                                  TopoDS_Edge& theEdge,
                                                  HLRAlgo_ListOfBPoint& theList,
                                                  NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                                  TopTools_ListOfShape& theLS,
                                                  const Standard_Boolean theIsConnex);

  Standard_EXPORT void Interpolation (HLRAlgo_ListOfBPoint& List, Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& XTI1, Standard_Real& YTI1, Standard_Real& ZTI1, Standard_Real& XTI2, Standard_Real& YTI2, Standard_Real& ZTI2, const Standard_Integer e, Standard_Real& U1, Standard_Real& U2, HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices1, HLRAlgo_PolyInternalNode::NodeData& Nod11RValues, HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices2, HLRAlgo_PolyInternalNode::NodeData& Nod12RValues, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i1, const Handle(HLRAlgo_PolyInternalData)& pid1, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPINod*& PINod1) const;
  
  Standard_EXPORT void Interpolation (HLRAlgo_ListOfBPoint& List, Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& XTI1, Standard_Real& YTI1, Standard_Real& ZTI1, Standard_Real& XTI2, Standard_Real& YTI2, Standard_Real& ZTI2, const Standard_Integer e, Standard_Real& U1, Standard_Real& U2, const GeomAbs_Shape rg, HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices, HLRAlgo_PolyInternalNode::NodeData& Nod11RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices, HLRAlgo_PolyInternalNode::NodeData& Nod12RValues, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i1, const Handle(HLRAlgo_PolyInternalData)& pid1, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices, HLRAlgo_PolyInternalNode::NodeData& Nod21RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices, HLRAlgo_PolyInternalNode::NodeData& Nod22RValues, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Integer i2, const Handle(HLRAlgo_PolyInternalData)& pid2, HLRAlgo_Array1OfTData*& TData2, HLRAlgo_Array1OfPISeg*& PISeg2, HLRAlgo_Array1OfPINod*& PINod2) const;
  
  Standard_EXPORT Standard_Boolean Interpolation (const Standard_Real U1, const Standard_Real U2, HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues, Standard_Real& X3, Standard_Real& Y3, Standard_Real& Z3, Standard_Real& XT3, Standard_Real& YT3, Standard_Real& ZT3, Standard_Real& coef3, Standard_Real& U3, Standard_Boolean& mP3P1) const;
  
  Standard_EXPORT void MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List, Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& XTI1, Standard_Real& YTI1, Standard_Real& ZTI1, Standard_Real& XTI2, Standard_Real& YTI2, Standard_Real& ZTI2, const Standard_Integer e, Standard_Real& U1, Standard_Real& U2, HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices, HLRAlgo_PolyInternalNode::NodeData& Nod11RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices, HLRAlgo_PolyInternalNode::NodeData& Nod12RValues, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i1, const Handle(HLRAlgo_PolyInternalData)& pid1, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPINod*& PINod1, const Standard_Real X3, const Standard_Real Y3, const Standard_Real Z3, const Standard_Real XT3, const Standard_Real YT3, const Standard_Real ZT3, const Standard_Real coef3, const Standard_Real U3, const Standard_Boolean insP3, const Standard_Boolean mP3P1, const Standard_Integer flag) const;
  
  Standard_EXPORT void MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List, Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& XTI1, Standard_Real& YTI1, Standard_Real& ZTI1, Standard_Real& XTI2, Standard_Real& YTI2, Standard_Real& ZTI2, const Standard_Integer e, Standard_Real& U1, Standard_Real& U2, HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices, HLRAlgo_PolyInternalNode::NodeData& Nod11RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices, HLRAlgo_PolyInternalNode::NodeData& Nod12RValues, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i1, const Handle(HLRAlgo_PolyInternalData)& pid1, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices, HLRAlgo_PolyInternalNode::NodeData& Nod21RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices, HLRAlgo_PolyInternalNode::NodeData& Nod22RValues, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Integer i2, const Handle(HLRAlgo_PolyInternalData)& pid2, HLRAlgo_Array1OfTData*& TData2, HLRAlgo_Array1OfPISeg*& PISeg2, HLRAlgo_Array1OfPINod*& PINod2, const Standard_Real X3, const Standard_Real Y3, const Standard_Real Z3, const Standard_Real XT3, const Standard_Real YT3, const Standard_Real ZT3, const Standard_Real coef3, const Standard_Real U3, const Standard_Boolean insP3, const Standard_Boolean mP3P1, const Standard_Integer flag) const;
  
  Standard_EXPORT void MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List, Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1, Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2, Standard_Real& XTI1, Standard_Real& YTI1, Standard_Real& ZTI1, Standard_Real& XTI2, Standard_Real& YTI2, Standard_Real& ZTI2, const Standard_Integer e, Standard_Real& U1, Standard_Real& U2, HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices, HLRAlgo_PolyInternalNode::NodeData& Nod11RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices, HLRAlgo_PolyInternalNode::NodeData& Nod12RValues, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i1, const Handle(HLRAlgo_PolyInternalData)& pid1, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices, HLRAlgo_PolyInternalNode::NodeData& Nod21RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices, HLRAlgo_PolyInternalNode::NodeData& Nod22RValues, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Integer i2, const Handle(HLRAlgo_PolyInternalData)& pid2, HLRAlgo_Array1OfTData*& TData2, HLRAlgo_Array1OfPISeg*& PISeg2, HLRAlgo_Array1OfPINod*& PINod2, const Standard_Real X3, const Standard_Real Y3, const Standard_Real Z3, const Standard_Real XT3, const Standard_Real YT3, const Standard_Real ZT3, const Standard_Real coef3, const Standard_Real U3, const Standard_Boolean insP3, const Standard_Boolean mP3P1, const Standard_Real X4, const Standard_Real Y4, const Standard_Real Z4, const Standard_Real XT4, const Standard_Real YT4, const Standard_Real ZT4, const Standard_Real coef4, const Standard_Real U4, const Standard_Boolean insP4, const Standard_Boolean mP4P1, const Standard_Integer flag) const;

  Standard_EXPORT void InsertOnOutLine (NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID);

  Standard_EXPORT void CheckFrBackTriangles (HLRAlgo_ListOfBPoint& theList,
                                             NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID);

  Standard_EXPORT void FindEdgeOnTriangle (const HLRAlgo_TriangleData& theTriangle, const Standard_Integer ip1, const Standard_Integer ip2, Standard_Integer& jtrouv, Standard_Boolean& isDirect) const;

  Standard_EXPORT void ChangeNode (const Standard_Integer theIp1,
                                   const Standard_Integer theIp2,
                                   HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                   HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                   HLRAlgo_PolyInternalNode::NodeIndices& theNod2Indices,
                                   HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                   const Standard_Real theCoef1,
                                   const Standard_Real theX3,
                                   const Standard_Real theY3,
                                   const Standard_Real theZ3,
                                   const Standard_Boolean theIsFirst,
                                   HLRAlgo_Array1OfTData& theTData,
                                   HLRAlgo_Array1OfPISeg& thePISeg,
                                   HLRAlgo_Array1OfPINod& thePINod) const;

  Standard_EXPORT void UpdateAroundNode (const Standard_Integer theINode,
                                         HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                         HLRAlgo_Array1OfTData& theTData,
                                         HLRAlgo_Array1OfPISeg& thePISeg,
                                         HLRAlgo_Array1OfPINod& thePINod) const;

  Standard_EXPORT void OrientTriangle (const Standard_Integer iTri, HLRAlgo_TriangleData& theTriangle, HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices1, HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices2, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues, HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices3, HLRAlgo_PolyInternalNode::NodeData& Nod3RValues) const;
  
  Standard_EXPORT Standard_Boolean Triangles (const Standard_Integer ip1, const Standard_Integer ip2, HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices, HLRAlgo_Array1OfPISeg*& PISeg, Standard_Integer& iTri1, Standard_Integer& iTri2) const;
  
  Standard_EXPORT Standard_Boolean NewNode (HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues, Standard_Real& coef1, Standard_Boolean& moveP1) const;
  
  Standard_EXPORT void UVNode (HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues, const Standard_Real coef1, Standard_Real& U3, Standard_Real& V3) const;
  
  Standard_EXPORT void CheckDegeneratedSegment (HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices, HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues) const;

  Standard_EXPORT void UpdateOutLines (HLRAlgo_ListOfBPoint& theList,
                                       NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID);

  Standard_EXPORT void UpdateEdgesBiPoints (HLRAlgo_ListOfBPoint& theList,
                                            const NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                            const Standard_Boolean theIsClosed);

  Standard_EXPORT void UpdatePolyData (NCollection_Array1<Handle(HLRAlgo_PolyData)>& thePD,
                                       NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                       const Standard_Boolean theClosed);

  Standard_EXPORT void TMultiply (Standard_Real& X, Standard_Real& Y, Standard_Real& Z, const Standard_Boolean VecPartOnly = Standard_False) const;

  void TMultiply(gp_XYZ& thePoint, const Standard_Boolean VecPartOnly = Standard_False) const
  {
    TMultiply(thePoint.ChangeCoord(1), thePoint.ChangeCoord(2), thePoint.ChangeCoord(3), VecPartOnly);
  }

  Standard_EXPORT void TTMultiply (Standard_Real& X, Standard_Real& Y, Standard_Real& Z, const Standard_Boolean VecPartOnly = Standard_False) const;

  void TTMultiply (gp_XYZ& thePoint, const Standard_Boolean VecPartOnly = Standard_False) const
  {
    TTMultiply(thePoint.ChangeCoord(1), thePoint.ChangeCoord(2), thePoint.ChangeCoord(3), VecPartOnly);\
  }

  Standard_EXPORT void TIMultiply (Standard_Real& X, Standard_Real& Y, Standard_Real& Z, const Standard_Boolean VecPartOnly = Standard_False) const;

  void TIMultiply (gp_XYZ& thePoint, const Standard_Boolean VecPartOnly = Standard_False) const
  {
    TIMultiply(thePoint.ChangeCoord(1), thePoint.ChangeCoord(2), thePoint.ChangeCoord(3), VecPartOnly);
  }

private:

  HLRAlgo_Projector myProj;
  Standard_Real TMat[3][3];
  Standard_Real TLoc[3];
  Standard_Real TTMa[3][3];
  Standard_Real TTLo[3];
  Standard_Real TIMa[3][3];
  Standard_Real TILo[3];
  TopTools_SequenceOfShape myShapes;
  TopTools_IndexedMapOfShape myEMap;
  TopTools_IndexedMapOfShape myFMap;
  Handle(HLRAlgo_PolyAlgo) myAlgo;
  Standard_Boolean myDebug;
  Standard_Real myTolSta;
  Standard_Real myTolEnd;
  Standard_Real myTolAngular;
  Handle(Geom_Surface) myGSurf;
  BRepAdaptor_Surface myBSurf;
  BRepAdaptor_Curve myBCurv;
  BRepAdaptor_Curve2d myPC;

};

#endif // _HLRBRep_PolyAlgo_HeaderFile
