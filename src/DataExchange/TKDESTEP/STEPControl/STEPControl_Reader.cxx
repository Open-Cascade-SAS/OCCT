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

#include <APIHeaderSection_MakeHeader.hxx>
#include <DE_ShapeFixParameters.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitName.hxx>
#include <StepBasic_SolidAngleMeasureWithUnit.hxx>
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
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Map.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//=================================================================================================

STEPControl_Reader::STEPControl_Reader()
{
  STEPControl_Controller::Init();
  SetNorm("STEP");
}

//=================================================================================================

STEPControl_Reader::STEPControl_Reader(const occ::handle<XSControl_WorkSession>& WS,
                                       const bool                                scratch)
{
  STEPControl_Controller::Init();
  SetWS(WS, scratch);
  SetNorm("STEP");
}

//=================================================================================================

occ::handle<StepData_StepModel> STEPControl_Reader::StepModel() const
{
  return occ::down_cast<StepData_StepModel>(Model());
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Reader::ReadFile(const char* filename)
{
  occ::handle<IFSelect_WorkLibrary> aLibrary  = WS()->WorkLibrary();
  occ::handle<Interface_Protocol>   aProtocol = WS()->Protocol();
  if (aLibrary.IsNull())
    return IFSelect_RetVoid;
  if (aProtocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<StepData_StepModel> aStepModel = new StepData_StepModel;
  aStepModel->InternalParameters.InitFromStatic();
  aStepModel->SetSourceCodePage(aStepModel->InternalParameters.ReadCodePage);
  APIHeaderSection_MakeHeader aHeaderMaker;
  aHeaderMaker.Apply(aStepModel);
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = aLibrary->ReadFile(filename, aStepModel, aProtocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    Interruption ReadFile par Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  WS()->SetModel(aStepModel);
  WS()->SetLoadedFile(filename);
  WS()->InitTransferReader(4);
  return status;
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Reader::ReadFile(const char*              filename,
                                                   const DESTEP_Parameters& theParams)
{
  occ::handle<IFSelect_WorkLibrary> aLibrary  = WS()->WorkLibrary();
  occ::handle<Interface_Protocol>   aProtocol = WS()->Protocol();
  if (aLibrary.IsNull())
    return IFSelect_RetVoid;
  if (aProtocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<StepData_StepModel> aStepModel = new StepData_StepModel;
  aStepModel->InternalParameters             = theParams;
  aStepModel->SetSourceCodePage(aStepModel->InternalParameters.ReadCodePage);
  APIHeaderSection_MakeHeader aHeaderMaker;
  aHeaderMaker.Apply(aStepModel);
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = aLibrary->ReadFile(filename, aStepModel, aProtocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    Interruption ReadFile par Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  WS()->SetModel(aStepModel);
  WS()->SetLoadedFile(filename);
  WS()->InitTransferReader(4);
  return status;
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Reader::ReadStream(const char* theName, std::istream& theIStream)
{
  occ::handle<IFSelect_WorkLibrary> aLibrary  = WS()->WorkLibrary();
  occ::handle<Interface_Protocol>   aProtocol = WS()->Protocol();
  if (aLibrary.IsNull())
    return IFSelect_RetVoid;
  if (aProtocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<StepData_StepModel> aStepModel = new StepData_StepModel;
  aStepModel->InternalParameters.InitFromStatic();
  aStepModel->SetSourceCodePage(aStepModel->InternalParameters.ReadCodePage);
  APIHeaderSection_MakeHeader aHeaderMaker;
  aHeaderMaker.Apply(aStepModel);
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = aLibrary->ReadStream(theName, theIStream, aStepModel, aProtocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    Interruption ReadFile par Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  WS()->SetModel(aStepModel);
  WS()->SetLoadedFile(theName);
  WS()->InitTransferReader(4);
  return status;
}

//=================================================================================================

IFSelect_ReturnStatus STEPControl_Reader::ReadStream(const char*              theName,
                                                     const DESTEP_Parameters& theParams,
                                                     std::istream&            theIStream)
{
  occ::handle<IFSelect_WorkLibrary> aLibrary  = WS()->WorkLibrary();
  occ::handle<Interface_Protocol>   aProtocol = WS()->Protocol();
  if (aLibrary.IsNull())
    return IFSelect_RetVoid;
  if (aProtocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<StepData_StepModel> aStepModel = new StepData_StepModel;
  aStepModel->InternalParameters             = theParams;
  aStepModel->SetSourceCodePage(aStepModel->InternalParameters.ReadCodePage);
  APIHeaderSection_MakeHeader aHeaderMaker;
  aHeaderMaker.Apply(aStepModel);
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = aLibrary->ReadStream(theName, theIStream, aStepModel, aProtocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    Interruption ReadFile par Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  WS()->SetModel(aStepModel);
  WS()->SetLoadedFile(theName);
  WS()->InitTransferReader(4);
  return status;
}

//=================================================================================================

bool STEPControl_Reader::TransferRoot(const int num, const Message_ProgressRange& theProgress)
{
  return TransferOneRoot(num, theProgress);
}

//=================================================================================================

int STEPControl_Reader::NbRootsForTransfer()
{
  if (therootsta)
    return theroots.Length();
  therootsta = true;

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(WS()->Model());
  // theroots.Clear();
  int nb = Model()->NbEntities();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> ent = Model()->Value(i);
    if (aStepModel->InternalParameters.ReadAllShapes == 1)
    {
      // Special case to read invalid shape_representation without links to shapes.
      if (ent->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep)))
      {
        Interface_EntityIterator aShareds = WS()->Graph().Sharings(ent);
        if (!aShareds.More())
        {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
      if (ent->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel)))
      {
        Interface_EntityIterator aShareds = WS()->Graph().Sharings(ent);
        if (!aShareds.More())
        {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
    }
    if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    {
      // PTV 31.01.2003 TRJ11 exclude Product Definition With Associated Document from roots
      if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments)))
      {
        // check if PDWAD-> PDF <-Document_Product_Equivalence.
        bool                                                            iSexclude = false;
        occ::handle<StepBasic_ProductDefinitionWithAssociatedDocuments> PDWAD =
          occ::down_cast<StepBasic_ProductDefinitionWithAssociatedDocuments>(ent);
        Interface_EntityIterator PDWADsubs = WS()->Graph().Shareds(PDWAD);
        for (PDWADsubs.Start(); PDWADsubs.More(); PDWADsubs.Next())
        {
          if (!PDWADsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation)))
            continue;
          occ::handle<StepBasic_ProductDefinitionFormation> localPDF =
            occ::down_cast<StepBasic_ProductDefinitionFormation>(PDWADsubs.Value());
          Interface_EntityIterator PDFsubs = WS()->Graph().Sharings(localPDF);
          for (PDFsubs.Start(); PDFsubs.More(); PDFsubs.Next())
            if (PDFsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_DocumentProductEquivalence)))
            {
              iSexclude = true;
              break;
            }
          if (iSexclude)
            break;
        }
        if (iSexclude)
        {
#ifdef OCCT_DEBUG
          std::cout << "Warning: STEPControl_Reader::NbRootsForTransfer exclude PDWAD from roots"
                    << std::endl;
#endif
          continue;
        }
      }
      occ::handle<StepBasic_ProductDefinition> PD =
        occ::down_cast<StepBasic_ProductDefinition>(ent);
      bool                   IsRoot = true;
      const Interface_Graph& graph  = WS()->Graph();
      // determinate roots used NextAssemblyUsageOccurrence
      Interface_EntityIterator subs = graph.Sharings(PD);
      for (subs.Start(); subs.More(); subs.Next())
      {
        occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO =
          occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(subs.Value());
        if (NAUO.IsNull())
          continue;
        if (PD == NAUO->RelatedProductDefinition())
          IsRoot = false;
      }
      // determinate roots used ProductDefinitionContext
      if (IsRoot)
      {
        DESTEP_Parameters::ReadMode_ProductContext aProdContMode =
          aStepModel->InternalParameters.ReadProductContext;
        TCollection_AsciiString str1 = aStepModel->InternalParameters.GetString(aProdContMode);
        int                     ICS  = aStepModel->InternalParameters.ReadProductContext;
        if (ICS > 1)
        {
          subs = graph.Shareds(PD);
          for (subs.Start(); subs.More(); subs.Next())
          {
            occ::handle<StepBasic_ProductDefinitionContext> PDC =
              occ::down_cast<StepBasic_ProductDefinitionContext>(subs.Value());
            if (PDC.IsNull())
              continue;
            const char* str2 = PDC->LifeCycleStage()->String().ToCString();
            const char* str3 = PDC->Name()->String().ToCString();
            if (strcasecmp(str1.ToCString(), str2) != 0
                  && strcasecmp(str1.ToCString(), str3) != 0)
              IsRoot = false;
          }
        }
      }
      // determinate roots used ProductDefinitionFormationRelationship
      // subs = graph.Shareds(PD);
      // for(subs.Start(); subs.More(); subs.Next()) {
      //  occ::handle<StepBasic_ProductDefinitionFormation> PDF =
      //    occ::down_cast<StepBasic_ProductDefinitionFormation>(subs.Value());
      //  if (PDF.IsNull()) continue;
      //  Interface_EntityIterator subs1 = graph.Sharings(PDF);
      //  for(subs1.Start(); subs1.More(); subs1.Next()) {
      //    occ::handle<StepBasic_ProductDefinitionFormationRelationship> PDFR =
      //      occ::down_cast<StepBasic_ProductDefinitionFormationRelationship>(subs1.Value());
      //    if (PDFR.IsNull()) continue;
      //    if (PDF==PDFR->RelatedProductDefinition()) IsRoot=false;
      //  }
      //}
      if (IsRoot)
      {
        theroots.Append(ent);
        WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
      }
    }
    bool aProdMode = aStepModel->InternalParameters.ReadProductMode;
    if (!aProdMode)
    {
      if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
      {
        bool                                                 IsRoot = true;
        occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
          occ::down_cast<StepShape_ShapeDefinitionRepresentation>(ent);
        occ::handle<StepRepr_PropertyDefinition> PropDef = SDR->Definition().PropertyDefinition();
        if (!PropDef.IsNull())
        {
          occ::handle<StepBasic_ProductDefinition> PD = PropDef->Definition().ProductDefinition();
          if (!PD.IsNull())
            IsRoot = false;
          if (IsRoot)
          {
            occ::handle<StepRepr_ShapeAspect> SA = PropDef->Definition().ShapeAspect();
            if (!SA.IsNull())
            {
              occ::handle<StepRepr_ProductDefinitionShape> PDS = SA->OfShape();
              PD = PDS->Definition().ProductDefinition();
              if (!PD.IsNull())
                IsRoot = false;
            }
          }
          if (IsRoot)
          {
            occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO =
              occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(
                PropDef->Definition().ProductDefinitionRelationship());
            if (!NAUO.IsNull())
              IsRoot = false;
          }
          if (IsRoot)
          {
            occ::handle<StepShape_ShapeRepresentation> SR =
              occ::down_cast<StepShape_ShapeRepresentation>(SDR->UsedRepresentation());
            if (SR.IsNull())
              IsRoot = false;
          }
        }
        if (IsRoot)
        {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
      if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)))
      {
        bool                                       IsRoot = true;
        occ::handle<StepShape_ShapeRepresentation> SR =
          occ::down_cast<StepShape_ShapeRepresentation>(ent);
        const Interface_Graph&   graph = WS()->Graph();
        Interface_EntityIterator subs  = graph.Sharings(SR);
        for (subs.Start(); subs.More() && IsRoot; subs.Next())
        {
          occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
            occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs.Value());
          if (!SDR.IsNull())
            IsRoot = false;
          if (IsRoot)
          {
            occ::handle<StepRepr_RepresentationRelationship> RR =
              occ::down_cast<StepRepr_RepresentationRelationship>(subs.Value());
            if (!RR.IsNull())
            {
              occ::handle<StepShape_ShapeRepresentation> SR2 =
                occ::down_cast<StepShape_ShapeRepresentation>(RR->Rep1());
              if (SR == SR2)
                SR2 = occ::down_cast<StepShape_ShapeRepresentation>(RR->Rep2());
              if (!SR2.IsNull())
              {
                Interface_EntityIterator subs2 = graph.Sharings(SR2);
                for (subs2.Start(); subs2.More(); subs2.Next())
                {
                  occ::handle<StepShape_ShapeDefinitionRepresentation> SDR2 =
                    occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs2.Value());
                  if (!SDR2.IsNull())
                    IsRoot = false;
                  // else {
                  //   if(SR==SRR->Rep2()) IsRoot = false;
                  // }
                }
              }
            }
          }
          if (IsRoot)
          {
            occ::handle<StepRepr_RepresentationMap> RM =
              occ::down_cast<StepRepr_RepresentationMap>(subs.Value());
            if (!RM.IsNull())
            {
              Interface_EntityIterator subs2 = graph.Sharings(RM);
              for (subs2.Start(); subs2.More(); subs2.Next())
              {
                occ::handle<StepRepr_MappedItem> MI =
                  occ::down_cast<StepRepr_MappedItem>(subs2.Value());
                if (!MI.IsNull())
                {
                  Interface_EntityIterator subs3 = graph.Sharings(MI);
                  for (subs3.Start(); subs3.More(); subs3.Next())
                  {
                    occ::handle<StepShape_ShapeRepresentation> SR2 =
                      occ::down_cast<StepShape_ShapeRepresentation>(subs3.Value());
                    if (!SR2.IsNull())
                      IsRoot = false;
                  }
                }
              }
            }
          }
        }
        if (IsRoot)
        {
          theroots.Append(ent);
          WS()->TransferReader()->TransientProcess()->RootsForTransfer()->Append(ent);
        }
      }
    }
  }

  return theroots.Length();
}

