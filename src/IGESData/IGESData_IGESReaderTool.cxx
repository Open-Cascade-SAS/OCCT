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


#include <IGESData_ColorEntity.hxx>
#include <IGESData_DirPart.hxx>
#include <IGESData_FileRecognizer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_ReadWriteModule.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ReaderModule.hxx>
#include <Message_Msg.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
// MGE 17/06/98
// To use Msg class
IGESData_IGESReaderTool::IGESData_IGESReaderTool
  (const Handle(IGESData_IGESReaderData)& reader,
   const Handle(IGESData_Protocol)& protocol)
      : theglib(protocol) , therlib(protocol)
      {  SetData (reader,protocol);  }


//  ###########################################################################
//  ########                        PREPARATION                        ########

    void IGESData_IGESReaderTool::Prepare
  (const Handle(IGESData_FileRecognizer)& reco)
{
  DeclareAndCast(IGESData_IGESReaderData,igesdat,Data());
  igesdat->SetEntityNumbers();
  thereco = reco;
  SetEntities();
  thelist = igesdat->Params(0);
}


    Standard_Boolean IGESData_IGESReaderTool::Recognize
  (const Standard_Integer num, Handle(Interface_Check)& ach,
   Handle(Standard_Transient)& ent)
{
  DeclareAndCast(IGESData_IGESReaderData,igesdat,Data());
  thecnum = num;  thectyp = igesdat->DirType(num);
  Handle(IGESData_IGESEntity) anent;
  Standard_Boolean res = Standard_False;

//  Recognizer -> Liste limitative
  if (!thereco.IsNull()) res = thereco->Evaluate(thectyp,anent);
  if (res) ent = anent;
//  Sinon, Library
  else res = RecognizeByLib (num,theglib,therlib,ach,ent);
  return res;
}


//  ###########################################################################
//  ########                LECTURE  (Controle General)                ########

//    (Elements enchaines par la classe de base Interface_FileReaderTool)

    void IGESData_IGESReaderTool::BeginRead
  (const Handle(Interface_InterfaceModel)& amodel)
{
  DeclareAndCast(IGESData_IGESModel,amod,amodel);
  DeclareAndCast(IGESData_IGESReaderData,igesdat,Data());
  const IGESData_GlobalSection& gs = igesdat->GlobalSection();
  amod->SetStartSection  (igesdat->StartSection(),Standard_False);
  amod->SetGlobalSection (gs);
  Handle(Interface_Check) glob = amod->GlobalCheck();
  glob->GetMessages (igesdat->GlobalCheck());
  amod->SetGlobalCheck   (glob);
  themaxweight  = gs.MaxLineWeight();
  thegradweight = gs.LineWeightGrad();
  if (thegradweight > 0)
    { themaxweight = themaxweight/thegradweight; thegradweight = 1; }
  thedefweight = igesdat->DefaultLineWeight();
}


