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

#include <StepToTopoDS_DataMapOfRI.hxx>
#include <StepToTopoDS_DataMapOfRINames.hxx>
#include <TopTools_ListOfShape.hxx>
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

  Standard_EXPORT StepToTopoDS_NMTool(const StepToTopoDS_DataMapOfRI&      MapOfRI,
                                      const StepToTopoDS_DataMapOfRINames& MapOfRINames);

  Standard_EXPORT void Init(const StepToTopoDS_DataMapOfRI&      MapOfRI,
                            const StepToTopoDS_DataMapOfRINames& MapOfRINames);

  Standard_EXPORT void SetActive(const Standard_Boolean isActive);

  Standard_EXPORT Standard_Boolean IsActive();

  Standard_EXPORT void CleanUp();

  Standard_EXPORT Standard_Boolean IsBound(const Handle(StepRepr_RepresentationItem)& RI);

  Standard_EXPORT Standard_Boolean IsBound(const TCollection_AsciiString& RIName);

  Standard_EXPORT void Bind(const Handle(StepRepr_RepresentationItem)& RI, const TopoDS_Shape& S);

  Standard_EXPORT void Bind(const TCollection_AsciiString& RIName, const TopoDS_Shape& S);

  Standard_EXPORT const TopoDS_Shape& Find(const Handle(StepRepr_RepresentationItem)& RI);

  Standard_EXPORT const TopoDS_Shape& Find(const TCollection_AsciiString& RIName);

  Standard_EXPORT void RegisterNMEdge(const TopoDS_Shape& Edge);

  Standard_EXPORT Standard_Boolean IsSuspectedAsClosing(const TopoDS_Shape& BaseShell,
                                                        const TopoDS_Shape& SuspectedShell);

  Standard_EXPORT Standard_Boolean IsPureNMShell(const TopoDS_Shape& Shell);

  Standard_EXPORT void SetIDEASCase(const Standard_Boolean IDEASCase);

  Standard_EXPORT Standard_Boolean IsIDEASCase();

protected:
private:
  Standard_EXPORT Standard_Boolean isEdgeRegisteredAsNM(const TopoDS_Shape& Edge);

  Standard_EXPORT Standard_Boolean isAdjacentShell(const TopoDS_Shape& ShellA,
                                                   const TopoDS_Shape& ShellB);

  StepToTopoDS_DataMapOfRI      myRIMap;
  StepToTopoDS_DataMapOfRINames myRINamesMap;
  TopTools_ListOfShape          myNMEdges;
  Standard_Boolean              myIDEASCase;
  Standard_Boolean              myActiveFlag;
};

#endif // _StepToTopoDS_NMTool_HeaderFile
