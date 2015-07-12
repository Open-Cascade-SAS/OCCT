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


#include <Dico_IteratorOfDictionaryOfInteger.hxx>
#include <IFSelect_Act.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <MoniTool_Option.hxx>
#include <MoniTool_Profile.hxx>
#include <MoniTool_TypedValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_TransferIterator.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_Functions.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>

//#include <TransferBRep_ShapeBinder.hxx>
//#include <TransferBRep_ShapeListBinder.hxx>
//#include <TransferBRep_ShapeMapper.hxx>
//#include <TransferBRep_OrientedShapeMapper.hxx>
// #######################################################################
// ##									##
// ##									##
// ##				FUNCTIONS				##
// ##									##
// ##									##
// #######################################################################
//=======================================================================
//function : xinit
//=======================================================================
static IFSelect_ReturnStatus XSControl_xinit(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xinit        ****
  if (argc > 1) return (XSControl::Session(pilot)->SelectNorm(arg1) ?
			IFSelect_RetDone : IFSelect_RetFail);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Selected Norm:"<<XSControl::Session(pilot)->SelectedNorm()<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : xnorm
//=======================================================================
static IFSelect_ReturnStatus XSControl_xnorm(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xnorm        ****
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  Handle(XSControl_Controller) control = WS->NormAdaptor();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc == 1)
    sout<<"Current Norm. xnorm newnorm [profile] to change , xnorm ? for the list"<<endl;
  else sout<<"Current Norm :"<<endl;
  if (control.IsNull()) sout<<"no norm currently defined"<<endl;
  else
    sout<<"  Long  Name (complete) : "<<control->Name(Standard_False)<<endl
      <<  "  Short name (resource) : "<<control->Name(Standard_True)<<endl;
  if (argc == 1) return IFSelect_RetVoid;

  if (arg1[0] == '?') {
    sout<<"List of available norms"<<endl;
    Standard_Integer i,nb;
    Handle(TColStd_HSequenceOfHAsciiString) norms = XSControl_Controller::ListRecorded(-1);
    nb = norms->Length();
    sout<<"Short Proper Names (complete names) : "<<nb<<" :";
    for (i = 1; i <= nb; i ++) sout<<"  "<<norms->Value(i)->ToCString();
    sout<<endl;
    norms = XSControl_Controller::ListRecorded(1);
    nb = norms->Length();
    sout<<"Long  Proper Names (resource names) : "<<nb<<" :";
    for (i = 1; i <= nb; i ++) sout<<"  "<<norms->Value(i)->ToCString();
    sout<<endl;
    norms = XSControl_Controller::ListRecorded(0);
    nb = norms->Length();
    sout<<"All Norm Names (short, long and aliases) "<<nb<<" :";
    for (i = 1; i <= nb; i ++) sout<<"  "<<norms->Value(i)->ToCString();
    sout<<endl;
    sout<<"To change, xnorm newnorm"<<endl;
    return IFSelect_RetVoid;
  }

  control = XSControl_Controller::Recorded(arg1);
  if (control.IsNull()) { sout<<" No norm named : "<<arg1<<endl; return IFSelect_RetError; }
  else {
    WS->SetController(control);
    sout<<"new norm : "<<control->Name()<<endl;
    if (argc > 2) {
      const Standard_CString arg2 = pilot->Arg(2);
      if (!control->Profile()->SetCurrent (arg2)) 
	sout<<"profile could not be set to "<<arg2<<endl;
    }
    sout<<"current profile : "<<control->Profile()->Current()<<endl;

    IFSelect_Activator::SetCurrentAlias (WS->SelectedNorm(Standard_True));
    return IFSelect_RetDone;
  }
}