// Manquent les procedures de reprise sur erreur en cours de route ...
    Standard_Boolean  IGESData_IGESReaderTool::AnalyseRecord
  (const Standard_Integer num, const Handle(Standard_Transient)& anent,
   Handle(Interface_Check)& ach)
{

  Handle(TCollection_HAsciiString) lab;

  DeclareAndCast(IGESData_IGESEntity,ent,anent);
  DeclareAndCast(IGESData_IGESReaderData,igesdat,Data());

//  Demarrage de la lecture : Faire Clear
  ent->Clear();

//  UndefinedEntity : une pre-analyse est faite
  DeclareAndCast(IGESData_UndefinedEntity,undent,ent);
  if (!undent.IsNull()) {
    IGESData_DirPart DP = igesdat->DirPart(num);    // qui le copie ...
    undent->ReadDir (igesdat,DP,ach);               // DP a pu etre modifie
    ReadDir (ent,igesdat,DP,ach);                   // Lecture avec ce DP
  }
  else ReadDir (ent,igesdat,igesdat->DirPart(num),ach);

  thestep = IGESData_ReadDir;

//   Liste de Parametres : controle de son entete
//  Handle(Interface_ParamList) list = Data()->Params(num);
  Standard_Integer nbpar = Data()->NbParams(num);
  Standard_Integer n0par = (num == 1 ? 1 : (Data()->ParamFirstRank(num-1) +1));
  if (nbpar < 1) {
//   Liste vide non admise, sauf si Undefined (par exemple type nul)
    if (!undent.IsNull()) return Standard_True;
    // Sending of message : DE : no parameter
    Message_Msg Msg27 ("XSTEP_27");
    Msg27.Arg(thecnum);
    ach->SendFail(Msg27);
    return Standard_False;
  }
  const Interface_FileParameter& FP = thelist->Value(n0par);
  if ((FP.ParamType() != Interface_ParamInteger) || (atoi(FP.CValue()) != ent->TypeNumber()))
    { 
     // Sending of message : DE : Incorrect type 
      Message_Msg Msg28 ("XSTEP_28");
      Msg28.Arg(thecnum);
      ach->SendFail(Msg28);  
      return Standard_False; 
    }

  IGESData_ParamReader PR (thelist,ach,n0par,nbpar,num);
  thestep = IGESData_ReadOwn;
  ReadOwnParams (ent,igesdat,PR);
  if ((thestep = PR.Stage()) == IGESData_ReadOwn) PR.NextStage();
  if (thestep == IGESData_ReadEnd) {
    if (!PR.IsCheckEmpty()) ach = PR.Check();
    return (!ach->HasFailed());
  }

  ReadAssocs (ent,igesdat,PR);
  if ((thestep = PR.Stage()) == IGESData_ReadAssocs) PR.NextStage();
  if (thestep == IGESData_ReadEnd) {
    if (!PR.IsCheckEmpty()) ach = PR.Check();
    return (!ach->HasFailed());
  }
  ReadProps (ent,igesdat,PR);
//  thestep = IGESData_ReadEnd;
  if   (!PR.IsCheckEmpty()) ach = PR.Check();
  return (!ach->HasFailed());
}

    void IGESData_IGESReaderTool::EndRead
  (const Handle(Interface_InterfaceModel)& /* amodel */)
{
/*
  DeclareAndCast(IGESData_IGESModel,amod,amodel);
  DeclareAndCast(IGESData_IGESReaderData,igesdat,Data());
  amod->SetLineWeights(igesdat->DefaultLineWeight());
*/
}



//  ###########################################################################
//  ########                        UNE  ENTITE                        ########