//=================================================================================================

void STEPControl_Reader::FileUnits(
  NCollection_Sequence<TCollection_AsciiString>& theUnitLengthNames,
  NCollection_Sequence<TCollection_AsciiString>& theUnitAngleNames,
  NCollection_Sequence<TCollection_AsciiString>& theUnitSolidAngleNames)
{
  int nbroots = NbRootsForTransfer();
  if (!nbroots)
    return;

  enum
  {
    LENGTH      = 0,
    ANLGE       = 1,
    SOLID_ANGLE = 2
  };

  const Interface_Graph&                   graph = WS()->Graph();
  NCollection_Map<TCollection_AsciiString> aMapUnits[3];

  for (int i = 1; i <= nbroots; i++)
  {
    occ::handle<Standard_Transient> anEnt = theroots(i);
    int                             num   = graph.EntityNumber(anEnt);
    if (!num)
      continue;
    occ::handle<StepBasic_ProductDefinition> aProdDef =
      occ::down_cast<StepBasic_ProductDefinition>(anEnt);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aShapeDefRepr;
    if (!aProdDef.IsNull())
    {
      Interface_EntityIterator subsPD = graph.Sharings(aProdDef);
      for (subsPD.Start(); subsPD.More() && aShapeDefRepr.IsNull(); subsPD.Next())
      {
        occ::handle<StepRepr_ProductDefinitionShape> aProdDefShape =
          occ::down_cast<StepRepr_ProductDefinitionShape>(subsPD.Value());
        if (aProdDefShape.IsNull())
          continue;
        Interface_EntityIterator                   subsSR = graph.Sharings(aProdDefShape);
        occ::handle<StepShape_ShapeRepresentation> SR;
        for (subsSR.Start(); subsSR.More() && aShapeDefRepr.IsNull(); subsSR.Next())
        {
          occ::handle<StepShape_ShapeDefinitionRepresentation> aCurShapeDefRepr =
            occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subsSR.Value());
          if (aCurShapeDefRepr.IsNull())
            continue;
          occ::handle<StepRepr_Representation> aUseRepr = aCurShapeDefRepr->UsedRepresentation();
          if (aUseRepr.IsNull())
            continue;
          occ::handle<StepShape_ShapeRepresentation> aShapeRepr =
            occ::down_cast<StepShape_ShapeRepresentation>(aUseRepr);
          if (aShapeRepr.IsNull())
            continue;
          aShapeDefRepr = aCurShapeDefRepr;
        }
      }
    }
    else
      aShapeDefRepr = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(anEnt);
    if (!aShapeDefRepr.IsNull())
    {
      occ::handle<StepShape_ShapeRepresentation> aShapeRepr =
        occ::down_cast<StepShape_ShapeRepresentation>(aShapeDefRepr->UsedRepresentation());
      occ::handle<StepRepr_RepresentationContext> aRepCont = aShapeRepr->ContextOfItems();
      if (aRepCont.IsNull())
        continue;
      NCollection_Array1<TCollection_AsciiString> aNameUnits(1, 3);
      NCollection_Array1<double>                  aFactorUnits(1, 3);
      if (findUnits(aRepCont, aNameUnits, aFactorUnits))
      {
        int k = LENGTH;
        for (; k <= SOLID_ANGLE; k++)
        {
          if (!aMapUnits[k].Contains(aNameUnits(k + 1)))
          {
            aMapUnits[k].Add(aNameUnits(k + 1));
            NCollection_Sequence<TCollection_AsciiString>& anUnitSeq =
              (k == LENGTH ? theUnitLengthNames
                           : (k == ANLGE ? theUnitAngleNames : theUnitSolidAngleNames));
            anUnitSeq.Append(aNameUnits(k + 1));
          }
        }
      }
    }
  }
  // for case when units was not found through PDF or SDR
  if (theUnitLengthNames.IsEmpty())
  {
    const occ::handle<Interface_InterfaceModel>& aModel = WS()->Model();
    if (aModel.IsNull())
      return;
    int nb = aModel->NbEntities();
    for (int i = 1; i <= nb; i++)
    {
      occ::handle<Standard_Transient>             anEnt = aModel->Value(i);
      occ::handle<StepRepr_RepresentationContext> aRepCont =
        occ::down_cast<StepRepr_RepresentationContext>(anEnt);
      if (aRepCont.IsNull())
        continue;
      NCollection_Array1<TCollection_AsciiString> aNameUnits(1, 3);
      NCollection_Array1<double>                  aFactorUnits(1, 3);
      if (findUnits(aRepCont, aNameUnits, aFactorUnits))
      {
        int k = LENGTH;
        for (; k <= SOLID_ANGLE; k++)
        {
          if (!aMapUnits[k].Contains(aNameUnits(k + 1)))
          {
            aMapUnits[k].Add(aNameUnits(k + 1));
            NCollection_Sequence<TCollection_AsciiString>& anUnitSeq =
              (k == LENGTH ? theUnitLengthNames
                           : (k == ANLGE ? theUnitAngleNames : theUnitSolidAngleNames));
            anUnitSeq.Append(aNameUnits(k + 1));
          }
        }
      }
    }
  }
}