//=======================================================================
//function : xprofile
//=======================================================================
static IFSelect_ReturnStatus XSControl_xprofile(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    xprofile        ****
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  Handle(XSControl_Controller) control = WS->NormAdaptor();
  if (control.IsNull()) return IFSelect_RetFail;
  Handle(MoniTool_Profile) prof = control->Profile();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Current Profile : "<<prof->Current().ToCString()<<endl;

  if (argc < 2) {
    sout<<"xprofile ?   for list of profile confs"<<endl
      <<  "xprofile .   to apply the current profile (after editing)"<<endl
      <<  "xprofile profname     to select one and apply it"<<endl
      <<  "xprofile profname .   to record current profile as profname"<<endl
      <<"  (in followings, profname may be replaced by  .  for current profile)"<<endl
      <<  "xprofile profname ?   to list its definition"<<endl
      <<  "xprofile profname -   to clear it completely"<<endl
      <<  "xprofile profname optname casename to edit an option of it"<<endl
      <<  "xprofile profname - optname  to clear an option from it"<<endl;
    return IFSelect_RetVoid;
  }

  if (argc == 2) {
    if (arg1[0] == '?') {
      Handle(TColStd_HSequenceOfAsciiString) confs = prof->ConfList();
      Standard_Integer i, nb = confs->Length();
      sout<<"List of Available Profile Configurations : "<<nb<<" Items"<<endl;
      for (i = 1; i <= nb; i ++) sout<<confs->Value(i).ToCString()<<endl;
      return IFSelect_RetVoid;
    } else if (arg1[0] == '.' && arg1[1] == '\0') {
      if (!control->ApplyProfile(WS,".")) {
	sout<<"Applying current profile has failed"<<endl;
	return IFSelect_RetFail;
      }
      return IFSelect_RetDone;
    } else {

      //   Select a Profile
      if (!control->ApplyProfile(WS,arg1)) {
	sout<<"Setting "<<arg1<<" as current has failed"<<endl;
	return IFSelect_RetFail;
      }
      sout<<"Setting "<<arg1<<" as current"<<endl;
      return IFSelect_RetDone;
    }
  }

  if (argc == 3) {
    const Standard_CString arg2 = pilot->Arg(2);

    //   List the definition of a profile
    if (arg2[0] == '?') {
      Handle(TColStd_HSequenceOfAsciiString) opts, cases;
      prof->SwitchList (arg1,opts,cases);
      Standard_Integer i,nb = opts->Length();
      sout<<"Option  --  Case  --  ("<<nb<<" switches on configuration "<<arg1<<")"<<endl;
      for (i = 1; i <= nb; i ++) {
	sout<<opts->Value(i).ToCString()<<"	"<<cases->Value(i).ToCString()<<endl;
      }
      return IFSelect_RetVoid;

      //   Clear a profile
    } else if (arg2[0] == '-' && arg2[1] == '\0') {
      if (!prof->ClearConf(arg1)) {
	sout<<"Clearing profile "<<arg2<<" has failed"<<endl;
	return IFSelect_RetFail;
      }
      return IFSelect_RetDone;

      //    Merge Profile arg2 to arg1
    } else {
      if (!prof->HasConf (arg1)) prof->AddConf (arg1);
      if (!prof->AddFromOtherConf (arg1,arg2)) {
	sout<<"Merging profile "<<arg2<<" to "<<arg1<<" has failed"<<endl;
	return IFSelect_RetFail;
      }
      return IFSelect_RetDone;
    }
  }

  //   Editing / Adding a switch in a profile
  if (argc == 4) {
    const Standard_CString arg2 = pilot->Arg(2);
    const Standard_CString arg3 = pilot->Arg(3);

    //    Removing a switch
    if (arg2[0] == '-' && arg2[1] == '\0') {
      if (!prof->RemoveSwitch (arg1,arg3)) {
	sout<<"Removing switch on option "<<arg3<<" in profile "<<arg1<<" has failed"<<endl;
	return IFSelect_RetFail;
      }
      sout<<"Edition of profile "<<arg1<<" done. To apply it : xprofile "<<arg1<<endl;
      return IFSelect_RetDone;

      //    Setting a switch
    } else {
      if (!prof->AddSwitch (arg1,arg2,arg3)) {
	sout<<"Setting profile "<<arg1<<" for option "<<arg2<<" to case "<<arg3<<" has failed"<<endl;
	return IFSelect_RetFail;
      }
      sout<<"Edition of profile "<<arg1<<" done. To apply it : xprofile "<<arg1<<endl;
      return IFSelect_RetDone;
    }
  }

  return IFSelect_RetVoid;
}