//  ########                      Directory  Part                      ########

    void  IGESData_IGESReaderTool::ReadDir
  (const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_DirPart& DP, Handle(Interface_Check)& ach) const 
{ 
    
  Standard_Integer v[17];
  Standard_Character nom[9]; Standard_Character snum[9], theRes1[9],theRes2[9];
  //char mess[50]; //szv#4:S4163:12Mar99 unused

  DP.Values(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],
	    v[12],v[13],v[14],v[15],v[16],theRes1,theRes2,nom,snum);

  ent->InitTypeAndForm (v[0] , v[16]);
  Handle(IGESData_IGESEntity) fieldent, Structure, fieldlab;
  if (v[2] <  0) Structure  = GetCasted
    (IGESData_IGESEntity,          IR->BoundEntity( (1-v[2])/2 ));

  Handle(IGESData_LineFontEntity) Lnf;
  if (v[3] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity, IR->BoundEntity( (1-v[3])/2 ));
    Lnf = GetCasted(IGESData_LineFontEntity, fieldent);
    if (Lnf.IsNull()) {
      // Sending of message : Incorrect Line Font Pattern
      Message_Msg Msg29 ("XSTEP_29");
      Msg29.Arg(thecnum);
      Msg29.Arg(thectyp.Type());
      ach->SendWarning(Msg29);
      ent->InitDirFieldEntity(4,fieldent);
    }
    else ent->InitLineFont (Lnf);
  }
  else ent->InitLineFont(Lnf,v[3]);    // ici Lnf Null

  Handle(IGESData_LevelListEntity) Lvs;
  if (v[4] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity, IR->BoundEntity( (1-v[4])/2 ));
    Lvs = GetCasted(IGESData_LevelListEntity, fieldent);
    if (Lvs.IsNull()) {
      // Sending of message : Incorrect Line Font Pattern
      Message_Msg Msg30 ("XSTEP_30");
      Msg30.Arg(thecnum);
      Msg30.Arg(thectyp.Type());
      ach->SendWarning(Msg30);
      ent->InitDirFieldEntity(5,fieldent);
    }
    else ent->InitLevel(Lvs,-1);
  }
  else ent->InitLevel(Lvs,v[4]);       // ici Lvs Null

  if (v[5] != 0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[5])/2 ));
    DeclareAndCast(IGESData_ViewKindEntity,View,fieldent);
    if (View.IsNull()) {
      // Sending of message : Incorrect View 
      Message_Msg Msg31 ("XSTEP_31");
      Msg31.Arg(thecnum);
      Msg31.Arg(thectyp.Type());
      ach->SendWarning(Msg31);
      ent->InitDirFieldEntity(6,fieldent);
    }
    else ent->InitView(View);
  }

  if (v[6] != 0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[6])/2 ));
    DeclareAndCast(IGESData_TransfEntity,Transf,fieldent);
    if (Transf.IsNull()) {
      // Sending of message : Incorrect Transformation Matrix 
      Message_Msg Msg32 ("XSTEP_32");
      Msg32.Arg(thecnum);
      Msg32.Arg(thectyp.Type());
      ach->SendWarning(Msg32);
      ent->InitDirFieldEntity(7,fieldent);
    }
    else ent->InitTransf(Transf);
  }

  Handle(IGESData_LabelDisplayEntity) Lbd;
  if (v[7] != 0) {
    fieldlab = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1+v[7])/2 ));
    Lbd = GetCasted(IGESData_LabelDisplayEntity,fieldent);
    if (Lbd.IsNull()) {
      // Sending of message : Incorrect Label Display 
      Message_Msg Msg33 ("XSTEP_33");
      Msg33.Arg(thecnum);
      Msg33.Arg(thectyp.Type());
      ach->SendWarning(Msg33);
    }
  }

  ent->InitStatus (v[8] , v[9] , v[10] , v[11]);

  Standard_Integer LWeightNum  = v[13];

  Handle(IGESData_ColorEntity) Color;
  if (v[14] <  0) {
    fieldent = GetCasted(IGESData_IGESEntity,IR->BoundEntity( (1-v[14])/2 ));
    Color = GetCasted(IGESData_ColorEntity, fieldent);
    if (Color.IsNull()) {
      // Sending of message : Incorrect Color Number 
      Message_Msg Msg34 ("XSTEP_34");
      Msg34.Arg(thecnum);
      Msg34.Arg(thectyp.Type());
      ach->SendWarning(Msg34);
      ent->InitDirFieldEntity(13,fieldent);
    }
    else ent->InitColor(Color);
  }
  else ent->InitColor(Color,v[14]);

  ent->InitMisc (Structure,Lbd,LWeightNum);
  ent->InitDirFieldEntity(8,fieldlab);

// ignores : 1(type),2(ptrPsect),13(type),16(lignesPsect),17(form)
// type et forme sont lus directement du DirPart; autres infos recalculees

//    Restent a analyser nom (short label) et snum (subscript number)
  Handle(TCollection_HAsciiString) ShortLabel;
  Standard_Integer SubScriptN = -1;
  Standard_Integer iacar = 0;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i < 8; i ++) { if (nom[i] > ' ') iacar = 1;  }
  if (iacar > 0) ShortLabel = new TCollection_HAsciiString(nom);
  iacar = 0;
  for (i = 0; i < 8; i ++)
    {  if (snum[i] > ' ') iacar = 1; if(snum[i] == 0) break;  }
  if (iacar > 0) SubScriptN = atoi(snum);
  ent->SetLabel(ShortLabel,SubScriptN);

//    Enfin, SetLineWeight, tenant compte du defaut
  ent->SetLineWeight (IR->DefaultLineWeight(),themaxweight,thegradweight);
}


