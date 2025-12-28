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

//: k8 abv 06.01.99: TR10: writing unique names for NAUOs
// :j4 16.03.99 gka S4134
// abv 18.11.99 renamed from StepPDR_MakeItem

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_Transformation.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

//  ProductDefinition (pour Relationship)
//  ContextDependentShapeRepresentation qui contient la Relationship
//  Relationship
//=================================================================================================

STEPConstruct_Assembly::STEPConstruct_Assembly() {}

//=================================================================================================

void STEPConstruct_Assembly::Init(const occ::handle<StepShape_ShapeDefinitionRepresentation>& aSDR,
                                  const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR0,
                                  const occ::handle<StepGeom_Axis2Placement3d>&               Ax0,
                                  const occ::handle<StepGeom_Axis2Placement3d>&               AxLoc)
{
  thesdr  = aSDR;
  thesdr0 = SDR0;
  thesr   = occ::down_cast<StepShape_ShapeRepresentation>(aSDR->UsedRepresentation());
  thesr0  = occ::down_cast<StepShape_ShapeRepresentation>(SDR0->UsedRepresentation());
  theval.Nullify();
  theax0            = Ax0;
  theloc            = AxLoc;
  myIsCartesianTrsf = false;
}

//=================================================================================================

void STEPConstruct_Assembly::Init(
  const occ::handle<StepShape_ShapeDefinitionRepresentation>&    aSDR,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>&    SDR0,
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& theTrsfOp)
{
  thesdr  = aSDR;
  thesdr0 = SDR0;
  thesr   = occ::down_cast<StepShape_ShapeRepresentation>(aSDR->UsedRepresentation());
  thesr0  = occ::down_cast<StepShape_ShapeRepresentation>(SDR0->UsedRepresentation());
  theval.Nullify();
  myTrsfOp          = theTrsfOp;
  myIsCartesianTrsf = true;
}

//=================================================================================================

// void STEPConstruct_Assembly::MakeMappedItem ()
//{
//   not yet implemented
//}

//=================================================================================================

void STEPConstruct_Assembly::MakeRelationship()
{
  // get PDs for assembly (sdr0) and component (sdr)
  occ::handle<StepBasic_ProductDefinition> PDED =
    thesdr->Definition().PropertyDefinition()->Definition().ProductDefinition();
  occ::handle<StepBasic_ProductDefinition> PDING =
    thesdr0->Definition().PropertyDefinition()->Definition().ProductDefinition();

  // create NAUO
  //: k8 abv 06 Jan 99: TR10: writing unique names for NAUOs  !!!!!
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO = new StepRepr_NextAssemblyUsageOccurrence;
  static int                                        id   = 0;
  occ::handle<TCollection_HAsciiString>             ocid = new TCollection_HAsciiString(++id);
  occ::handle<TCollection_HAsciiString>             ocname = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString>             ocdesc = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString>             refdes; // reste nulle
  NAUO->Init(ocid, ocname, true, ocdesc, PDING, PDED, false, refdes);

  // create PDS for link CDSR->PDS->NAUO
  occ::handle<StepRepr_ProductDefinitionShape> PDS     = new StepRepr_ProductDefinitionShape;
  occ::handle<TCollection_HAsciiString>        pdsname = new TCollection_HAsciiString("Placement");
  occ::handle<TCollection_HAsciiString>        pdsdesc =
    new TCollection_HAsciiString("Placement of an item");
  StepRepr_CharacterizedDefinition CD;
  CD.SetValue(NAUO);
  PDS->Init(pdsname, true, pdsdesc, CD);

  // create transformation
  occ::handle<Standard_Transient> aTrsfItem;
  if (!myIsCartesianTrsf)
  {
    occ::handle<StepRepr_ItemDefinedTransformation> ItemDef =
      new StepRepr_ItemDefinedTransformation;
    occ::handle<TCollection_HAsciiString> idname = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> idescr = new TCollection_HAsciiString("");
    ItemDef->Init(idname, idescr, theax0, theloc);
    aTrsfItem = ItemDef;
  }
  else
  {
    aTrsfItem = myTrsfOp;
  }

  // create SRRWT
  occ::handle<StepRepr_ShapeRepresentationRelationshipWithTransformation> SRRWT =
    new StepRepr_ShapeRepresentationRelationshipWithTransformation;
  occ::handle<TCollection_HAsciiString> stname = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString> stescr = new TCollection_HAsciiString("");
  StepRepr_Transformation               StepTrans;
  StepTrans.SetValue(aTrsfItem);
  SRRWT->Init(stname, stescr, thesr, thesr0, StepTrans);

  // create CDSR (final result, root)
  occ::handle<StepShape_ContextDependentShapeRepresentation> CDSR =
    new StepShape_ContextDependentShapeRepresentation;
  CDSR->Init(SRRWT, PDS);

  theval = CDSR;
}

