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
#include <Interface_Graph.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_Static.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepBasic_HArray1OfNamedUnit.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitName.hxx>
#include <StepBasic_SolidAngleMeasureWithUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : STEPControl_Reader
//purpose  : 
//=======================================================================
STEPControl_Reader::STEPControl_Reader ()
{
  STEPControl_Controller::Init();
  SetNorm ("STEP");
}

//=======================================================================
//function : STEPControl_Reader
//purpose  : 
//=======================================================================

STEPControl_Reader::STEPControl_Reader
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  STEPControl_Controller::Init();
  SetWS (WS,scratch);
  SetNorm ("STEP");
}

//=======================================================================
//function : StepModel
//purpose  : 
//=======================================================================

Handle(StepData_StepModel) STEPControl_Reader::StepModel () const
{
  return Handle(StepData_StepModel)::DownCast(Model());
}

//=======================================================================
//function : TransferRoot
//purpose  : 
//=======================================================================

Standard_Boolean STEPControl_Reader::TransferRoot (const Standard_Integer num)
{
  return TransferOneRoot (num);
}

//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer STEPControl_Reader::NbRootsForTransfer() 
{
  if (therootsta) return theroots.Length();
  therootsta = Standard_True;

  //theroots.Clear();
  Standard_Integer nb = Model()->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = Model()->Value(i);
    if (Interface_Static::IVal("read.step.all.shapes") == 1) {
      // Special case to read invalid shape_representation without links to shapes.
      if (ent->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep))) {
        Interface_EntityIterator aShareds = WS()->Graph().Sharings(ent);
        if (!aShareds.More()) {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
      if (ent->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) {
        Interface_EntityIterator aShareds = WS()->Graph().Sharings(ent);
        if (!aShareds.More()) {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
    }
    if(ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) {
      // PTV 31.01.2003 TRJ11 exclude Product Definition With Associated Document from roots
      if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments))) {
        // check if PDWAD-> PDF <-Document_Product_Equivalence.
        Standard_Boolean iSexclude = Standard_False;
        Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) PDWAD =
          Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(ent);
        Interface_EntityIterator PDWADsubs = WS()->Graph().Shareds(PDWAD);
        for (PDWADsubs.Start(); PDWADsubs.More(); PDWADsubs.Next()) {
          if ( !PDWADsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation)))
            continue;
          Handle(StepBasic_ProductDefinitionFormation) localPDF = 
            Handle(StepBasic_ProductDefinitionFormation)::DownCast(PDWADsubs.Value());
          Interface_EntityIterator PDFsubs = WS()->Graph().Sharings(localPDF);
          for( PDFsubs.Start(); PDFsubs.More(); PDFsubs.Next() )
            if (PDFsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_DocumentProductEquivalence))) {
              iSexclude = Standard_True;
              break;
            }
          if (iSexclude)
            break;
        }
        if (iSexclude) {
#ifdef OCCT_DEBUG
          cout << "Warning: STEPControl_Reader::NbRootsForTransfer exclude PDWAD from roots" << endl;
#endif
          continue;
        }
      }
      Handle(StepBasic_ProductDefinition) PD = 
        Handle(StepBasic_ProductDefinition)::DownCast(ent);
      Standard_Boolean IsRoot = Standard_True;
      const Interface_Graph& graph = WS()->Graph();
      // determinate roots used NextAssemblyUsageOccurrence
      Interface_EntityIterator subs = graph.Sharings(PD);
      for(subs.Start(); subs.More(); subs.Next()) {
        Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
          Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subs.Value());
        if (NAUO.IsNull()) continue;
        if (PD==NAUO->RelatedProductDefinition()) IsRoot=Standard_False;
      }
      // determinate roots used ProductDefinitionContext
      if(IsRoot) {
        const char *str1 = Interface_Static::CVal("read.step.product.context");
        Standard_Integer ICS = Interface_Static::IVal("read.step.product.context");
        if(ICS>1) {
          subs = graph.Shareds(PD);
          for(subs.Start(); subs.More(); subs.Next()) {
            Handle(StepBasic_ProductDefinitionContext) PDC = 
              Handle(StepBasic_ProductDefinitionContext)::DownCast(subs.Value());
            if (PDC.IsNull()) continue;
            const char *str2 = PDC->LifeCycleStage()->String().ToCString();
            const char *str3 = PDC->Name()->String().ToCString();
            if( !( strcasecmp(str1,str2)==0 || strcasecmp(str1,str3)==0 ) )
              IsRoot=Standard_False;
          }
        }
      }
      // determinate roots used ProductDefinitionFormationRelationship
      //subs = graph.Shareds(PD);
      //for(subs.Start(); subs.More(); subs.Next()) {
      //  Handle(StepBasic_ProductDefinitionFormation) PDF = 
      //    Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
      //  if (PDF.IsNull()) continue;
      //  Interface_EntityIterator subs1 = graph.Sharings(PDF);
      //  for(subs1.Start(); subs1.More(); subs1.Next()) {
      //    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      //      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs1.Value());
      //    if (PDFR.IsNull()) continue;
      //    if (PDF==PDFR->RelatedProductDefinition()) IsRoot=Standard_False;
      //  }
      //}
      if (IsRoot) {
        theroots.Append(ent);
        WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
      }
    }
    TCollection_AsciiString aProdMode = Interface_Static::CVal("read.step.product.mode");
    if(!aProdMode.IsEqual("ON")) {
      if(ent->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
        Standard_Boolean IsRoot = Standard_True;
        Handle(StepShape_ShapeDefinitionRepresentation) SDR =
          Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(ent);
        Handle(StepRepr_PropertyDefinition) PropDef = SDR->Definition().PropertyDefinition();
        if(!PropDef.IsNull()) {
          Handle(StepBasic_ProductDefinition) PD = PropDef->Definition().ProductDefinition();
          if(!PD.IsNull()) IsRoot = Standard_False;
          if(IsRoot) {
            Handle(StepRepr_ShapeAspect) SA = PropDef->Definition().ShapeAspect();
            if(!SA.IsNull()) {
              Handle(StepRepr_ProductDefinitionShape) PDS = SA->OfShape();
              PD = PDS->Definition().ProductDefinition();
              if(!PD.IsNull()) IsRoot = Standard_False;
            }
          }
          if(IsRoot) {
            Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
              Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(PropDef->Definition().ProductDefinitionRelationship());
            if(!NAUO.IsNull()) IsRoot = Standard_False;
          }
          if(IsRoot) {
            Handle(StepShape_ShapeRepresentation) SR =
              Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
            if(SR.IsNull()) IsRoot = Standard_False;
          }
        }
        if(IsRoot) {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
      if(ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
        Standard_Boolean IsRoot = Standard_True;
        Handle(StepShape_ShapeRepresentation) SR =
          Handle(StepShape_ShapeRepresentation)::DownCast(ent);
        const Interface_Graph& graph = WS()->Graph();
        Interface_EntityIterator subs = graph.Sharings(SR);
        for(subs.Start(); subs.More() && IsRoot; subs.Next()) {
          Handle(StepShape_ShapeDefinitionRepresentation) SDR = 
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
          if(!SDR.IsNull()) IsRoot = Standard_False;
          if(IsRoot) {
            Handle(StepRepr_RepresentationRelationship) RR =
              Handle(StepRepr_RepresentationRelationship)::DownCast(subs.Value());
            if(!RR.IsNull()) {
              Handle(StepShape_ShapeRepresentation) SR2 =
                Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep1());
              if(SR==SR2)
                SR2 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep2());
              if(!SR2.IsNull())
              {
                Interface_EntityIterator subs2 = graph.Sharings(SR2);
                for(subs2.Start(); subs2.More(); subs2.Next()) {
                  Handle(StepShape_ShapeDefinitionRepresentation) SDR2 = 
                    Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs2.Value());
                  if(!SDR2.IsNull()) IsRoot = Standard_False;
                  //else {
                  //  if(SR==SRR->Rep2()) IsRoot = Standard_False;
                  //}
                }
              }
            }
          }
          if(IsRoot) {
            Handle(StepRepr_RepresentationMap) RM =
              Handle(StepRepr_RepresentationMap)::DownCast(subs.Value());
            if(!RM.IsNull()) {
              Interface_EntityIterator subs2 = graph.Sharings(RM);
              for(subs2.Start(); subs2.More(); subs2.Next()) {
                Handle(StepRepr_MappedItem) MI = Handle(StepRepr_MappedItem)::DownCast(subs2.Value());
                if(!MI.IsNull()) {
                  Interface_EntityIterator subs3 = graph.Sharings(MI);
                  for(subs3.Start(); subs3.More(); subs3.Next()) {
                    Handle(StepShape_ShapeRepresentation) SR2 =
                      Handle(StepShape_ShapeRepresentation)::DownCast(subs3.Value());
                    if(!SR2.IsNull()) IsRoot = Standard_False;
                  }
                }
              }
            }
          }
        }
        if(IsRoot) {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
    }

  }


  return theroots.Length();
}

