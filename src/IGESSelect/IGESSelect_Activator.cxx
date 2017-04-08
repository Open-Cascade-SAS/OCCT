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


#include <IFSelect_IntParam.hxx>
#include <IFSelect_PacketList.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <IFSelect_WorkSession.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESSelect_Activator.hxx>
#include <IGESSelect_AddGroup.hxx>
#include <IGESSelect_AutoCorrect.hxx>
#include <IGESSelect_ComputeStatus.hxx>
#include <IGESSelect_DispPerDrawing.hxx>
#include <IGESSelect_DispPerSingleView.hxx>
#include <IGESSelect_FloatFormat.hxx>
#include <IGESSelect_IGESTypeForm.hxx>
#include <IGESSelect_RebuildDrawings.hxx>
#include <IGESSelect_RebuildGroups.hxx>
#include <IGESSelect_SelectBypassGroup.hxx>
#include <IGESSelect_SelectDrawingFrom.hxx>
#include <IGESSelect_SelectFromDrawing.hxx>
#include <IGESSelect_SelectFromSingleView.hxx>
#include <IGESSelect_SelectLevelNumber.hxx>
#include <IGESSelect_SelectName.hxx>
#include <IGESSelect_SelectSingleViewFrom.hxx>
#include <IGESSelect_SelectVisibleStatus.hxx>
#include <IGESSelect_SetGlobalParameter.hxx>
#include <IGESSelect_SetVersion5.hxx>
#include <IGESSelect_SplineToBSpline.hxx>
#include <IGESSelect_UpdateCreationDate.hxx>
#include <IGESSelect_UpdateFileName.hxx>
#include <IGESSelect_UpdateLastChange.hxx>
#include <IGESSelect_ViewSorter.hxx>
#include <IGESSelect_WorkLibrary.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_Activator,IFSelect_Activator)

static int THE_IGESSelect_Activator_initActivator = 0;

