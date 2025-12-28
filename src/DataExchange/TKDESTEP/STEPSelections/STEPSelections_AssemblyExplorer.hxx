// Created on: 1999-03-24
// Created by: data exchange team
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

#ifndef _STEPSelections_AssemblyExplorer_HeaderFile
#define _STEPSelections_AssemblyExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <STEPSelections_AssemblyComponent.hxx>
#include <NCollection_Sequence.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Integer.hxx>
class StepShape_ShapeDefinitionRepresentation;
class StepBasic_ProductDefinition;
class STEPSelections_AssemblyComponent;
class Standard_Transient;
class StepRepr_NextAssemblyUsageOccurrence;

class STEPSelections_AssemblyExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPSelections_AssemblyExplorer(const Interface_Graph& G);

  Standard_EXPORT void Init(const Interface_Graph& G);

  Standard_EXPORT void Dump(Standard_OStream& os) const;

  Standard_EXPORT occ::handle<StepShape_ShapeDefinitionRepresentation> FindSDRWithProduct(
    const occ::handle<StepBasic_ProductDefinition>& product) const;

  Standard_EXPORT void FillListWithGraph(const occ::handle<STEPSelections_AssemblyComponent>& cmp);

  Standard_EXPORT occ::handle<Standard_Transient> FindItemWithNAUO(
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& nauo) const;

  //! Returns the number of root assemblies;
  int NbAssemblies() const;

  //! Returns root of assenbly by its rank;
  occ::handle<STEPSelections_AssemblyComponent> Root(const int rank = 1) const;

private:
  NCollection_Sequence<occ::handle<STEPSelections_AssemblyComponent>> myRoots;
  Interface_Graph                            myGraph;
  NCollection_IndexedDataMap<occ::handle<Standard_Transient>, occ::handle<Standard_Transient>> myMap;
};

#include <STEPSelections_AssemblyExplorer.lxx>

#endif // _STEPSelections_AssemblyExplorer_HeaderFile
