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

// pdn 26.02.99 added initializing of compound in function OneShape
//:   gka 14.04.99: S4136: apply scaling

#include <BRep_Builder.hxx>
#include <IFSelect_Functions.hxx>
#include <Interface_ShareFlags.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_Reader.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

// #include <ShapeCustom.hxx>
// #include <ShapeAlgo.hxx>
// #include <ShapeAlgo_AlgoContainer.hxx>
//=================================================================================================

XSControl_Reader::XSControl_Reader()
{
  SetWS(new XSControl_WorkSession);
}

//=================================================================================================

XSControl_Reader::XSControl_Reader(const char* const norm)
{
  SetNorm(norm);
}

//=================================================================================================

XSControl_Reader::XSControl_Reader(const occ::handle<XSControl_WorkSession>& WS,
                                   const bool               scratch)
{
  SetWS(WS, scratch);
}

//=================================================================================================

bool XSControl_Reader::SetNorm(const char* const norm)
{
  if (thesession.IsNull())
    SetWS(new XSControl_WorkSession);
  bool stat = thesession->SelectNorm(norm);
  if (stat)
  {
    thesession->InitTransferReader(0);
    thesession->InitTransferReader(4);
  }
  return stat;
}

//=================================================================================================

void XSControl_Reader::SetWS(const occ::handle<XSControl_WorkSession>& WS,
                             const bool               scratch)
{
  therootsta = false;
  theroots.Clear();
  thesession = WS;
  //  There must be a Controller ...  Otherwise we'll see later (after SetNorm)
  if (thesession->NormAdaptor().IsNull())
    return;
  occ::handle<Interface_InterfaceModel> model = thesession->Model();
  if (scratch || model.IsNull())
    model = thesession->NewModel();
  thesession->InitTransferReader(0);
  thesession->InitTransferReader(4);
}

//=================================================================================================

occ::handle<XSControl_WorkSession> XSControl_Reader::WS() const
{
  return thesession;
}

//=================================================================================================

IFSelect_ReturnStatus XSControl_Reader::ReadFile(const char* const filename)
{
  IFSelect_ReturnStatus stat = thesession->ReadFile(filename);
  thesession->InitTransferReader(4);
  return stat;
}

//=================================================================================================