//=================================================================================================

void STEPControl_Reader::SetSystemLengthUnit(const double theLengthUnit)
{
  if (StepModel().IsNull())
  {
    return;
  }
  StepModel()->SetLocalLengthUnit(theLengthUnit);
}

//=================================================================================================

double STEPControl_Reader::SystemLengthUnit() const
{
  if (StepModel().IsNull())
  {
    return 1.;
  }
  return StepModel()->LocalLengthUnit();
}

//=================================================================================================

inline static TCollection_AsciiString getSiName(const occ::handle<StepBasic_SiUnit>& theUnit)
{

  TCollection_AsciiString aName;
  if (theUnit->HasPrefix())
  {
    switch (theUnit->Prefix())
    {
      case StepBasic_spExa:
        aName += "exa";
        break;
      case StepBasic_spPeta:
        aName += "peta";
        break;
      case StepBasic_spTera:
        aName += "tera";
        break;
      case StepBasic_spGiga:
        aName += "giga";
        break;
      case StepBasic_spMega:
        aName += "mega";
        break;
      case StepBasic_spHecto:
        aName += "hecto";
        break;
      case StepBasic_spDeca:
        aName += "deca";
        break;
      case StepBasic_spDeci:
        aName += "deci";
        break;

      case StepBasic_spPico:
        aName += "pico";
        break;
      case StepBasic_spFemto:
        aName += "femto";
        break;
      case StepBasic_spAtto:
        aName += "atto";
        break;

      case StepBasic_spKilo:
        aName += "kilo";
        break;
      case StepBasic_spCenti:
        aName += "centi";
        break;
      case StepBasic_spMilli:
        aName += "milli";
        break;
      case StepBasic_spMicro:
        aName += "micro";
        break;
      case StepBasic_spNano:
        aName += "nano";
        break;
      default:
        break;
    };
  }

  switch (theUnit->Name())
  {
    case StepBasic_sunMetre:
      aName += "metre";
      break;
    case StepBasic_sunRadian:
      aName += "radian";
      break;
    case StepBasic_sunSteradian:
      aName += "steradian";
      break;
    default:
      break;
  };
  return aName;
}

