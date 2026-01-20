// Created on: 1999-11-29
// Created by: Peter KURNEV
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

#ifndef _TopOpeBRepBuild_VertexInfo_HeaderFile
#define _TopOpeBRepBuild_VertexInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Integer.hxx>

class TopOpeBRepBuild_VertexInfo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_VertexInfo();

  Standard_EXPORT void SetVertex(const TopoDS_Vertex& aV);

  Standard_EXPORT const TopoDS_Vertex& Vertex() const;

  Standard_EXPORT void SetSmart(const bool aFlag);

  Standard_EXPORT bool Smart() const;

  Standard_EXPORT int NbCases() const;

  Standard_EXPORT int FoundOut() const;

  Standard_EXPORT void AddIn(const TopoDS_Edge& anE);

  Standard_EXPORT void AddOut(const TopoDS_Edge& anE);

  Standard_EXPORT void SetCurrentIn(const TopoDS_Edge& anE);

  Standard_EXPORT const NCollection_IndexedMap<TopoDS_Shape>& EdgesIn() const;

  Standard_EXPORT const NCollection_IndexedMap<TopoDS_Shape>& EdgesOut() const;

  Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape>& ChangeEdgesOut();

  Standard_EXPORT void Dump() const;

  Standard_EXPORT const TopoDS_Edge& CurrentOut();

  Standard_EXPORT void AppendPassed(const TopoDS_Edge& anE);

  Standard_EXPORT void RemovePassed();

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& ListPassed() const;

  Standard_EXPORT void Prepare(const NCollection_List<TopoDS_Shape>& aL);

private:
  TopoDS_Vertex                      myVertex;
  TopoDS_Edge                        myCurrent;
  TopoDS_Edge                        myCurrentIn;
  bool                   mySmart;
  NCollection_IndexedMap<TopoDS_Shape> myEdgesIn;
  NCollection_IndexedMap<TopoDS_Shape> myEdgesOut;
  NCollection_IndexedMap<TopoDS_Shape> myLocalEdgesOut;
  NCollection_List<TopoDS_Shape>               myEdgesPassed;
  int                   myFoundOut;
};

#endif // _TopOpeBRepBuild_VertexInfo_HeaderFile
