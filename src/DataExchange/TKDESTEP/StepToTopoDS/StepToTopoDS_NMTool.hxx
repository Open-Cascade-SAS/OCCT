// Created on: 2010-11-15
// Created by: Sergey SLYADNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef _StepToTopoDS_NMTool_HeaderFile
#define _StepToTopoDS_NMTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepRepr_RepresentationItem.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_List.hxx>
#include <Standard_Boolean.hxx>
class StepRepr_RepresentationItem;
class TCollection_AsciiString;
class TopoDS_Shape;

//! Provides data to process non-manifold topology when
//! reading from STEP.
class StepToTopoDS_NMTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepToTopoDS_NMTool();

  Standard_EXPORT StepToTopoDS_NMTool(
    const NCollection_DataMap<occ::handle<StepRepr_RepresentationItem>, TopoDS_Shape>& MapOfRI,
    const NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape>& MapOfRINames);

  Standard_EXPORT void Init(
    const NCollection_DataMap<occ::handle<StepRepr_RepresentationItem>, TopoDS_Shape>& MapOfRI,
    const NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape>& MapOfRINames);

  Standard_EXPORT void SetActive(const bool isActive);

  Standard_EXPORT bool IsActive();

  Standard_EXPORT void CleanUp();

  Standard_EXPORT bool IsBound(const occ::handle<StepRepr_RepresentationItem>& RI);

  Standard_EXPORT bool IsBound(const TCollection_AsciiString& RIName);

  Standard_EXPORT void Bind(const occ::handle<StepRepr_RepresentationItem>& RI,
                            const TopoDS_Shape&                             S);

  Standard_EXPORT void Bind(const TCollection_AsciiString& RIName, const TopoDS_Shape& S);

  Standard_EXPORT const TopoDS_Shape& Find(const occ::handle<StepRepr_RepresentationItem>& RI);

  Standard_EXPORT const TopoDS_Shape& Find(const TCollection_AsciiString& RIName);

  Standard_EXPORT void RegisterNMEdge(const TopoDS_Shape& Edge);

  Standard_EXPORT bool IsSuspectedAsClosing(const TopoDS_Shape& BaseShell,
                                            const TopoDS_Shape& SuspectedShell);

  Standard_EXPORT bool IsPureNMShell(const TopoDS_Shape& Shell);

  Standard_EXPORT void SetIDEASCase(const bool IDEASCase);

  Standard_EXPORT bool IsIDEASCase();

private:
  Standard_EXPORT bool isEdgeRegisteredAsNM(const TopoDS_Shape& Edge);

  Standard_EXPORT bool isAdjacentShell(const TopoDS_Shape& ShellA, const TopoDS_Shape& ShellB);

  NCollection_DataMap<occ::handle<StepRepr_RepresentationItem>, TopoDS_Shape> myRIMap;
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape>                  myRINamesMap;
  NCollection_List<TopoDS_Shape>                                              myNMEdges;
  bool                                                                        myIDEASCase;
  bool                                                                        myActiveFlag;
};

#endif // _StepToTopoDS_NMTool_HeaderFile
