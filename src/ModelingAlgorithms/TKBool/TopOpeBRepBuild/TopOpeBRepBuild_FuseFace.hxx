// Created on: 1998-07-28
// Created by: LECLERE Florence
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

#ifndef _TopOpeBRepBuild_FuseFace_HeaderFile
#define _TopOpeBRepBuild_FuseFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>

class TopOpeBRepBuild_FuseFace
{
public:
  DEFINE_STANDARD_ALLOC

  TopOpeBRepBuild_FuseFace();

  TopOpeBRepBuild_FuseFace(const NCollection_List<TopoDS_Shape>& LIF,
                           const NCollection_List<TopoDS_Shape>& LRF,
                           const int                             CXM);

  Standard_EXPORT void Init(const NCollection_List<TopoDS_Shape>& LIF,
                            const NCollection_List<TopoDS_Shape>& LRF,
                            const int                             CXM);

  Standard_EXPORT void PerformFace();

  Standard_EXPORT void PerformEdge();

  Standard_EXPORT void ClearEdge();

  Standard_EXPORT void ClearVertex();

  bool IsDone() const;

  bool IsModified() const;

  const NCollection_List<TopoDS_Shape>& LFuseFace() const;

  const NCollection_List<TopoDS_Shape>& LInternEdge() const;

  const NCollection_List<TopoDS_Shape>& LExternEdge() const;

  const NCollection_List<TopoDS_Shape>& LModifEdge() const;

  const NCollection_List<TopoDS_Shape>& LInternVertex() const;

  const NCollection_List<TopoDS_Shape>& LExternVertex() const;

  const NCollection_List<TopoDS_Shape>& LModifVertex() const;

protected:
  NCollection_List<TopoDS_Shape> myLIE;
  NCollection_List<TopoDS_Shape> myLEE;
  NCollection_List<TopoDS_Shape> myLME;
  NCollection_List<TopoDS_Shape> myLIV;
  NCollection_List<TopoDS_Shape> myLEV;
  NCollection_List<TopoDS_Shape> myLMV;

private:
  NCollection_List<TopoDS_Shape> myLIF;
  NCollection_List<TopoDS_Shape> myLRF;
  NCollection_List<TopoDS_Shape> myLFF;
  bool                           myInternal;
  bool                           myModified;
  bool                           myDone;
};

#include <TopOpeBRepBuild_FuseFace.lxx>

#endif // _TopOpeBRepBuild_FuseFace_HeaderFile