//=======================================================================
//function : xoption
//=======================================================================
static IFSelect_ReturnStatus XSControl_xoption(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);
  //        ****    xoption        ****
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  Handle(XSControl_Controller) control = WS->NormAdaptor();
  if (control.IsNull()) return IFSelect_RetFail;
  Handle(MoniTool_Profile) prof = control->Profile();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Current Profile : "<<prof->Current().ToCString()<<endl;

  if (argc < 2) {
    sout<<"xoption anopt  : query an option"<<endl
      <<  "xoption anopt newcase  : switch (basic definition)"
	<<"  (but is superseded by current configuration)"<<endl
	<<"xoption + optname [param]  : create an option on a parameter"<<endl
	  <<"  (param absent is taken  param=optname)"
	<<"xoption anopt + casename [value] : add a case for a parameter"<<endl
	<<"  (value absent is taken  value=casename)"<<endl;

    Handle(TColStd_HSequenceOfAsciiString) optlist = prof->OptionList();
    Standard_Integer iopt, nbopt = optlist->Length();
    sout<<"Total : "<<nbopt<<" Options"<<endl;
    for (iopt = 1; iopt <= nbopt; iopt ++) {
      TCollection_AsciiString optname = optlist->Value(iopt);
      Handle(MoniTool_Option) opt = prof->Option (optname.ToCString());
      sout<<optname.ToCString()<<" : "<<opt->CaseName()<<endl;
    }
    return IFSelect_RetVoid;
  }

  //  xoption optname : description

  if (argc == 2) {
    Handle(MoniTool_Option) opt = prof->Option (arg1);
    if (opt.IsNull()) { sout<<"Not a recorded Option : "<<arg1<<endl; return IFSelect_RetError; }

    //  On va lister les valeurs admises
    Handle(TColStd_HSequenceOfAsciiString) caselist = opt->ItemList();
    Standard_Integer icase, nbcase = caselist->Length();
    Handle(MoniTool_TypedValue) tv = opt->TypedValue();
    if (tv.IsNull()) sout<<"Option : "<<arg1<<"  Type : "<<opt->Type()->Name();
    else sout<<"Option : "<<arg1<<"  TypedValue.  Name : "<<tv->Name()<<endl<<"  Definition : "<<tv->Definition();

    sout<<endl<<"  Current Case (basic) : "<<opt->CaseName()<<"  Total : "<<nbcase<<" Cases :"<<endl;
    for (icase = 1; icase <= nbcase; icase ++) {
      const TCollection_AsciiString& acase = caselist->Value(icase);
      sout<<acase.ToCString();

      //  Aliases ?
      Handle(TColStd_HSequenceOfAsciiString) aliases = opt->Aliases(acase.ToCString());
      Standard_Integer ial, nbal = (aliases.IsNull() ? 0 : aliases->Length());
      if (nbal > 0) sout<<"  -  Alias:";
      for (ial = 1; ial <= nbal; ial ++)  sout<<"  "<<aliases->Value(ial);
      if (!tv.IsNull()) {
        //  TypedValue : on peut afficher la valeur
	Handle(Standard_Transient) str;
	opt->Item (acase.ToCString(),str);
	if (!str.IsNull()) sout<<" - Value:"<< Handle(TCollection_HAsciiString)::DownCast(str)->ToCString();
      }

      sout<<endl;
    }
    return IFSelect_RetVoid;
  }

  //   xoption + optname [paramname]
  if (argc >= 3 && arg1[0] == '+' && arg1[1] == '\0') {
    Standard_CString parname = pilot->Arg(argc-1);
    Handle(Interface_Static) param = Interface_Static::Static(parname);
    if (param.IsNull()) { sout<<"No static parameter is named "<<parname<<endl;
			  return IFSelect_RetError; }
    const Handle(MoniTool_TypedValue)& aparam = param; // to avoid ambiguity
    Handle(MoniTool_Option) opt = new MoniTool_Option(aparam,arg2);
    prof->AddOption (opt);
    return IFSelect_RetDone;
  }

  //  xoption optname + case [val]
  if (argc >= 4 && arg2[0] == '+' && arg2[1] == '\0') {
    Handle(MoniTool_Option) opt = prof->Option (arg1);
    if (opt.IsNull()) { sout<<"Not a recorded Option : "<<arg1<<endl; return IFSelect_RetError; }
    Handle(MoniTool_TypedValue) tv = opt->TypedValue();
    if (tv.IsNull()) { sout<<"Option not for a Parameter : "<<arg1<<endl; return IFSelect_RetError; }
    Standard_CString valname = pilot->Arg(argc-1);
    if (!opt->AddBasic (arg3,valname)) {
      sout<<"Option "<<arg1<<" : not an allowed value : "<<valname<<endl;
      return IFSelect_RetError;
    }
    return IFSelect_RetDone;
  }

  //  xoption optname newcase : edition
  if (argc == 3) {
    Handle(MoniTool_Option) opt = prof->Option (arg1);
    if (opt.IsNull()) { sout<<"Not a recorded Option : "<<arg1<<endl; return IFSelect_RetError; }
    if (!opt->Switch (arg2)) {
      sout<<"Option : "<<arg1<<" , Not a suitable case : "<<arg2<<endl;
      return IFSelect_RetFail;
    }
    sout<<"Option : "<<arg1<<" switched to case : "<<arg2<<endl;
    return IFSelect_RetDone;
  }


  return IFSelect_RetVoid;
}


