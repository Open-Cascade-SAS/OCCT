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


#include <IFGraph_AllShared.hxx>
#include <IFSelect_Dispatch.hxx>
#include <IFSelect_PacketList.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_ShareOutResult.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>

IFSelect_ShareOutResult::IFSelect_ShareOutResult
  (const Handle(IFSelect_ShareOut)& sho,
   const Handle(Interface_InterfaceModel)& amodel)
      : thegraph(amodel) , thedispres(amodel,Standard_False)
{
  theshareout = sho;
  theeval     = Standard_False;
//  thedisplist = new TColStd_SequenceOfInteger();
}

    IFSelect_ShareOutResult::IFSelect_ShareOutResult
  (const Handle(IFSelect_ShareOut)& sho, const Interface_Graph& G)
      : thegraph(G) , thedispres(G,Standard_False)
{
  theshareout = sho;
  theeval     = Standard_False;
//  thedisplist = new TColStd_SequenceOfInteger();
}

    IFSelect_ShareOutResult::IFSelect_ShareOutResult
  (const Handle(IFSelect_Dispatch)& disp,
   const Handle(Interface_InterfaceModel)& amodel)
      : thegraph(amodel) , thedispres(amodel,Standard_False)
{
  thedispatch = disp;
  theeval     = Standard_False;
//  thedisplist = new TColStd_SequenceOfInteger();
}

    IFSelect_ShareOutResult::IFSelect_ShareOutResult
  (const Handle(IFSelect_Dispatch)& disp, const Interface_Graph& G)
      : thegraph(G) , thedispres(G,Standard_False)
{
  thedispatch = disp;
  theeval     = Standard_False;
//  thedisplist = new TColStd_SequenceOfInteger();
}


    Handle(IFSelect_ShareOut)  IFSelect_ShareOutResult::ShareOut () const
      {  return theshareout;  }

    const Interface_Graph&  IFSelect_ShareOutResult::Graph () const
      { return thegraph;  }

    void  IFSelect_ShareOutResult::Reset ()
      {  theeval = Standard_False;  }

    void  IFSelect_ShareOutResult::Evaluate ()
{
  if (theeval) return;      // deja fait. si pas OK, faire Reset avant
  Prepare();
  theeval = Standard_True;
}


    Handle(IFSelect_PacketList)  IFSelect_ShareOutResult::Packets
  (const Standard_Boolean complete)
{
  Evaluate();
  Handle(IFSelect_PacketList) list = new IFSelect_PacketList(thegraph.Model());
  Interface_EntityIterator iter;
  for ( ; More(); Next()) {
    list->AddPacket();
    if (complete) list->AddList (PacketContent().Content());
    else          list->AddList (PacketRoot().Content());
  }
  return list;
}

    Standard_Integer  IFSelect_ShareOutResult::NbPackets () 
      {  Evaluate();  return thedispres.NbParts();  }


    void  IFSelect_ShareOutResult::Prepare ()
{
  thedisplist.Clear();
//  On alimente thedispres, thedisplist
  thedispres.Reset();
  IFGraph_AllShared A(thegraph);
  Handle(IFSelect_Dispatch) disp = thedispatch;
  Standard_Integer nb = 1, first = 1;
  if (!theshareout.IsNull()) {
    nb    = theshareout->NbDispatches();
    first = theshareout->LastRun() + 1;
  }
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = first; i <= nb; i ++) {
    if (!theshareout.IsNull()) disp = theshareout->Dispatch(i);
    if (disp->FinalSelection().IsNull()) continue;    // Dispatch neutralise
    IFGraph_SubPartsIterator packs(thegraph,Standard_False);
    disp->Packets(thegraph,packs);
    for (packs.Start(); packs.More(); packs.Next()) {
      Interface_EntityIterator iter = packs.Entities();
      if (iter.NbEntities() == 0) continue;
      thedispres.AddPart();
      thedispres.GetFromIter(iter);  // on enregistre ce paquet
      A.ResetData();
      A.GetFromIter(iter);
      thedisplist.Append(i);         // n0 du dispatch producteur
    }
  }
  thedispnum = thepacknum = 1;
  thepackdisp = 1;  // calcul sur 1er Dispatch
  thenbindisp = 0;
  for (i = thepacknum; i <= thedisplist.Length(); i ++) {
    if (thedisplist.Value(i) != thedispnum) break;
    thenbindisp ++;
  }
}

    Standard_Boolean  IFSelect_ShareOutResult::More ()
      {  return thedispres.More();  } // thepacknum < thedisplist.Length());

    void  IFSelect_ShareOutResult::Next ()
{
  thedispres.Next();
  thepacknum ++;
  Standard_Integer dispnum;
  if (thepacknum <= thedisplist.Length())
    dispnum = thedisplist.Value(thepacknum);
  else {
    thenbindisp = 0;
#if !defined No_Exception
//    cout<<" ** **  IFSelect_ShareOutResult::Next, void dispatch ignored"<<endl;
#endif
    return;
  }
  if (thedispnum == dispnum) thepackdisp ++;
  else {
    thedispnum = dispnum;
    thepackdisp = 1;
    thenbindisp = 0;
    for (Standard_Integer i = thepacknum; i <= thedisplist.Length(); i ++) {
      if (thedisplist.Value(i) != thedispnum) break;
      thenbindisp ++;
    }
    if (!theshareout.IsNull()) thedispatch = theshareout->Dispatch(thedispnum);
  }
}

    void  IFSelect_ShareOutResult::NextDispatch ()
{
  for (; thepacknum <= thedisplist.Length(); thepacknum ++) {
    thedispres.Next();
    if (thedispnum != thedisplist.Value(thepacknum)) {
      thedispnum = thedisplist.Value(thepacknum);
//  Calcul donnees propres au Dispatch
      thepackdisp = 1;
      thenbindisp = 0;
      for (Standard_Integer i = thepacknum; i <= thedisplist.Length(); i ++) {
	if (thedisplist.Value(i) != thedispnum) break;
	thenbindisp ++;
      }
      if (!theshareout.IsNull()) thedispatch = theshareout->Dispatch(thedispnum);
      return;
    }
  }
  thepacknum = thedisplist.Length() + 1;  // no next dispatch ...
  thedispnum = thepackdisp = thenbindisp = 0;
}

    Handle(IFSelect_Dispatch)  IFSelect_ShareOutResult::Dispatch () const
      {  return thedispatch;  }

    Standard_Integer  IFSelect_ShareOutResult::DispatchRank () const 
      {  return thedispnum;  }

    void IFSelect_ShareOutResult::PacketsInDispatch
  (Standard_Integer& numpack, Standard_Integer& nbpacks) const
      {  numpack = thepackdisp;  nbpacks = thenbindisp;  }

    Interface_EntityIterator  IFSelect_ShareOutResult::PacketRoot ()
      {  return thedispres.Entities();  }

    Interface_EntityIterator  IFSelect_ShareOutResult::PacketContent ()
{
//  IFGraph_Cumulate G(thegraph);
  Interface_EntityIterator iter = thedispres.Entities();
  Interface_Graph G(thegraph);
//  G.GetFromIter(thedispres.Entities(),0);
  for (iter.Start(); iter.More(); iter.Next())
    G.GetFromEntity(iter.Value(),Standard_True);
  Interface_GraphContent GC(G);
  return GC.Result();
}

    TCollection_AsciiString IFSelect_ShareOutResult::FileName () const
{
  Standard_Integer nd = DispatchRank();
  Standard_Integer np,nbp;
  PacketsInDispatch(np,nbp);
  return  theshareout->FileName(nd,np,nbp);
}
