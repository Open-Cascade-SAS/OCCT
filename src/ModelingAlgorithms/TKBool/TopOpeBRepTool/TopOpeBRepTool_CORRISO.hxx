// Created on: 1998-11-25
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_CORRISO_HeaderFile
#define _TopOpeBRepTool_CORRISO_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
class TopoDS_Edge;
class TopOpeBRepTool_C2DF;
class TopoDS_Vertex;

//! Fref is built on x-periodic surface (x=u,v).
//! S built on Fref's geometry, should be UVClosed.
//!
//! Give us E, an edge of S. 2drep(E) is not UV connexed.
//! We translate 2drep(E) in xdir*xperiod if necessary.
//!
//! call : TopOpeBRepTool_CORRISO Tool(Fref);
//! Tool.Init(S);
//! if (!Tool.UVClosed()) {
//! // initialize EdsToCheck,nfybounds,stopatfirst
//!
//! Tool.EdgeWithFaultyUV(EdsToCheck,nfybounds,FyEds,stopatfirst);
//! if (Tool.SetUVClosed()) S = Tool.GetnewS();
//! }
class TopOpeBRepTool_CORRISO
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_CORRISO();

  Standard_EXPORT TopOpeBRepTool_CORRISO(const TopoDS_Face& FRef);

  Standard_EXPORT const TopoDS_Face& Fref() const;

  Standard_EXPORT const GeomAdaptor_Surface& GASref() const;

  Standard_EXPORT bool Refclosed(const int x,
                                             double&         xperiod) const;

  Standard_EXPORT bool Init(const TopoDS_Shape& S);

  Standard_EXPORT const TopoDS_Shape& S() const;

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Eds() const;

  Standard_EXPORT bool UVClosed() const;

  Standard_EXPORT double Tol(const int I, const double tol3d) const;

  Standard_EXPORT bool PurgeFyClosingE(const NCollection_List<TopoDS_Shape>& ClEds,
                                                   NCollection_List<TopoDS_Shape>&       fyClEds) const;

  Standard_EXPORT int EdgeOUTofBoundsUV(const TopoDS_Edge&     E,
                                                     const bool onU,
                                                     const double    tolx,
                                                     double&         parspE) const;

  Standard_EXPORT bool
    EdgesOUTofBoundsUV(const NCollection_List<TopoDS_Shape>&             EdsToCheck,
                       const bool                  onU,
                       const double                     tolx,
                       NCollection_DataMap<TopoDS_Shape, int>& FyEds) const;

  Standard_EXPORT bool EdgeWithFaultyUV(const TopoDS_Edge& E,
                                                    int&  Ivfaulty) const;

  Standard_EXPORT bool
    EdgesWithFaultyUV(const NCollection_List<TopoDS_Shape>&             EdsToCheck,
                      const int                  nfybounds,
                      NCollection_DataMap<TopoDS_Shape, int>& FyEds,
                      const bool                  stopatfirst = false) const;

  Standard_EXPORT bool EdgeWithFaultyUV(const NCollection_List<TopoDS_Shape>& EdsToCheck,
                                                    const int      nfybounds,
                                                    TopoDS_Shape&               fyE,
                                                    int&           Ifaulty) const;

  Standard_EXPORT bool TrslUV(const bool                        onU,
                                          const NCollection_DataMap<TopoDS_Shape, int>& FyEds);

  Standard_EXPORT bool GetnewS(TopoDS_Face& newS) const;

  Standard_EXPORT bool UVRep(const TopoDS_Edge& E, TopOpeBRepTool_C2DF& C2DF) const;

  Standard_EXPORT bool SetUVRep(const TopoDS_Edge& E, const TopOpeBRepTool_C2DF& C2DF);

  Standard_EXPORT bool Connexity(const TopoDS_Vertex&  V,
                                             NCollection_List<TopoDS_Shape>& Eds) const;

  Standard_EXPORT bool SetConnexity(const TopoDS_Vertex&        V,
                                                const NCollection_List<TopoDS_Shape>& Eds);

  Standard_EXPORT bool AddNewConnexity(const TopoDS_Vertex& V, const TopoDS_Edge& E);

  Standard_EXPORT bool RemoveOldConnexity(const TopoDS_Vertex& V, const TopoDS_Edge& E);

private:
  TopoDS_Face                               myFref;
  GeomAdaptor_Surface                       myGAS;
  bool                          myUclosed;
  bool                          myVclosed;
  double                             myUper;
  double                             myVper;
  TopoDS_Shape                              myS;
  NCollection_List<TopoDS_Shape>                      myEds;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_C2DF> myERep2d;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>        myVEds;
};

#endif // _TopOpeBRepTool_CORRISO_HeaderFile
