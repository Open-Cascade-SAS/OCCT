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

// #58 rln 28.12.98 changing default values for Global Section
// sln 14.01.2002 OCC51 : verifying whether entry model of method ActorRead is IGESDatat_IGESModel

#include <IFSelect_EditForm.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectType.hxx>
#include <IFSelect_SignMultiple.hxx>
#include <IFSelect_SignType.hxx>
#include <IGESAppli.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESControl_ActorWrite.hxx>
#include <IGESControl_AlgoContainer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESSelect_AutoCorrect.hxx>
#include <IGESSelect_ComputeStatus.hxx>
#include <IGESSelect_CounterOfLevelNumber.hxx>
#include <IGESSelect_EditDirPart.hxx>
#include <IGESSelect_EditHeader.hxx>
#include <IGESSelect_FloatFormat.hxx>
#include <IGESSelect_IGESName.hxx>
#include <IGESSelect_IGESTypeForm.hxx>
#include <IGESSelect_RemoveCurves.hxx>
#include <IGESSelect_SelectBasicGeom.hxx>
#include <IGESSelect_SelectBypassGroup.hxx>
#include <IGESSelect_SelectBypassSubfigure.hxx>
#include <IGESSelect_SelectFaces.hxx>
#include <IGESSelect_SelectPCurves.hxx>
#include <IGESSelect_SelectSubordinate.hxx>
#include <IGESSelect_SelectVisibleStatus.hxx>
#include <IGESSelect_SetGlobalParameter.hxx>
#include <IGESSelect_SetLabel.hxx>
#include <IGESSelect_SignColor.hxx>
#include <IGESSelect_SignLevelNumber.hxx>
#include <IGESSelect_SignStatus.hxx>
#include <IGESSelect_UpdateFileName.hxx>
#include <IGESSelect_WorkLibrary.hxx>
#include <IGESSolid.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Static.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSAlgo.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_WorkSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_Controller, XSControl_Controller)

//=================================================================================================

IGESControl_Controller::IGESControl_Controller(const bool mod)
    : XSControl_Controller((const char*)(mod ? "FNES" : "IGES"),
                           (const char*)(mod ? "fnes" : "iges")),
      themode(mod)
{
  static bool init = false;
  if (!init)
  {
    IGESSolid::Init();
    IGESAppli::Init();
    init = true;
  }
  AddSessionItem(new IGESSelect_RemoveCurves(true), "iges-remove-pcurves");
  AddSessionItem(new IGESSelect_RemoveCurves(false), "iges-remove-curves-3d");
  AddSessionItem(new IGESSelect_SetLabel(0, true), "iges-clear-label");
  AddSessionItem(new IGESSelect_SetLabel(1, false), "iges-set-label-dnum");

  AddSessionItem(new IGESSelect_AutoCorrect, "iges-auto-correct", true);
  AddSessionItem(new IGESSelect_ComputeStatus, "iges-compute-status", true);

  occ::handle<IGESSelect_FloatFormat> flf = new IGESSelect_FloatFormat;
  flf->SetDefault(12);
  AddSessionItem(flf, "iges-float-digits-12", true);

  //  --   Sender Product Identification   --  (not a static ...)
  occ::handle<IGESSelect_SetGlobalParameter> set3 = new IGESSelect_SetGlobalParameter(3);
  occ::handle<TCollection_HAsciiString>      pa3 =
    Interface_Static::Static("write.iges.header.product")->HStringValue();
  set3->SetValue(pa3);
  AddSessionItem(pa3, "iges-header-val-sender");
  AddSessionItem(set3, "iges-header-set-sender", true);

  AddSessionItem(new IGESSelect_UpdateFileName, "iges-update-file-name", true);

  //  --   Receiver   --   Access by Static, adjustable
  occ::handle<IGESSelect_SetGlobalParameter> set12 = new IGESSelect_SetGlobalParameter(12);
  occ::handle<TCollection_HAsciiString>      pa12 =
    Interface_Static::Static("write.iges.header.receiver")->HStringValue();
  set12->SetValue(pa12);
  AddSessionItem(pa12, "iges-header-val-receiver");
  AddSessionItem(set12, "iges-header-set-receiver", true);

  //  --   Author   --   access by Static (started by whoami), adjustable
  occ::handle<IGESSelect_SetGlobalParameter> set21 = new IGESSelect_SetGlobalParameter(21);
  occ::handle<TCollection_HAsciiString>      pa21 =
    Interface_Static::Static("write.iges.header.author")->HStringValue();
  set21->SetValue(pa21);
  AddSessionItem(pa21, "iges-header-val-author");
  AddSessionItem(set21, "iges-header-set-author", true);

  //  --   Company (of the author)   --   access by Static, adjustable
  occ::handle<IGESSelect_SetGlobalParameter> set22 = new IGESSelect_SetGlobalParameter(22);
  occ::handle<TCollection_HAsciiString>      pa22 =
    Interface_Static::Static("write.iges.header.company")->HStringValue();
  set22->SetValue(pa22);
  AddSessionItem(pa22, "iges-header-val-company");
  AddSessionItem(set22, "iges-header-set-company", true);

  //  -- STATICS

  TraceStatic("read.iges.bspline.approxd1.mode", 5);
  TraceStatic("read.iges.bspline.continuity", 5);

  TraceStatic("write.iges.header.receiver", 2);
  TraceStatic("write.iges.header.author", 2);
  TraceStatic("write.iges.header.company", 2);
  TraceStatic("write.iges.unit", 6);
  TraceStatic("write.iges.brep.mode", 6);

  myAdaptorLibrary  = new IGESSelect_WorkLibrary(themode);
  myAdaptorProtocol = IGESSelect_WorkLibrary::DefineProtocol();

  occ::handle<IGESToBRep_Actor> anactiges = new IGESToBRep_Actor;
  anactiges->SetContinuity(0);
  myAdaptorRead = anactiges;

  myAdaptorWrite = new IGESControl_ActorWrite;

  SetModeWrite(0, 1);
  SetModeWriteHelp(0, "Faces");
  SetModeWriteHelp(1, "BRep");
}

