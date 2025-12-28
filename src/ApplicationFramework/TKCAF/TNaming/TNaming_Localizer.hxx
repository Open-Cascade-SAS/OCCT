// Created on: 1997-06-10
// Created by: Yves FRICAUD
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

#ifndef _TNaming_Localizer_HeaderFile
#define _TNaming_Localizer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_List.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_Map.hxx>
class TNaming_UsedShapes;
class TopoDS_Shape;
class TDF_Label;
class TNaming_NamedShape;

class TNaming_Localizer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TNaming_Localizer();

  Standard_EXPORT void Init(const occ::handle<TNaming_UsedShapes>& US, const int CurTrans);

  Standard_EXPORT const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& SubShapes(const TopoDS_Shape&    S,
                                                       const TopAbs_ShapeEnum Type);

  Standard_EXPORT const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Ancestors(
    const TopoDS_Shape&    S,
    const TopAbs_ShapeEnum Type);

  Standard_EXPORT void FindFeaturesInAncestors(const TopoDS_Shape&  S,
                                               const TopoDS_Shape&  In,
                                               NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& AncInFeatures);

  Standard_EXPORT void GoBack(const TopoDS_Shape&       S,
                              const TDF_Label&          Lab,
                              const TNaming_Evolution   Evol,
                              NCollection_List<TopoDS_Shape>&     OldS,
                              NCollection_List<occ::handle<TNaming_NamedShape>>& OldLab);

  Standard_EXPORT void Backward(const occ::handle<TNaming_NamedShape>& NS,
                                const TopoDS_Shape&               S,
                                NCollection_Map<occ::handle<TNaming_NamedShape>>&          Primitives,
                                NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              ValidShapes);

  Standard_EXPORT void FindNeighbourg(const TopoDS_Shape&  Cont,
                                      const TopoDS_Shape&  S,
                                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Neighbourg);

  Standard_EXPORT static bool IsNew(const TopoDS_Shape&               S,
                                                const occ::handle<TNaming_NamedShape>& NS);

  Standard_EXPORT static void FindGenerator(const occ::handle<TNaming_NamedShape>& NS,
                                            const TopoDS_Shape&               S,
                                            NCollection_List<TopoDS_Shape>&             theListOfGenerators);

  //! Finds context of the shape <S>.
  Standard_EXPORT static void FindShapeContext(const occ::handle<TNaming_NamedShape>& NS,
                                               const TopoDS_Shape&               theS,
                                               TopoDS_Shape&                     theSC);

private:
  int                               myCurTrans;
  occ::handle<TNaming_UsedShapes>                     myUS;
  NCollection_List<TopoDS_Shape>                           myShapeWithSubShapes;
  NCollection_List<NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>>                       mySubShapes;
  NCollection_List<TopoDS_Shape>                           myShapeWithAncestors;
  NCollection_List<NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>> myAncestors;
};

#endif // _TNaming_Localizer_HeaderFile