//=======================================================================
//function : newmodel
//=======================================================================
static IFSelect_ReturnStatus XSControl_newmodel(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    newmodel        ****
  if (!XSControl::Session(pilot)->NewModel().IsNull()) return IFSelect_RetDone;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"No new Model produced"<<endl;
  return IFSelect_RetFail;
}


//=======================================================================
//function : tpclear
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpclear(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    tpclear/twclear        ****
  Standard_Boolean modew = Standard_False;
  if (pilot->Word(0).Value(2) == 'w') modew = Standard_True;
  Handle(Transfer_FinderProcess)    FP = XSControl::Session(pilot)->MapWriter();
  Handle(Transfer_TransientProcess) TP = XSControl::Session(pilot)->MapReader();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if ( modew) { if(!FP.IsNull()) FP->Clear(); else sout<<"No Transfer Write"<<endl; }
  else        { if(!TP.IsNull()) TP->Clear(); else sout<<"No Transfer Read"<<endl; }
  return IFSelect_RetDone;
}


//=======================================================================
//function : tpstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //const Standard_CString arg2 = pilot->Arg(2);
  Handle(Transfer_TransientProcess) TP= XSControl::Session(pilot)->MapReader();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return IFSelect_RetError;}
  //        ****    tpstat        ****

  Standard_Integer mod1 = -1;
  Standard_Integer mod2 = 0;
  //  g : general  c : check (compte) C (liste)   f : fails(compte)  F (liste)
  //  resultats racines :  n : n0s entites  s : status  b : binders
  //    t : compte par type  r : compte par resultat   l : liste(type-resultat)
  //   *n *s *b *t *r *l : idem sur tout
  //   ?n  etc.. : idem sur resultats anormaux
  //   ?  tout court pour help
  
  if (argc > 1) {
    char a2 = arg1[1]; if (a2 == '\0') a2 = '!';
    switch (arg1[0]) {
    case 'g' : mod1 = 0; break;
    case 'c' : mod1 = 4; mod2 = 4; break;
    case 'C' : mod1 = 4; mod2 = 2; break;
    case 'f' : mod1 = 5; mod2 = 4; break;
    case 'F' : mod1 = 5; mod2 = 2; break;
    case '*' : mod1 = 2; break;
    case '?' : mod1 = 3; break;
    default  : mod1 = 1; if (argc > 2) mod1 = 2; a2 = arg1[0]; break;
    }
    if (mod1 < 1 || mod1 > 3) a2 = '!';
    switch (a2) {
    case 'n' : mod2 = 0; break;
    case 's' : mod2 = 1; break;
    case 'b' : mod2 = 2; break;
    case 't' : mod2 = 3; break;
    case 'r' : mod2 = 4; break;
    case 'l' : mod2 = 5; break;
    case 'L' : mod2 = 6; break;
    case '!' : break;
    case '?' : mod1 = -1; break;
    default  : mod1 = -2; break;
    }
  }
  //  A present help eventuel
  if (mod1 < -1) sout<<"Unknown Mode"<<endl;
  if (mod1 < 0) {
    sout<<"Modes available :\n"
      <<"g : general    c : checks (count)  C (list)"<<"\n"
      <<"               f : fails  (count)  F (list)"<<"\n"
      <<"  n : numbers of transferred entities (on TRANSFER ROOTS)"<<"\n"
      <<"  s : their status (type entity-result , presence checks)"<<"\n"
      <<"  b : detail of binders"<<"\n"
      <<"  t : count per entity type    r : per type/status result"<<"\n"
      <<"  l : count per couple  type entity/result"<<"\n"
      <<"  L : list  per couple  type entity/result"<<"\n"
      <<"  *n  *s  *b  *t  *r  *l  *L : idem on ALL recorded items"<<"\n"
      <<"  ?n  ?s  ?b  ?t ... : idem on abnormal items"<<"\n"
      <<"  n select : n applied on a selection   idem for  s b t r l"<<endl;
    if (mod1 < -1) return IFSelect_RetError;
    return IFSelect_RetVoid;
  }

  if (!TP.IsNull()) {
    sout<<"TransferRead :";
    if (TP->Model() != pilot->Session()->Model()) sout<<"Model differs from the session";
    Handle(TColStd_HSequenceOfTransient) list =
      IFSelect_Functions::GiveList(pilot->Session(),pilot->CommandPart(2));
    XSControl_TransferReader::PrintStatsOnList (TP,list,mod1,mod2);
//    TP->PrintStats (1,sout);
  }
  else sout<<"TransferRead : not defined"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tpent
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpent(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(Transfer_TransientProcess) TP= XSControl::Session(pilot)->MapReader();
  //        ****    tpent        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TP.IsNull()) { sout<<"No Transfer Read"<<endl; return IFSelect_RetError;}
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (model.IsNull())  return IFSelect_RetFail;

  if (argc < 2) { sout<<"Give ENTITY NUMBER (IN MODEL TransferProcess)"<<endl; return IFSelect_RetError; }
  Standard_Integer num = atoi(arg1);
  if (num <= 0 || num > model->NbEntities()) { sout<<"Number not in [1 - "<<model->NbEntities()<<"]"<<endl; return IFSelect_RetError; }
  Handle(Standard_Transient) ent = model->Value(num);
  Standard_Integer index = TP->MapIndex  (ent);
  if (index == 0) sout<<"Entity "<<num<<"  not recorded in transfer"<<endl;
  else XSControl::Session(pilot)->PrintTransferStatus (index,Standard_False,sout);
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tpitem
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpitem(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    tpitem/tproot/twitem/twroot        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give ITEM NUMBER (in TransferProcess)"<<endl; return IFSelect_RetError; }
  Standard_Integer num = atoi(arg1);
  if (pilot->Word(0).Value(3) == 'r') num = -num;
  Standard_Boolean modew = Standard_False;
  if (pilot->Word(0).Value(2) == 'w') modew = Standard_True;
  Handle(Transfer_Binder) binder;
  Handle(Transfer_Finder) finder;
  Handle(Standard_Transient) ent;
  if (!XSControl::Session(pilot)->PrintTransferStatus(num,modew,sout))
    sout<<" - Num="<<num<<" incorrect"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tpatr
//=======================================================================
static IFSelect_ReturnStatus XSControl_tpatr(const Handle(IFSelect_SessionPilot)& /*pilot*/)
{
/*skl
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  Handle(Transfer_TransientProcess) TP = WS->MapReader();

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  //  tpatr tout court : liste tous les attributs
  //  tpatr nomatr     : sur cet attribut, liste les valeurs par entite

  if (argc < 2) {
    Handle(Dico_DictionaryOfInteger) list = TP->Attributes();
    for (Dico_IteratorOfDictionaryOfInteger iter(list); iter.More(); iter.Next()) {
      TCollection_AsciiString name = iter.Name();
      Standard_Integer nbatr = iter.Value();
      Interface_ParamType aty = TP->AttributeType (name.ToCString());
      sout<<"Name : "<<name<<"  Count="<<nbatr<<"  Type : ";
      switch (aty) {
        case Interface_ParamInteger : sout<<"Integer";  break;
        case Interface_ParamReal    : sout<<"Real";     break;
        case Interface_ParamIdent   : sout<<"Object";   break;
        case Interface_ParamText    : sout<<"String";   break;
	default : sout<<"(Mixed)";
      }
      sout<<endl;
    }
  }

  else {

    Standard_Integer num , nb = 0;
    sout<<"Attribute Name : "<<arg1<<endl;
    for (num = TP->NextItemWithAttribute(arg1,0);  num > 0;
	 num = TP->NextItemWithAttribute(arg1,num)) {
      nb ++;
      sout<<"Item "<<num<<" , Entity ";
      WS->Model()->Print(TP->Mapped(num),sout);
      Handle(Transfer_Binder) bnd = TP->MapItem (num);
      Interface_ParamType aty = bnd->AttributeType(arg1);
      switch (aty) {
        case Interface_ParamInteger : sout<<"  Integer="<<bnd->IntegerAttribute(arg1); break;
        case Interface_ParamReal    : sout<<"  Real="<<bnd->RealAttribute(arg1); break;
        case Interface_ParamIdent   : sout<<"  Object,Type:"<<bnd->Attribute(arg1)->DynamicType()->Name(); break;
        case Interface_ParamText    : sout<<"  String="<<bnd->StringAttribute(arg1);
	default : sout<<"(none)"; break;
      }
      sout<<endl;
    }
    sout<<"Attribute Name : "<<arg1<<" on "<<nb<<" Items"<<endl;
  }
skl*/

  return IFSelect_RetVoid;
}


//=======================================================================
//function : trecord
//=======================================================================
static IFSelect_ReturnStatus XSControl_trecord(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(Transfer_TransientProcess) TP = XSControl::Session(pilot)->MapReader();
//        ****    trecord : TransferReader        ****
  Standard_Boolean tous = (argc == 1);
  Standard_Integer num = -1;
  Handle(Interface_InterfaceModel)  mdl = XSControl::Session(pilot)->Model();
  Handle(XSControl_TransferReader)  TR  = XSControl::Session(pilot)->TransferReader();
  Handle(Standard_Transient) ent;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mdl.IsNull() || TR.IsNull() || TP.IsNull())
    { sout<<" init not done"<<endl; return IFSelect_RetError; }
  if (!tous) num = atoi(arg1);
  //    Enregistrer les racines
  if (tous) {
    Standard_Integer nb = TP->NbRoots();
    sout<<" Recording "<<nb<<" Roots"<<endl;
    for (Standard_Integer i = 1; i <= nb; i ++) {
      ent = TP->Root(i);
      if (TR->RecordResult (ent)) sout<<" Root n0."<<i<<endl;
      else sout<<" Root n0."<<i<<" not recorded"<<endl;
    }
  } else {
    if (num < 1 ||  num > mdl->NbEntities()) sout<<"incorrect number:"<<num<<endl;
    else if (TR->RecordResult(mdl->Value(num))) sout<<" Entity n0."<<num<<endl;
    else sout<<" Entity n0."<<num<<" not recorded"<<endl;
  }
  return IFSelect_RetDone;
}


//=======================================================================
//function : trstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_trstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
//        ****    trstat : TransferReader        ****
  Handle(XSControl_TransferReader)  TR  = XSControl::Session(pilot)->TransferReader();
  if (TR.IsNull()) { sout<<" init not done"<<endl; return IFSelect_RetError; }
  Handle(Interface_InterfaceModel)  mdl = TR->Model();
  if (mdl.IsNull()) { sout<<" No model"<<endl; return IFSelect_RetError; }
  sout<<" Statistics : FileName : "<<TR->FileName()<<endl;
  if (argc == 1) {
    // stats generales
    TR->PrintStats(10,0);
  } else {
    // stats unitaires
    Standard_Integer num = atoi(arg1);
    if (num < 1 || num > mdl->NbEntities()) { sout<<" incorrect number:"<<arg1<<endl; return IFSelect_RetError; }
    Handle(Standard_Transient) ent = mdl->Value(num);
    if (!TR->IsRecorded(ent)) { sout<<" Entity "<<num<<" not recorded"<<endl; return IFSelect_RetError; }
    Handle(Transfer_ResultFromModel) RM = TR->FinalResult(ent);
    Handle(TColStd_HSequenceOfTransient) list = TR->CheckedList(ent);
    Standard_Integer i, nb = list->Length();
    if (nb > 0) sout<<" Entities implied by Check/Result :"<<nb<<" i.e.:";
    for (i = 1; i <= nb; i ++) { sout<<"  "; mdl->Print(list->Value(i),sout); }
    sout<<endl;
    if (RM.IsNull()) { sout<<" no other info"<<endl; return IFSelect_RetVoid; }
    Interface_CheckIterator chl = RM->CheckList(Standard_False);
    pilot->Session()->PrintCheckList(chl,Standard_False,IFSelect_EntitiesByItem);
  }
  return IFSelect_RetVoid;
}


