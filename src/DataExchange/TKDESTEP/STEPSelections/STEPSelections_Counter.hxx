// Created on: 1999-02-11
// Created by: Pavel DURANDIN
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

#ifndef _STEPSelections_Counter_HeaderFile
#define _STEPSelections_Counter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
class Interface_Graph;
class Standard_Transient;
class StepShape_ConnectedFaceSet;
class StepGeom_CompositeCurve;

class STEPSelections_Counter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPSelections_Counter();

  Standard_EXPORT void Count(const Interface_Graph& graph, const occ::handle<Standard_Transient>& start);

  Standard_EXPORT void Clear();

  int NbInstancesOfFaces() const;

  int POP() const;

  int POP2() const;

  int NbInstancesOfShells() const;

  int NbInstancesOfSolids() const;

  int NbInstancesOfEdges() const;

  int NbInstancesOfWires() const;

  int NbSourceFaces() const;

  int NbSourceShells() const;

  int NbSourceSolids() const;

  int NbSourceEdges() const;

  int NbSourceWires() const;

private:
  Standard_EXPORT void AddShell(const occ::handle<StepShape_ConnectedFaceSet>& cfs);

  Standard_EXPORT void AddCompositeCurve(const occ::handle<StepGeom_CompositeCurve>& ccurve);

  int       myNbFaces;
  int       myNbShells;
  int       myNbSolids;
  int       myNbEdges;
  int       myNbWires;
  NCollection_Map<occ::handle<Standard_Transient>> myMapOfFaces;
  NCollection_Map<occ::handle<Standard_Transient>> myMapOfShells;
  NCollection_Map<occ::handle<Standard_Transient>> myMapOfSolids;
  NCollection_Map<occ::handle<Standard_Transient>> myMapOfEdges;
  NCollection_Map<occ::handle<Standard_Transient>> myMapOfWires;
};

#include <STEPSelections_Counter.lxx>

#endif // _STEPSelections_Counter_HeaderFile
