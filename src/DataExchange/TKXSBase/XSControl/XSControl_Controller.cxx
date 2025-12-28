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

#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_DispPerFiles.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_DispPerSignature.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_GraphCounter.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_SignAncestor.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCategory.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignType.hxx>
#include <IFSelect_SignValidity.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Static.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_ConnectedShapes.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_SignTransferStatus.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_Controller, Standard_Transient)

//  ParamEditor
//  Transferts

static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> listad;

//=================================================================================================

XSControl_Controller::XSControl_Controller(const char* theLongName,
                                           const char* theShortName)
    : myShortName(theShortName),
      myLongName(theLongName)
{
  // Standard parameters
  Interface_Static::Standards();
  TraceStatic("read.precision.mode", 5);
  TraceStatic("read.precision.val", 5);
  TraceStatic("write.precision.mode", 6);
  TraceStatic("write.precision.val", 6);
}

//=================================================================================================

void XSControl_Controller::TraceStatic(const char* theName,
                                       const int theUse)
{
  occ::handle<Interface_Static> val = Interface_Static::Static(theName);
  if (val.IsNull())
    return;
  myParams.Append(val);
  myParamUses.Append(theUse);
}

//=================================================================================================

void XSControl_Controller::SetNames(const char* theLongName,
                                    const char* theShortName)
{
  if (theLongName && theLongName[0] != '\0')
  {
    myLongName.Clear();
    myLongName.AssignCat(theLongName);
  }
  if (theShortName && theShortName[0] != '\0')
  {
    myShortName.Clear();
    myShortName.AssignCat(theShortName);
  }
}

//=================================================================================================

void XSControl_Controller::Record(const char* theName) const
{
  if (listad.IsBound(theName))
  {
    occ::handle<Standard_Transient> thisadapt(this);
    occ::handle<Standard_Transient> newadapt = listad.ChangeFind(theName);
    if (newadapt->IsKind(thisadapt->DynamicType()))
      return;
    if (!(thisadapt->IsKind(newadapt->DynamicType())) && thisadapt != newadapt)
      throw Standard_DomainError("XSControl_Controller : Record");
  }
  listad.Bind(theName, this);
}

//=================================================================================================

occ::handle<XSControl_Controller> XSControl_Controller::Recorded(const char* theName)
{
  occ::handle<Standard_Transient> recorded;
  return (listad.Find(theName, recorded) ? occ::down_cast<XSControl_Controller>(recorded)
                                         : occ::handle<XSControl_Controller>());
}

//    ####    DEFINITION    ####

//=================================================================================================

occ::handle<Transfer_ActorOfTransientProcess> XSControl_Controller::ActorRead(
  const occ::handle<Interface_InterfaceModel>&) const
{
  return myAdaptorRead;
}

//=================================================================================================

occ::handle<Transfer_ActorOfFinderProcess> XSControl_Controller::ActorWrite() const
{
  return myAdaptorWrite;
}

// ###########################
//  Transfer Help : value control + help

//=================================================================================================

void XSControl_Controller::SetModeWrite(const int modemin,
                                        const int modemax,
                                        const bool)
{
  if (modemin > modemax)
  {
    myModeWriteShapeN.Nullify();
    return;
  }
  myModeWriteShapeN = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(modemin, modemax);
}

//=================================================================================================

void XSControl_Controller::SetModeWriteHelp(const int modetrans,
                                            const char* help,
                                            const bool)
{
  if (myModeWriteShapeN.IsNull())
    return;
  if (modetrans < myModeWriteShapeN->Lower() || modetrans > myModeWriteShapeN->Upper())
    return;
  occ::handle<TCollection_HAsciiString> hl = new TCollection_HAsciiString(help);
  myModeWriteShapeN->SetValue(modetrans, hl);
}

//=================================================================================================

bool XSControl_Controller::ModeWriteBounds(int& modemin,
                                                       int& modemax,
                                                       const bool) const
{
  modemin = modemax = 0;
  if (myModeWriteShapeN.IsNull())
    return false;
  modemin = myModeWriteShapeN->Lower();
  modemax = myModeWriteShapeN->Upper();
  return true;
}

//=================================================================================================

bool XSControl_Controller::IsModeWrite(const int modetrans,
                                                   const bool) const
{
  if (myModeWriteShapeN.IsNull())
    return true;
  if (modetrans < myModeWriteShapeN->Lower())
    return false;
  if (modetrans > myModeWriteShapeN->Upper())
    return false;
  return true;
}

//=================================================================================================

const char* XSControl_Controller::ModeWriteHelp(const int modetrans,
                                                     const bool) const
{
  if (myModeWriteShapeN.IsNull())
    return "";
  if (modetrans < myModeWriteShapeN->Lower())
    return "";
  if (modetrans > myModeWriteShapeN->Upper())
    return "";
  occ::handle<TCollection_HAsciiString> str = myModeWriteShapeN->Value(modetrans);
  if (str.IsNull())
    return "";
  return str->ToCString();
}

