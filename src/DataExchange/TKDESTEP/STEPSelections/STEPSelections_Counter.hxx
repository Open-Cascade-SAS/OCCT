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
#include <TColStd_MapOfTransient.hxx>
class Interface_Graph;
class Standard_Transient;
class StepShape_ConnectedFaceSet;
class StepGeom_CompositeCurve;

class STEPSelections_Counter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPSelections_Counter();

  Standard_EXPORT void Count(const Interface_Graph& graph, const Handle(Standard_Transient)& start);

  Standard_EXPORT void Clear();

  Standard_Integer NbInstancesOfFaces() const;

  Standard_Integer POP() const;

  Standard_Integer POP2() const;

  Standard_Integer NbInstancesOfShells() const;

  Standard_Integer NbInstancesOfSolids() const;

  Standard_Integer NbInstancesOfEdges() const;

  Standard_Integer NbInstancesOfWires() const;

  Standard_Integer NbSourceFaces() const;

  Standard_Integer NbSourceShells() const;

  Standard_Integer NbSourceSolids() const;

  Standard_Integer NbSourceEdges() const;

  Standard_Integer NbSourceWires() const;

protected:
private:
  Standard_EXPORT void AddShell(const Handle(StepShape_ConnectedFaceSet)& cfs);

  Standard_EXPORT void AddCompositeCurve(const Handle(StepGeom_CompositeCurve)& ccurve);

  Standard_Integer       myNbFaces;
  Standard_Integer       myNbShells;
  Standard_Integer       myNbSolids;
  Standard_Integer       myNbEdges;
  Standard_Integer       myNbWires;
  TColStd_MapOfTransient myMapOfFaces;
  TColStd_MapOfTransient myMapOfShells;
  TColStd_MapOfTransient myMapOfSolids;
  TColStd_MapOfTransient myMapOfEdges;
  TColStd_MapOfTransient myMapOfWires;
};

#include <STEPSelections_Counter.lxx>

#endif // _STEPSelections_Counter_HeaderFile
