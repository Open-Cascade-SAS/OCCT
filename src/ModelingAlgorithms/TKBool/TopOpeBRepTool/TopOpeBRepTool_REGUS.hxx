// Created on: 1999-01-04
// Created by: Xuan PHAM PHU
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_REGUS_HeaderFile
#define _TopOpeBRepTool_REGUS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
class TopoDS_Face;
class TopoDS_Edge;

class TopOpeBRepTool_REGUS
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepTool_REGUS();

  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT const TopoDS_Shape& S() const;

  Standard_EXPORT bool MapS();

  Standard_EXPORT static bool WireToFace(const TopoDS_Face&                    Fanc,
                                         const NCollection_List<TopoDS_Shape>& nWs,
                                         NCollection_List<TopoDS_Shape>&       nFs);

  Standard_EXPORT static bool SplitF(const TopoDS_Face&              Fanc,
                                     NCollection_List<TopoDS_Shape>& FSplits);

  Standard_EXPORT bool SplitFaces();

  Standard_EXPORT bool REGU();

  Standard_EXPORT void SetFsplits(
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      Fsplits);

  Standard_EXPORT void GetFsplits(
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      Fsplits) const;

  Standard_EXPORT void SetOshNsh(
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      OshNsh);

  Standard_EXPORT void GetOshNsh(
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      OshNsh) const;

  Standard_EXPORT bool InitBlock();

  Standard_EXPORT bool NextinBlock();

  Standard_EXPORT bool NearestF(const TopoDS_Edge&                    e,
                                const NCollection_List<TopoDS_Shape>& lof,
                                TopoDS_Face&                          ffound) const;

private:
  bool hasnewsplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myFsplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
               myOshNsh;
  TopoDS_Shape myS;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mymapeFsstatic;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                mymapeFs;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mymapemult;
  int                                                           mynF;
  int                                                           myoldnF;
  TopoDS_Shape                                                  myf;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        myedstoconnect;
  NCollection_List<TopoDS_Shape>                                mylFinBlock;
};

#endif // _TopOpeBRepTool_REGUS_HeaderFile