//=======================================================================
//function : FileUnits
//purpose  : 
//=======================================================================

void STEPControl_Reader::FileUnits( TColStd_SequenceOfAsciiString& theUnitLengthNames,
                                   TColStd_SequenceOfAsciiString& theUnitAngleNames,
                                   TColStd_SequenceOfAsciiString& theUnitSolidAngleNames) 
{
  Standard_Integer nbroots = NbRootsForTransfer();
  if(!nbroots)
    return;
  enum
  {
    LENGTH = 0,
    ANLGE =  1,
    SOLID_ANGLE = 2
  };
  const Interface_Graph& graph = WS()->Graph();
  TColStd_MapOfAsciiString aMapUnits[3];

  for(Standard_Integer i = 1; i <= nbroots; i++)
  {
    Handle(Standard_Transient) anEnt = theroots(i);
    Standard_Integer num   = graph.EntityNumber(anEnt);
    if(!num )
      continue;
    Handle(StepBasic_ProductDefinition) aProdDef = 
      Handle(StepBasic_ProductDefinition)::DownCast(anEnt);
    Handle(StepShape_ShapeDefinitionRepresentation) aShapeDefRepr;
    if(!aProdDef.IsNull())
    {
      Interface_EntityIterator subsPD = graph.Sharings(aProdDef);
      for(subsPD.Start(); subsPD.More() && aShapeDefRepr.IsNull(); subsPD.Next()) 
      {
        Handle(StepRepr_ProductDefinitionShape) aProdDefShape =
          Handle(StepRepr_ProductDefinitionShape)::DownCast(subsPD.Value());
        if(aProdDefShape.IsNull())
          continue;
        Interface_EntityIterator subsSR = graph.Sharings(aProdDefShape);
        Handle(StepShape_ShapeRepresentation) SR;
        for(subsSR.Start(); subsSR.More() && aShapeDefRepr.IsNull(); subsSR.Next())
        {
          Handle(StepShape_ShapeDefinitionRepresentation) aCurShapeDefRepr =
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subsSR.Value());
          if(aCurShapeDefRepr.IsNull()) 
            continue;
          Handle(StepRepr_Representation) aUseRepr = aCurShapeDefRepr->UsedRepresentation();
          if(aUseRepr.IsNull())
            continue;
          Handle(StepShape_ShapeRepresentation) aShapeRepr = 
            Handle(StepShape_ShapeRepresentation)::DownCast(aUseRepr);
          if(aShapeRepr.IsNull()) 
            continue;
          aShapeDefRepr = aCurShapeDefRepr;

        }


      }

    }
    else
      aShapeDefRepr = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(anEnt);
    if(!aShapeDefRepr.IsNull())
    {
      Handle(StepShape_ShapeRepresentation) aShapeRepr =
        Handle(StepShape_ShapeRepresentation)::DownCast(aShapeDefRepr->UsedRepresentation());
      Handle(StepRepr_RepresentationContext) aRepCont = aShapeRepr->ContextOfItems();
      if (aRepCont.IsNull())
        continue;
      TColStd_Array1OfAsciiString aNameUnits(1,3);
      TColStd_Array1OfReal aFactorUnits(1,3);
      if(findUnits(aRepCont,aNameUnits,aFactorUnits))
      {
        Standard_Integer k = LENGTH;
        for ( ; k <= SOLID_ANGLE ; k++)
        {
          if(!aMapUnits[k].Contains(aNameUnits(k+1)))
          {
            aMapUnits[k].Add(aNameUnits(k+1));
            TColStd_SequenceOfAsciiString& anUnitSeq = (k == LENGTH ? 
                theUnitLengthNames : ( k == ANLGE ? theUnitAngleNames : theUnitSolidAngleNames ));
            anUnitSeq.Append(aNameUnits(k+1));
          }
        }
      }

    }

  }
  //for case when units was not found through PDF or SDR
  if(theUnitLengthNames.IsEmpty())
  {
    const Handle(Interface_InterfaceModel) &aModel = WS()->Model();
    if(aModel.IsNull())
      return;
    Standard_Integer nb = aModel->NbEntities();
    for(Standard_Integer i = 1; i <= nb; i++)
    {
      Handle(Standard_Transient) anEnt = aModel->Value(i);
      Handle(StepRepr_RepresentationContext) aRepCont = Handle(StepRepr_RepresentationContext)::DownCast(anEnt);
      if (aRepCont.IsNull())
        continue;
      TColStd_Array1OfAsciiString aNameUnits(1,3);
      TColStd_Array1OfReal aFactorUnits(1,3);
      if(findUnits(aRepCont,aNameUnits,aFactorUnits))
      {
        Standard_Integer k = LENGTH;
        for ( ; k <= SOLID_ANGLE ; k++)
        {
          if(!aMapUnits[k].Contains(aNameUnits(k+1)))
          {
            aMapUnits[k].Add(aNameUnits(k+1));
            TColStd_SequenceOfAsciiString& anUnitSeq = (k == LENGTH ? 
               theUnitLengthNames : ( k == ANLGE ? theUnitAngleNames : theUnitSolidAngleNames ));
            anUnitSeq.Append(aNameUnits(k+1));
          }
        }
      }
    }
  }
}