//=======================================================================
//function : trbegin
//=======================================================================
static IFSelect_ReturnStatus XSControl_trbegin(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    trbegin : TransferReader        ****
  Handle(XSControl_TransferReader)  TR  = XSControl::Session(pilot)->TransferReader();
  Standard_Boolean init = TR.IsNull();
  if (pilot->NbWords() > 1) { if (pilot->Arg(1)[0] == 'i') init = Standard_True; }
  if (init) {
    XSControl::Session(pilot)->InitTransferReader (0);
    TR  = XSControl::Session(pilot)->TransferReader();
    if (TR.IsNull()) { 
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<" init not done or failed"<<endl; 
      return IFSelect_RetError; 
    }
  }
  TR->BeginTransfer();
  return IFSelect_RetDone;
}


//=======================================================================
//function : tread
//=======================================================================
static IFSelect_ReturnStatus XSControl_tread(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  //const Standard_CString arg1 = pilot->Arg(1);
  //        ****    tread : TransferReader        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  Handle(XSControl_TransferReader)  TR  = XSControl::Session(pilot)->TransferReader();
  if (TR.IsNull()) { sout<<" init not done"<<endl; return IFSelect_RetError; }
  Handle(Interface_InterfaceModel)  mdl = TR->Model();
  if (mdl.IsNull()) { sout<<" No model"<<endl; return IFSelect_RetError; }
  if (argc < 2) {
//      DeclareAndCast(IFSelect_Selection,sel,pilot->Session()->NamedItem("xst-model-roots"));
    Handle(Standard_Transient) sel = pilot->Session()->NamedItem("xst-model-roots");
    if (sel.IsNull()) { sout<<"Select Roots absent"<<endl; return IFSelect_RetError; }
    Handle(TColStd_HSequenceOfTransient) list = pilot->Session()->GiveList(sel);
    sout<<" Transferring all roots i.e. : "<<TR->TransferList(list)<<endl;
  } else {
    Handle(TColStd_HSequenceOfTransient) list =
      IFSelect_Functions::GiveList(pilot->Session(),pilot->CommandPart(1));
    sout<<" Transfer of "<<list->Length()<<" entities"<<endl;
    Standard_Integer nb = TR->TransferList(list);
    sout<<" Gives "<<nb<<" results"<<endl;
  }
  return IFSelect_RetDone;
}