//  ########                     Partie Specifique                     ########

    void  IGESData_IGESReaderTool::ReadOwnParams
  (const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR,
   IGESData_ParamReader& PR) const 
{
  Handle(Interface_Check) ach = new Interface_Check;;
  Handle(Interface_ReaderModule) imodule;
  Standard_Integer CN;
  
//  Les Modules font tout
  if (therlib.Select(ent,imodule,CN))
  {
    Handle(IGESData_ReadWriteModule) module =
      Handle(IGESData_ReadWriteModule)::DownCast (imodule);
    module->ReadOwnParams(CN,ent,IR,PR);
  }
  else if (ent.IsNull()) {
//  Pas trouve dutout
    // Sending of message : Null Entity
    Message_Msg Msg35 ("XSTEP_35");
    Msg35.Arg(thecnum);
    ach->SendFail(Msg35);
//  Cas de UndefinedEntity
  } else if (ent->IsKind(STANDARD_TYPE(IGESData_UndefinedEntity))) {
    DeclareAndCast(IGESData_UndefinedEntity,undent,ent);
    undent->ReadOwnParams(IR,PR);
//    IGESEntity creee puis non reconnue ... (bizarre, non ?)
  } else {
//    IGESData_IGESType DT = ent->IGESType();
    // Sending of message : Unknown Entity
    Message_Msg Msg36 ("XSTEP_36");
    Msg36.Arg(thecnum);
    ach->SendFail(Msg36);
  }
}


//  ########                        Proprietes                         ########

    void  IGESData_IGESReaderTool::ReadProps
  (const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR,
   IGESData_ParamReader& PR) const 
{
 // MGE 17/06/98
 // Building of Messages
 //=====================================
 Message_Msg Msg38 ("XSTEP_38");
 //Message_Msg Msg221 ("XSTEP_221");
 //=====================================
 Handle(Interface_Check) ach = new Interface_Check;
  Msg38.Arg(thecnum);
  Msg38.Arg(thectyp.Type());
 if (PR.Stage() != IGESData_ReadProps) ach->SendFail(Msg38);
  Standard_Integer ncur = PR.CurrentNumber();
  Standard_Integer nbp  = PR.NbParams();
  if (ncur == nbp + 1) {  PR.EndAll();  return;  }
  else if (ncur > nbp || ncur == 0) ach->SendWarning(Msg38);

  Standard_Integer nbprops = 0;
  if (!PR.DefinedElseSkip()) return;
  if (!PR.ReadInteger(ncur,nbprops)) {
    Message_Msg Msg221 ("XSTEP_221");
    PR.SendFail(Msg221);
    return;
  }
  if (nbprops == 0)
  {
    return;
  }

  ++ncur;
  Interface_EntityList props;
  if (PR.ReadEntList
      (IR,PR.CurrentList(nbprops),Msg38, props,Standard_False) )
    ent->LoadProperties(props);
}


//  ########                      Associativites                       ########

    void  IGESData_IGESReaderTool::ReadAssocs
  (const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR,
   IGESData_ParamReader& PR) const 
{
  // MGE 17/06/98
  // Building of Messages
  //=====================================
  Message_Msg Msg37 ("XSTEP_37");
//  Message_Msg Msg220 ("XSTEP_220");
  //=====================================
  Msg37.Arg(thecnum);
  Msg37.Arg(thectyp.Type());
  Handle(Interface_Check) ach = new Interface_Check;
  if (PR.Stage() != IGESData_ReadAssocs) ach->SendFail(Msg37);
  Standard_Integer ncur = PR.CurrentNumber(); 
  Standard_Integer nbp  = PR.NbParams();
  if (ncur == nbp + 1) {  PR.EndAll();  return;  }
  else if (ncur > nbp || ncur == 0) ach->SendWarning(Msg37);

  Standard_Integer nbassocs = 0;
  if (!PR.DefinedElseSkip()) return;
  if (!PR.ReadInteger(PR.Current(),nbassocs)){
  Message_Msg Msg220 ("XSTEP_220");
  PR.SendFail(Msg220);
  return;
}
  if (nbassocs == 0) return;
  Interface_EntityList assocs;
  if (PR.ReadEntList
      (IR,PR.CurrentList(nbassocs),Msg37, assocs,Standard_False) )
    ent->LoadAssociativities(assocs);
}
