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
#include <IFSelect_GeneralModifier.hxx>
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
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_ConnectedShapes.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_SignTransferStatus.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_Controller,Standard_Transient)

//  ParamEditor
//  Transferts

static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> listad;

//=======================================================================
//function : XSControl_Controller
//purpose  : Constructor
//=======================================================================

XSControl_Controller::XSControl_Controller (const Standard_CString theLongName, const Standard_CString theShortName)
: myShortName(theShortName), myLongName(theLongName)
{
  // Standard parameters
  Interface_Static::Standards();
  TraceStatic ("read.precision.mode" , 5);
  TraceStatic ("read.precision.val"  , 5);
  TraceStatic ("write.precision.mode" , 6);
  TraceStatic ("write.precision.val"  , 6);
}

//=======================================================================
//function : TraceStatic
//purpose  : 
//=======================================================================

void XSControl_Controller::TraceStatic (const Standard_CString theName, const Standard_Integer theUse)
{
  Handle(Interface_Static) val = Interface_Static::Static(theName);
  if (val.IsNull()) return;
  myParams.Append (val);
  myParamUses.Append(theUse);
}

//=======================================================================
//function : SetNames
//purpose  : 
//=======================================================================

void XSControl_Controller::SetNames (const Standard_CString theLongName, const Standard_CString theShortName)
{
  if (theLongName && theLongName[0] != '\0') {
    myLongName.Clear();  myLongName.AssignCat (theLongName);
  }
  if (theShortName && theShortName[0] != '\0') {
    myShortName.Clear(); myShortName.AssignCat(theShortName);
  }
}

//=======================================================================
//function : Record
//purpose  : 
//=======================================================================

void XSControl_Controller::Record (const Standard_CString theName) const
{
  if (listad.IsBound(theName)) {
    Handle(Standard_Transient) thisadapt(this);
    Handle(Standard_Transient) newadapt = listad.ChangeFind(theName);
    if (newadapt->IsKind(thisadapt->DynamicType()))
      return;
    if (!(thisadapt->IsKind(newadapt->DynamicType())) && thisadapt != newadapt)
      throw Standard_DomainError("XSControl_Controller : Record");
  }
  listad.Bind(theName, this);
}

//=======================================================================
//function : Recorded
//purpose  : 
//=======================================================================

Handle(XSControl_Controller) XSControl_Controller::Recorded(const Standard_CString theName)
{
  Handle(Standard_Transient) recorded;
  return (listad.Find(theName, recorded)?
    Handle(XSControl_Controller)::DownCast(recorded) :
    Handle(XSControl_Controller)());
}

//    ####    DEFINITION    ####

//=======================================================================
//function : ActorRead
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfTransientProcess) XSControl_Controller::ActorRead (const Handle(Interface_InterfaceModel)&) const
{
  return myAdaptorRead;
}

//=======================================================================
//function : ActorWrite
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfFinderProcess) XSControl_Controller::ActorWrite () const
{
  return myAdaptorWrite;
}

// ###########################
//  Help du Transfer : controle de valeur + help

//=======================================================================
//function : SetModeWrite
//purpose  : 
//=======================================================================

void XSControl_Controller::SetModeWrite
  (const Standard_Integer modemin, const Standard_Integer modemax, const Standard_Boolean )
{
  if (modemin > modemax)  {  myModeWriteShapeN.Nullify(); return;  }
  myModeWriteShapeN = new Interface_HArray1OfHAsciiString (modemin,modemax);
}

//=======================================================================
//function : SetModeWriteHelp
//purpose  : 
//=======================================================================

void XSControl_Controller::SetModeWriteHelp
  (const Standard_Integer modetrans, const Standard_CString help, const Standard_Boolean )
{
  if (myModeWriteShapeN.IsNull()) return;
  if (modetrans < myModeWriteShapeN->Lower() ||
      modetrans > myModeWriteShapeN->Upper()) return;
  Handle(TCollection_HAsciiString) hl = new TCollection_HAsciiString (help);
  myModeWriteShapeN->SetValue (modetrans,hl);
}

//=======================================================================
//function : ModeWriteBounds
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_Controller::ModeWriteBounds
  (Standard_Integer& modemin, Standard_Integer& modemax, const Standard_Boolean ) const
{
  modemin = modemax = 0;
  if (myModeWriteShapeN.IsNull()) return Standard_False;
  modemin = myModeWriteShapeN->Lower();
  modemax = myModeWriteShapeN->Upper();
  return Standard_True;
}

