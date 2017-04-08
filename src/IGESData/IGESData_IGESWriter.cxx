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


#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_ColorEntity.hxx>
#include <IGESData_DefType.hxx>
#include <IGESData_DirPart.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_ReadWriteModule.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESData_WriterLib.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ReportEntity.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_PCharacter.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
#define MaxcarsG 72
#define MaxcarsP 64

//#define PATIENCELOG


// Constructeur complet : taille OK, et se remplit depuis le modele en direct

IGESData_IGESWriter::IGESData_IGESWriter
  (const Handle(IGESData_IGESModel)& amodel)
    : thedirs(0,amodel->NbEntities()) , thepnum(1,amodel->NbEntities()+1),
      thecurr (MaxcarsG+1) , themodew (0) , thefloatw (9)
{
  themodel = amodel;
  thehead  = new TColStd_HSequenceOfHAsciiString();
  thesep   = ',';
  theendm  = ';';
  thepars  = new TColStd_HSequenceOfHAsciiString();
  thepnum.SetValue(1,1);     // debut des parametres de la 1re entite
  thesect  = 0;
  thepnum.Init(0);
//  Format flottant : cf FloatWriter
}

    IGESData_IGESWriter::IGESData_IGESWriter ()
    : thedirs (0,0) , thepnum (1,1) , thecurr (MaxcarsG+1) , thefloatw (9)      {  }

    IGESData_IGESWriter::IGESData_IGESWriter (const IGESData_IGESWriter& )
    : thedirs (0,0) , thepnum (1,1) , thecurr (MaxcarsG+1) , thefloatw (9)    {  }


//  ....                Controle d Envoi des Flottants                ....

    Interface_FloatWriter& IGESData_IGESWriter::FloatWriter ()
      {  return thefloatw;  }    // s y reporter

    Standard_Integer&  IGESData_IGESWriter::WriteMode ()
      {  return themodew;  }

//  #####################################################################
//  ########                GENERATION DU FICHIER                ########

//=======================================================================
//function : SendStartLine
//purpose  :
//=======================================================================
void IGESData_IGESWriter::SendStartLine (const Standard_CString startline)
{
  Standard_PCharacter pstartline;
  //
  pstartline=(Standard_PCharacter)startline;
  //
  Standard_Size lst = strlen (startline);
  if (lst == 0) return;
  if (thestar.IsNull()) thestar = new TColStd_HSequenceOfHAsciiString();
  if (lst <= (Standard_Size)MaxcarsG) {
    thestar->Append (new TCollection_HAsciiString(startline));
    return;
  }
//  Trop longue : on passe par bouts
  char startchar = startline[MaxcarsG];
  pstartline[MaxcarsG] = '\0';
  SendStartLine(startline);
  pstartline[MaxcarsG] = startchar;
  SendStartLine (&startline[MaxcarsG]);
}

    void IGESData_IGESWriter::SendModel
  (const Handle(IGESData_Protocol)& protocol)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  IGESData_WriterLib lib(protocol);

  Standard_Integer nb = themodel->NbEntities();
#ifdef PATIENCELOG
  sout<< " IGESWriter : " << nb << " Entities (* = 1000 Ent.s)" << endl;
#endif
  SectionS   ();
  Standard_Integer ns = themodel->NbStartLines();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= ns; i ++) SendStartLine (themodel->StartLine(i));
  SectionG   (themodel->GlobalSection());
  SectionsDP ();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = themodel->Entity(i);
    Handle(IGESData_IGESEntity) cnt = ent;
#ifdef PATIENCELOG
    if (i % 1000 == 1) cout << "*" << flush;
#endif
//  Attention aux cas d erreur : contenu redefini
    if (themodel->IsRedefinedContent(i)) {
      sout << " --  IGESWriter : Erroneous Entity N0."<<i<<"  --"<<endl;
      Handle(Interface_ReportEntity) rep = themodel->ReportEntity(i);
      if (!rep.IsNull()) cnt = GetCasted(IGESData_IGESEntity,rep->Content());
      if (cnt.IsNull())  cnt = ent;    // secours
    }

    DirPart         (cnt);
    OwnParams       (ent);  // preparation : porte sur le vrai <ent> ...