void IGESControl_Controller::Customise(occ::handle<XSControl_WorkSession>& WS)
{
  XSControl_Controller::Customise(WS);

  //   ---  SELECTIONS, SIGNATURES, COMPTEURS, EDITEURS
  //   --   BypassGroup / xst-model-roots

  // Should be already set by the above call to Customise
  occ::handle<IFSelect_SelectModelEntities> xma;
  occ::handle<Standard_Transient>           xma1 = WS->NamedItem("xst-model-all");
  if (xma1.IsNull())
    xma = new IFSelect_SelectModelEntities;
  else
  {
    xma = occ::down_cast<IFSelect_SelectModelEntities>(xma1);
    WS->AddNamedItem("xst-model-all", xma);
  }

  occ::handle<IFSelect_SelectModelRoots> xmr;
  occ::handle<Standard_Transient>        xmr1 = WS->NamedItem("xst-model-roots");
  if (!xmr1.IsNull())
    xmr = occ::down_cast<IFSelect_SelectModelRoots>(xmr1);
  else
  {
    xmr = new IFSelect_SelectModelRoots;
    WS->AddNamedItem("xst-model-roots", xmr);
  }

  occ::handle<XSControl_SelectForTransfer> xtr;
  occ::handle<Standard_Transient>          xtr1 = WS->NamedItem("xst-transferrable-roots");
  if (!xtr1.IsNull())
    xtr = occ::down_cast<XSControl_SelectForTransfer>(xtr1);
  else
  {
    xtr = new XSControl_SelectForTransfer;
    xtr->SetReader(WS->TransferReader());
    WS->AddNamedItem("xst-transferrable-roots", xtr);
  }

  if (!xmr.IsNull())
  {
    occ::handle<IGESSelect_SelectVisibleStatus> visa = new IGESSelect_SelectVisibleStatus;
    visa->SetInput(xmr);
    WS->AddNamedItem("iges-visible-roots", visa);
    occ::handle<IGESSelect_SelectVisibleStatus> vist = new IGESSelect_SelectVisibleStatus;
    vist->SetInput(xtr);
    WS->AddNamedItem("iges-visible-transf-roots", vist);
    occ::handle<IGESSelect_SelectVisibleStatus> blka = new IGESSelect_SelectVisibleStatus;
    blka->SetDirect(false);
    blka->SetInput(xmr);
    WS->AddNamedItem("iges-blanked-roots", blka);
    occ::handle<IGESSelect_SelectVisibleStatus> blkt = new IGESSelect_SelectVisibleStatus;
    blkt->SetDirect(false);
    blkt->SetInput(xtr);
    WS->AddNamedItem("iges-blanked-transf-roots", blkt);
    occ::handle<IGESSelect_SelectSubordinate> indp = new IGESSelect_SelectSubordinate(0);
    indp->SetInput(xma);
    WS->AddNamedItem("iges-status-independant", indp);

    occ::handle<IGESSelect_SelectBypassGroup> sb = new IGESSelect_SelectBypassGroup;
    sb->SetInput(xmr);
    WS->AddNamedItem("iges-bypass-group", sb);
    occ::handle<IGESSelect_SelectBypassSubfigure> sfi = new IGESSelect_SelectBypassSubfigure;
    sfi->SetInput(xmr);
    WS->AddNamedItem("iges-bypass-subfigure", sfi);
    occ::handle<IGESSelect_SelectBypassGroup> sfb = new IGESSelect_SelectBypassGroup;
    sfb->SetInput(sfi);
    WS->AddNamedItem("iges-bypass-group-subfigure", sfb);
    occ::handle<IGESSelect_SelectBasicGeom> sc3d = new IGESSelect_SelectBasicGeom(1);
    sc3d->SetInput(sfi);
    WS->AddNamedItem("iges-curves-3d", sc3d);
    occ::handle<IGESSelect_SelectBasicGeom> sb3d = new IGESSelect_SelectBasicGeom(2);
    sb3d->SetInput(sfi);
    WS->AddNamedItem("iges-basic-curves-3d", sb3d);
    occ::handle<IGESSelect_SelectBasicGeom> sbg = new IGESSelect_SelectBasicGeom(0);
    sbg->SetInput(sfi);
    WS->AddNamedItem("iges-basic-geom", sbg);
    occ::handle<IGESSelect_SelectBasicGeom> srf = new IGESSelect_SelectBasicGeom(-1);
    srf->SetInput(sfi);
    WS->AddNamedItem("iges-surfaces", srf);
    occ::handle<IGESSelect_SelectFaces> sfa = new IGESSelect_SelectFaces;
    sfa->SetInput(sfi);
    WS->AddNamedItem("iges-faces", sfa);
    occ::handle<IGESSelect_SelectPCurves> spc = new IGESSelect_SelectPCurves(true);
    spc->SetInput(sfa);
    WS->AddNamedItem("iges-pcurves", spc);

    occ::handle<IFSelect_SelectType> snosub =
      new IFSelect_SelectType(STANDARD_TYPE(IGESBasic_SubfigureDef));
    snosub->SetDirect(false);
    snosub->SetInput(xmr);
    WS->AddNamedItem("iges-no-indep-subfigure-def", snosub);

    occ::handle<IGESSelect_IGESTypeForm> itf = new IGESSelect_IGESTypeForm(true);
    WS->AddNamedItem("iges-type", itf);

    occ::handle<IGESSelect_SignStatus> sigst = new IGESSelect_SignStatus;
    occ::handle<IFSelect_SignMultiple> typsta =
      new IFSelect_SignMultiple("IGES Type Form    Status");
    typsta->Add(itf, 15);
    typsta->Add(sigst);
    WS->AddNamedItem("iges-type-status", typsta);

    occ::handle<IFSelect_SignMultiple> typnam =
      new IFSelect_SignMultiple("IGES Type Form  TypeName");
    typnam->Add(itf, 4);
    typnam->Add(new IFSelect_SignType(true));
    WS->AddNamedItem("iges-type-name", typnam);

    occ::handle<IFSelect_SignCounter> itfs = new IFSelect_SignCounter(itf, false, true);
    WS->AddNamedItem("iges-types", itfs);
    occ::handle<IGESSelect_CounterOfLevelNumber> levs = new IGESSelect_CounterOfLevelNumber;
    WS->AddNamedItem("iges-levels", levs);

    occ::handle<IGESSelect_SignLevelNumber> slev = new IGESSelect_SignLevelNumber(false);
    WS->AddNamedItem("iges-level-number", slev);
    occ::handle<IGESSelect_IGESName> igna = new IGESSelect_IGESName;
    WS->AddNamedItem("iges-name", igna);

    occ::handle<IGESSelect_SignColor> scol1 = new IGESSelect_SignColor(1);
    WS->AddNamedItem("iges-color-number", scol1);
    occ::handle<IGESSelect_SignColor> scol2 = new IGESSelect_SignColor(2);
    WS->AddNamedItem("iges-color-name", scol2);
    occ::handle<IGESSelect_SignColor> scol3 = new IGESSelect_SignColor(3);
    WS->AddNamedItem("iges-color-rgb", scol3);
    occ::handle<IGESSelect_SignColor> scol4 = new IGESSelect_SignColor(4);
    WS->AddNamedItem("iges-color-red", scol4);
    occ::handle<IGESSelect_SignColor> scol5 = new IGESSelect_SignColor(5);
    WS->AddNamedItem("iges-color-green", scol5);
    occ::handle<IGESSelect_SignColor> scol6 = new IGESSelect_SignColor(6);
    WS->AddNamedItem("iges-color-blue", scol6);

    occ::handle<IGESSelect_EditHeader> edhead = new IGESSelect_EditHeader;
    WS->AddNamedItem("iges-header-edit", edhead);
    occ::handle<IFSelect_EditForm> edheadf = edhead->Form(false);
    WS->AddNamedItem("iges-header", edheadf);

    occ::handle<IGESSelect_EditDirPart> eddirp = new IGESSelect_EditDirPart;
    WS->AddNamedItem("iges-dir-part-edit", eddirp);
    occ::handle<IFSelect_EditForm> eddirpf = eddirp->Form(false);
    WS->AddNamedItem("iges-dir-part", eddirpf);

    // szv:mySignType = typnam;
    WS->SetSignType(typnam);
  }
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> IGESControl_Controller::NewModel() const
{
  //  We take a model that we prepare with registered statics
  DeclareAndCast(IGESData_IGESModel, igm, Interface_InterfaceModel::Template("iges"));
  IGESData_GlobalSection GS = igm->GlobalSection();

  GS.SetReceiveName(Interface_Static::Static("write.iges.header.receiver")->HStringValue());
  GS.SetUnitFlag(Interface_Static::IVal("write.iges.unit"));
  GS.SetUnitName(new TCollection_HAsciiString(Interface_Static::CVal("write.iges.unit")));
  GS.SetAuthorName(Interface_Static::Static("write.iges.header.author")->HStringValue());
  GS.SetCompanyName(Interface_Static::Static("write.iges.header.company")->HStringValue());
  igm->SetGlobalSection(GS);
  return igm;
}

//=================================================================================================

occ::handle<Transfer_ActorOfTransientProcess> IGESControl_Controller::ActorRead(
  const occ::handle<Interface_InterfaceModel>& model) const
{
  DeclareAndCast(IGESToBRep_Actor, anactiges, myAdaptorRead);
  if (!anactiges.IsNull())
  {
    // sln 14.01.2002 OCC51 : verifying whether entry model is IGESDatat_IGESModel,
    // if this condition is false new model is created
    occ::handle<Interface_InterfaceModel> aModel =
      (model->IsKind(STANDARD_TYPE(IGESData_IGESModel)) ? model : NewModel());
    anactiges->SetModel(GetCasted(IGESData_IGESModel, aModel));
    anactiges->SetContinuity(Interface_Static::IVal("read.iges.bspline.continuity"));
  }
  return myAdaptorRead;
}

//  ####    TRANSFER (SHAPE WRITING)    ####
//  modetrans : 0  <5.1 (group of faces),  1 BREP-5.1

//=================================================================================================

IFSelect_ReturnStatus IGESControl_Controller::TransferWriteShape(
  const TopoDS_Shape&                          shape,
  const occ::handle<Transfer_FinderProcess>&   FP,
  const occ::handle<Interface_InterfaceModel>& model,
  const int                                    modetrans,
  const Message_ProgressRange&                 theProgress) const
{
  return XSControl_Controller::TransferWriteShape(shape, FP, model, modetrans, theProgress);
}

//=================================================================================================

bool IGESControl_Controller::Init()
{
  static bool inic = false;
  if (!inic)
  {
    occ::handle<IGESControl_Controller> ADIGES = new IGESControl_Controller(false);
    ADIGES->AutoRecord();
    XSAlgo::Init();
    IGESToBRep::Init();
    IGESToBRep::SetAlgoContainer(new IGESControl_AlgoContainer());
    inic = true;
  }
  return true;
}
