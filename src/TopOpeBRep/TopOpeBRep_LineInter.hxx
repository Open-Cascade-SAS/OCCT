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

#ifndef _TopOpeBRep_LineInter_HeaderFile
#define _TopOpeBRep_LineInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
#include <TopOpeBRep_WPointInter.hxx>
#include <TopOpeBRep_HArray1OfVPointInter.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
#include <Standard_OStream.hxx>
class IntPatch_Line;
class IntPatch_ALine;
class IntPatch_RLine;
class IntPatch_WLine;
class IntPatch_GLine;
class BRepAdaptor_Surface;
class TopOpeBRep_VPointInter;
class Geom_Curve;
class TCollection_AsciiString;
class TopOpeBRep_Bipoint;



class TopOpeBRep_LineInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
    TopOpeBRep_LineInter();
  
  Standard_EXPORT void SetLine (const Handle(IntPatch_Line)& L, const BRepAdaptor_Surface& S1, const BRepAdaptor_Surface& S2);
  
    void SetFaces (const TopoDS_Face& F1, const TopoDS_Face& F2);
  
    TopOpeBRep_TypeLineCurve TypeLineCurve() const;
  
    Standard_Integer NbVPoint() const;
  
  Standard_EXPORT const TopOpeBRep_VPointInter& VPoint (const Standard_Integer I) const;
  
  Standard_EXPORT TopOpeBRep_VPointInter& ChangeVPoint (const Standard_Integer I);
  
  Standard_EXPORT void SetINL();
  
    Standard_Boolean INL() const;
  
  Standard_EXPORT void SetIsVClosed();
  
    Standard_Boolean IsVClosed() const;
  
  Standard_EXPORT void SetOK (const Standard_Boolean B);
  
    Standard_Boolean OK() const;
  
  Standard_EXPORT void SetHasVPonR();
  
    Standard_Boolean HasVPonR() const;
  
  Standard_EXPORT void SetVPBounds();
  
  Standard_EXPORT void VPBounds (Standard_Integer& f, Standard_Integer& l, Standard_Integer& n) const;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const;
  
  Standard_EXPORT Standard_Real Period() const;
  
  Standard_EXPORT void Bounds (Standard_Real& f, Standard_Real& l) const;
  
  Standard_EXPORT Standard_Boolean HasVInternal();
  
  Standard_EXPORT Standard_Integer NbWPoint() const;
  
  Standard_EXPORT const TopOpeBRep_WPointInter& WPoint (const Standard_Integer I);
  
    IntSurf_TypeTrans TransitionOnS1() const;
  
    IntSurf_TypeTrans TransitionOnS2() const;
  
    IntSurf_Situation SituationS1() const;
  
    IntSurf_Situation SituationS2() const;
  
  Standard_EXPORT Handle(Geom_Curve) Curve() const;
  
  Standard_EXPORT Handle(Geom_Curve) Curve (const Standard_Real parmin, const Standard_Real parmax) const;
  
  //! returns the edge of a RESTRICTION line (or a null edge).
  Standard_EXPORT const TopoDS_Shape& Arc() const;
  
  //! returns true if Arc() edge (of a RESTRICTION line) is
  //! an edge of the original face <Index> (1 or 2).
  Standard_EXPORT Standard_Boolean ArcIsEdge (const Standard_Integer I) const;
  
    const Handle(IntPatch_WLine)& LineW() const;
  
    const Handle(IntPatch_GLine)& LineG() const;
  
    const Handle(IntPatch_RLine)& LineR() const;
  
  Standard_EXPORT Standard_Boolean HasFirstPoint() const;
  
  Standard_EXPORT Standard_Boolean HasLastPoint() const;
  
  Standard_EXPORT void ComputeFaceFaceTransition();
  
  Standard_EXPORT const TopOpeBRepDS_Transition& FaceFaceTransition (const Standard_Integer I) const;
  
    void Index (const Standard_Integer I);
  
    Standard_Integer Index() const;
  
  Standard_EXPORT void DumpType() const;
  
  Standard_EXPORT void DumpVPoint (const Standard_Integer I, const TCollection_AsciiString& s1, const TCollection_AsciiString& s2) const;
  
  Standard_EXPORT void DumpBipoint (const TopOpeBRep_Bipoint& B, const TCollection_AsciiString& s1, const TCollection_AsciiString& s2) const;
  
  Standard_EXPORT void SetTraceIndex (const Standard_Integer exF1, const Standard_Integer exF2);
  
  Standard_EXPORT void GetTraceIndex (Standard_Integer& exF1, Standard_Integer& exF2) const;
  
  Standard_EXPORT Standard_OStream& DumpLineTransitions (Standard_OStream& OS) const;




protected:





private:



  Standard_Boolean myOK;
  Standard_Integer myIndex;
  Standard_Integer myNbVPoint;
  Standard_Boolean myIsVClosed;
  Standard_Boolean myHasVPonR;
  Standard_Boolean myINL;
  Standard_Boolean myVPBDefined;
  Standard_Integer myVPF;
  Standard_Integer myVPL;
  Standard_Integer myVPN;
  TopOpeBRep_TypeLineCurve myTypeLineCurve;
  Handle(IntPatch_Line) myIL;
  Handle(IntPatch_ALine) myILA;
  Handle(IntPatch_RLine) myILR;
  Handle(IntPatch_WLine) myILW;
  Handle(IntPatch_GLine) myILG;
  TopOpeBRep_WPointInter myCurrentWP;
  Handle(TopOpeBRep_HArray1OfVPointInter) myHAVP;
  TopoDS_Face myF1;
  TopoDS_Face myF2;
  TopOpeBRepDS_Transition myLineTonF1;
  TopOpeBRepDS_Transition myLineTonF2;
  TopoDS_Shape myNullShape;
  Standard_Integer myexF1;
  Standard_Integer myexF2;


};


#include <TopOpeBRep_LineInter.lxx>





#endif // _TopOpeBRep_LineInter_HeaderFile
