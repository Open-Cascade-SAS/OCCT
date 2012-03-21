// Created on: 1997-02-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef _TopOpeBRepDS_ProcessInterferencesTool_HeaderFile
#define _TopOpeBRepDS_ProcessInterferencesTool_HeaderFile

#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeCPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK); // POINT/VERTEX
Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master
Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK, // POINT/VERTEX
 const TopOpeBRepDS_Kind SK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master
Standard_EXPORT Standard_Boolean FUN_hasStateShape(const TopOpeBRepDS_Transition& T,const TopAbs_State state,const TopAbs_ShapeEnum shape);
Standard_EXPORT Standard_Integer FUN_selectTRASHAinterference(TopOpeBRepDS_ListOfInterference& L1,const TopAbs_ShapeEnum sha,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectITRASHAinterference(TopOpeBRepDS_ListOfInterference& L1,const Standard_Integer Index, TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectTRAUNKinterference(TopOpeBRepDS_ListOfInterference& L1,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectTRAORIinterference(TopOpeBRepDS_ListOfInterference& L1, const TopAbs_Orientation O, TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectGKinterference(TopOpeBRepDS_ListOfInterference& L1,const TopOpeBRepDS_Kind GK,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectSKinterference(TopOpeBRepDS_ListOfInterference& L1,const TopOpeBRepDS_Kind SK,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectGIinterference(TopOpeBRepDS_ListOfInterference& L1,const Standard_Integer GI,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Integer FUN_selectSIinterference(TopOpeBRepDS_ListOfInterference& L1,const Standard_Integer SI,TopOpeBRepDS_ListOfInterference& L2);
Standard_EXPORT Standard_Boolean FUN_interfhassupport(const TopOpeBRepDS_DataStructure& DS,const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& S);
Standard_EXPORT Standard_Boolean FUN_transitionEQUAL(const TopOpeBRepDS_Transition&,const TopOpeBRepDS_Transition&);
Standard_EXPORT Standard_Boolean FUN_transitionSTATEEQUAL(const TopOpeBRepDS_Transition&,const TopOpeBRepDS_Transition&);
Standard_EXPORT Standard_Boolean FUN_transitionSHAPEEQUAL(const TopOpeBRepDS_Transition&,const TopOpeBRepDS_Transition&);
Standard_EXPORT Standard_Boolean FUN_transitionINDEXEQUAL(const TopOpeBRepDS_Transition&,const TopOpeBRepDS_Transition&);
Standard_EXPORT void FUN_reducedoublons(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT void FUN_unkeepUNKNOWN(TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX);
Standard_EXPORT Standard_Integer FUN_select2dI(const Standard_Integer SIX, TopOpeBRepDS_DataStructure& BDS,const TopAbs_ShapeEnum TRASHAk,TopOpeBRepDS_ListOfInterference& lI, TopOpeBRepDS_ListOfInterference& l2dI);
Standard_EXPORT Standard_Integer FUN_selectpure2dI(const TopOpeBRepDS_ListOfInterference& lF, TopOpeBRepDS_ListOfInterference& lFE, TopOpeBRepDS_ListOfInterference& l2dFE);
Standard_EXPORT Standard_Integer FUN_select1dI(const Standard_Integer SIX, TopOpeBRepDS_DataStructure& BDS,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& l1dI);
Standard_EXPORT void FUN_select3dinterference
(const Standard_Integer SIX, TopOpeBRepDS_DataStructure& BDS,TopOpeBRepDS_ListOfInterference& lF, TopOpeBRepDS_ListOfInterference& l3dF,
 TopOpeBRepDS_ListOfInterference& lFE,TopOpeBRepDS_ListOfInterference& lFEresi,TopOpeBRepDS_ListOfInterference& l3dFE, TopOpeBRepDS_ListOfInterference& l3dFEresi, 
 TopOpeBRepDS_ListOfInterference& l2dFE);
#endif