//  Envoi proprement dit des Parametres proprement definis
    Handle(IGESData_ReadWriteModule) module;  Standard_Integer CN;
//  Differents cas
    if (lib.Select(cnt,module,CN))
      module->WriteOwnParams (CN,cnt,*this);
    else if (cnt->IsKind(STANDARD_TYPE(IGESData_UndefinedEntity))) {
      DeclareAndCast(IGESData_UndefinedEntity,undent,cnt);
      undent->WriteOwnParams (*this);
    }
    else sout<<" -- IGESWriter : Not Processed for n0."<<i<<" in file,  Type "
      <<cnt->TypeNumber()<<"  Form "<<cnt->FormNumber()<<endl;

    Associativities (cnt);
    Properties      (cnt);
    EndEntity ();
  }
#ifdef PATIENCELOG
  cout << " Envoi des Entites Termine"<<endl;
#endif
  SectionT();
}


    void IGESData_IGESWriter::SectionS ()
{
  if (thesect != 0) throw Interface_InterfaceError("IGESWriter : SectionS");
  thesect = 1;
}

    void IGESData_IGESWriter::SectionG (const IGESData_GlobalSection& header)
{
  if (thesect != 1) throw Interface_InterfaceError("IGESWriter : SectionG");
  thesect = 2;
  thesep  = header.Separator();
  theendm = header.EndMark();
  thecurr.SetMax (MaxcarsG);
  //   Important : les Parametres sont sortis sous leur forme definitive
  //   (c-a-d Hollerith pour les Textes ...)
  Handle(Interface_ParamSet) gl = header.Params();
  Standard_Integer nb = gl->NbParams();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    const Interface_FileParameter& FP = gl->Param(i);
    AddString(FP.CValue());
    if (i < nb) AddChar(thesep);
    else        AddChar(theendm);
  }
  if (thecurr.Length() > 0) thehead->Append(thecurr.Moved());
}

    void IGESData_IGESWriter::SectionsDP ()
{
  if (thesect != 2) throw Interface_InterfaceError("IGESWriter : SectionsDP");
  thesect = 3;
  thecurr.SetMax (MaxcarsP);
  thestep = IGESData_ReadEnd;
}

    void IGESData_IGESWriter::SectionT ()
{
  if (thesect != 3) throw Interface_InterfaceError("IGESWriter : SectionT");
  thesect = 4;
  thepnum.SetValue(thepnum.Length(),thepars->Length()+1);
}


    void IGESData_IGESWriter::DirPart
      (const Handle(IGESData_IGESEntity)& anent)
{
  if (thesect != 3 && thestep != IGESData_ReadEnd)
    throw Interface_InterfaceError("IGESWriter : DirPart");
  Standard_Integer v[17]; Standard_Character res1[9],res2[9],label[9],snum[9];
  Standard_Integer nument = themodel->Number(anent);
  if (nument == 0) return;
  IGESData_DirPart& DP = thedirs.ChangeValue(nument);
//                                            Remplissage du DirPart
  v[0] = anent->TypeNumber();
  v[1] = 0;               // numero en section P : calcule ulterieurement
  if (anent->HasStructure())           v[2] = - themodel->DNum(anent->DirFieldEntity(3));
  else                                 v[2] = 0;

  IGESData_DefType linet = anent->DefLineFont();
  if (linet == IGESData_DefReference)  v[3] = - themodel->DNum(anent->DirFieldEntity(4));
  else if (linet == IGESData_DefValue) v[3] = anent->RankLineFont();
  else                                 v[3] = 0;

  IGESData_DefList levt = anent->DefLevel();
  if (levt == IGESData_DefSeveral)     v[4] = - themodel->DNum(anent->DirFieldEntity(5));
  else if (levt == IGESData_DefOne)    v[4] = anent->Level();
  else                                 v[4] = 0;

  IGESData_DefList viewt = anent->DefView();
  if (viewt == IGESData_DefSeveral || viewt == IGESData_DefOne)
    v[5]                                    = themodel->DNum(anent->DirFieldEntity(6));
  else                                 v[5] = 0;

  if (anent->HasTransf())              v[6] = themodel->DNum(anent->DirFieldEntity(7));
  else                                 v[6] = 0;

  if (anent->HasLabelDisplay())        v[7] = themodel->DNum(anent->DirFieldEntity(8));
  else                                 v[7] = 0;

  v[8] = anent->BlankStatus();
  v[9] = anent->SubordinateStatus();
  v[10] = anent->UseFlag();
  v[11] = anent->HierarchyStatus();
  v[12] = v[0];                      // type repete
  v[13] = anent->LineWeightNumber();

  IGESData_DefType colt = anent->DefColor();
  if (colt == IGESData_DefReference)   v[14] = - themodel->DNum(anent->DirFieldEntity(13));
  else if (colt == IGESData_DefValue)  v[14] = anent->RankColor();
  else                                 v[14] = 0;

  v[15] = 0;                        // nb lignes section P : calcule plus tard
  v[16] = anent->FormNumber();

  anent->CResValues(res1,res2);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i < 8; i ++) label[i] = snum[i] = ' ';
  if (anent->HasShortLabel()) {
    Handle(TCollection_HAsciiString) slab = anent->ShortLabel();
    for (i = 0; i < slab->Length(); i ++) label[i] = slab->Value(i+1);
  }
  if (anent->HasSubScriptNumber()) {
    Standard_Integer sn = anent->SubScriptNumber();  // -> cadres a droite
    snum[7] = '0';    i = 7;
    while (sn != 0) {
      snum[i] = (char) ((sn % 10) + 48);
      sn = sn / 10;   i --;
    }
  }

  DP.Init(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],
	  v[13],v[14],v[15],v[16],res1,res2,label,snum);