IGESSelect_Activator::IGESSelect_Activator()
{
  if (THE_IGESSelect_Activator_initActivator)
  {
    return;
  }
  THE_IGESSelect_Activator_initActivator = 1;
//  Add ( 0,"load");
//  Add ( 0,"loadiges");    // homonyme
  SetForGroup ("XSTEP-IGES");

  Add ( 5,"listviews");
  Add ( 6,"listdrawings");
  Add ( 7,"listsviews");

  AddSet (10,"igestype");
  AddSet (11,"bypassgroup");
  AddSet (12,"igesname");
  AddSet (13,"igesvisible");
  AddSet (14,"igeslevel");

  AddSet (21,"igesfromdrawing");
  AddSet (22,"igesfromview");
  AddSet (23,"igesdrawingfrom");
  AddSet (24,"igesviewfrom");

  AddSet (31,"igesdispdrawing");
  AddSet (32,"igesdispview");

  AddSet (40,"floatformat");
  AddSet (41,"setglobalparam");
  AddSet (42,"setversion5");
  AddSet (43,"updatecreationdate");
  AddSet (44,"updatelastchange");
  AddSet (45,"updatefilename");

  AddSet (50,"autocorrect");
  AddSet (51,"computestatus");
  AddSet (52,"rebuildgroups");
  AddSet (53,"rebuilddrawings");
  AddSet (54,"addgroup");

  AddSet (60,"splinetobspline");

  Add    (70,"setuseflag");
}


    IFSelect_ReturnStatus  IGESSelect_Activator::Do
  (const Standard_Integer number,
   const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Word(1).ToCString();
  const Standard_CString arg2 = pilot->Word(2).ToCString();
//  const Standard_CString arg3 = pilot->Word(3).ToCString();
  Handle(IFSelect_WorkSession) WS = pilot->Session();

  switch (number) {

    case  5 :     //        ****    ListViews (sans tri complementaire)
    case  6 :     //        ****    ListDrawings
    case  7 : {   //        ****    ListS(ingle)Views
      Standard_Integer listmode = 0;
      if (argc == 2 && arg1[0] == '?') argc = -1;
      if (argc < 2) {
	cout<<"Ajouter un argument pour avoir :\n"<<
	  " l : liste resumee"<<
	  " c : liste complete par item (mais pas pour remaining)\n"<<
	  " r : idem + liste complete remaining\n"<<
	  "  sur tout le modele. Ajouter nom selection pour lister sur une partie"<<endl;
	return  (argc >= 0 ? IFSelect_RetError : IFSelect_RetVoid);
      }
      if (arg1[0] == 'l') {
	listmode = 0;  cout<<"Liste resumee";
      }
      else if (arg1[0] == 'c') {
	listmode = 1;  cout<<"Liste complete par item (pas pour Remaining)"<<endl;
      }
      else if (arg1[0] == 'r') {
	listmode = 2;  cout<<"Liste complete par item et pour Remaining"<<endl;
      }
      else {
	cout<<"Parametre de listage non compris"<<endl; return IFSelect_RetError;
      }

      Handle(IGESSelect_ViewSorter) vs = new IGESSelect_ViewSorter;
      vs->SetModel(GetCasted(IGESData_IGESModel,WS->Model()));
      if (argc == 2) vs->AddModel(WS->Model());
      else {
//   on demande une selection
	DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg2));
	if (sel.IsNull()) {
	  cout<<"Pas une selection : "<<arg2<<endl;
	  return IFSelect_RetError;
	}
	vs->AddList (WS->SelectionResult(sel));
      }
      Standard_Boolean listdr = (number > 5);
      if (number == 6) vs->SortDrawings(WS->Graph());
      if (number == 7) vs->SortSingleViews(Standard_True);
      Handle(IFSelect_PacketList) sets = vs->Sets(listdr);
      Standard_Integer nb = vs->NbSets(listdr);
      cout<<" --  ViewSorter for";
      if (number == 5) cout<<" Views & Drawings";
      if (number == 6) cout<<" Drawings only (complete)";
      if (number == 7) cout<<" Single Views & Drawing Frames";
      cout<<", on "<<vs->NbEntities()<<" ent.s, give "<<nb<<" Sets"<<endl;

      Interface_EntityIterator iter;
      for (Standard_Integer i = 1; i <= nb; i ++) {
	cout<<" --  Set n0 "<<i<<" Item=entity n0: "<<
	  WS->Model()->Number(vs->SetItem(i,listdr))<<"  List:";
        cout<<sets->NbEntities(i)<<" ent.s:"<<endl;
	if (listmode == 0) continue;
	iter = sets->Entities(i);
	WS->ListEntities (iter,0);
      }

      cout <<" --  Remaining Entities (not yet sorted) :"
	<<sets->NbDuplicated(0,Standard_False)<<endl;
      if (listmode < 2) return IFSelect_RetVoid;
      iter = sets->Duplicated(0,Standard_False);
      WS->ListEntities(iter,0);
      return IFSelect_RetVoid;
    }

    case 10 : {   //        ****    IGESType (form Type/Form)
      if (argc < 2) {
	cout<<"Donner le n0 de type desire, + en option la forme\n"
	  <<"  Si pas de forme, prend toutes les formes du type demande"<<endl;
	return IFSelect_RetError;
      }
      char signature[20];
      if (argc == 2) sprintf(signature,"%s",arg1);
      else sprintf(signature,"%s %s",arg1,arg2);
      Handle(IFSelect_SelectSignature) sel = new IFSelect_SelectSignature
	(new IGESSelect_IGESTypeForm, signature,(argc > 2));
      return pilot->RecordItem (sel);
    }

    case 11 : {   //        ****    IGES BypassGroup
      return pilot->RecordItem (new IGESSelect_SelectBypassGroup);
    }

    case 12 :  {  //        ****    IGES Name
      if (argc < 2) {
        cout<<"Donner un Nom de TextParam pour IGESName"<<endl;
        return IFSelect_RetError;
      }
      Handle(IGESSelect_SelectName) sel = new IGESSelect_SelectName;
      sel->SetName
	(GetCasted(TCollection_HAsciiString,WS->NamedItem(arg1)) );
      return pilot->RecordItem (sel);
    }

    case 13 : {   //        ****    IGES VisibleStatus
      return pilot->RecordItem (new IGESSelect_SelectVisibleStatus);
    }

    case 14 : {   //        ****    IGES LevelNumber
      if (argc < 2) {
        cout<<"Donner nom IntParam pour Level"<<endl;
        return IFSelect_RetError;
      }
      DeclareAndCast(IFSelect_IntParam,lev,WS->NamedItem(arg1));
      if (lev.IsNull()) {
        cout<<arg1<<" : pas un IntParam (pour Level)"<<endl;
        return IFSelect_RetError;
      }
      Handle(IGESSelect_SelectLevelNumber) sel = new IGESSelect_SelectLevelNumber;
      sel->SetLevelNumber(lev);
      return pilot->RecordItem (sel);
    }

    case 21 : {   //        ****    IGES FromDrawing
      return pilot->RecordItem (new IGESSelect_SelectFromDrawing);
    }
    case 22 : {   //        ****    IGES FromSingleView
      return pilot->RecordItem (new IGESSelect_SelectFromSingleView);
    }
    case 23 : {   //        ****    IGES FromDrawing
      return pilot->RecordItem (new IGESSelect_SelectDrawingFrom);
    }
    case 24 : {   //        ****    IGES FromDrawing
      return pilot->RecordItem (new IGESSelect_SelectSingleViewFrom);
    }

    case 31 : {   //        ****    IGES DispatchPerDrawing
      return pilot->RecordItem (new IGESSelect_DispPerDrawing);
    }
    case 32 : {   //        ****    IGES DispatchPerSingleView
      return pilot->RecordItem (new IGESSelect_DispPerSingleView);
    }

    case 40 : {   //        ****    FloatFormat
      char prem = ' ';
      if (argc < 2) prem = '?';
      else if (argc == 5) {
        cout<<"floatformat tout court donne les formes admises"<<endl;
        return IFSelect_RetError;
      }
      else prem = arg1[0];
      Standard_Boolean zerosup=Standard_False;
      Standard_Integer digits = 0;
      if      (prem == 'N' || prem == 'n') zerosup = Standard_False;
      else if (prem == 'Z' || prem == 'z') zerosup = Standard_True;
      else if (prem >= 48  && prem <= 57)  digits  = atoi(arg1);
      else {
	cout<<"floatformat digits, digits=nb de chiffres signifiants, ou\n"
	  <<  "floatformat NZ %mainformat [%rangeformat [Rmin Rmax]]\n"
	  <<"  NZ : N ou n pour Non-zero-suppress, Z ou z pour zero-suppress\n"
	  <<" %mainformat  : format principal type printf, ex,: %E\n"
	  <<" + optionnel  : format secondaire (flottants autour de 1.) :\n"
	  <<" %rangeformat Rmin Rmax : format type printf entre Rmin et Rmax\n"
	  <<" %rangeformat tout seul : format type printf entre 0.1 et 1000.\n"
	    <<flush;
	return (prem == '?' ? IFSelect_RetVoid : IFSelect_RetError);
      }
      Standard_Real Rmin=0., Rmax=0.;
      if (argc > 4) {
	Rmin = Atof(pilot->Word(4).ToCString());
	Rmax = Atof(pilot->Word(5).ToCString());
	if (Rmin <= 0 || Rmax <= 0) { cout<<"intervalle : donner reels > 0"<<endl; return IFSelect_RetError; }
      }
      Handle(IGESSelect_FloatFormat) fm = new IGESSelect_FloatFormat;
      if (argc == 2) fm->SetDefault(digits);
      else {
	fm->SetZeroSuppress(zerosup);
	fm->SetFormat (arg2);
	if      (argc == 4) fm->SetFormatForRange(pilot->Word(3).ToCString());
	else if (argc >= 6) fm->SetFormatForRange(pilot->Word(3).ToCString(),Rmin,Rmax);
	else                fm->SetFormatForRange("");
      }
      return pilot->RecordItem(fm);
    }

    case 41 : {   //        ****    SetGlobalParameter
      if (argc < 3) {
        cout<<"Donner entier=n0 param a changer + nom TextParam pour la valeur"<<endl;
        return IFSelect_RetError;
      }
      Standard_Integer numpar = atoi(arg1);
      if (numpar <= 0) {
        cout<<"Pas un n0 de param global correct:"<<arg1<<endl;
        return IFSelect_RetError;
      }
      DeclareAndCast(TCollection_HAsciiString,val,WS->NamedItem(arg2));
      if (val.IsNull()) {
        cout<<"Pas un nom de TextParam:"<<arg2<<endl;
        return IFSelect_RetError;
      }
      Handle(IGESSelect_SetGlobalParameter) mod =
	new IGESSelect_SetGlobalParameter(numpar);
      mod->SetValue(val);
      return pilot->RecordItem (mod);
    }

    case 42 : {   //        ****    Set IGES Version 5.1
      return pilot->RecordItem (new IGESSelect_SetVersion5);
    }
    case 43 : {   //        ****    Update Creation Date
      return pilot->RecordItem (new IGESSelect_UpdateCreationDate);
    }
    case 44 : {   //        ****    Update Last Change Date
      return pilot->RecordItem (new IGESSelect_UpdateLastChange);
    }
    case 45 : {   //        ****    Update File Name
      return pilot->RecordItem (new IGESSelect_UpdateFileName);
    }

    case 50 : {   //        ****    AutoCorrect
      return pilot->RecordItem (new IGESSelect_AutoCorrect);
    }
    case 51 : {   //        ****    Compute Status
      return pilot->RecordItem (new IGESSelect_ComputeStatus);
    }
    case 52 : {   //        ****    Rebuild Groups
      return pilot->RecordItem (new IGESSelect_RebuildGroups);
    }
    case 53 : {   //        ****    Rebuild Drawings
      return pilot->RecordItem (new IGESSelect_RebuildDrawings);
    }
    case 54 : {   //        ****    Add Group
      return pilot->RecordItem (new IGESSelect_AddGroup);
    }

    case 60 : {   //        ****    Spline To BSpline
      if (argc < 2) {
        cout<<"Pour SplineToBSpline, donner mode :\n"<<
			" n pour normal, t pour tryC2"<<endl;
        return IFSelect_RetError;
      }
      Standard_Boolean tryC2;
      if (arg1[0] == 'n' || arg1[0] == 'N')
        tryC2 = Standard_False;
      else if (arg1[0] == 't' || arg1[0] == 'T')
        tryC2 = Standard_True;
      else {
        cout<<" Mode incorrect : "<<arg1<<endl;
        return IFSelect_RetError;
      }
      Handle(IGESSelect_SplineToBSpline) conv =
	new IGESSelect_SplineToBSpline (tryC2);
      return pilot->RecordItem(conv);
    }

    case 70 : {   //        ****    SetUseFlag
      Standard_Integer usefl = atoi (arg1);
      if (argc > 2) {
	Handle(TColStd_HSequenceOfTransient) list = WS->GiveList(pilot->CommandPart(2));
	Standard_Integer i, nb = list->Length();
	for (i = 1; i <= nb; i ++) {
	  DeclareAndCast(IGESData_IGESEntity,ent,list->Value(i));
	  if (!ent.IsNull()) ent->InitStatus
	    (ent->BlankStatus(),ent->SubordinateStatus(),usefl,ent->HierarchyStatus());
	}
      }
      else {
	Standard_Integer i, nb = WS->NbStartingEntities();
	for (i = 1; i <= nb; i ++) {
	  DeclareAndCast(IGESData_IGESEntity,ent,WS->StartingEntity(i));
	  if (!ent.IsNull()) ent->InitStatus
	    (ent->BlankStatus(),ent->SubordinateStatus(),usefl,ent->HierarchyStatus());
	}
      }
      return IFSelect_RetDone;
    }

    default : break;
  }
  return IFSelect_RetVoid;

}