//=================================================================================================

occ::handle<Standard_Transient> STEPConstruct_Assembly::ItemValue() const
{
  if (theval.IsNull())
    return occ::handle<Standard_Transient>(thesr);
  return theval;
}

//=================================================================================================

occ::handle<StepGeom_Axis2Placement3d> STEPConstruct_Assembly::ItemLocation() const
{
  return theloc;
}

//=================================================================================================

occ::handle<StepRepr_NextAssemblyUsageOccurrence> STEPConstruct_Assembly::GetNAUO() const
{
  occ::handle<StepShape_ContextDependentShapeRepresentation> CDSR =
    occ::down_cast<StepShape_ContextDependentShapeRepresentation>(ItemValue());
  if (!CDSR.IsNull())
  {
    occ::handle<StepBasic_ProductDefinitionRelationship> PDR =
      CDSR->RepresentedProductRelation()->Definition().ProductDefinitionRelationship();
    return occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(PDR);
  }
  return 0;
}

//=================================================================================================

bool STEPConstruct_Assembly::CheckSRRReversesNAUO(
  const Interface_Graph&                                            theGraph,
  const occ::handle<StepShape_ContextDependentShapeRepresentation>& CDSR)
{
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> nauo =
    occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(
      CDSR->RepresentedProductRelation()->Definition().ProductDefinitionRelationship());
  if (nauo.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: No NAUO found in CDSR !" << std::endl;
#endif
    return false;
  }

  occ::handle<StepBasic_ProductDefinition> pd1, pd2;
  occ::handle<StepRepr_Representation>     rep1 = CDSR->RepresentationRelation()->Rep1();
  occ::handle<StepRepr_Representation>     rep2 = CDSR->RepresentationRelation()->Rep2();
  if (rep1.IsNull() || rep2.IsNull())
    return false;

  // find SDRs corresponding to Rep1 and Rep2 and remember their PDs
  occ::handle<Standard_Type> tSDR   = STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation);
  Interface_EntityIterator   anIter = theGraph.Sharings(rep1);
  for (; anIter.More() && pd1.IsNull(); anIter.Next())
  {
    const occ::handle<Standard_Transient>& enti = anIter.Value();
    if (enti->DynamicType() == tSDR)
    {
      occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
        occ::down_cast<StepShape_ShapeDefinitionRepresentation>(enti);
      if (SDR->UsedRepresentation() == rep1)
        pd1 = SDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
    }
  }

  anIter = theGraph.Sharings(rep2);
  for (; anIter.More() && pd2.IsNull(); anIter.Next())
  {
    const occ::handle<Standard_Transient>& enti = anIter.Value();
    if (enti->DynamicType() == tSDR)
    {
      occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
        occ::down_cast<StepShape_ShapeDefinitionRepresentation>(enti);
      if (SDR->UsedRepresentation() == rep2)
        pd2 = SDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
    }
  }

  // checks..

  if (pd1 == nauo->RelatedProductDefinition() && // OK
      pd2 == nauo->RelatingProductDefinition())
    return false;

  if (pd2 == nauo->RelatedProductDefinition() && // Reversed
      pd1 == nauo->RelatingProductDefinition())
  {
    return true;
  }

#ifdef OCCT_DEBUG
  std::cout << "Warning: SRR and NAUO are incompatible" << std::endl;
//  std::cout << "NAUO = " << Model->StringLabel(nauo)->ToCString() <<
//       ",\tCDSR = " << Model->StringLabel(CDSR)->ToCString() << std::endl;
//  std::cout << "Rep1 = " << Model->StringLabel(rep1)->ToCString() <<
//       ",\tRep2 = " << Model->StringLabel(rep2)->ToCString() << std::endl;
//  std::cout << "PD1  = " << Model->StringLabel(pd1)->ToCString() <<
//       ",\tPD2  = " << Model->StringLabel(pd2)->ToCString() << std::endl;
//  std::cout << "Rel1 = " << Model->StringLabel(nauo->RelatingProductDefinition())->ToCString() <<
//       ",\tRel2 = " << Model->StringLabel(nauo->RelatedProductDefinition())->ToCString() <<
//       std::endl;
#endif

  if (pd2 == nauo->RelatedProductDefinition() || //: k3 abv 25 Nov 98: rp1sd.stp - bad assemblies
      pd1 == nauo->RelatingProductDefinition())
  {
    return true;
  }

  return false;
}
