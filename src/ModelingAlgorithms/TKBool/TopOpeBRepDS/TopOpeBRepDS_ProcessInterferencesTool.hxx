// Created on: 1997-02-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_ProcessInterferencesTool_HeaderFile
#define _TopOpeBRepDS_ProcessInterferencesTool_HeaderFile

#include <TopOpeBRepDS_EXPORT.hxx>
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeCPVInterference(
  const TopOpeBRepDS_Transition& T, // transition
  const int                      S, // curve/edge index
  const int                      G, // point/vertex index
  const double                   P, // parameter of G on S
  const TopOpeBRepDS_Kind        GK);      // POINT/VERTEX
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T, // transition
  const int                      S, // curve/edge index
  const int                      G, // point/vertex index
  const double                   P, // parameter of G on S
  const TopOpeBRepDS_Kind        GK,
  const bool                     B); // G is a vertex (or not) of the interference master
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int                      S,  // curve/edge index
  const int                      G,  // point/vertex index
  const double                   P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK, // POINT/VERTEX
  const TopOpeBRepDS_Kind        SK,
  const bool                     B); // G is a vertex (or not) of the interference master
Standard_EXPORT bool FUN_hasStateShape(const TopOpeBRepDS_Transition& T,
                                       const TopAbs_State             state,
                                       const TopAbs_ShapeEnum         shape);
Standard_EXPORT int  FUN_selectTRASHAinterference(
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const TopAbs_ShapeEnum                                    sha,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectITRASHAinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const int                                                 Index,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectTRAUNKinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectTRAORIinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const TopAbs_Orientation                                  O,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectGKinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const TopOpeBRepDS_Kind                                   GK,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectSKinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const TopOpeBRepDS_Kind                                   SK,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectGIinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const int                                                 GI,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT int FUN_selectSIinterference(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
  const int                                                 SI,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2);
Standard_EXPORT bool FUN_interfhassupport(const TopOpeBRepDS_DataStructure&             DS,
                                          const occ::handle<TopOpeBRepDS_Interference>& I,
                                          const TopoDS_Shape&                           S);
Standard_EXPORT bool FUN_transitionEQUAL(const TopOpeBRepDS_Transition&,
                                         const TopOpeBRepDS_Transition&);
Standard_EXPORT bool FUN_transitionSTATEEQUAL(const TopOpeBRepDS_Transition&,
                                              const TopOpeBRepDS_Transition&);
Standard_EXPORT bool FUN_transitionSHAPEEQUAL(const TopOpeBRepDS_Transition&,
                                              const TopOpeBRepDS_Transition&);
Standard_EXPORT bool FUN_transitionINDEXEQUAL(const TopOpeBRepDS_Transition&,
                                              const TopOpeBRepDS_Transition&);
Standard_EXPORT void FUN_reducedoublons(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
  const TopOpeBRepDS_DataStructure&                         BDS,
  const int                                                 SIX);
Standard_EXPORT void FUN_unkeepUNKNOWN(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                                       TopOpeBRepDS_DataStructure& BDS,
                                       const int                   SIX);
Standard_EXPORT int  FUN_select2dI(const int                                                 SIX,
                                   TopOpeBRepDS_DataStructure&                               BDS,
                                   const TopAbs_ShapeEnum                                    TRASHAk,
                                   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lI,
                                   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l2dI);
Standard_EXPORT int  FUN_selectpure2dI(
   const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lF,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lFE,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       l2dFE);
Standard_EXPORT int  FUN_select1dI(const int                                                 SIX,
                                   TopOpeBRepDS_DataStructure&                               BDS,
                                   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                                   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l1dI);
Standard_EXPORT void FUN_select3dinterference(
  const int                                                 SIX,
  TopOpeBRepDS_DataStructure&                               BDS,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFEresi,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFEresi,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l2dFE);
#endif
