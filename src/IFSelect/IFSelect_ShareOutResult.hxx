// Created on: 1992-11-17
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_ShareOutResult_HeaderFile
#define _IFSelect_ShareOutResult_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Interface_Graph.hxx>
#include <IFGraph_SubPartsIterator.hxx>
class IFSelect_ShareOut;
class IFSelect_Dispatch;
class Interface_InterfaceModel;
class IFSelect_PacketList;
class Interface_EntityIterator;
class TCollection_AsciiString;

//! This class gives results computed from a ShareOut : simulation
//! before transfer, helps to list entities ...
//! Transfer itself will later be performed, either by a
//! TransferCopy to simply divide up a file, or a TransferDispatch
//! which can be parametred with more details
class IFSelect_ShareOutResult 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a ShareOutResult from a ShareOut, to work on a Model
  //! (without any more precision; uses Active Protocol)
  Standard_EXPORT IFSelect_ShareOutResult(const Handle(IFSelect_ShareOut)& sho, const Handle(Interface_InterfaceModel)& mod);
  
  //! Creates a ShareOutResult from a ShareOut, to work on a Graph
  //! already computed, which defines the Input Model and can
  //! specialize some Entities
  Standard_EXPORT IFSelect_ShareOutResult(const Handle(IFSelect_ShareOut)& sho, const Interface_Graph& G);
  
  //! Creates a ShareOutResult from a unique Dispatch, to work on
  //! a Model. As if it was a ShareOut with only one Dispatch
  //! (without any more precision; uses Active Protocol)
  //! Allows to compute the effect of a single Dispatch
  Standard_EXPORT IFSelect_ShareOutResult(const Handle(IFSelect_Dispatch)& disp, const Handle(Interface_InterfaceModel)& mod);
  
  //! Creates a ShareOutResult from a unique Dispatch, to work on
  //! a Graph. As if it was a ShareOut with only one Dispatch
  //! Allows to compute the effect of a single Dispatch
  Standard_EXPORT IFSelect_ShareOutResult(const Handle(IFSelect_Dispatch)& disp, const Interface_Graph& G);
  
  //! Returns the ShareOut used to create the ShareOutResult
  //! if creation from a Dispatch, returns a Null Handle
  Standard_EXPORT Handle(IFSelect_ShareOut) ShareOut() const;
  
  //! Returns the Graph used to create theShareOutResult
  Standard_EXPORT const Interface_Graph& Graph() const;
  
  //! Erases computed data, in order to command a new Evaluation
  Standard_EXPORT void Reset();
  
  //! Evaluates the result of a ShareOut : determines Entities to be
  //! forgotten by the ShareOut, Entities to be transferred several
  //! times (duplicated), prepares an iteration on the packets to be
  //! produced
  //! Called the first time anyone question is asked, or after a
  //! call to Reset. Works by calling the method Prepare.
  Standard_EXPORT void Evaluate();

  //! Returns the list of recorded Packets, under two modes :
  //! - <complete> = False, the strict definition of Packets, i.e.
  //! for each one, the Root Entities, to be explicitly sent
  //! - <complete> = True (Default), the completely evaluated list,
  //! i.e. which really gives the destination of each entity :
  //! this mode allows to evaluate duplications
  //! Remark that to send packets, iteration remains preferable
  //! (file names are managed)
  Standard_EXPORT Handle(IFSelect_PacketList) Packets (const Standard_Boolean complete = Standard_True);

  //! Returns the total count of produced non empty packets
  //! (in out : calls Evaluate as necessary)
  Standard_EXPORT Standard_Integer NbPackets();
  
  //! Prepares the iteration on the packets
  //! This method is called by Evaluate, but can be called anytime
  //! The iteration consists in taking each Dispatch of the ShareOut
  //! beginning by the first one, compute its packets, then iterate
  //! on these packets. Once all these packets are iterated, the
  //! iteration passes to the next Dispatch, or stops.
  //! For a creation from a unique Dispatch, same but with only
  //! this Dispatch.
  //! Each packet can be listed, or really transferred (producing
  //! a derived Model, from which a file can be generated)
  //!
  //! Prepare sets the iteration to the first Dispatch, first Packet
  Standard_EXPORT void Prepare();
  
  //! Returns True if there is more packets in the current Dispatch,
  //! else if there is more Dispatch in the ShareOut
  Standard_EXPORT Standard_Boolean More();
  
  //! Passes to the next Packet in the current Dispatch, or if there
  //! is none, to the next Dispatch in the ShareOut
  Standard_EXPORT void Next();
  
  //! Passes to the next Dispatch, regardless about remaining packets
  Standard_EXPORT void NextDispatch();
  
  //! Returns the current Dispatch
  Standard_EXPORT Handle(IFSelect_Dispatch) Dispatch() const;
  
  //! Returns the Rank of the current Dispatch in the ShareOut
  //! Returns Zero if there is none (iteration finished)
  Standard_EXPORT Standard_Integer DispatchRank() const;
  
  //! Returns Number (rank) of current Packet in current Dispatch,
  //! and total count of Packets in current Dispatch, as arguments
  Standard_EXPORT void PacketsInDispatch (Standard_Integer& numpack, Standard_Integer& nbpacks) const;
  
  //! Returns the list of Roots of the current Packet (never empty)
  //! (i.e. the Entities to be themselves asked for transfer)
  //! Error if there is none (iteration finished)
  Standard_EXPORT Interface_EntityIterator PacketRoot();
  
  //! Returns the complete content of the current Packet (i.e.
  //! with shared entities, which will also be put in the file)
  Standard_EXPORT Interface_EntityIterator PacketContent();
  
  //! Returns the File Name which corresponds to current Packet
  //! (computed by ShareOut)
  //! If current Packet has no associated name (see ShareOut),
  //! the returned value is Null
  Standard_EXPORT TCollection_AsciiString FileName() const;

protected:

  Interface_Graph thegraph;
  IFGraph_SubPartsIterator thedispres;

private:

  Handle(IFSelect_ShareOut) theshareout;
  Handle(IFSelect_Dispatch) thedispatch;
  Standard_Boolean theeval;
  Standard_Integer thedispnum;
  Standard_Integer thepacknum;
  Standard_Integer thepackdisp;
  Standard_Integer thenbindisp;
  TColStd_SequenceOfInteger thedisplist;

};

#endif // _IFSelect_ShareOutResult_HeaderFile
