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
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
// TopOpeBRepDS_redu.cxx
Standard_EXPORT void             FUN_scanloi(const TopOpeBRepDS_ListOfInterference& lII,
                                             TopOpeBRepDS_ListOfInterference&       lFOR,
                                             Standard_Integer&                      FOR,
                                             TopOpeBRepDS_ListOfInterference&       lREV,
                                             Standard_Integer&                      REV,
                                             TopOpeBRepDS_ListOfInterference&       lINT,
                                             Standard_Integer&                      INT);
Standard_EXPORT Standard_Boolean FUN_ds_redu2d1d(const TopOpeBRepDS_DataStructure&        BDS,
                                                 const Standard_Integer                   ISE,
                                                 const Handle(TopOpeBRepDS_Interference)& I2d,
                                                 const TopOpeBRepDS_ListOfInterference&   l1d,
                                                 TopOpeBRepDS_Transition&                 newT2d);
Standard_EXPORT Standard_Boolean FUN_ds_GetTr(const TopOpeBRepDS_DataStructure&      BDS,
                                              const Standard_Integer                 ISE,
                                              const Standard_Integer                 G,
                                              const TopOpeBRepDS_ListOfInterference& LIG,
                                              TopAbs_State&                          stb,
                                              Standard_Integer&                      isb,
                                              Standard_Integer&                      bdim,
                                              TopAbs_State&                          sta,
                                              Standard_Integer&                      isa,
                                              Standard_Integer&                      adim);
// TopOpeBRepDS_EXPORT.cxx
Standard_EXPORT void FDS_SetT(TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Transition& T0);
Standard_EXPORT Standard_Boolean FDS_hasUNK(const TopOpeBRepDS_Transition& T);
Standard_EXPORT void             FDS_copy(const TopOpeBRepDS_ListOfInterference& LI,
                                          TopOpeBRepDS_ListOfInterference&       LII);
Standard_EXPORT void FDS_copy(const TopTools_ListOfShape& LI, TopTools_ListOfShape& LII);
Standard_EXPORT void FDS_assign(const TopOpeBRepDS_ListOfInterference& LI,
                                TopOpeBRepDS_ListOfInterference&       LII);
Standard_EXPORT void FDS_assign(const TopTools_ListOfShape& LI, TopTools_ListOfShape& LII);
Standard_EXPORT void FUN_ds_samRk(const TopOpeBRepDS_DataStructure& BDS,
                                  const Standard_Integer            Rk,
                                  TopTools_ListOfShape&             LI,
                                  TopTools_ListOfShape&             LIsrk);
Standard_EXPORT void FDS_data(const Handle(TopOpeBRepDS_Interference)& I,
                              TopOpeBRepDS_Kind&                       GT1,
                              Standard_Integer&                        G1,
                              TopOpeBRepDS_Kind&                       ST1,
                              Standard_Integer&                        S1);
Standard_EXPORT Standard_Boolean FDS_data(const TopOpeBRepDS_ListIteratorOfListOfInterference& it,
                                          Handle(TopOpeBRepDS_Interference)&                   I,
                                          TopOpeBRepDS_Kind&                                   GT1,
                                          Standard_Integer&                                    G1,
                                          TopOpeBRepDS_Kind&                                   ST1,
                                          Standard_Integer&                                    S1);
Standard_EXPORT void             FDS_Tdata(const Handle(TopOpeBRepDS_Interference)& I,
                                           TopAbs_ShapeEnum&                        SB,
                                           Standard_Integer&                        IB,
                                           TopAbs_ShapeEnum&                        SA,
                                           Standard_Integer&                        IA);
Standard_EXPORT void             FDS_Idata(const Handle(TopOpeBRepDS_Interference)& I,
                                           TopAbs_ShapeEnum&                        SB,
                                           Standard_Integer&                        IB,
                                           TopAbs_ShapeEnum&                        SA,
                                           Standard_Integer&                        IA,
                                           TopOpeBRepDS_Kind&                       GT1,
                                           Standard_Integer&                        G1,
                                           TopOpeBRepDS_Kind&                       ST1,
                                           Standard_Integer&                        S1);
Standard_EXPORT Standard_Boolean FUN_ds_getVsdm(const TopOpeBRepDS_DataStructure& BDS,
                                                const Standard_Integer            iV,
                                                Standard_Integer&                 iVsdm);
Standard_EXPORT Standard_Boolean FUN_ds_sdm(const TopOpeBRepDS_DataStructure& BDS,
                                            const TopoDS_Shape&               s1,
                                            const TopoDS_Shape&               s2);

Standard_EXPORT Standard_Boolean FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                               const TopoDS_Shape&               ES,
                                               const TopoDS_Shape&               F1,
                                               const TopoDS_Shape&               F2);
Standard_EXPORT Standard_Boolean FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                               const Standard_Integer            SI,
                                               const Standard_Integer            isb1,
                                               const Standard_Integer            isb2);
// clang-format off
Standard_EXPORT Standard_Boolean FDS_EdgeIsConnexToSameDomainFaces(const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS);  // not used
// clang-format on
Standard_EXPORT Standard_Boolean
                                 FDS_SIisGIofIofSBAofTofI(const TopOpeBRepDS_DataStructure&        BDS,
                                                          const Standard_Integer                   SI,
                                                          const Handle(TopOpeBRepDS_Interference)& I);
Standard_EXPORT Standard_Real    FDS_Parameter(const Handle(TopOpeBRepDS_Interference)& I);
Standard_EXPORT Standard_Boolean FDS_Parameter(const Handle(TopOpeBRepDS_Interference)& I,
                                               Standard_Real&                           par);
