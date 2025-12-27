// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRep_VPointInter_HeaderFile
#define _TopOpeBRep_VPointInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRep_PThePointOfIntersection.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <IntSurf_Transition.hxx>
#include <Standard_OStream.hxx>
class gp_Pnt;
class gp_Pnt2d;
class TopoDS_Edge;
class TopoDS_Face;

class TopOpeBRep_VPointInter
{
public:
  DEFINE_STANDARD_ALLOC

  TopOpeBRep_VPointInter();

  Standard_EXPORT void SetPoint(const IntPatch_Point& P);

  void SetShapes(const int I1, const int I2);

  void GetShapes(int& I1, int& I2) const;

  IntSurf_Transition TransitionOnS1() const;

  IntSurf_Transition TransitionOnS2() const;

  IntSurf_Transition TransitionLineArc1() const;

  IntSurf_Transition TransitionLineArc2() const;

  bool IsOnDomS1() const;

  bool IsOnDomS2() const;

  void ParametersOnS1(double& u, double& v) const;

  void ParametersOnS2(double& u, double& v) const;

  const gp_Pnt& Value() const;

  double Tolerance() const;

  Standard_EXPORT const TopoDS_Shape& ArcOnS1() const;

  Standard_EXPORT const TopoDS_Shape& ArcOnS2() const;

  double ParameterOnLine() const;

  double ParameterOnArc1() const;

  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the first surface.
  bool IsVertexOnS1() const;

  //! Returns the information about the point when it is
  //! on the domain of the first patch, i-e when the function
  //! IsVertexOnS1 returns True.
  //! Otherwise, an exception is raised.
  Standard_EXPORT const TopoDS_Shape& VertexOnS1() const;

  double ParameterOnArc2() const;

  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the second surface.
  bool IsVertexOnS2() const;

  //! Returns the information about the point when it is
  //! on the domain of the second patch, i-e when the function
  //! IsVertexOnS2 returns True.
  //! Otherwise, an exception is raised.
  Standard_EXPORT const TopoDS_Shape& VertexOnS2() const;

  bool IsInternal() const;

  //! Returns True if the point belongs to several intersection
  //! lines.
  bool IsMultiple() const;

  //! get state of VPoint within the domain of geometric shape
  //! domain <I> (= 1 or 2).
  Standard_EXPORT TopAbs_State State(const int I) const;

  //! Set the state of VPoint within the domain of
  //! the geometric shape <I> (= 1 or 2).
  Standard_EXPORT void State(const TopAbs_State S, const int I);

  //! set the shape Eon of shape I (1,2) containing the point,
  //! and parameter <Par> of point on <Eon>.
  Standard_EXPORT void EdgeON(const TopoDS_Shape&    Eon,
                              const double    Par,
                              const int I);

  //! get the edge of shape I (1,2) containing the point.
  Standard_EXPORT const TopoDS_Shape& EdgeON(const int I) const;

  //! get the parameter on edge of shape I (1,2) containing the point.
  Standard_EXPORT double EdgeONParameter(const int I) const;

  //! returns value of filed myShapeIndex = 0,1,2,3
  //! 0 means the VPoint is on no restriction
  //! 1 means the VPoint is on the restriction 1
  //! 2 means the VPoint is on the restriction 2
  //! 3 means the VPoint is on the restrictions 1 and 2
  int ShapeIndex() const;

  //! set value of shape supporting me (0,1,2,3).
  void ShapeIndex(const int I);

  //! get the edge of shape I (1,2) containing the point.
  //! Returned shape is null if the VPoint is not on an edge
  //! of shape I (1,2).
  Standard_EXPORT const TopoDS_Shape& Edge(const int I) const;

  //! get the parameter on edge of shape I (1,2) containing the point
  Standard_EXPORT double EdgeParameter(const int I) const;

  //! get the parameter on surface of shape I (1,2) containing the point
  Standard_EXPORT gp_Pnt2d SurfaceParameters(const int I) const;

  Standard_EXPORT bool IsVertex(const int I) const;

  Standard_EXPORT const TopoDS_Shape& Vertex(const int I) const;

  //! set myKeep value according to current states.
  Standard_EXPORT void UpdateKeep();

  //! Returns value of myKeep (does not evaluate states)
  //! False at creation of VPoint.
  //! Updated by State(State from TopAbs,Integer from Standard)
  bool Keep() const;

  //! updates VPointInter flag "keep" with <keep>.
  void ChangeKeep(const bool keep);

  //! returns <True> if the 3d points and the parameters of the
  //! VPoints are same
  Standard_EXPORT bool EqualpP(const TopOpeBRep_VPointInter& VP) const;

  //! returns <false> if the vpoint is not given on arc <E>,
  //! else returns <par> parameter on <E>
  Standard_EXPORT bool ParonE(const TopoDS_Edge& E, double& par) const;

  void Index(const int I);

  int Index() const;

  Standard_EXPORT Standard_OStream& Dump(const int I,
                                         const TopoDS_Face&     F,
                                         Standard_OStream&      OS) const;

  Standard_EXPORT Standard_OStream& Dump(const TopoDS_Face& F1,
                                         const TopoDS_Face& F2,
                                         Standard_OStream&  OS) const;

  Standard_EXPORT TopOpeBRep_PThePointOfIntersection PThePointOfIntersectionDummy() const;

private:
  TopOpeBRep_PThePointOfIntersection myPPOI;
  int                   myShapeIndex;
  TopAbs_State                       myState1;
  TopAbs_State                       myState2;
  bool                   myKeep;
  TopoDS_Shape                       myEdgeON1;
  TopoDS_Shape                       myEdgeON2;
  double                      myEdgeONPar1;
  double                      myEdgeONPar2;
  int                   myIndex;
  TopoDS_Shape                       myNullShape;
  int                   myS1;
  int                   myS2;
};

#include <TopOpeBRep_VPointInter.lxx>

#endif // _TopOpeBRep_VPointInter_HeaderFile
