// Created on: 1997-04-21
// Created by: Prestataire Mary FABIEN
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

#ifndef _TopOpeBRepDS_Filter_HeaderFile
#define _TopOpeBRepDS_Filter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_HDataStructure;

class TopOpeBRepDS_Filter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_Filter(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const TopOpeBRepTool_PShapeClassifier& pClassif = nullptr);

  Standard_EXPORT void ProcessInterferences();

  Standard_EXPORT void ProcessFaceInterferences(
    const NCollection_DataMap<TopoDS_Shape,
                              TopOpeBRepDS_ListOfShapeOn1State,
                              TopTools_ShapeMapHasher>& MEsp);

  Standard_EXPORT void ProcessFaceInterferences(
    const int                                           I,
    const NCollection_DataMap<TopoDS_Shape,
                              TopOpeBRepDS_ListOfShapeOn1State,
                              TopTools_ShapeMapHasher>& MEsp);

  Standard_EXPORT void ProcessEdgeInterferences();

  Standard_EXPORT void ProcessEdgeInterferences(const int I);

  Standard_EXPORT void ProcessCurveInterferences();

  Standard_EXPORT void ProcessCurveInterferences(const int I);

private:
  occ::handle<TopOpeBRepDS_HDataStructure> myHDS;
  TopOpeBRepTool_PShapeClassifier          myPShapeClassif;
};

#endif // _TopOpeBRepDS_Filter_HeaderFile
