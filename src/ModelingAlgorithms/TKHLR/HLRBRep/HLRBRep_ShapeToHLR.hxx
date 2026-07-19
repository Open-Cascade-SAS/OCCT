// Created on: 1993-05-04
// Created by: Modelistation
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

#ifndef _HLRBRep_ShapeToHLR_HeaderFile
#define _HLRBRep_ShapeToHLR_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedMap.hxx>
class HLRBRep_Data;
class HLRTopoBRep_OutLiner;
class HLRAlgo_Projector;
class TopoDS_Face;

//! compute the OutLinedShape of a Shape with an
//! OutLiner, a Projector and create the Data
//! Structure of a Shape.
class HLRBRep_ShapeToHLR
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a DataStructure containing the OutLiner
  //! <S> depending on the projector <P> and nbIso.
  Standard_EXPORT static occ::handle<HLRBRep_Data> Load(
    const occ::handle<HLRTopoBRep_OutLiner>&                                         S,
    const HLRAlgo_Projector&                                                         P,
    NCollection_DataMap<TopoDS_Shape, BRepTopAdaptor_Tool, TopTools_ShapeMapHasher>& MST,
    const int                                                                        nbIso = 0);

private:
  Standard_EXPORT static void ExploreFace(
    const occ::handle<HLRTopoBRep_OutLiner>&                             S,
    const occ::handle<HLRBRep_Data>&                                     DS,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& FM,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& EM,
    int&                                                                 i,
    const TopoDS_Face&                                                   F,
    const bool                                                           closed);

  Standard_EXPORT static void ExploreShape(
    const occ::handle<HLRTopoBRep_OutLiner>&                             S,
    const occ::handle<HLRBRep_Data>&                                     DS,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& FM,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& EM);
};

#endif // _HLRBRep_ShapeToHLR_HeaderFile