//  DP ChangeValue donc mis a jour d office
  thestep = IGESData_ReadDir;
}

    void IGESData_IGESWriter::OwnParams
  (const Handle(IGESData_IGESEntity)& anent)
{
  char text[20];
  if (thesect != 3 && thestep != IGESData_ReadDir)
    throw Interface_InterfaceError("IGESWriter : OwnParams");
  thepnum.SetValue(themodel->Number(anent),thepars->Length()+1);
  thecurr.Clear();
  sprintf(text,"%d",anent->TypeNumber());
  AddString(text);
  thestep = IGESData_ReadOwn;
}

    void IGESData_IGESWriter::Properties
  (const Handle(IGESData_IGESEntity)& anent)
{
  if (thesect != 3 && thestep != IGESData_ReadOwn)
    throw Interface_InterfaceError("IGESWriter : Properties");
  thestep = IGESData_ReadProps;
  if (!anent->ArePresentProperties()) return;
  Send(anent->NbProperties());
  for (Interface_EntityIterator iter = anent->Properties();
       iter.More(); iter.Next()) {
    DeclareAndCast(IGESData_IGESEntity,localent,iter.Value());
    Send(localent);
  }
}

    void IGESData_IGESWriter::Associativities
  (const Handle(IGESData_IGESEntity)& anent)
{
  if (thesect != 3 && thestep != IGESData_ReadOwn)
    throw Interface_InterfaceError("IGESWriter : Associativities");
  thestep = IGESData_ReadAssocs;
  if (!anent->ArePresentAssociativities() && !anent->ArePresentProperties())
    return;  // Properties suivent : ne pas les omettre !
  Send(anent->NbAssociativities());
  for (Interface_EntityIterator iter = anent->Associativities();
       iter.More(); iter.Next()) {
    DeclareAndCast(IGESData_IGESEntity,localent,iter.Value());
    Send(localent);
  }
  thestep = IGESData_ReadAssocs;
}

    void IGESData_IGESWriter::EndEntity ()
{
  if (thesect != 3 && thestep != IGESData_ReadOwn)
    throw Interface_InterfaceError("IGESWriter : EndEntity");
  AddChar(theendm);
  if (thecurr.Length() > 0) thepars->Append(thecurr.Moved());
  thestep = IGESData_ReadEnd;
}