//=======================================================================
//function : IsModeWrite
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_Controller::IsModeWrite
  (const Standard_Integer modetrans, const Standard_Boolean ) const
{
  if (myModeWriteShapeN.IsNull()) return Standard_True;
  if (modetrans < myModeWriteShapeN->Lower()) return Standard_False;
  if (modetrans > myModeWriteShapeN->Upper()) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : ModeWriteHelp
//purpose  : 
//=======================================================================

Standard_CString  XSControl_Controller::ModeWriteHelp
  (const Standard_Integer modetrans, const Standard_Boolean ) const
{
  if (myModeWriteShapeN.IsNull()) return "";
  if (modetrans < myModeWriteShapeN->Lower()) return "";
  if (modetrans > myModeWriteShapeN->Upper()) return "";
  Handle(TCollection_HAsciiString) str = myModeWriteShapeN->Value(modetrans);
  if (str.IsNull()) return "";
  return str->ToCString();
}


// ###########################
//  Transfer : on fait ce qu il faut par defaut (avec ActorWrite)
//    peut etre redefini ...

//=======================================================================
//function : RecognizeWriteTransient
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_Controller::RecognizeWriteTransient
  (const Handle(Standard_Transient)& obj,
   const Standard_Integer modetrans) const
{
  if (myAdaptorWrite.IsNull()) return Standard_False;
  myAdaptorWrite->ModeTrans() = modetrans;
  return myAdaptorWrite->Recognize (new Transfer_TransientMapper(obj));
}

//=======================================================================
//function : TransferFinder
//purpose  : internal function
//=======================================================================

static IFSelect_ReturnStatus TransferFinder
  (const Handle(Transfer_ActorOfFinderProcess)& theActor,
   const Handle(Transfer_Finder)& theMapper,
   const Handle(Transfer_FinderProcess)& theFP,
   const Handle(Interface_InterfaceModel)& theModel,
   const Standard_Integer theModeTrans)
{
  if (theActor.IsNull()) return IFSelect_RetError;
  if (theModel.IsNull()) return IFSelect_RetError;
  theActor->ModeTrans() = theModeTrans;
  theFP->SetModel (theModel);
  theFP->SetActor (theActor);
  theFP->Transfer (theMapper);

  IFSelect_ReturnStatus stat = IFSelect_RetFail;
  Handle(Transfer_Binder) binder = theFP->Find (theMapper);
  Handle(Transfer_SimpleBinderOfTransient) bindtr;
  while (!binder.IsNull()) {
    bindtr = Handle(Transfer_SimpleBinderOfTransient)::DownCast (binder);
    if (!bindtr.IsNull()) {
      Handle(Standard_Transient) ent = bindtr->Result();
      if (!ent.IsNull()) {
        stat = IFSelect_RetDone;
        theModel->AddWithRefs (ent);
      }
    }
    binder = binder->NextResult();
  }
  return stat;
}

//=======================================================================
//function : TransferWriteTransient
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_Controller::TransferWriteTransient
  (const Handle(Standard_Transient)& theObj,
   const Handle(Transfer_FinderProcess)& theFP,
   const Handle(Interface_InterfaceModel)& theModel,
   const Standard_Integer theModeTrans) const
{
  if (theObj.IsNull()) return IFSelect_RetVoid;
  return TransferFinder
    (myAdaptorWrite,new Transfer_TransientMapper(theObj),theFP,theModel,theModeTrans);
}

//=======================================================================
//function : RecognizeWriteShape
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_Controller::RecognizeWriteShape
  (const TopoDS_Shape& shape,
   const Standard_Integer modetrans) const
{
  if (myAdaptorWrite.IsNull()) return Standard_False;
  myAdaptorWrite->ModeTrans() = modetrans;
  return myAdaptorWrite->Recognize (new TransferBRep_ShapeMapper(shape));
}

//=======================================================================
//function : TransferWriteShape
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_Controller::TransferWriteShape
  (const TopoDS_Shape& shape,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modetrans) const
{
  if (shape.IsNull()) return IFSelect_RetVoid;

  IFSelect_ReturnStatus theReturnStat = TransferFinder
    (myAdaptorWrite,new TransferBRep_ShapeMapper(shape),FP,model,modetrans);
  return theReturnStat;
}

// ###########################
//  Cutomisation ! On enregistre des Items pour une WorkSession
//     (annule et remplace)
//     Ensuite, on les remet en place a la demande

//=======================================================================
//function : AddSessionItem
//purpose  : 
//=======================================================================

void XSControl_Controller::AddSessionItem
  (const Handle(Standard_Transient)& theItem, const Standard_CString theName, const Standard_Boolean toApply)
{
  if (theItem.IsNull() || theName[0] == '\0') return;
  myAdaptorSession.Bind(theName,theItem);
  if (toApply && theItem->IsKind(STANDARD_TYPE(IFSelect_GeneralModifier)))
    myAdaptorApplied.Append(theItem);
}

//=======================================================================
//function : SessionItem
//purpose  : 
//=======================================================================

Handle(Standard_Transient)  XSControl_Controller::SessionItem (const Standard_CString theName) const
{
  Handle(Standard_Transient) item;
  if (!myAdaptorSession.IsEmpty())
    item = myAdaptorSession.Find(theName);
  return item;
}

//=======================================================================
//function : Customise
//purpose  : 
//=======================================================================

void XSControl_Controller::Customise (Handle(XSControl_WorkSession)& WS)
{
  WS->SetParams (myParams,myParamUses);

  // General
  if (!myAdaptorSession.IsEmpty()) {
    NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator iter(myAdaptorSession);
    for (; iter.More(); iter.Next())
      WS->AddNamedItem (iter.Key().ToCString(), iter.ChangeValue());
  }

  if (WS->NamedItem("xst-model-all").IsNull()) {

    Handle(IFSelect_SelectModelEntities) sle = new IFSelect_SelectModelEntities;
    WS->AddNamedItem ("xst-model-all",sle);

    Handle(IFSelect_SelectModelRoots)    slr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem ("xst-model-roots",slr);

    if(strcasecmp(WS->SelectedNorm(),"STEP")) {
      Handle(XSControl_SelectForTransfer) st1 = new XSControl_SelectForTransfer;
      st1->SetInput (slr);
      st1->SetReader (WS->TransferReader());
      WS->AddNamedItem ("xst-transferrable-roots",st1);
    }

    Handle(XSControl_SelectForTransfer) st2 = new XSControl_SelectForTransfer;
    st2->SetInput (sle);
    st2->SetReader (WS->TransferReader());
    WS->AddNamedItem ("xst-transferrable-all",st2);
   
    Handle(XSControl_SignTransferStatus) strs = new XSControl_SignTransferStatus;
    strs->SetReader (WS->TransferReader());
    WS->AddNamedItem ("xst-transfer-status",strs);
  
    Handle(XSControl_ConnectedShapes) scs = new XSControl_ConnectedShapes;
    scs->SetReader (WS->TransferReader());
    WS->AddNamedItem ("xst-connected-faces",scs);

    Handle(IFSelect_SignType) stp = new IFSelect_SignType (Standard_False);
    WS->AddNamedItem ("xst-long-type",stp);

    Handle(IFSelect_SignType) stc = new IFSelect_SignType (Standard_True);
    WS->AddNamedItem ("xst-type",stc);

    WS->AddNamedItem ("xst-ancestor-type",new IFSelect_SignAncestor);
    WS->AddNamedItem ("xst-types",new IFSelect_SignCounter(stp,Standard_False,Standard_True));
    WS->AddNamedItem ("xst-category",new IFSelect_SignCategory);
    WS->AddNamedItem ("xst-validity",new IFSelect_SignValidity);

    Handle(IFSelect_DispPerOne) dispone = new IFSelect_DispPerOne;
    dispone->SetFinalSelection(slr);
    WS->AddNamedItem ("xst-disp-one",dispone);

    Handle(IFSelect_DispPerCount) dispcount = new IFSelect_DispPerCount;
    Handle(IFSelect_IntParam) intcount = new IFSelect_IntParam;
    intcount->SetValue(5);
    dispcount->SetCount(intcount);
    dispcount->SetFinalSelection(slr);
    WS->AddNamedItem ("xst-disp-count",dispcount);

    Handle(IFSelect_DispPerFiles) dispfiles = new IFSelect_DispPerFiles;
    Handle(IFSelect_IntParam) intfiles = new IFSelect_IntParam;
    intfiles->SetValue(10);
    dispfiles->SetCount(intfiles);
    dispfiles->SetFinalSelection(slr);
    WS->AddNamedItem ("xst-disp-files",dispfiles);

    Handle(IFSelect_DispPerSignature) dispsign = new IFSelect_DispPerSignature;
    dispsign->SetSignCounter(new IFSelect_SignCounter(Handle(IFSelect_Signature)(stc)));
    dispsign->SetFinalSelection(slr);
    WS->AddNamedItem ("xst-disp-sign",dispsign);

    // Not used directly but useful anyway
    WS->AddNamedItem ("xst-pointed",new IFSelect_SelectPointed);
    WS->AddNamedItem ("xst-sharing",new IFSelect_SelectSharing);
    WS->AddNamedItem ("xst-shared",new IFSelect_SelectShared);
    WS->AddNamedItem ("xst-nb-selected",new IFSelect_GraphCounter);

    //szv:mySignType = stp;
    WS->SetSignType( stp );
  }

  // Applied Modifiers
  Standard_Integer i, nb = myAdaptorApplied.Length();
  for (i = 1; i <= nb; i ++) {
    const Handle(Standard_Transient) &anitem = myAdaptorApplied.Value(i);
    Handle(TCollection_HAsciiString) name = WS->Name(anitem);
    WS->SetAppliedModifier(GetCasted(IFSelect_GeneralModifier,anitem),WS->ShareOut());
  }

  // Editors of Parameters
  // Here for the specific manufacturers of controllers could create the
  // Parameters: So wait here

  Handle(TColStd_HSequenceOfHAsciiString) listat = Interface_Static::Items();
  Handle(IFSelect_ParamEditor) paramed = IFSelect_ParamEditor::StaticEditor (listat,"All Static Parameters");
  WS->AddNamedItem ("xst-static-params-edit",paramed);
  Handle(IFSelect_EditForm) paramform = paramed->Form(Standard_False);
  WS->AddNamedItem ("xst-static-params",paramform);
}