// ###########################
//  Transfer : we do what is needed by default (with ActorWrite)
//    can be redefined ...

//=================================================================================================

bool XSControl_Controller::RecognizeWriteTransient(
  const occ::handle<Standard_Transient>& obj,
  const int            modetrans) const
{
  if (myAdaptorWrite.IsNull())
    return false;
  myAdaptorWrite->ModeTrans() = modetrans;
  return myAdaptorWrite->Recognize(new Transfer_TransientMapper(obj));
}

//=================================================================================================

static IFSelect_ReturnStatus TransferFinder(const occ::handle<Transfer_ActorOfFinderProcess>& theActor,
                                            const occ::handle<Transfer_Finder>&               theMapper,
                                            const occ::handle<Transfer_FinderProcess>&        theFP,
                                            const occ::handle<Interface_InterfaceModel>&      theModel,
                                            const int       theModeTrans,
                                            const Message_ProgressRange& theProgress)
{
  if (theActor.IsNull())
    return IFSelect_RetError;
  if (theModel.IsNull())
    return IFSelect_RetError;
  theActor->ModeTrans() = theModeTrans;
  theFP->SetModel(theModel);
  theFP->SetActor(theActor);
  theFP->Transfer(theMapper, theProgress);

  IFSelect_ReturnStatus                    stat   = IFSelect_RetFail;
  occ::handle<Transfer_Binder>                  binder = theFP->Find(theMapper);
  occ::handle<Transfer_SimpleBinderOfTransient> bindtr;
  while (!binder.IsNull())
  {
    bindtr = occ::down_cast<Transfer_SimpleBinderOfTransient>(binder);
    if (!bindtr.IsNull())
    {
      occ::handle<Standard_Transient> ent = bindtr->Result();
      if (!ent.IsNull())
      {
        stat = IFSelect_RetDone;
        theModel->AddWithRefs(ent);
      }
    }
    binder = binder->NextResult();
  }
  return stat;
}

//=================================================================================================

IFSelect_ReturnStatus XSControl_Controller::TransferWriteTransient(
  const occ::handle<Standard_Transient>&       theObj,
  const occ::handle<Transfer_FinderProcess>&   theFP,
  const occ::handle<Interface_InterfaceModel>& theModel,
  const int                  theModeTrans,
  const Message_ProgressRange&            theProgress) const
{
  if (theObj.IsNull())
    return IFSelect_RetVoid;
  return TransferFinder(myAdaptorWrite,
                        new Transfer_TransientMapper(theObj),
                        theFP,
                        theModel,
                        theModeTrans,
                        theProgress);
}

//=================================================================================================

bool XSControl_Controller::RecognizeWriteShape(const TopoDS_Shape&    shape,
                                                           const int modetrans) const
{
  if (myAdaptorWrite.IsNull())
    return false;
  myAdaptorWrite->ModeTrans() = modetrans;
  return myAdaptorWrite->Recognize(new TransferBRep_ShapeMapper(shape));
}

//=================================================================================================

IFSelect_ReturnStatus XSControl_Controller::TransferWriteShape(
  const TopoDS_Shape&                     shape,
  const occ::handle<Transfer_FinderProcess>&   FP,
  const occ::handle<Interface_InterfaceModel>& model,
  const int                  modetrans,
  const Message_ProgressRange&            theProgress) const
{
  if (shape.IsNull())
    return IFSelect_RetVoid;

  IFSelect_ReturnStatus theReturnStat = TransferFinder(myAdaptorWrite,
                                                       new TransferBRep_ShapeMapper(shape),
                                                       FP,
                                                       model,
                                                       modetrans,
                                                       theProgress);
  return theReturnStat;
}

// ###########################
//  Customization ! We register Items for a WorkSession
//     (cancels and replaces)
//     Then, we put them back in place on demand

//=================================================================================================

void XSControl_Controller::AddSessionItem(const occ::handle<Standard_Transient>& theItem,
                                          const char*            theName,
                                          const bool            toApply)
{
  if (theItem.IsNull() || theName[0] == '\0')
    return;
  myAdaptorSession.Bind(theName, theItem);
  if (toApply && theItem->IsKind(STANDARD_TYPE(IFSelect_GeneralModifier)))
    myAdaptorApplied.Append(theItem);
}

//=================================================================================================

occ::handle<Standard_Transient> XSControl_Controller::SessionItem(const char* theName) const
{
  occ::handle<Standard_Transient> item;
  if (!myAdaptorSession.IsEmpty())
    item = myAdaptorSession.Find(theName);
  return item;
}

//=================================================================================================