//=======================================================================
//function : trtp
//=======================================================================
static IFSelect_ReturnStatus XSControl_trtp(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    TReader -> TProcess         ****
  Handle(XSControl_TransferReader)  TR  = XSControl::Session(pilot)->TransferReader();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (TR.IsNull()) sout<<" No TransferReader"<<endl;
  else if (TR->TransientProcess().IsNull()) sout<<" Transfer Reader without Process"<<endl;
  ////else { XSDRAW::SetTransferProcess(TR->TransientProcess()); return IFSelect_RetDone; }
  return IFSelect_RetVoid;
}


//=======================================================================
//function : tptr
//=======================================================================
static IFSelect_ReturnStatus XSControl_tptr(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    TProcess -> TReader         ****
  XSControl::Session(pilot)->InitTransferReader (3);
  return IFSelect_RetDone;
}


//=======================================================================
//function : twmode
//=======================================================================
static IFSelect_ReturnStatus XSControl_twmode(const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  //        ****    twmode         ****
  Handle(XSControl_TransferWriter) TW = XSControl::Session(pilot)->TransferWriter();
  Handle(XSControl_Controller) control = XSControl::Session(pilot)->NormAdaptor();
  Standard_Integer modemin,modemax;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (control->ModeWriteBounds (modemin,modemax)) {
    sout<<"Write Mode : allowed values  "<<modemin<<" to "<<modemax<<endl;
    for (Standard_Integer modd = modemin; modd <= modemax; modd ++) {
      sout<<modd<<"	: "<<control->ModeWriteHelp (modd)<<endl;;
    }
  }
  sout<<"Write Mode : actual = "<<TW->TransferMode()<<endl;
  if (argc <= 1) return IFSelect_RetVoid;
  Standard_Integer mod = atoi(arg1);
  sout<<"New value -> "<<arg1<<endl;
  TW->SetTransferMode(mod);
  if (!control->IsModeWrite (mod)) sout<<"Warning : this new value is not supported"<<endl;
  return IFSelect_RetDone;
}


