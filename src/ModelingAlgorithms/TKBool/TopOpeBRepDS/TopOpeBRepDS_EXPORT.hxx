// Created on: 1997-12-15
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

#ifndef _TopOpeBRepDS_EXPORT_HeaderFile
#define _TopOpeBRepDS_EXPORT_HeaderFile

#include <TopOpeBRepDS_define.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
// TopOpeBRepDS_redu.cxx
Standard_EXPORT void FUN_scanloi(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lII,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lFOR,
  int&                                                            FOR,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lREV,
  int&                                                            REV,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lINT,
  int&                                                            INT);
Standard_EXPORT bool FUN_ds_redu2d1d(
  const TopOpeBRepDS_DataStructure&                               BDS,
  const int                                                       ISE,
  const occ::handle<TopOpeBRepDS_Interference>&                   I2d,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l1d,
  TopOpeBRepDS_Transition&                                        newT2d);
Standard_EXPORT bool FUN_ds_GetTr(
  const TopOpeBRepDS_DataStructure&                               BDS,
  const int                                                       ISE,
  const int                                                       G,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIG,
  TopAbs_State&                                                   stb,
  int&                                                            isb,
  int&                                                            bdim,
  TopAbs_State&                                                   sta,
  int&                                                            isa,
  int&                                                            adim);
// TopOpeBRepDS_EXPORT.cxx
Standard_EXPORT void FDS_SetT(TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Transition& T0);
Standard_EXPORT bool FDS_hasUNK(const TopOpeBRepDS_Transition& T);
Standard_EXPORT void FDS_copy(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                              NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       LII);
Standard_EXPORT void FDS_copy(const NCollection_List<TopoDS_Shape>& LI,
                              NCollection_List<TopoDS_Shape>&       LII);
Standard_EXPORT void FDS_assign(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                                NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII);
Standard_EXPORT void FDS_assign(const NCollection_List<TopoDS_Shape>& LI,
                                NCollection_List<TopoDS_Shape>&       LII);
Standard_EXPORT void FUN_ds_samRk(const TopOpeBRepDS_DataStructure& BDS,
                                  const int                         Rk,
                                  NCollection_List<TopoDS_Shape>&   LI,
                                  NCollection_List<TopoDS_Shape>&   LIsrk);
Standard_EXPORT void FDS_data(const occ::handle<TopOpeBRepDS_Interference>& I,
                              TopOpeBRepDS_Kind&                            GT1,
                              int&                                          G1,
                              TopOpeBRepDS_Kind&                            ST1,
                              int&                                          S1);
Standard_EXPORT bool FDS_data(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& it,
  occ::handle<TopOpeBRepDS_Interference>&                                   I,
  TopOpeBRepDS_Kind&                                                        GT1,
  int&                                                                      G1,
  TopOpeBRepDS_Kind&                                                        ST1,
  int&                                                                      S1);
Standard_EXPORT void FDS_Tdata(const occ::handle<TopOpeBRepDS_Interference>& I,
                               TopAbs_ShapeEnum&                             SB,
                               int&                                          IB,
                               TopAbs_ShapeEnum&                             SA,
                               int&                                          IA);
Standard_EXPORT void FDS_Idata(const occ::handle<TopOpeBRepDS_Interference>& I,
                               TopAbs_ShapeEnum&                             SB,
                               int&                                          IB,
                               TopAbs_ShapeEnum&                             SA,
                               int&                                          IA,
                               TopOpeBRepDS_Kind&                            GT1,
                               int&                                          G1,
                               TopOpeBRepDS_Kind&                            ST1,
                               int&                                          S1);
Standard_EXPORT bool FUN_ds_getVsdm(const TopOpeBRepDS_DataStructure& BDS,
                                    const int                         iV,
                                    int&                              iVsdm);
Standard_EXPORT bool FUN_ds_sdm(const TopOpeBRepDS_DataStructure& BDS,
                                const TopoDS_Shape&               s1,
                                const TopoDS_Shape&               s2);

Standard_EXPORT bool FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                   const TopoDS_Shape&               ES,
                                   const TopoDS_Shape&               F1,
                                   const TopoDS_Shape&               F2);
Standard_EXPORT bool FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                   const int                         SI,
                                   const int                         isb1,
                                   const int                         isb2);
// clang-format off
Standard_EXPORT bool FDS_EdgeIsConnexToSameDomainFaces(const TopoDS_Shape& E,const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);  // not used
// clang-format on
Standard_EXPORT bool   FDS_SIisGIofIofSBAofTofI(const TopOpeBRepDS_DataStructure&             BDS,
                                                const int                                     SI,
                                                const occ::handle<TopOpeBRepDS_Interference>& I);