Standard_CString  IGESSelect_Activator::Help
  (const Standard_Integer number) const
{
  switch (number) {
    case  5 : return "Liste Vues (tous types). Nom selection sinon tout modele";
    case  6 : return "Liste Drawings. Nom selection sinon tout modele";
    case  7 : return "Liste Vues SIMPLES. Nom selection sinon tout modele";

    case 10 : return "type:integer [form:integer]  : cree Select IGESType";
    case 11 : return "cree Select Bypass Group";
    case 12 : return "name:TextParam  : cree Select Name (Name/Label)";
    case 13 : return "cree Select Visible Status (Blank Status = 0 sinon 1)";
    case 14 : return "level:IntParam  : cree Select LevelNumber";

    case 21 : return "cree Select From Drawing";
    case 22 : return "cree Select From Single View";
    case 23 : return "cree Select Drawing From, drawing(s) pour une liste d entites";
    case 24 : return "cree Select View From, views pour une liste d entites";

    case 31 : return "cree Dispatch Per Drawing";
    case 32 : return "cree Dispatch Per SingleView";

    case 40 : return "options... : cree FloatFormat ... floatformat tout court->help";
    case 41 : return "numpar:integer  value:TextParam  : cree Set Global Param";
    case 42 : return "cree Set Version -> 5.1";
    case 43 : return "cree Update Creation Date";
    case 44 : return "cree Update Last Change Date";
    case 45 : return "cree Update File Name (dynamic)";

    case 50 : return "cree AutoCorrect";
    case 51 : return "cree Compute Status (in D.E.)";
    case 52 : return "cree Rebuild Groups";
    case 53 : return "cree Rebuild Drawings";
    case 54 : return "cree AddGroup";

    case 60 : return "cree SplineToBSpline Conversion";

    case 70 : return "useflag givelist  :  Set Use Flag to value";

    default : break;
  }
  return "??";
}