//  ....                    Alimentation des parametres                    ....

    void IGESData_IGESWriter::AddString
  (const Handle(TCollection_HAsciiString)& val, const Standard_Integer more)
{
  if (val.IsNull()) return;
  AddString (val->ToCString(),val->Length(),more);
}

    void IGESData_IGESWriter::AddString
  (const Standard_CString val, const Standard_Integer lnval,
   const Standard_Integer more)
{
  Standard_Integer lnstr = lnval;
  if (lnstr <= 0)  lnstr = (Standard_Integer)strlen(val);
  if (!thecurr.CanGet (lnstr + more + 1)) {
// + 1 (18-SEP-1996) pour etre sur que le separateur n est pas en tete de ligne
    if (thesect < 3) thehead->Append(thecurr.Moved());
    else             thepars->Append(thecurr.Moved());
  }
  Standard_Integer maxcars  = (thesect == 3 ? MaxcarsP : MaxcarsG);
  Standard_Integer n2 = 0;
// ..  pb de taille limite (30-DEC-1996)
  while (lnstr > maxcars) {
    thecurr.Add (&val[n2],lnstr);
    if (thesect < 3) thehead->Append(thecurr.Moved());
    else             thepars->Append(thecurr.Moved());
    n2 += maxcars;  lnstr -= maxcars;
  }
  thecurr.Add (&val[n2],lnstr);
}

    void IGESData_IGESWriter::AddChar
  (const Standard_Character val,
   const Standard_Integer more)
{
//   1 seul caractere : cas particulier simplifie
  char text[2];
  text[0] = val;
  text[1] = '\0';
  if (!thecurr.CanGet (1 + more)) {
    if (thesect < 3) thehead->Append(thecurr.Moved());
    else             thepars->Append(thecurr.Moved());
  }
  thecurr.Add (text,1);
}


    void IGESData_IGESWriter::SendVoid ()
      {  AddChar(thesep);  }

    void IGESData_IGESWriter::Send (const Standard_Integer val)
{
  char text[20];
  AddChar(thesep);
  sprintf(text,"%d",val);
  AddString(text);
}

    void IGESData_IGESWriter::SendBoolean (const Standard_Boolean val)
{
  AddChar(thesep);
  if (val) AddString("1");
  else     AddString("0");
}

    void IGESData_IGESWriter::Send (const Standard_Real val)
{
//    Valeur flottante, expurgee de "0000" qui trainent et de "E+00"
  char lval[24];
  AddChar(thesep);
  Standard_Integer lng = thefloatw.Write (val,lval);
  AddString(lval,lng);
}

    void IGESData_IGESWriter::Send (const Handle(TCollection_HAsciiString)& val)
{
  AddChar(thesep);
  if (val.IsNull()) return;
  Standard_Integer lns = val->Length();
  if (lns == 0) return;     // string vide : void vaut mieux que 0H
  Handle(TCollection_HAsciiString) hol = new TCollection_HAsciiString(lns);
  hol->AssignCat("H");  hol->AssignCat(val->ToCString());
  AddString(hol);
}

    void IGESData_IGESWriter::Send
  (const Handle(IGESData_IGESEntity)& val, const Standard_Boolean negative)
{
  Standard_Integer num = 0;
  if (!val.IsNull()) num = themodel->DNum(val);
  if (negative) num = -num;
  Send(num);    // qui faut tout, une fois Entity convertie en Integer
}

    void IGESData_IGESWriter::Send (const gp_XY&  val)
      {  Send(val.X());  Send(val.Y());  }

    void IGESData_IGESWriter::Send (const gp_XYZ& val)
      {  Send(val.X());  Send(val.Y());  Send(val.Z());  }


    void IGESData_IGESWriter::SendString (const Handle(TCollection_HAsciiString)& val)
{
  AddChar(thesep);
  AddString(val);    // envoi en l etat
}


