// Created on: 1994-10-27
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

#ifndef _TopOpeBRep_FFTransitionTool_HeaderFile
#define _TopOpeBRep_FFTransitionTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
class TopOpeBRepDS_Transition;
class TopOpeBRep_VPointInter;
class TopOpeBRep_LineInter;
class TopoDS_Shape;



class TopOpeBRep_FFTransitionTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static TopOpeBRepDS_Transition ProcessLineTransition (const TopOpeBRep_VPointInter& P, const Standard_Integer Index, const TopAbs_Orientation EdgeOrientation);
  
  Standard_EXPORT static TopOpeBRepDS_Transition ProcessLineTransition (const TopOpeBRep_VPointInter& P, const TopOpeBRep_LineInter& L);
  
  Standard_EXPORT static TopOpeBRepDS_Transition ProcessEdgeTransition (const TopOpeBRep_VPointInter& P, const Standard_Integer Index, const TopAbs_Orientation LineOrientation);
  
  Standard_EXPORT static TopOpeBRepDS_Transition ProcessFaceTransition (const TopOpeBRep_LineInter& L, const Standard_Integer Index, const TopAbs_Orientation FaceOrientation);
  
  //! compute transition on "IntPatch_Restriction line" edge <R>
  //! when crossing edge <E> of face <F> at point <VP>.
  //! VP is given on edge <E> of face <F> of index <Index> (1 or 2).
  //! <VP> has been classified by FacesFiller as TopAbs_ON an edge <R>
  //! of the other face than <F> of current (face/face) intersection.
  //! Transition depends on the orientation of E in F.
  //! This method should be provided by IntPatch_Line (NYI)
  Standard_EXPORT static TopOpeBRepDS_Transition ProcessEdgeONTransition (const TopOpeBRep_VPointInter& VP, const Standard_Integer Index, const TopoDS_Shape& R, const TopoDS_Shape& E, const TopoDS_Shape& F);




protected:





private:





};







#endif // _TopOpeBRep_FFTransitionTool_HeaderFile
