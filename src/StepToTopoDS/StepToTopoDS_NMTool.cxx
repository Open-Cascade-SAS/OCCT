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

#include <StepToTopoDS_NMTool.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

// ============================================================================
// Method  : StepToTopoDS_NMTool
// Purpose : Default constructor
// ============================================================================

StepToTopoDS_NMTool::StepToTopoDS_NMTool() {
  myIDEASCase = Standard_False;
  myActiveFlag = Standard_False;
}

// ============================================================================
// Method  : StepToTopoDS_NMTool
// Purpose : Constructor with a Map for Representation Items and their names
// ============================================================================

StepToTopoDS_NMTool::StepToTopoDS_NMTool(const StepToTopoDS_DataMapOfRI& MapOfRI, 
                                         const StepToTopoDS_DataMapOfRINames& MapOfRINames) {
  myIDEASCase = Standard_False;
  myActiveFlag = Standard_False;
  Init(MapOfRI, MapOfRINames);
}

// ============================================================================
// Method  : Init
// Purpose : Initializes internal maps of the tool with the passed ones
// ============================================================================

void StepToTopoDS_NMTool::Init(const StepToTopoDS_DataMapOfRI& MapOfRI, 
                               const StepToTopoDS_DataMapOfRINames& MapOfRINames) { 
  myRIMap = MapOfRI;
  myRINamesMap = MapOfRINames;
}

// ============================================================================
// Method  : SetActive
// Purpose : Turns the tool ON/OFF (OFF by default)
// ============================================================================

void StepToTopoDS_NMTool::SetActive(const Standard_Boolean isActive) {
  myActiveFlag = isActive;
}

// ============================================================================
// Method  : IsActive
// Purpose : TRUE if active, FALSE - otherwise
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsActive() {
  return myActiveFlag;
}

// ============================================================================
// Method  : CleanUp
// Purpose : Clears all internal containers
// ============================================================================

void StepToTopoDS_NMTool::CleanUp() { 
  myRIMap.Clear();
  myRINamesMap.Clear();
}

// ============================================================================
// Method  : IsBound
// Purpose : Indicates weither a RI is bound or not in the Map
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsBound(const Handle(StepRepr_RepresentationItem)& RI) {
  return myRIMap.IsBound(RI);
}

// ============================================================================
// Method  : IsBound
// Purpose : Indicates weither a RI is bound or not in the Map by name
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsBound(const TCollection_AsciiString& RIName) {
  return myRINamesMap.IsBound(RIName);
}

// ============================================================================
// Method  : Bind
// Purpose : Binds a RI with a Shape in the Map
// ============================================================================

void StepToTopoDS_NMTool::Bind(const Handle(StepRepr_RepresentationItem)& RI, const TopoDS_Shape& S) {
  myRIMap.Bind(RI, S);
}

// ============================================================================
// Method  : Bind
// Purpose : Binds a RI's name with a Shape in the Map
// ============================================================================

void StepToTopoDS_NMTool::Bind(const TCollection_AsciiString& RIName, const TopoDS_Shape& S) {
  myRINamesMap.Bind(RIName, S);
}

// ============================================================================
// Method  : Find
// Purpose : Returns the Shape corresponding to the bounded RI
// ============================================================================

const TopoDS_Shape& StepToTopoDS_NMTool::Find(const Handle(StepRepr_RepresentationItem)& RI) {
  return myRIMap.Find(RI);
}

// ============================================================================
// Method  : Find
// Purpose : Returns the Shape corresponding to the bounded RI's name
// ============================================================================

const TopoDS_Shape& StepToTopoDS_NMTool::Find(const TCollection_AsciiString& RIName) {
  return myRINamesMap.Find(RIName);
}

// ============================================================================
// Method  : RegisterNMEdge
// Purpose : Register non-manifold Edge in the internal storage if it wasn't
//           registered before
// ============================================================================

void StepToTopoDS_NMTool::RegisterNMEdge(const TopoDS_Shape& Edge) {
  if ( !this->isEdgeRegisteredAsNM(Edge) )
    myNMEdges.Append(Edge);
}

// ============================================================================
// Method  : IsSuspectedAsClosing
// Purpose : Checks whether SuspectedShell is pure non-manifold and adjacent
//           to BaseShell
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsSuspectedAsClosing(const TopoDS_Shape& BaseShell,
                                                           const TopoDS_Shape& SuspectedShell) {
  return this->IsPureNMShell(SuspectedShell) &&
         this->isAdjacentShell(BaseShell, SuspectedShell);

}

// ============================================================================
// Method  : SetIDEASCase
// Purpose : Sets myIDEASCase flag (I-DEAS-like STP is processed)
// ============================================================================

void StepToTopoDS_NMTool::SetIDEASCase(const Standard_Boolean IDEASCase) {
  myIDEASCase = IDEASCase;
}

// ============================================================================
// Method  : GetIDEASCase
// Purpose : Gets myIDEASCase flag (I-DEAS-like STP is processed)
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsIDEASCase() {
  return myIDEASCase;
}

// ============================================================================
// Method  : IsPureNMShell
// Purpose : Checks if the Shell passed contains only non-manifold Edges
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::IsPureNMShell(const TopoDS_Shape& Shell) {
  Standard_Boolean result = Standard_True;
  TopExp_Explorer edgeExp(Shell, TopAbs_EDGE);
  for ( ; edgeExp.More(); edgeExp.Next() ) {
    TopoDS_Shape currentEdge = edgeExp.Current();
    if ( !this->isEdgeRegisteredAsNM(currentEdge) ) {
      result = Standard_False;
      break;
    }    
  }
  return result;
}

// ============================================================================
// Method  : isEdgeRegisteredAsNM
// Purpose : Checks if the Edge passed is registered as non-manifold one
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::isEdgeRegisteredAsNM(const TopoDS_Shape& Edge) {
  Standard_Boolean result = Standard_False;
  TopTools_ListIteratorOfListOfShape it(myNMEdges);
  for ( ; it.More(); it.Next() ) {
    TopoDS_Shape currentShape = it.Value();
    if ( currentShape.IsSame(Edge) ) {
      result =  Standard_True;
      break;
    }
  }
  return result;
}

// ============================================================================
// Method  : isAdjacentShell
// Purpose : Checks if the ShellA is adjacent to the ShellB
// ============================================================================

Standard_Boolean StepToTopoDS_NMTool::isAdjacentShell(const TopoDS_Shape& ShellA,
                                                      const TopoDS_Shape& ShellB) {
  if ( ShellA.IsSame(ShellB) )
    return Standard_False;

  TopExp_Explorer edgeExpA(ShellA, TopAbs_EDGE);
  for ( ; edgeExpA.More(); edgeExpA.Next() ) {
    TopoDS_Shape currentEdgeA = edgeExpA.Current();
    TopExp_Explorer edgeExpB(ShellB, TopAbs_EDGE);
    for ( ; edgeExpB.More(); edgeExpB.Next() ) {
      TopoDS_Shape currentEdgeB = edgeExpB.Current();
      if ( currentEdgeA.IsSame(currentEdgeB) )
        return Standard_True;
    }  
  }

  return Standard_False;
}