//=======================================================================
//function : getSiName
//purpose  : 
//=======================================================================

inline static TCollection_AsciiString getSiName(const Handle(StepBasic_SiUnit)& theUnit)
{
 
  TCollection_AsciiString aName;
  if (theUnit->HasPrefix()) {
    switch (theUnit->Prefix()) {
      case StepBasic_spExa:   aName += "exa"; break;
      case StepBasic_spPeta: aName += "peta"; break;
      case StepBasic_spTera: aName += "tera"; break;
      case StepBasic_spGiga: aName += "giga"; break;
      case StepBasic_spMega:  aName += "mega"; break;
      case StepBasic_spHecto: aName += "hecto"; break;
      case StepBasic_spDeca:  aName += "deca"; break;
      case StepBasic_spDeci:  aName += "deci"; break;
      
      case StepBasic_spPico:  aName += "pico"; break;
      case StepBasic_spFemto: aName += "femto"; break;
      case StepBasic_spAtto:  aName += "atto"; break;
      
      case StepBasic_spKilo : aName += "kilo"; break;
      case StepBasic_spCenti :aName += "centi"; break;
      case StepBasic_spMilli :aName += "milli"; break;
      case StepBasic_spMicro :aName += "micro"; break;
      case StepBasic_spNano :aName += "nano"; break;
      default: break;
    };
  }
  
  switch(theUnit->Name()) {
    case StepBasic_sunMetre : aName += "metre"; break;
    case StepBasic_sunRadian : aName += "radian"; break;
    case StepBasic_sunSteradian : aName += "steradian"; break; 
    default: break;
  };
  return aName;
}