Standard_EXPORT Standard_Boolean FDS_HasSameDomain3d(const TopOpeBRepDS_DataStructure& BDS,
                                                     const TopoDS_Shape&               E,
                                                     TopTools_ListOfShape*             PLSD = NULL);
Standard_EXPORT Standard_Boolean FDS_Config3d(const TopoDS_Shape&  E1,
                                              const TopoDS_Shape&  E2,
                                              TopOpeBRepDS_Config& c);
Standard_EXPORT Standard_Boolean FDS_HasSameDomain2d(const TopOpeBRepDS_DataStructure& BDS,
                                                     const TopoDS_Shape&               E,
                                                     TopTools_ListOfShape*             PLSD = NULL);
Standard_EXPORT void             FDS_getupperlower(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
                                                   const Standard_Integer                     edgeIndex,
                                                   const Standard_Real                        paredge,
                                                   Standard_Real&                             p1,
                                                   Standard_Real&                             p2);
Standard_EXPORT Standard_Boolean FUN_ds_getoov(const TopoDS_Shape&               v,
                                               const TopOpeBRepDS_DataStructure& BDS,
                                               TopoDS_Shape&                     oov);
Standard_EXPORT Standard_Boolean FUN_ds_getoov(const TopoDS_Shape&                        v,
                                               const Handle(TopOpeBRepDS_HDataStructure)& HDS,
                                               TopoDS_Shape&                              oov);
Standard_EXPORT Standard_Boolean
                     FUN_selectTRAINTinterference(const TopOpeBRepDS_ListOfInterference& li,
                                                  TopOpeBRepDS_ListOfInterference&       liINTERNAL);
Standard_EXPORT void FUN_ds_completeforSE1(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE2(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE3(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE4(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE5(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE6(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
// Standard_EXPORT void FUN_ds_completeforSE7(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforE7(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE8(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
// Standard_EXPORT void FUN_ds_completeFEIGb1(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_PURGEforE9(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_completeforSE9(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_complete1dForSESDM(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_redusamsha(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT Standard_Boolean FUN_ds_shareG(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
                                               const Standard_Integer                     iF1,
                                               const Standard_Integer                     iF2,
                                               const Standard_Integer                     iE2,
                                               const TopoDS_Edge&                         Esp,
                                               Standard_Boolean&                          shareG);
Standard_EXPORT Standard_Boolean FUN_ds_mkTonFsdm(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
                                                  const Standard_Integer                     iF1,
                                                  const Standard_Integer                     iF2,
                                                  const Standard_Integer                     iE2,
                                                  const Standard_Integer                     iEG,
                                                  const Standard_Real      paronEG,
                                                  const TopoDS_Edge&       Esp,
                                                  const Standard_Boolean   pardef,
                                                  TopOpeBRepDS_Transition& T);
Standard_EXPORT Standard_Integer FUN_ds_oriEinF(const TopOpeBRepDS_DataStructure& BDS,
                                                const TopoDS_Edge&                E,
                                                const TopoDS_Shape&               F,
                                                TopAbs_Orientation&               O);
Standard_EXPORT void FUN_ds_FillSDMFaces(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FUN_ds_addSEsdm1d(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT Standard_Integer FUN_ds_hasI2d(const Standard_Integer                 EIX,
                                               const TopOpeBRepDS_ListOfInterference& LI,
                                               TopOpeBRepDS_ListOfInterference&       LI2d);
Standard_EXPORT void FUN_ds_PointToVertex(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT Standard_Boolean FUN_ds_hasFEI(const TopOpeBRepDS_PDataStructure& pDS2d,
                                               const TopoDS_Shape&                F,
                                               const Standard_Integer             GI,
                                               const Standard_Integer             ITRA);
Standard_EXPORT Standard_Boolean FUN_ds_ONesd(const TopOpeBRepDS_DataStructure& BDS,
                                              const Standard_Integer            IE,
                                              const TopoDS_Shape&               EspON,
                                              Standard_Integer&                 IEsd);

Standard_EXPORT Standard_Boolean FDS_stateEwithF2d(const TopOpeBRepDS_DataStructure& BDS,
                                                   const TopoDS_Edge&                E,
                                                   const Standard_Real               pE,
                                                   const TopOpeBRepDS_Kind           KDS,
                                                   const Standard_Integer            GDS,
                                                   const TopoDS_Face&                F1,
                                                   TopOpeBRepDS_Transition&          TrmemeS);
Standard_EXPORT Standard_Boolean FDS_parbefaft(const TopOpeBRepDS_DataStructure& BDS,
                                               const TopoDS_Edge&                E,
                                               const Standard_Real               pE,
                                               const Standard_Real&              pbef,
                                               const Standard_Real&              paft,
                                               const Standard_Boolean&           isonboundper,
                                               Standard_Real&                    p1,
                                               Standard_Real&                    p2);
Standard_EXPORT Standard_Boolean FDS_LOIinfsup(const TopOpeBRepDS_DataStructure&      BDS,
                                               const TopoDS_Edge&                     E,
                                               const Standard_Real                    pE,
                                               const TopOpeBRepDS_Kind                KDS,
                                               const Standard_Integer                 GDS,
                                               const TopOpeBRepDS_ListOfInterference& LOI,
                                               Standard_Real&                         pbef,
                                               Standard_Real&                         paft,
                                               Standard_Boolean&                      isonboundper);
Standard_EXPORT void             FUN_ds_FEIGb1TO0(Handle(TopOpeBRepDS_HDataStructure)&                  HDS,
                                                  const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEspON);
#endif