Standard_EXPORT double FDS_Parameter(const occ::handle<TopOpeBRepDS_Interference>& I);
Standard_EXPORT bool   FDS_Parameter(const occ::handle<TopOpeBRepDS_Interference>& I, double& par);
Standard_EXPORT bool   FDS_HasSameDomain3d(const TopOpeBRepDS_DataStructure& BDS,
                                           const TopoDS_Shape&               E,
                                           NCollection_List<TopoDS_Shape>*   PLSD = NULL);
Standard_EXPORT bool   FDS_Config3d(const TopoDS_Shape&  E1,
                                    const TopoDS_Shape&  E2,
                                    TopOpeBRepDS_Config& c);
Standard_EXPORT bool   FDS_HasSameDomain2d(const TopOpeBRepDS_DataStructure& BDS,
                                           const TopoDS_Shape&               E,
                                           NCollection_List<TopoDS_Shape>*   PLSD = NULL);
Standard_EXPORT void   FDS_getupperlower(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                         const int                                       edgeIndex,
                                         const double                                    paredge,
                                         double&                                         p1,
                                         double&                                         p2);
Standard_EXPORT bool   FUN_ds_getoov(const TopoDS_Shape&               v,
                                     const TopOpeBRepDS_DataStructure& BDS,
                                     TopoDS_Shape&                     oov);
Standard_EXPORT bool   FUN_ds_getoov(const TopoDS_Shape&                             v,
                                     const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                     TopoDS_Shape&                                   oov);
Standard_EXPORT bool   FUN_selectTRAINTinterference(
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& li,
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       liINTERNAL);
Standard_EXPORT void FUN_ds_completeforSE1(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE2(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE3(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE4(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE5(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE6(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
// Standard_EXPORT void FUN_ds_completeforSE7(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforE7(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE8(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
// Standard_EXPORT void FUN_ds_completeFEIGb1(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_PURGEforE9(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_completeforSE9(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_complete1dForSESDM(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_redusamsha(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT bool FUN_ds_shareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                   const int                                       iF1,
                                   const int                                       iF2,
                                   const int                                       iE2,
                                   const TopoDS_Edge&                              Esp,
                                   bool&                                           shareG);
Standard_EXPORT bool FUN_ds_mkTonFsdm(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const int                                       iF1,
                                      const int                                       iF2,
                                      const int                                       iE2,
                                      const int                                       iEG,
                                      const double                                    paronEG,
                                      const TopoDS_Edge&                              Esp,
                                      const bool                                      pardef,
                                      TopOpeBRepDS_Transition&                        T);
Standard_EXPORT int  FUN_ds_oriEinF(const TopOpeBRepDS_DataStructure& BDS,
                                    const TopoDS_Edge&                E,
                                    const TopoDS_Shape&               F,
                                    TopAbs_Orientation&               O);
Standard_EXPORT void FUN_ds_FillSDMFaces(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT void FUN_ds_addSEsdm1d(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT int  FUN_ds_hasI2d(
   const int                                                       EIX,
   const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       LI2d);
Standard_EXPORT void FUN_ds_PointToVertex(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);
Standard_EXPORT bool FUN_ds_hasFEI(const TopOpeBRepDS_PDataStructure& pDS2d,
                                   const TopoDS_Shape&                F,
                                   const int                          GI,
                                   const int                          ITRA);
Standard_EXPORT bool FUN_ds_ONesd(const TopOpeBRepDS_DataStructure& BDS,
                                  const int                         IE,
                                  const TopoDS_Shape&               EspON,
                                  int&                              IEsd);

Standard_EXPORT bool FDS_stateEwithF2d(const TopOpeBRepDS_DataStructure& BDS,
                                       const TopoDS_Edge&                E,
                                       const double                      pE,
                                       const TopOpeBRepDS_Kind           KDS,
                                       const int                         GDS,
                                       const TopoDS_Face&                F1,
                                       TopOpeBRepDS_Transition&          TrmemeS);
Standard_EXPORT bool FDS_parbefaft(const TopOpeBRepDS_DataStructure& BDS,
                                   const TopoDS_Edge&                E,
                                   const double                      pE,
                                   const double&                     pbef,
                                   const double&                     paft,
                                   const bool&                       isonboundper,
                                   double&                           p1,
                                   double&                           p2);
Standard_EXPORT bool FDS_LOIinfsup(
  const TopOpeBRepDS_DataStructure&                               BDS,
  const TopoDS_Edge&                                              E,
  const double                                                    pE,
  const TopOpeBRepDS_Kind                                         KDS,
  const int                                                       GDS,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI,
  double&                                                         pbef,
  double&                                                         paft,
  bool&                                                           isonboundper);
Standard_EXPORT void FUN_ds_FEIGb1TO0(occ::handle<TopOpeBRepDS_HDataStructure>&           HDS,
                                      const NCollection_DataMap<TopoDS_Shape,
                                                                TopOpeBRepDS_ListOfShapeOn1State,
                                                                TopTools_ShapeMapHasher>& MEspON);
#endif
