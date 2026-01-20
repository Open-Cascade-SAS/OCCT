// Created on: 1999-03-24
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <STEPSelections_AssemblyExplorer.hxx>
#include <STEPSelections_AssemblyLink.hxx>
#include <STEPSelections_AssemblyLink.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

STEPSelections_AssemblyExplorer::STEPSelections_AssemblyExplorer(const Interface_Graph& G)
    : myGraph(G)
{
  Init(G);
}

occ::handle<Standard_Transient> STEPSelections_AssemblyExplorer::FindItemWithNAUO(
  const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& nauo) const
{
  occ::handle<Standard_Transient>              item;
  occ::handle<StepRepr_ProductDefinitionShape> pds;
  Interface_EntityIterator                subs = myGraph.Sharings(nauo);
  for (subs.Start(); subs.More() && pds.IsNull(); subs.Next())
    if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
      pds = occ::down_cast<StepRepr_ProductDefinitionShape>(subs.Value());
  if (pds.IsNull())
    return item;

  subs = myGraph.Sharings(pds);
  occ::handle<StepShape_ContextDependentShapeRepresentation> cdsr;
  occ::handle<StepShape_ShapeDefinitionRepresentation>       shdefrep;
  occ::handle<Standard_Transient>                            itmp;
  for (subs.Start(); subs.More() && shdefrep.IsNull(); subs.Next())
  {
    itmp = subs.Value();
    if (itmp->IsKind(STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation)))
      return itmp;
    if (itmp->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
      shdefrep = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(itmp);
  }
  if (shdefrep.IsNull())
    return shdefrep;
  occ::handle<StepShape_ShapeRepresentation> srep =
    occ::down_cast<StepShape_ShapeRepresentation>(shdefrep->UsedRepresentation());
  if (srep.IsNull())
    return srep;
  for (int i = 1; i <= srep->NbItems(); i++)
  {
    occ::handle<StepRepr_RepresentationItem> repitem = srep->ItemsValue(i);
    if (repitem->IsKind(STANDARD_TYPE(StepRepr_MappedItem)))
      return repitem;
  }
  return item;
}

occ::handle<StepShape_ShapeDefinitionRepresentation> STEPSelections_AssemblyExplorer::FindSDRWithProduct(
  const occ::handle<StepBasic_ProductDefinition>& product) const
{
  Interface_EntityIterator subs = myGraph.Sharings(product);
  for (subs.Start(); subs.More(); subs.Next())
    if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition)))
    {
      Interface_EntityIterator subs1 = myGraph.Sharings(subs.Value());
      for (subs1.Start(); subs1.More(); subs1.Next())
        if (subs1.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
        {
          DeclareAndCast(StepShape_ShapeDefinitionRepresentation, SDR, subs1.Value());
          return SDR;
        }
    }
  occ::handle<StepShape_ShapeDefinitionRepresentation> sdr;
  return sdr;
}

void STEPSelections_AssemblyExplorer::FillListWithGraph(
  const occ::handle<STEPSelections_AssemblyComponent>& cmp)
{
  occ::handle<StepShape_ShapeDefinitionRepresentation> SDR  = cmp->GetSDR();
  occ::handle<NCollection_HSequence<occ::handle<STEPSelections_AssemblyLink>>>  list = cmp->GetList();
  occ::handle<StepRepr_ProductDefinitionShape>         pdsh =
    occ::down_cast<StepRepr_ProductDefinitionShape>(SDR->Definition().PropertyDefinition());
  if (pdsh.IsNull())
    return;
  occ::handle<StepBasic_ProductDefinition> pdf = pdsh->Definition().ProductDefinition();
  if (pdf.IsNull())
    return;
  Interface_EntityIterator subs = myGraph.Sharings(pdf);
  for (subs.Start(); subs.More(); subs.Next())
    if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence)))
    {
      DeclareAndCast(StepRepr_NextAssemblyUsageOccurrence, nauo, subs.Value());
      if (pdf == nauo->RelatingProductDefinition())
      {
        occ::handle<STEPSelections_AssemblyLink> link = new STEPSelections_AssemblyLink;
        link->SetNAUO(nauo);
        link->SetItem(FindItemWithNAUO(nauo));
        occ::handle<StepBasic_ProductDefinition> pdrComponent = nauo->RelatedProductDefinition();
        if (pdrComponent.IsNull())
          continue;
        occ::handle<StepShape_ShapeDefinitionRepresentation> subSDR = FindSDRWithProduct(pdrComponent);
        if (subSDR.IsNull())
          continue;
        int index = myMap.FindIndex(subSDR);
        if (index)
          link->SetComponent(
            occ::down_cast<STEPSelections_AssemblyComponent>(myMap.FindFromIndex(index)));
        else
        {
          occ::handle<NCollection_HSequence<occ::handle<STEPSelections_AssemblyLink>>> sublist =
            new NCollection_HSequence<occ::handle<STEPSelections_AssemblyLink>>;
          occ::handle<STEPSelections_AssemblyComponent> subCmp =
            new STEPSelections_AssemblyComponent(subSDR, sublist);
          FillListWithGraph(subCmp);
          link->SetComponent(subCmp);
          myMap.Add(subSDR, subCmp);
        }
        list->Append(link);
      }
    }
}

