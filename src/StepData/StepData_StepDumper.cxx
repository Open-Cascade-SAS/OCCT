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
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_StepDumper.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

#include <stdio.h>
StepData_StepDumper::StepData_StepDumper
  (const Handle(StepData_StepModel)& amodel,
   const Handle(StepData_Protocol)& protocol,
   const Standard_Integer mode)
    : theslib (protocol) , thewlib (protocol) , thewriter (amodel)
{
  themodel = amodel;
  if (mode > 0) thewriter.LabelMode () = 2;
}


StepData_StepWriter&  StepData_StepDumper::StepWriter ()
{
  return thewriter;
}


Standard_Boolean  StepData_StepDumper::Dump
  (const Handle(Message_Messenger)& S, const Handle(Standard_Transient)& ent,
   const Standard_Integer level)
{
  Standard_Integer i, nb = themodel->NbEntities();
  TColStd_Array1OfInteger ids(0,nb); ids.Init(0);
  Standard_Integer num  = themodel->Number(ent);
  Standard_Integer nlab = themodel->IdentLabel(ent);
  ids.SetValue(num, (nlab > 0 ? nlab : -1) );

  if (level <= 0) {
    Handle(StepData_ReadWriteModule) module;
    Standard_Integer CN;
    if (num > 0) S << "#" << num << " = ";
    else S << "#??? = ";
    if (thewlib.Select(ent,module,CN)) {
      if (module->IsComplex(CN)) {
	TColStd_SequenceOfAsciiString listypes;
	if (!module->ComplexType(CN,listypes))
	  S << "(Complex Type : ask level > 0) cdl = "
	    << ent->DynamicType()->Name() << " (...);" << endl;
	else {
	  Standard_Integer n = listypes.Length();
	  for (i = 1; i <= n; i ++) S << listypes.Value(i) << " (...)";
	  S << endl;
	}
      }
      else S << module->StepType(CN)  << " (...);" << endl;
    }
    else S << "(Unrecognized Type for protocol) cdl = "
        << ent->DynamicType()->Name() << " (...);" << endl;
    if (nlab > 0) S << "/*   Ident in file for "<<num<<" : #"<<nlab<<"   */"<<endl;
  }

  else if (level == 1) {
//  ...  Idents  ...
    Handle(Standard_Transient) anent;
    Handle(Interface_GeneralModule) module;  Standard_Integer CN;
    if (theslib.Select(ent,module,CN)) {
      Interface_EntityIterator iter;
      module->FillSharedCase  (CN,ent,iter);
      module->ListImpliedCase (CN,ent,iter);  // on cumule ...
      for (; iter.More(); iter.Next()) {
	anent = iter.Value();
	nlab = themodel->IdentLabel(anent);
	ids.SetValue (themodel->Number(anent), (nlab > 0 ? nlab : -1) );
      }
    }
//  ...  Envoi  ...
    thewriter.SendEntity (num,thewlib);
////    thewriter.Print(S);
  }
  else {
    Handle(Standard_Transient) anent;
//    S << " --  Dumping Entity n0 " << num << "  --" << endl;
//  ...  Envoi  ...
    TColStd_Array1OfInteger tab(0,nb); tab.Init(0);
    tab.SetValue(num,1);
    Handle(Interface_GeneralModule) module;  Standard_Integer CN;
    if (theslib.Select(ent,module,CN)) {
      Interface_EntityIterator iter;
      module->FillSharedCase  (CN,ent,iter);
      module->ListImpliedCase (CN,ent,iter);  // on cumule ...
      for (; iter.More(); iter.Next()) {
	tab.SetValue(themodel->Number(iter.Value()),1);
      }
    }
    for (i = 1; i <= nb; i ++) {
// ...   Listes des idents  ...
      if (tab.Value(i) == 0) continue;
      anent = themodel->Value(i);
      thewriter.SendEntity(i,thewlib);
      if (theslib.Select(anent,module,CN)) {
	Interface_EntityIterator iter;
	module->FillSharedCase  (CN,anent,iter);
	module->ListImpliedCase (CN,anent,iter);  // on cumule ...
	for (; iter.More(); iter.Next()) {
	  anent = iter.Value();
	  nlab = themodel->IdentLabel(anent);
	  ids.SetValue (themodel->Number(anent), (nlab > 0 ? nlab : -1) );
	}
      }
    }
////    thewriter.Print(S);
  }

//  ....  Affichage des idents  silya  ....
  Standard_Integer nbi = 0 , nbe = 0 , nbq = 0 , nbu = 0;
  for (i = 1; i <= nb; i ++) {
    nlab = ids.Value(i);
    if (nlab == 0) continue;
    nbe ++;
    if      (nlab <  0) nbu = 0;
    else if (nlab == i) nbq = 0;
    else if (nlab >  0) nbi ++;
  }
  if (nbe > 0) {
//    S <<" --   Displayed nums:"<<nbe<<"  with ident=num:"<<nbq<<" , distinct proper ident:"<<nbi<<"\n";
    if (nbu > 0) {
      S<<" (no ident): ";
      for (i = 1; i <= nb; i ++)
	{  if (ids.Value(i) >=  0) continue;	S<<" #"<<i;      }
      S<<endl;
    }
    if (nbq > 0) {
      S<<" (ident = num): ";
      for (i = 1; i <= nb; i ++)  {  if (ids.Value(i) == i) S<<" #"<<i;  }
      S<<endl;
    }
    if (nbi < 0) {  // on n affiche plus num:#id , on envoie un petit help
      Standard_Integer nbl = 0, nbr = 0, nbr0 = 0, nbc = 0;
      char unid[30];
//      S<<" (proper ident):  #num	     #ident"<<endl;
      S<<" (proper ident):  num:#ident  num:#ident  ..."<<endl;
      for (i = 1; i <= nb; i ++)  {
	if (ids.Value(i) <= 0 || ids.Value(i) == i) continue;
	sprintf (unid,"%d:#%d",i,ids.Value(i));
	nbc = (Standard_Integer) strlen (unid);  nbr = ((80-nbc) %4) +2;
	nbl +=  nbc;
	if (nbl+nbr0 > 79) { nbl  = nbc;  S<<endl; }
	else               { nbl += nbr0; for (; nbr0 > 0; nbr0 --) S << " "; }
	S<<unid;
	nbr0 = nbr;

//	if (nbl+nbc > 79) { S<<endl<<unid; nbl  = 0; }
//	else              { S<<unid;       }
//	nbl += (nbc+nbr);
//	nbr = ((80-nbc) % 4) +1;
//	S<<"  "<<i<<" ->#"<<ids.Value(i);
//	nbl ++; if (nbl > 5) {  nbl = nbr = 0;  S<<endl;  }
      }
      if (nbl > 0) S<<endl;
    }
    if (nbi > 0) S<<"In dump, iii:#jjj means : entity rank iii has step ident #jjj"<<endl;
//    S <<" --   Dumping data, entity "<<num<<"  level "<<level<<" :"<<endl;
  }
  if (level > 0) 
  {
    Standard_SStream aStream;
    thewriter.Print(aStream);
    S << aStream;
  }
  return Standard_True;
}


Standard_Boolean  StepData_StepDumper::Dump
  (const Handle(Message_Messenger)& S, const Standard_Integer num,
   const Standard_Integer level)
{
  if (num <= 0 || num > themodel->NbEntities()) return Standard_False;
  Handle(Standard_Transient) ent = themodel->Value(num);
  return Dump (S,ent,level);
}