//=======================================================================
//function : twstat
//=======================================================================
static IFSelect_ReturnStatus XSControl_twstat(const Handle(IFSelect_SessionPilot)& pilot)
{
  //Standard_Integer argc = pilot->NbWords();
  //const Standard_CString arg1 = pilot->Arg(1);
  //const Standard_CString arg2 = pilot->Arg(2);
  Handle(Transfer_FinderProcess)    FP = XSControl::Session(pilot)->MapWriter();
  //        ****    twstat        ****
  //  Pour Write
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!FP.IsNull()) {
    sout<<"TransferWrite:";
    //    XSControl_TransferWriter::PrintStatsProcess (FP,mod1,mod2);
    FP->PrintStats (1,sout);
  }
  else sout<<"TransferWrite: not defined"<<endl;
  return IFSelect_RetVoid;
}


//=======================================================================
//function : settransfert
//=======================================================================
static IFSelect_ReturnStatus XSControl_settransfert(const Handle(IFSelect_SessionPilot)& pilot)
{
  //        ****    SelectForTransfer           ****
  return pilot->RecordItem(new XSControl_SelectForTransfer(XSControl::Session(pilot)->TransferReader()));
}



static int initactor = 0;

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XSControl_Functions::Init ()
{
  if (initactor) return;  initactor = 1;
  IFSelect_Act::SetGroup("DE: General");

  IFSelect_Act::AddFunc ("xinit","[norm:string to change norme] reinitialises according to the norm",XSControl_xinit);
  IFSelect_Act::AddFunc ("xnorm","displays current norm   +norm : changes it",XSControl_xnorm);
  IFSelect_Act::AddFunc ("xprofile","displays current profile   +prof : changes it",XSControl_xprofile);
  IFSelect_Act::AddFunc ("xoption","lists options  +opt : lists cases  +case : changes current case",XSControl_xoption);

  IFSelect_Act::AddFunc ("newmodel","produces a new empty model, for the session",XSControl_newmodel);

  IFSelect_Act::AddFunc ("tpclear","Clears  TransferProcess (READ)",XSControl_tpclear);
  IFSelect_Act::AddFunc ("twclear","Clears  TransferProcess (WRITE)",XSControl_tpclear);

  IFSelect_Act::AddFunc ("tpstat","Statistics on TransferProcess (READ)",XSControl_tpstat);

  IFSelect_Act::AddFunc ("tpent","[num:integer] Statistics on an entity of the model (READ)",XSControl_tpent);

  IFSelect_Act::AddFunc ("tpitem","[num:integer] Statistics on ITEM of transfer (READ)"    ,XSControl_tpitem);
  IFSelect_Act::AddFunc ("tproot","[num:integer] Statistics on a ROOT of transfert (READ)" ,XSControl_tpitem);
  IFSelect_Act::AddFunc ("twitem","[num:integer] Statistics on an ITEM of transfer (WRITE)"   ,XSControl_tpitem);
  IFSelect_Act::AddFunc ("twroot","[num:integer] Statistics on a ROOT of transfer (WRITE)",XSControl_tpitem);
  IFSelect_Act::AddFunc ("tpatr","[name] List all Attributes, or values for a Name",XSControl_tpatr);

  IFSelect_Act::AddFunc ("trecord","record : all root results; or num : for entity n0.num",XSControl_trecord);
  IFSelect_Act::AddFunc ("trstat","general statistics;  or num : stats on entity n0 num",XSControl_trstat);
  IFSelect_Act::AddFunc ("trbegin","begin-transfer-reader [init]",XSControl_trbegin);
  IFSelect_Act::AddFunc ("tread","transfers all roots, or num|sel|sel num : entity list, by transfer-reader",XSControl_tread);

  IFSelect_Act::AddFunc ("trtp","feeds commands tp... with results from tr...",XSControl_trtp);
  IFSelect_Act::AddFunc ("tptr","feeds tr... from tp... (may be incomplete)",XSControl_tptr);

  IFSelect_Act::AddFunc ("twmode","displays mode transfer write, + num  changes it",XSControl_twmode);
  IFSelect_Act::AddFunc ("twstat","Statistics on TransferProcess (WRITE)",XSControl_twstat);

  IFSelect_Act::AddFSet ("selecttransfer","selection (recognize from transfer actor)",XSControl_settransfert);
}