//=================================================================================================

DE_ShapeFixParameters STEPControl_Reader::GetDefaultShapeFixParameters() const
{
  return DESTEP_Parameters::GetDefaultShapeFixParameters();
}

//=================================================================================================

ShapeProcess::OperationsFlags STEPControl_Reader::GetDefaultShapeProcessFlags() const
{
  ShapeProcess::OperationsFlags aFlags;
  aFlags.set(ShapeProcess::Operation::FixShape);
  return aFlags;
}

//=================================================================================================

bool STEPControl_Reader::findUnits(const occ::handle<StepRepr_RepresentationContext>& theRepCont,
                                   NCollection_Array1<TCollection_AsciiString>&       theNameUnits,
                                   NCollection_Array1<double>& theFactorUnits)
{
  occ::handle<StepRepr_GlobalUnitAssignedContext>        aContext;
  occ::handle<StepRepr_GlobalUncertaintyAssignedContext> aTol;

  if (theRepCont->IsKind(
        STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)))
  {
    aContext = occ::down_cast<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext>(
                 theRepCont)
                 ->GlobalUnitAssignedContext();
  }

  if (theRepCont->IsKind(
        STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)))
  {
    aContext =
      occ::down_cast<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx>(theRepCont)
        ->GlobalUnitAssignedContext();
  }
  if (aContext.IsNull())
    return false;
  // Start Computation
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> anUnits = aContext->Units();
  int                                                                nbU     = aContext->NbUnits();
  int                                                                nbFind  = 0;
  for (int i = 1; i <= nbU; i++)
  {
    occ::handle<StepBasic_NamedUnit>           aNamedUnit = aContext->UnitsValue(i);
    occ::handle<StepBasic_ConversionBasedUnit> aConvUnit =
      occ::down_cast<StepBasic_ConversionBasedUnit>(aNamedUnit);
    int                     anInd = 0;
    TCollection_AsciiString aName;
    double                  anUnitFact = 0;
    if (!aConvUnit.IsNull())
    {
      occ::handle<StepBasic_MeasureWithUnit> aMeasWithUnit;
      occ::handle<Standard_Transient>        aConvFactor = aConvUnit->ConversionFactor();
      if (aConvFactor->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)))
      {
        aMeasWithUnit = occ::down_cast<StepBasic_MeasureWithUnit>(aConvFactor);
      }
      else if (aConvFactor->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
      {
        occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aReprMeasureItem =
          occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(aConvFactor);
        aMeasWithUnit = aReprMeasureItem->GetMeasureWithUnit();
      }

      if (aMeasWithUnit.IsNull())
        continue;

      if (aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_LengthMeasureWithUnit)))
        anInd = 1;
      else if (aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit)))
        anInd = 2;
      else if (aMeasWithUnit->IsKind(STANDARD_TYPE(StepBasic_SolidAngleMeasureWithUnit)))
        anInd = 3;
      if (!anInd)
        continue;
      aName      = aConvUnit->Name()->String();
      anUnitFact = aMeasWithUnit->ValueComponent();
    }
    else
    {
      occ::handle<StepBasic_SiUnit> aSiUnit = occ::down_cast<StepBasic_SiUnit>(aNamedUnit);
      if (aSiUnit.IsNull())
        continue;
      if (aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)))
        anInd = 1;
      else if (aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)))
        anInd = 2;
      else if (aSiUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndSolidAngleUnit)))
        anInd = 3;
      if (!anInd)
        continue;
      anUnitFact =
        (!aSiUnit->HasPrefix() ? 1.
                               : STEPConstruct_UnitContext::ConvertSiPrefix(aSiUnit->Prefix()));
      aName = getSiName(aSiUnit);
    }
    if (!anInd)
      continue;

    theNameUnits.SetValue(anInd, aName);
    theFactorUnits.SetValue(anInd, anUnitFact);
    nbFind++;
  }

  return nbFind != 0;
}