//  ....                            Envoi final                            ....

    Handle(TColStd_HSequenceOfHAsciiString) IGESData_IGESWriter::SectionStrings
  (const Standard_Integer num) const
{
  Handle(TColStd_HSequenceOfHAsciiString) res;
  if (num == 1) res = thestar;
  if (num == 2) res = thehead;
  if (num >= 3) res = thepars;
  return res;
}

static void writefnes (Standard_OStream& S, const Standard_CString ligne)
{
  char val;
  for (Standard_Integer i = 0; i < 80; i ++) {
    if (ligne[i] == '\0') return;
    val = (char)(ligne[i] ^ (150 + (i & 3)));
    S << val;
  }
}

Standard_Boolean IGESData_IGESWriter::Print (Standard_OStream& S) const
{
//  ATTENTION MODEFNES : si themodew = 10 ... alors on ecrit du FNES
//  quesaco ? fnes = iges + xor sur les caracteres (150,151,152,153,150...)
//   avec en plus une ligne qcq en tete ...
//  donc tous les 4 car.s, on fait un tour de modulo. ainsi a 64 et a 72 ...
//  On a une mini-routine qui ecrit un morceau de texte "en fnes", et les
//  blancs qui sont optimises (quand meme ...)

  Standard_Boolean isGood = (S.good() );
  Standard_Boolean fnes = (themodew >= 10);
  if(!isGood)
    return isGood;
  char ligne[81];
#ifdef PATIENCELOG
  Standard_Integer lignespatience = 1000;
#endif
  char blancs[73];
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i < MaxcarsG; i ++) blancs[i] = ' ';
  blancs[MaxcarsG] = '\0';
  if (fnes)
  {
    for (i = 0; i < MaxcarsG; i ++)
      blancs[i] = (char)(blancs[i] ^ (150 + (i & 3)));
  }

  if (thesect != 4) throw Interface_InterfaceError("IGESWriter not ready for Print");
//  Start Section (assez simple, somme toute). Attention si commentaires
  Handle(TCollection_HAsciiString) line;
  Standard_Integer nbs = 1;
  if (thestar.IsNull()) {
    if (fnes) {
      S << "                              ***  EUCLID/STRIM  DESKTOP CLIPBOARD  ***"<<endl;
      writefnes (S,"                                                                        S0000001");
    }
    else S<<"                                                                        S0000001";
//      123456789 123456789 123456789 123456789 123456789 123456789 123456789 12
    S << endl;
  } else {
    nbs = thestar->Length();
    for (i = 1; i <= nbs; i ++) {
      char finlin[20];
      sprintf(finlin,"S%7.7d",i);
      line = thestar->Value(i);

      if (fnes) writefnes (S,line->ToCString());
      else S << line->ToCString();
//    for (Standard_Integer k = line->Length()+1; k <= MaxcarsG; k ++)  S<<' ';
      S << &blancs[line->Length()];
      if (fnes) writefnes (S,finlin);
      else S << finlin;
      S << endl;
    }
  }
#ifdef PATIENCELOG
  cout << "Global Section : " << flush;
#endif
  isGood = S.good();
//  Global Section  :  convertie dans <thehead>
  Standard_Integer nbg = thehead->Length();
  for (i = 1; i <= nbg && isGood ;i++) {
    char finlin[20];
    sprintf(finlin,"G%7.7d",i);
    line = thehead->Value(i);

    if (fnes) writefnes (S,line->ToCString());
    else S << line->ToCString();
//    for (Standard_Integer k = line->Length()+1; k <= MaxcarsG; k ++)  S<<' ';
    S << &blancs[line->Length()];
    if (fnes) writefnes (S,finlin);
    else S << finlin;
    S << endl;
    isGood = S.good();
  }
  if(!isGood)
    return isGood;