void STEPSelections_AssemblyExplorer::Init(const Interface_Graph& G)
{
  myGraph = G;
  myRoots.Clear();
  myMap.Clear();
  Interface_EntityIterator roots = myGraph.RootEntities();
  for (roots.Start(); roots.More(); roots.Next())
    if (roots.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
    {
      occ::handle<STEPSelections_AssemblyComponent> cmp = new STEPSelections_AssemblyComponent;
      cmp->SetSDR(occ::down_cast<StepShape_ShapeDefinitionRepresentation>(roots.Value()));
      cmp->SetList(new NCollection_HSequence<occ::handle<STEPSelections_AssemblyLink>>);
      FillListWithGraph(cmp);
      myRoots.Append(cmp);
    }
}

static const char* GetProductName(const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR)
{
  const char*                    str = "";
  occ::handle<StepBasic_Product>           empty;
  occ::handle<StepRepr_PropertyDefinition> PropDf = SDR->Definition().PropertyDefinition();
  if (PropDf.IsNull())
    return str;
  occ::handle<StepBasic_ProductDefinition> PD = PropDf->Definition().ProductDefinition();
  if (PD.IsNull())
    return str;
  occ::handle<StepBasic_ProductDefinitionFormation> PDF = PD->Formation();
  if (PDF.IsNull())
    return str;
  return PDF->OfProduct()->Name()->ToCString();
}

static void PrintSubAssembly(Standard_OStream&                               os,
                             const occ::handle<STEPSelections_AssemblyComponent>& cmp,
                             const occ::handle<Interface_InterfaceModel>&         Model,
                             int                                level)
{
  // for ( int j=0; j < level; j++ ) os << "\t";
  os << "SDR: " << Model->StringLabel(cmp->GetSDR())->ToCString() << "\t";

  os << "Product: " << GetProductName(cmp->GetSDR()) << std::endl;
  for (int i = 1; i <= cmp->GetList()->Length(); i++)
  {
    for (int j = 0; j < level + 1; j++)
      os << "\t";
    os << "NAUO :" << Model->StringLabel(cmp->GetList()->Value(i)->GetNAUO())->ToCString() << ";\t";
    if (cmp->GetList()->Value(i)->GetItem()->IsKind(STANDARD_TYPE(StepRepr_MappedItem)))
      os << "MI ";
    else if (cmp->GetList()->Value(i)->GetItem()->IsKind(
               STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation)))
      os << "CDSR ";
    else
      os << "UNKNOWN LINK!!!";
    PrintSubAssembly(os, cmp->GetList()->Value(i)->GetComponent(), Model, level + 1);
  }
}

void STEPSelections_AssemblyExplorer::Dump(Standard_OStream& os) const
{
  occ::handle<Interface_InterfaceModel> model = myGraph.Model();
  for (int i = 1; i <= myRoots.Length(); i++)
  {
    os << "Assembly N: " << i << std::endl << std::endl;
    PrintSubAssembly(os, myRoots.Value(i), model, 0);
  }
}
