// Created on: 1993-06-15
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_PaveSet_HeaderFile
#define _TopOpeBRepBuild_PaveSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopOpeBRepBuild_Pave.hxx>
#include <NCollection_List.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>
class TopoDS_Shape;
class TopOpeBRepBuild_Pave;
class TopOpeBRepBuild_Loop;

//! class providing an exploration of a set of vertices to build edges.
//! It is similar to LoopSet from TopOpeBRepBuild where Loop is Pave.
class TopOpeBRepBuild_PaveSet : public TopOpeBRepBuild_LoopSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! Create a Pave set on edge <E>. It contains <E> vertices.
  Standard_EXPORT TopOpeBRepBuild_PaveSet(const TopoDS_Shape& E);

  Standard_EXPORT void RemovePV(const bool B);

  //! Add <PV> in the Pave set.
  Standard_EXPORT void Append(const occ::handle<TopOpeBRepBuild_Pave>& PV);

  Standard_EXPORT virtual void InitLoop() override;

  Standard_EXPORT virtual bool MoreLoop() const override;

  Standard_EXPORT virtual void NextLoop() override;

  Standard_EXPORT virtual occ::handle<TopOpeBRepBuild_Loop> Loop() const override;

  Standard_EXPORT const TopoDS_Edge& Edge() const;

  Standard_EXPORT bool HasEqualParameters();

  Standard_EXPORT double EqualParameters() const;

  Standard_EXPORT bool ClosedVertices();

  Standard_EXPORT static void SortPave(
    const NCollection_List<occ::handle<TopOpeBRepBuild_Pave>>& Lin,
    NCollection_List<occ::handle<TopOpeBRepBuild_Pave>>&       Lout);

private:
  Standard_EXPORT void Prepare();

  TopoDS_Edge                                                   myEdge;
  NCollection_List<occ::handle<TopOpeBRepBuild_Pave>>           myVertices;
  NCollection_List<occ::handle<TopOpeBRepBuild_Pave>>::Iterator myVerticesIt;
  bool                                                          myHasEqualParameters;
  double                                                        myEqualParameters;
  bool                                                          myClosed;
  bool                                                          myPrepareDone;
  bool                                                          myRemovePV;
};

#endif // _TopOpeBRepBuild_PaveSet_HeaderFile