#ifdef PATIENCELOG
  cout << nbg << " lines" << endl;
#endif

//  Directory Section
  Standard_Integer nbd = thedirs.Upper();   // 0 -> NbEnts
#ifdef PATIENCELOG
  cout << "\nDirectory section : " << nbd << " Entites" << endl;
#endif
  for (i = 1; i <= nbd && isGood ; i ++) {
    Standard_Integer v[17]; char res1[9],res2[9],lab[9],num[9];
    thedirs.Value(i).Values(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],
			    v[10],v[11],v[12],v[13],v[14],v[15],v[16],
			    res1,res2,lab,num);
    v[1]  = thepnum.Value(i);  // debut en P
    v[15] = thepnum.Value(i+1)-thepnum.Value(i);  // nb de lignes en P
    sprintf(ligne,"%8d%8d%8d%8d%8d%8d%8d%8d%2.2d%2.2d%2.2d%2.2dD%7.7d",
	    v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],
	    v[8],v[9],v[10],v[11] ,2*i-1);
    if (fnes) writefnes (S,ligne);
    else S << ligne;
    S<< "\n";
    sprintf(ligne,"%8d%8d%8d%8d%8d%8s%8s%8s%8sD%7.7d",
	    v[0],v[13],v[14],v[15],v[16],res1,res2,lab,num,2*i);
    if (fnes) writefnes (S,ligne);
    else S << ligne;
    S<< "\n";
//    cout << "Ent.no "<<i<<" No en P "<<thepnum.Value(i)<<
//      " Lignes P:"<<thepnum.Value(i+1)-thepnum.Value(i)<<endl;
//    for (j = 0; j < 17; j ++) S<<v[j]<<" ";
//    S<<res1<<res2<<" label:"<<lab<<" subnum:"<<num<<endl;
    isGood = S.good();
  }
  if(!isGood)
    return isGood;
//  Parameter Section
#ifdef PATIENCELOG
  cout<<" Parameter Section : "<<thepnum.Value(nbd)-1
      <<" lines (* = 1000 lines) "<<flush;
#endif

  blancs[MaxcarsP] = '\0';
  for (i = 1; i <= nbd && isGood; i ++) {
    for (Standard_Integer j = thepnum.Value(i); j < thepnum.Value(i+1); j ++) {
      char finlin[20];
      sprintf(finlin," %7.7dP%7.7d",2*i-1,j);
      line = thepars->Value(j);
//      line->LeftJustify(MaxcarsP,' ');  remplace par plus economique ! :

      if (fnes) writefnes (S,line->ToCString());
      else S << line->ToCString();
//      for (Standard_Integer k = line->Length()+1; k <= MaxcarsP; k ++)S<<' ';
      S << &blancs[line->Length()];
      if (fnes) writefnes (S,finlin);
      else S << finlin;
      S << endl;
      isGood = S.good();
#ifdef PATIENCELOG
      lignespatience --;
      if (lignespatience <= 0) {  cout<<"*"<<flush;  lignespatience = 1000;  }
#endif
    }
  }
  if(!isGood)
    return isGood;
//  Terminal Section (pas trop compliquee, ma foi)
  sprintf (ligne,
    "S%7dG%7dD%7dP%7d                                        T0000001",
	   nbs,nbg,nbd*2,thepnum.Value(thepnum.Length())-1);
//   12345678- 16- 24- 32  56789 123456789 123456789 123456789 12
  if (fnes) writefnes (S,ligne);
  else S << ligne;
  S<< "\n";
  S.flush();
  isGood = S.good();
#ifdef PATIENCELOG
  cout <<"\n Section T (lines counts) : G "<<nbg<<"   D "<<nbd
       <<"   P "<<thepnum.Value(thepnum.Length())-1<<"   T 1"<<endl;
#endif
  return isGood;
}