void XSControl_Controller::Customise(occ::handle<XSControl_WorkSession>& WS)
{
  WS->SetParams(myParams, myParamUses);

  // General
  if (!myAdaptorSession.IsEmpty())
  {
    NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(
      myAdaptorSession);
    for (; iter.More(); iter.Next())
      WS->AddNamedItem(iter.Key().ToCString(), iter.ChangeValue());
  }

  if (WS->NamedItem("xst-model-all").IsNull())
  {

    occ::handle<IFSelect_SelectModelEntities> sle = new IFSelect_SelectModelEntities;
    WS->AddNamedItem("xst-model-all", sle);

    occ::handle<IFSelect_SelectModelRoots> slr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem("xst-model-roots", slr);

    if (strcasecmp(WS->SelectedNorm(), "STEP"))
    {
      occ::handle<XSControl_SelectForTransfer> st1 = new XSControl_SelectForTransfer;
      st1->SetInput(slr);
      st1->SetReader(WS->TransferReader());
      WS->AddNamedItem("xst-transferrable-roots", st1);
    }

    occ::handle<XSControl_SelectForTransfer> st2 = new XSControl_SelectForTransfer;
    st2->SetInput(sle);
    st2->SetReader(WS->TransferReader());
    WS->AddNamedItem("xst-transferrable-all", st2);

    occ::handle<XSControl_SignTransferStatus> strs = new XSControl_SignTransferStatus;
    strs->SetReader(WS->TransferReader());
    WS->AddNamedItem("xst-transfer-status", strs);

    occ::handle<XSControl_ConnectedShapes> scs = new XSControl_ConnectedShapes;
    scs->SetReader(WS->TransferReader());
    WS->AddNamedItem("xst-connected-faces", scs);

    occ::handle<IFSelect_SignType> stp = new IFSelect_SignType(false);
    WS->AddNamedItem("xst-long-type", stp);

    occ::handle<IFSelect_SignType> stc = new IFSelect_SignType(true);
    WS->AddNamedItem("xst-type", stc);

    WS->AddNamedItem("xst-ancestor-type", new IFSelect_SignAncestor);
    WS->AddNamedItem("xst-types", new IFSelect_SignCounter(stp, false, true));
    WS->AddNamedItem("xst-category", new IFSelect_SignCategory);
    WS->AddNamedItem("xst-validity", new IFSelect_SignValidity);

    occ::handle<IFSelect_DispPerOne> dispone = new IFSelect_DispPerOne;
    dispone->SetFinalSelection(slr);
    WS->AddNamedItem("xst-disp-one", dispone);

    occ::handle<IFSelect_DispPerCount> dispcount = new IFSelect_DispPerCount;
    occ::handle<IFSelect_IntParam>     intcount  = new IFSelect_IntParam;
    intcount->SetValue(5);
    dispcount->SetCount(intcount);
    dispcount->SetFinalSelection(slr);
    WS->AddNamedItem("xst-disp-count", dispcount);

    occ::handle<IFSelect_DispPerFiles> dispfiles = new IFSelect_DispPerFiles;
    occ::handle<IFSelect_IntParam>     intfiles  = new IFSelect_IntParam;
    intfiles->SetValue(10);
    dispfiles->SetCount(intfiles);
    dispfiles->SetFinalSelection(slr);
    WS->AddNamedItem("xst-disp-files", dispfiles);

    occ::handle<IFSelect_DispPerSignature> dispsign = new IFSelect_DispPerSignature;
    dispsign->SetSignCounter(new IFSelect_SignCounter(occ::handle<IFSelect_Signature>(stc)));
    dispsign->SetFinalSelection(slr);
    WS->AddNamedItem("xst-disp-sign", dispsign);

    // Not used directly but useful anyway
    WS->AddNamedItem("xst-pointed", new IFSelect_SelectPointed);
    WS->AddNamedItem("xst-sharing", new IFSelect_SelectSharing);
    WS->AddNamedItem("xst-shared", new IFSelect_SelectShared);
    WS->AddNamedItem("xst-nb-selected", new IFSelect_GraphCounter);

    // szv:mySignType = stp;
    WS->SetSignType(stp);
  }

  // Applied Modifiers
  int i, nb = myAdaptorApplied.Length();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Standard_Transient>& anitem = myAdaptorApplied.Value(i);
    occ::handle<TCollection_HAsciiString>  name   = WS->Name(anitem);
    WS->SetAppliedModifier(GetCasted(IFSelect_GeneralModifier, anitem), WS->ShareOut());
  }

  // Editors of Parameters
  // Here for the specific manufacturers of controllers could create the
  // Parameters: So wait here

  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> listat = Interface_Static::Items();
  occ::handle<IFSelect_ParamEditor>            paramed =
    IFSelect_ParamEditor::StaticEditor(listat, "All Static Parameters");
  WS->AddNamedItem("xst-static-params-edit", paramed);
  occ::handle<IFSelect_EditForm> paramform = paramed->Form(false);
  WS->AddNamedItem("xst-static-params", paramform);
}