//=======================================================================
//function : findUnits
//purpose  : 
//=======================================================================

Standard_Boolean STEPControl_Reader::findUnits(
       const Handle(StepRepr_RepresentationContext)& theRepCont,
       TColStd_Array1OfAsciiString& theNameUnits,
       TColStd_Array1OfReal& theFactorUnits)
{
   Handle(StepRepr_GlobalUnitAssignedContext) aContext;
  Handle(StepRepr_GlobalUncertaintyAssignedContext) aTol;

  if (theRepCont->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext))) {
    aContext = Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::
      DownCast(theRepCont)->GlobalUnitAssignedContext();
  }

 
  if (theRepCont->IsKind(STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx))) {
    aContext = Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::
      DownCast(theRepCont)->GlobalUnitAssignedContext();
   
  }
  if(aContext.IsNull())
    return Standard_False;
  // Start Computation
  Handle(StepBasic_HArray1OfNamedUnit) anUnits = aContext->Units();
  Standard_Integer nbU = aContext->NbUnits();
  Standard_Integer nbFind = 0;
  for (Standard_Integer i = 1; i <= nbU; i++) {
    Handle(StepBasic_NamedUnit) aNamedUnit = aContext->UnitsValue(i);
    Handle(StepBasic_ConversionBasedUnit) aConvUnit =
      Handle(StepBasic_ConversionBasedUnit)::DownCast(aNamedUnit);
    Standard_Integer anInd = 0;
    TCollection_AsciiString aName;
    Standard_Real anUnitFact = 0;
    if( !aConvUnit.IsNull() )
    {
      Handle(StepBasic_MeasureWithUnit) aMeasWithUnit = 
        aConvUnit->ConversionFactor();
      
       if(aMeasWithUnit.IsNull())
         continue;
       
       if( aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_LengthMeasureWithUnit)) )
         anInd = 1;
       else if( aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit)) )
         anInd = 2;
       else if( aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_SolidAngleMeasureWithUnit)) )
         anInd = 3;
      if(!anInd)
        continue;
      aName = aConvUnit->Name()->String(); 
      anUnitFact = aMeasWithUnit->ValueComponent();
    }
    else
    {
      Handle(StepBasic_SiUnit) aSiUnit = Handle(StepBasic_SiUnit)::DownCast(aNamedUnit);
      if( aSiUnit.IsNull())
        continue;
      if(aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)))
        anInd =1;
      else if(aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)))
        anInd = 2;
      else if(aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndSolidAngleUnit)))
        anInd = 3;
      if( !anInd )
         continue;
      anUnitFact = (!aSiUnit->HasPrefix()  ? 
                    1. : STEPConstruct_UnitContext::ConvertSiPrefix(aSiUnit->Prefix()));
      aName = getSiName(aSiUnit);
      
           
    }
    if( !anInd )
      continue;
 
   theNameUnits.SetValue(anInd, aName);
   theFactorUnits.SetValue(anInd, anUnitFact);
    nbFind++;
       
   }

  return nbFind != 0;
}
