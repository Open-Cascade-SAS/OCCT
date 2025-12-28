// Created on: 1998-12-08
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

#ifndef _TopOpeBRepTool_REGUW_HeaderFile
#define _TopOpeBRepTool_REGUW_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_connexity.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt2d.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Dir2d.hxx>
class TopoDS_Face;
class TopOpeBRepTool_connexity;

class TopOpeBRepTool_REGUW
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_REGUW(const TopoDS_Face& FRef);

  Standard_EXPORT const TopoDS_Face& Fref() const;

  Standard_EXPORT void SetEsplits(NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Esplits);

  Standard_EXPORT void GetEsplits(NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Esplits) const;

  Standard_EXPORT void SetOwNw(NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& OwNw);

  Standard_EXPORT void GetOwNw(NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& OwNw) const;

  Standard_EXPORT bool SplitEds();

  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT const TopoDS_Shape& S() const;

  Standard_EXPORT bool HasInit() const;

  Standard_EXPORT bool MapS();

  Standard_EXPORT bool REGU(const int istep,
                                        const TopoDS_Shape&    Scur,
                                        NCollection_List<TopoDS_Shape>&  Splits);

  Standard_EXPORT bool REGU();

  Standard_EXPORT bool GetSplits(NCollection_List<TopoDS_Shape>& Splits) const;

  Standard_EXPORT bool InitBlock();

  Standard_EXPORT bool NextinBlock();

  Standard_EXPORT bool NearestE(const NCollection_List<TopoDS_Shape>& loe,
                                            TopoDS_Edge&                efound) const;

  Standard_EXPORT bool Connexity(const TopoDS_Vertex&      v,
                                             TopOpeBRepTool_connexity& co) const;

  Standard_EXPORT bool AddNewConnexity(const TopoDS_Vertex&   v,
                                                   const int OriKey,
                                                   const TopoDS_Edge&     e);

  Standard_EXPORT bool RemoveOldConnexity(const TopoDS_Vertex&   v,
                                                      const int OriKey,
                                                      const TopoDS_Edge&     e);

  Standard_EXPORT bool UpdateMultiple(const TopoDS_Vertex& v);

private:
  Standard_EXPORT void InitStep(const TopoDS_Shape& S);

  TopoDS_Shape                                  myS;
  TopOpeBRepTool_CORRISO                        myCORRISO;
  bool                              hasnewsplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>            myEsplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>            myOwNw;
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepTool_connexity, TopTools_ShapeMapHasher> mymapvEds;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                           mymapvmultiple;
  NCollection_List<TopoDS_Shape>                          myListVmultiple;
  int                              iStep;
  double                                 mytol2d;
  bool                              isinit0;
  TopoDS_Vertex                                 myv0;
  gp_Pnt2d                                      myp2d0;
  TopoDS_Vertex                                 myv;
  TopoDS_Edge                                   myed;
  gp_Pnt2d                                      myp2d;
  gp_Dir2d                                      mytg2d;
};

#endif // _TopOpeBRepTool_REGUW_HeaderFile
