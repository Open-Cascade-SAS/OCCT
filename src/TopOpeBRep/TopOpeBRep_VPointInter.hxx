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
#include <Standard_Handle.hxx>

#include <TopOpeBRep_PThePointOfIntersection.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <IntSurf_Transition.hxx>
#include <Standard_OStream.hxx>
class Standard_DomainError;
class IntPatch_Point;
class gp_Pnt;
class TopoDS_Shape;
class gp_Pnt2d;
class TopoDS_Edge;
class TopoDS_Face;



class TopOpeBRep_VPointInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
    TopOpeBRep_VPointInter();
  
  Standard_EXPORT void SetPoint (const IntPatch_Point& P);
  
    void SetShapes (const Standard_Integer I1, const Standard_Integer I2);
  
    void GetShapes (Standard_Integer& I1, Standard_Integer& I2) const;
  
    IntSurf_Transition TransitionOnS1() const;
  
    IntSurf_Transition TransitionOnS2() const;
  
    IntSurf_Transition TransitionLineArc1() const;
  
    IntSurf_Transition TransitionLineArc2() const;
  
    Standard_Boolean IsOnDomS1() const;
  
    Standard_Boolean IsOnDomS2() const;
  
    void ParametersOnS1 (Standard_Real& u, Standard_Real& v) const;
  
    void ParametersOnS2 (Standard_Real& u, Standard_Real& v) const;
  
    const gp_Pnt& Value() const;
  
    Standard_Real Tolerance() const;
  
  Standard_EXPORT const TopoDS_Shape& ArcOnS1() const;
  
  Standard_EXPORT const TopoDS_Shape& ArcOnS2() const;
  
    Standard_Real ParameterOnLine() const;
  
    Standard_Real ParameterOnArc1() const;
  
  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the first surface.
    Standard_Boolean IsVertexOnS1() const;
  
  //! Returns the information about the point when it is
  //! on the domain of the first patch, i-e when the function
  //! IsVertexOnS1 returns True.
  //! Otherwise, an exception is raised.
  Standard_EXPORT const TopoDS_Shape& VertexOnS1() const;
  
    Standard_Real ParameterOnArc2() const;
  
  //! Returns TRUE if the point is a vertex on the initial
  //! restriction facet of the second surface.
    Standard_Boolean IsVertexOnS2() const;
  
  //! Returns the information about the point when it is
  //! on the domain of the second patch, i-e when the function
  //! IsVertexOnS2 returns True.
  //! Otherwise, an exception is raised.
  Standard_EXPORT const TopoDS_Shape& VertexOnS2() const;
  
    Standard_Boolean IsInternal() const;
  
  //! Returns True if the point belongs to several intersection
  //! lines.
    Standard_Boolean IsMultiple() const;
  
  //! get state of VPoint within the domain of geometric shape
  //! domain <I> (= 1 or 2).
  Standard_EXPORT TopAbs_State State (const Standard_Integer I) const;
  
  //! Set the state of VPoint within the  domain of
  //! the geometric shape <I> (= 1 or 2).
  Standard_EXPORT void State (const TopAbs_State S, const Standard_Integer I);
  
  //! set the shape Eon of shape I (1,2) containing the point,
  //! and parameter <Par> of point on <Eon>.
  Standard_EXPORT void EdgeON (const TopoDS_Shape& Eon, const Standard_Real Par, const Standard_Integer I);
  
  //! get the edge of shape I (1,2) containing the point.
  Standard_EXPORT const TopoDS_Shape& EdgeON (const Standard_Integer I) const;
  
  //! get the parameter on edge of shape I (1,2) containing the point.
  Standard_EXPORT Standard_Real EdgeONParameter (const Standard_Integer I) const;
  
  //! returns value of filed myShapeIndex = 0,1,2,3
  //! 0 means the VPoint is on no restriction
  //! 1 means the VPoint is on the restriction 1
  //! 2 means the VPoint is on the restriction 2
  //! 3 means the VPoint is on the restrictions 1 and 2
    Standard_Integer ShapeIndex() const;
  
  //! set value of shape supporting me (0,1,2,3).
    void ShapeIndex (const Standard_Integer I);
  
  //! get the edge of shape I (1,2) containing the point.
  //! Returned shape is null if the VPoint is not on an edge
  //! of shape I (1,2).
  Standard_EXPORT const TopoDS_Shape& Edge (const Standard_Integer I) const;
  
  //! get the parameter on edge of shape I (1,2) containing the point
  Standard_EXPORT Standard_Real EdgeParameter (const Standard_Integer I) const;
  
  //! get the parameter on surface of shape I (1,2) containing the point
  Standard_EXPORT gp_Pnt2d SurfaceParameters (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Boolean IsVertex (const Standard_Integer I) const;
  
  Standard_EXPORT const TopoDS_Shape& Vertex (const Standard_Integer I) const;
  
  //! set myKeep value according to current states.
  Standard_EXPORT void UpdateKeep();
  

  //! Returns value of myKeep (does not evaluate states)
  //! False at creation of VPoint.
  //! Updated by State(State from TopAbs,Integer from Standard)
    Standard_Boolean Keep() const;
  
  //! updates VPointInter flag "keep" with <keep>.
    void ChangeKeep (const Standard_Boolean keep);
  
  //! returns <True> if the 3d points and the parameters of the
  //! VPoints are same
  Standard_EXPORT Standard_Boolean EqualpP (const TopOpeBRep_VPointInter& VP) const;
  
  //! returns <false> if the vpoint is not given on arc <E>,
  //! else returns <par> parameter on <E>
  Standard_EXPORT Standard_Boolean ParonE (const TopoDS_Edge& E, Standard_Real& par) const;
  
    void Index (const Standard_Integer I);
  
    Standard_Integer Index() const;
  
  Standard_EXPORT Standard_OStream& Dump (const Standard_Integer I, const TopoDS_Face& F, Standard_OStream& OS) const;
  
  Standard_EXPORT Standard_OStream& Dump (const TopoDS_Face& F1, const TopoDS_Face& F2, Standard_OStream& OS) const;
  
  Standard_EXPORT TopOpeBRep_PThePointOfIntersection PThePointOfIntersectionDummy() const;




protected:





private:



  TopOpeBRep_PThePointOfIntersection myPPOI;
  Standard_Integer myShapeIndex;
  TopAbs_State myState1;
  TopAbs_State myState2;
  Standard_Boolean myKeep;
  TopoDS_Shape myEdgeON1;
  TopoDS_Shape myEdgeON2;
  Standard_Real myEdgeONPar1;
  Standard_Real myEdgeONPar2;
  Standard_Integer myIndex;
  TopoDS_Shape myNullShape;
  Standard_Integer myS1;
  Standard_Integer myS2;


};


#include <TopOpeBRep_VPointInter.lxx>





#endif // _TopOpeBRep_VPointInter_HeaderFile
