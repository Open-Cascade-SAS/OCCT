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

//gka 06.01.99 S3767
//abv 10.04.99 S4136: eliminate using BRepAPI::Precision()

#include <BRepLib.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_Functions.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESData_FileProtocol.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESToBRep_Actor.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Timer.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

#include <stdio.h>
// S3767 dce 18/01/1999
//Transfer_Iterator.hxx>
// add of stdio.h for NT compilation
//=======================================================================
//function : IGESControl_Reader
//purpose  : 
//=======================================================================
IGESControl_Reader::IGESControl_Reader ()
{
  IGESControl_Controller::Init();
  SetWS (new XSControl_WorkSession);
  SetNorm("IGES");
  theReadOnlyVisible = Standard_False;
}


//=======================================================================
//function : IGESControl_Reader
//purpose  : 
//=======================================================================

IGESControl_Reader::IGESControl_Reader
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  IGESControl_Controller::Init();
  SetWS (WS,scratch);
  SetNorm ("IGES");
  theReadOnlyVisible = Standard_False;
}


//=======================================================================
//function : IGESModel
//purpose  : 
//=======================================================================

Handle(IGESData_IGESModel) IGESControl_Reader::IGESModel () const
{
  return Handle(IGESData_IGESModel)::DownCast(Model());
}



//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer  IGESControl_Reader::NbRootsForTransfer()
{
  if (therootsta) return theroots.Length();
  therootsta = Standard_True;
  
  Handle(IGESData_IGESModel) model = IGESModel(); 
  if (model.IsNull()) return 0;
  
  Handle(XSControl_WorkSession) session = WS();
  Handle(Interface_Protocol) protocol = session->Protocol();
  Handle(XSControl_Controller) controller = session->NormAdaptor();
  Handle(Transfer_ActorOfTransientProcess) actor = controller->ActorRead(model);
  
  Interface_ShareFlags SH (model,protocol);
   
  // sln 11.06.2002 OCC448
  Interface_Static::SetIVal("read.iges.onlyvisible",theReadOnlyVisible);
  
  Standard_Integer nb = model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = model->Entity(i);
    if ( SH.IsShared(ent) || ! actor->Recognize (ent) ) continue;
    // on ajoute un traitement pour ne prendre que les entites visibles
    if ( ! theReadOnlyVisible || ent->BlankStatus() == 0 ) {
      theroots.Append(ent);
    }
  }
  
  return theroots.Length();
}

//  ####    Reliquat de methodes a reprendre    ####

//=======================================================================
// Function : PrintTransferInfo
// Purpose  : Print statistics information on transfer using MoniTool message management
// Created  : 18/01/98 DCE for S3767
// Modified : 
//=======================================================================