IFSelect_ReturnStatus XSControl_Reader::ReadStream(const char* const theName,
                                                   std::istream&          theIStream)
{
  IFSelect_ReturnStatus stat = thesession->ReadStream(theName, theIStream);
  thesession->InitTransferReader(4);
  return stat;
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> XSControl_Reader::Model() const
{
  return thesession->Model();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_Reader::GiveList(const char* const first,
                                                                const char* const second)
{
  if (first && first[0] != '\0')
  {
    return thesession->GiveList(first, second);
  }

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  int                     i, nbr = NbRootsForTransfer();
  for (i = 1; i <= nbr; i++)
    list->Append(RootForTransfer(i));
  return list;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_Reader::GiveList(
  const char* const            first,
  const occ::handle<Standard_Transient>& list)
{
  return thesession->GiveListFromList(first, list);
}

//=================================================================================================

int XSControl_Reader::NbRootsForTransfer()
{
  if (therootsta)
    return theroots.Length();
  therootsta = true;
  Interface_ShareFlags sf(thesession->Graph());
  int     i, nbr = sf.NbRoots();
  for (i = 1; i <= nbr; i++)
  {
    //    on filtre les racines qu on sait transferer
    occ::handle<Standard_Transient> start = sf.Root(i);
    if (thesession->TransferReader()->Recognize(start))
      theroots.Append(start);
  }
  return theroots.Length();
}

//=================================================================================================

occ::handle<Standard_Transient> XSControl_Reader::RootForTransfer(const int num)
{
  occ::handle<Standard_Transient> voidroot;
  int           nbr = NbRootsForTransfer();
  if (num < 1 || num > nbr)
    return voidroot;
  return theroots.Value(num);
}

//  ####        TRANSFERT        ####

//=================================================================================================

bool XSControl_Reader::TransferOneRoot(const int       num,
                                                   const Message_ProgressRange& theProgress)
{
  return TransferEntity(RootForTransfer(num), theProgress);
}

//=================================================================================================

bool XSControl_Reader::TransferOne(const int       num,
                                               const Message_ProgressRange& theProgress)
{
  return TransferEntity(thesession->StartingEntity(num), theProgress);
}

//=================================================================================================

bool XSControl_Reader::TransferEntity(const occ::handle<Standard_Transient>& start,
                                                  const Message_ProgressRange&      theProgress)
{
  if (start.IsNull())
    return false;
  const occ::handle<XSControl_TransferReader>& TR = thesession->TransferReader();
  TR->BeginTransfer();
  InitializeMissingParameters();
  if (TR->TransferOne(start, true, theProgress) == 0)
    return false;
  TopoDS_Shape sh = TR->ShapeResult(start);
  // ShapeExtend_Explorer STU;
  // SMH May 00: allow empty shapes (STEP CAX-IF, external references)
  // if (STU.ShapeType(sh,true) == TopAbs_SHAPE) return false;  // nulle-vide
  theshapes.Append(sh);
  return true;
}

//=================================================================================================

int XSControl_Reader::TransferList(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
                                                const Message_ProgressRange& theProgress)
{
  if (list.IsNull())
    return 0;
  int                        nbt = 0;
  int                        i, nb = list->Length();
  const occ::handle<XSControl_TransferReader>& TR = thesession->TransferReader();
  TR->BeginTransfer();
  InitializeMissingParameters();
  ClearShapes();
  ShapeExtend_Explorer  STU;
  Message_ProgressScope PS(theProgress, NULL, nb);
  for (i = 1; i <= nb && PS.More(); i++)
  {
    occ::handle<Standard_Transient> start = list->Value(i);
    if (TR->TransferOne(start, true, PS.Next()) == 0)
      continue;
    TopoDS_Shape sh = TR->ShapeResult(start);
    if (STU.ShapeType(sh, true) == TopAbs_SHAPE)
      continue; // nulle-vide
    theshapes.Append(sh);
    nbt++;
  }
  return nbt;
}

//=================================================================================================

int XSControl_Reader::TransferRoots(const Message_ProgressRange& theProgress)
{
  NbRootsForTransfer();
  int                        nbt = 0;
  int                        i, nb = theroots.Length();
  const occ::handle<XSControl_TransferReader>& TR = thesession->TransferReader();

  TR->BeginTransfer();
  InitializeMissingParameters();
  ClearShapes();
  ShapeExtend_Explorer  STU;
  Message_ProgressScope PS(theProgress, "Root", nb);
  for (i = 1; i <= nb && PS.More(); i++)
  {
    occ::handle<Standard_Transient> start = theroots.Value(i);
    if (TR->TransferOne(start, true, PS.Next()) == 0)
      continue;
    TopoDS_Shape sh = TR->ShapeResult(start);
    if (STU.ShapeType(sh, true) == TopAbs_SHAPE)
      continue; // nulle-vide
    theshapes.Append(sh);
    nbt++;
  }
  return nbt;
}

//=================================================================================================

void XSControl_Reader::ClearShapes()
{
  theshapes.Clear();
}

//=================================================================================================

int XSControl_Reader::NbShapes() const
{
  return theshapes.Length();
}

//=================================================================================================

NCollection_Sequence<TopoDS_Shape>& XSControl_Reader::Shapes()
{
  return theshapes;
}

//=================================================================================================

TopoDS_Shape XSControl_Reader::Shape(const int num) const
{
  return theshapes.Value(num);
}

//=================================================================================================

TopoDS_Shape XSControl_Reader::OneShape() const
{
  TopoDS_Shape     sh;
  int i, nb = theshapes.Length();
  if (nb == 0)
    return sh;
  if (nb == 1)
    return theshapes.Value(1);
  TopoDS_Compound C;
  BRep_Builder    B;
  // pdn 26.02.99 testing S4133
  B.MakeCompound(C);
  for (i = 1; i <= nb; i++)
    B.Add(C, theshapes.Value(i));
  return C;
}

//=================================================================================================

void XSControl_Reader::PrintCheckLoad(Standard_OStream&         theStream,
                                      const bool    failsonly,
                                      const IFSelect_PrintCount mode) const
{
  thesession->PrintCheckList(theStream, thesession->ModelCheckList(), failsonly, mode);
}

//=================================================================================================

void XSControl_Reader::PrintCheckLoad(const bool    failsonly,
                                      const IFSelect_PrintCount mode) const
{
  Message_Messenger::StreamBuffer aBuffer = Message::SendInfo();
  PrintCheckLoad(aBuffer, failsonly, mode);
}

//=================================================================================================

void XSControl_Reader::PrintCheckTransfer(Standard_OStream&         theStream,
                                          const bool    failsonly,
                                          const IFSelect_PrintCount mode) const
{
  thesession->PrintCheckList(theStream,
                             thesession->TransferReader()->LastCheckList(),
                             failsonly,
                             mode);
}

//=================================================================================================

void XSControl_Reader::PrintCheckTransfer(const bool    failsonly,
                                          const IFSelect_PrintCount mode) const
{
  Message_Messenger::StreamBuffer aBuffer = Message::SendInfo();
  PrintCheckTransfer(aBuffer, failsonly, mode);
}

//=================================================================================================

void XSControl_Reader::PrintStatsTransfer(Standard_OStream&      theStream,
                                          const int what,
                                          const int mode) const
{
  thesession->TransferReader()->PrintStats(theStream, what, mode);
}

//=================================================================================================

void XSControl_Reader::PrintStatsTransfer(const int what,
                                          const int mode) const
{
  Message_Messenger::StreamBuffer aBuffer = Message::SendInfo();
  PrintStatsTransfer(aBuffer, what, mode);
}

//=================================================================================================

void XSControl_Reader::GetStatsTransfer(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
                                        int&                           nbMapped,
                                        int&                           nbWithResult,
                                        int& nbWithFail) const
{
  const occ::handle<Transfer_TransientProcess>& TP = thesession->TransferReader()->TransientProcess();
  Transfer_IteratorOfProcessForTransient   itrp(true);
  itrp = TP->CompleteResult(true);
  if (!list.IsNull())
    itrp.Filter(list);
  nbMapped = nbWithFail = nbWithResult = 0;

  for (itrp.Start(); itrp.More(); itrp.Next())
  {
    const occ::handle<Transfer_Binder>& binder = itrp.Value();
    nbMapped++;
    if (binder.IsNull())
      nbWithFail++;
    else if (!binder->HasResult())
      nbWithFail++;
    else
    {
      Interface_CheckStatus cst = binder->Check()->Status();
      if ((cst == Interface_CheckOK) || (cst == Interface_CheckWarning))
        nbWithResult++;
      else
        nbWithFail++;
    }
  }
}

//=============================================================================

void XSControl_Reader::SetShapeFixParameters(
  const XSAlgo_ShapeProcessor::ParameterMap& theParameters)
{
  if (occ::handle<Transfer_ActorOfTransientProcess> anActor = GetActor())
  {
    anActor->SetShapeFixParameters(theParameters);
  }
}

//=============================================================================

void XSControl_Reader::SetShapeFixParameters(XSAlgo_ShapeProcessor::ParameterMap&& theParameters)
{
  if (occ::handle<Transfer_ActorOfTransientProcess> anActor = GetActor())
  {
    anActor->SetShapeFixParameters(std::move(theParameters));
  }
}

//=============================================================================

void XSControl_Reader::SetShapeFixParameters(
  const DE_ShapeFixParameters&               theParameters,
  const XSAlgo_ShapeProcessor::ParameterMap& theAdditionalParameters)
{
  if (occ::handle<Transfer_ActorOfTransientProcess> anActor = GetActor())
  {
    anActor->SetShapeFixParameters(theParameters, theAdditionalParameters);
  }
}

//=============================================================================

const XSAlgo_ShapeProcessor::ParameterMap& XSControl_Reader::GetShapeFixParameters() const
{
  static const XSAlgo_ShapeProcessor::ParameterMap anEmptyMap;
  const occ::handle<Transfer_ActorOfTransientProcess>   anActor = GetActor();
  return anActor.IsNull() ? anEmptyMap : anActor->GetShapeFixParameters();
}

//=============================================================================

void XSControl_Reader::SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags)
{
  if (occ::handle<Transfer_ActorOfTransientProcess> anActor = GetActor())
  {
    anActor->SetProcessingFlags(theFlags);
  }
}

//=============================================================================

const XSAlgo_ShapeProcessor::ProcessingFlags& XSControl_Reader::GetShapeProcessFlags() const
{
  static const XSAlgo_ShapeProcessor::ProcessingFlags anEmptyFlags;
  const occ::handle<Transfer_ActorOfTransientProcess>      anActor = GetActor();
  return anActor.IsNull() ? anEmptyFlags : anActor->GetProcessingFlags();
}

//=============================================================================

occ::handle<Transfer_ActorOfTransientProcess> XSControl_Reader::GetActor() const
{
  occ::handle<XSControl_WorkSession> aSession = WS();
  if (aSession.IsNull())
  {
    return nullptr;
  }

  occ::handle<XSControl_TransferReader> aReader = aSession->TransferReader();
  if (aReader.IsNull())
  {
    return nullptr;
  }

  return aReader->Actor();
}

//=============================================================================

void XSControl_Reader::InitializeMissingParameters()
{
  if (GetShapeFixParameters().IsEmpty())
  {
    SetShapeFixParameters(GetDefaultShapeFixParameters());
  }
  if (!GetShapeProcessFlags().second)
  {
    SetShapeProcessFlags(GetDefaultShapeProcessFlags());
  }
}
