// Created on: 2000-01-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <STEPConstruct.hxx>

#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=================================================================================================

occ::handle<StepRepr_RepresentationItem> STEPConstruct::FindEntity(
  const occ::handle<Transfer_FinderProcess>& FinderProcess,
  const TopoDS_Shape&                   Shape)
{
  occ::handle<StepRepr_RepresentationItem> item;
  occ::handle<TransferBRep_ShapeMapper>    mapper = TransferBRep::ShapeMapper(FinderProcess, Shape);
  FinderProcess->FindTypedTransient(mapper, STANDARD_TYPE(StepRepr_RepresentationItem), item);
#ifdef OCCT_DEBUG
  if (item.IsNull())
    std::cout << Shape.TShape()->DynamicType()->Name() << ": RepItem not found" << std::endl;
  else
    std::cout << Shape.TShape()->DynamicType()->Name()
              << ": RepItem found: " << item->DynamicType()->Name() << std::endl;
#endif
  return item;
}

//=================================================================================================

occ::handle<StepRepr_RepresentationItem> STEPConstruct::FindEntity(
  const occ::handle<Transfer_FinderProcess>& FinderProcess,
  const TopoDS_Shape&                   Shape,
  TopLoc_Location&                      Loc)
{
  occ::handle<StepRepr_RepresentationItem> item;
  Loc                                     = Shape.Location();
  occ::handle<TransferBRep_ShapeMapper> mapper = TransferBRep::ShapeMapper(FinderProcess, Shape);
  if (!FinderProcess->FindTypedTransient(mapper, STANDARD_TYPE(StepRepr_RepresentationItem), item)
      && !Loc.IsIdentity())
  {
    Loc.Identity();
    TopoDS_Shape S = Shape;
    S.Location(Loc);
    mapper = TransferBRep::ShapeMapper(FinderProcess, S);
    FinderProcess->FindTypedTransient(mapper, STANDARD_TYPE(StepRepr_RepresentationItem), item);
  }
#ifdef OCCT_DEBUG
  if (item.IsNull())
    std::cout << Shape.TShape()->DynamicType()->Name() << ": RepItem not found" << std::endl;
  else if (Loc != Shape.Location())
    std::cout << Shape.TShape()->DynamicType()->Name()
              << ": RepItem found for shape without location: " << item->DynamicType()->Name()
              << std::endl;
  else
    std::cout << Shape.TShape()->DynamicType()->Name()
              << ": RepItem found: " << item->DynamicType()->Name() << std::endl;
#endif
  return item;
}

//=================================================================================================

TopoDS_Shape STEPConstruct::FindShape(const occ::handle<Transfer_TransientProcess>&   TransientProcess,
                                      const occ::handle<StepRepr_RepresentationItem>& item)
{
  TopoDS_Shape            S;
  occ::handle<Transfer_Binder> binder = TransientProcess->Find(item);
  if (!binder.IsNull() && binder->HasResult())
  {
    S = TransferBRep::ShapeResult(TransientProcess, binder);
  }
  return S;
}

//=================================================================================================

bool STEPConstruct::FindCDSR(
  const occ::handle<Transfer_Binder>&                         ComponentBinder,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& AssemblySDR,
  occ::handle<StepShape_ContextDependentShapeRepresentation>& ComponentCDSR)
{
  bool result = false;

  occ::handle<StepRepr_PropertyDefinition> PropD = AssemblySDR->Definition().PropertyDefinition();
  if (!PropD.IsNull())
  {
    occ::handle<StepBasic_ProductDefinition> AssemblyPD = PropD->Definition().ProductDefinition();
    if (!AssemblyPD.IsNull())
    {
      occ::handle<Transfer_Binder>                         binder = ComponentBinder;
      occ::handle<Transfer_SimpleBinderOfTransient>        trb;
      occ::handle<StepRepr_ProductDefinitionShape>         PDS;
      occ::handle<StepBasic_ProductDefinitionRelationship> NAUO;
      occ::handle<StepBasic_ProductDefinition>             ComponentPD;
      while (!binder.IsNull() && !result)
      {
        trb = occ::down_cast<Transfer_SimpleBinderOfTransient>(binder);
        if (!trb.IsNull())
        {
          ComponentCDSR =
            occ::down_cast<StepShape_ContextDependentShapeRepresentation>(trb->Result());
          if (!ComponentCDSR.IsNull())
          {
            PDS = ComponentCDSR->RepresentedProductRelation();
            if (!PDS.IsNull())
            {
              NAUO = PDS->Definition().ProductDefinitionRelationship();
              if (!NAUO.IsNull())
              {
                ComponentPD = NAUO->RelatingProductDefinition();
                result      = (ComponentPD == AssemblyPD);
              }
            }
          }
        }
        binder = binder->NextResult();
      }
    }
  }
  return result;
}