void  IGESControl_Reader::PrintTransferInfo
  (const IFSelect_PrintFail failsonly, const IFSelect_PrintCount mode) const
{
  Standard_Integer nbWarn = 0, nbFail= 0, nbEntities =0, nbRoots = 0, nbResults = 0;  
  const Handle(Transfer_TransientProcess) &TP = WS()->TransferReader()->TransientProcess();
  Handle(Message_Messenger) TF = TP->Messenger();
  const Handle(Interface_InterfaceModel) &model = TP->Model();
  if (! model.IsNull()) {
    nbEntities = model->NbEntities();
    nbRoots = TP->NbRoots();
    //nbResults = TP->NbMapped();
    Transfer_IteratorOfProcessForTransient iterTrans = TP->RootResult(Standard_True);
    NCollection_DataMap<TCollection_AsciiString, Standard_Integer> aMapCountResult;
    NCollection_DataMap<TCollection_AsciiString, Standard_Integer> aMapCountMapping;
    for (iterTrans.Start(); iterTrans.More() ; iterTrans.Next() ) {
      nbResults++;
      // Init for dicoCountResult for IFSelect_ResultCount
      if ( mode == IFSelect_ResultCount ) {
        char mess[300];
        const Handle(Transfer_Binder) aBinder = iterTrans.Value();
        sprintf(mess,"\t%s",aBinder->ResultTypeName());
        if (aMapCountResult.IsBound(mess))
          aMapCountResult.ChangeFind(mess)++;
        else
          aMapCountResult.Bind(mess,1);
      }
      // Init for dicoCountMapping for IFSelect_Mapping
      else if ( mode == IFSelect_Mapping ) {
        char mess[300];
        const Handle(Transfer_Binder) aBinder = iterTrans.Value();
        DeclareAndCast(IGESData_IGESEntity,igesEnt,iterTrans.Starting());

        sprintf(mess,"%d\t%d\t%s\t%s", igesEnt->TypeNumber(), igesEnt->FormNumber(),
        "%d", aBinder->ResultTypeName());
        //cout << mess << endl;
        if (aMapCountMapping.IsBound(mess))
          aMapCountMapping.ChangeFind(mess)++;
        else
          aMapCountMapping.Bind(mess, 1);
      }
    }

    Interface_CheckIterator checkIterator = TP->CheckList(Standard_False);
    NCollection_DataMap<TCollection_AsciiString, Standard_Integer> aMapCount;
    NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfInteger)> aMapList;
    // Init the dicoCount dicoList and nbWarn ,nb Fail.
    for(checkIterator.Start(); checkIterator.More(); checkIterator.Next() ) {
      char mess[300];
      const Handle(Interface_Check) aCheck = checkIterator.Value(); 
      Handle(Standard_Transient) ent = model->Value(checkIterator.Number());
      DeclareAndCast(IGESData_IGESEntity,igesEnt,ent);
      Standard_Integer type = igesEnt->TypeNumber(), form = igesEnt->FormNumber();
      Standard_Integer nw = aCheck->NbWarnings(), nf = aCheck->NbFails(), i;
      for(i = 1; (failsonly==IFSelect_FailAndWarn) && (i<= nw); i++) {
        sprintf(mess,"\t W\t%d\t%d\t%s",type,form,aCheck->CWarning(i));
        if (aMapCount.IsBound(mess))
          aMapCount.ChangeFind(mess)++;
        else
          aMapCount.Bind(mess, 1);

        Handle(TColStd_HSequenceOfInteger) alist;
        if (aMapList.IsBound(mess))
          alist = aMapList.ChangeFind(mess);
        else {
          alist = new TColStd_HSequenceOfInteger();
          aMapList.Bind(mess, alist);
        }
        alist->Append(model->Number(igesEnt)*2-1);
      }
      for(i = 1; i<= nf; i++) {
        sprintf(mess,"\t F\t%d\t%d\t%s",type,form,aCheck->CFail(i));
        // TF << mess << endl;
        if (aMapCount.IsBound(mess))
          aMapCount.ChangeFind(mess)++;
        else
          aMapCount.Bind(mess, 1);
        Handle(TColStd_HSequenceOfInteger) alist;
        if (aMapList.IsBound(mess))
          alist = aMapList.ChangeFind(mess);
        else {
          alist = new TColStd_HSequenceOfInteger();
          aMapList.Bind(mess, alist);
        }
        alist->Append(model->Number(igesEnt)*2-1);
      }
      nbWarn += nw;
      nbFail += nf;
    }
    Message_Msg msg3000("IGES_3000");  // *************************
    TF->Send (msg3000, Message_Info); //smh#14
    
    switch (mode) {
    case IFSelect_GeneralInfo : {
      Message_Msg msg3005("IGES_3005");TF->Send(msg3005, Message_Info);
      Message_Msg msg3010("IGES_3010");msg3010.Arg(nbEntities);TF->Send(msg3010, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3020("IGES_3020");msg3020.Arg(nbWarn);TF->Send(msg3020, Message_Info);
      Message_Msg msg3025("IGES_3025");msg3025.Arg(nbFail);TF->Send(msg3025, Message_Info);
      break;
    }
    case IFSelect_CountByItem : 
    case IFSelect_ListByItem : {
      Message_Msg msg3030("IGES_3030");
      TF->Send(msg3030, Message_Info);
      NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator aMapCountIter(aMapCount);
      NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfInteger)>::Iterator aMapListIter(aMapList);
      for(; aMapCountIter.More() && aMapListIter.More();
            aMapCountIter.Next(), aMapListIter.Next()) {
        TF << aMapCountIter.Value() << aMapCountIter.Key() << endl;
        if (mode == IFSelect_ListByItem) {
          Handle(TColStd_HSequenceOfInteger) entityList = aMapListIter.Value();
          Standard_Integer length = entityList->Length();
          Message_Msg msg3035("IGES_3035");
          TF->Send(msg3035, Message_Info);
          char line[80];
          sprintf(line, "\t\t\t");
          TF << line;
          Standard_Integer nbInLine = 0;
          for (Standard_Integer i = 1; i <= length; i++) {
            // IDT_Out << (entityList->Value(i)) << " ";
            sprintf(line, "\t %d", entityList->Value(i));
            TF << line;
            if (++nbInLine == 6) {
              nbInLine = 0;
              sprintf(line, "\n\t\t\t");
              TF << line;
            }
          }
          TF << endl;
        }
      }
      break;
    }
    case IFSelect_ResultCount : { 
      Message_Msg msg3040("IGES_3040");TF->Send(msg3040, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3045("IGES_3045");TF->Send(msg3045, Message_Info);

      NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator aMapIter(aMapCountResult);
      for (; aMapIter.More(); aMapIter.Next())
      {
        TF << aMapIter.Key() << aMapIter.Value() << endl;
      }
      break;
    }
    case IFSelect_Mapping : { 
      Message_Msg msg3040("IGES_3050");TF->Send(msg3040, Message_Info);
      Message_Msg msg3011("IGES_3011");msg3011.Arg(nbRoots);TF->Send(msg3011, Message_Info);      
      Message_Msg msg3015("IGES_3015");msg3015.Arg(nbResults);TF->Send(msg3015, Message_Info);
      Message_Msg msg3045("IGES_3055");TF->Send(msg3045, Message_Info);
      // Add failed entities in dicoCountMapping
      if (nbRoots!=nbResults) {
        for (Standard_Integer i = 1; i <= nbRoots; i++) {
          DeclareAndCast(IGESData_IGESEntity, root, TP->Root(i));
          if (!TP->IsBound(root)) {
            char mess[300];

            sprintf(mess, "%d\t%d \t%s\t%s", root->TypeNumber(), root->FormNumber(),
              "%d", "Failed");
            //cout << mess << endl;
            if (aMapCountMapping.IsBound(mess))
              aMapCountMapping.ChangeFind(mess)++;
            else
              aMapCountMapping.Bind(mess, 1);
          }
        }
      }
      NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator aMapCountIter(aMapCountMapping);
      for(; aMapCountIter.More(); aMapCountIter.Next()) {
        char mess[80];
        sprintf(mess, aMapCountIter.Key().ToCString(), aMapCountIter.Value());
        TF << mess << endl; //dicoCountIter.Value() << dicoCountIter.Name() << endl;
      }
      break;
    }
    default: break;
    }
  }
}
