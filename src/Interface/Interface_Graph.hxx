// Created on: 1992-09-22
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

#ifndef _Interface_Graph_HeaderFile
#define _Interface_Graph_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_BitMap.hxx>
#include <Interface_InterfaceModel.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfListOfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

class Interface_GeneralLib;
class Interface_Protocol;
class Interface_GTool;
class Standard_Transient;
class Interface_EntityIterator;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Gives basic data structure for operating and storing
//! graph results (usage is normally internal)
//! Entities are Mapped according their Number in the Model
//!
//! Each Entity from the Model can be known as "Present" or
//! not; if it is, it is Mapped with a Status : an Integer
//! which can be used according to needs of each algorithm
//! In addition, the Graph brings a BitMap which can be used
//! by any caller
//!
//! Also, it is bound with two lists : a list of Shared
//! Entities (in fact, their Numbers in the Model) which is
//! filled by a ShareTool, and a list of Sharing Entities,
//! computed by deduction from the Shared Lists
//!
//! Moreover, it is possible to redefine the list of Entities
//! Shared by an Entity (instead of standard answer by general
//! service Shareds) : this new list can be empty; it can
//! be changed or reset (i.e. to come back to standard answer)
class Interface_Graph 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty graph, ready to receive Entities from amodel
  //! Note that this way of Creation allows <me> to verify that
  //! Entities to work with are contained in <amodel>
  //! Basic Shared and Sharing lists are obtained from a General
  //! Services Library, given directly as an argument
  Standard_EXPORT Interface_Graph(const Handle(Interface_InterfaceModel)& amodel, const Interface_GeneralLib& lib, const Standard_Boolean theModeStats = Standard_True);
  
  //! Same as above, but the Library is defined through a Protocol
  Standard_EXPORT Interface_Graph(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& protocol, const Standard_Boolean theModeStats = Standard_True);
  
  //! Same as above, but the Library is defined through a Protocol
  Standard_EXPORT Interface_Graph(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_GTool)& gtool, const Standard_Boolean theModeStats = Standard_True);
  
  //! Same a above but works with the Protocol recorded in the Model
  Standard_EXPORT Interface_Graph(const Handle(Interface_InterfaceModel)& amodel, const Standard_Boolean theModeStats = Standard_True);
  
  //! Creates a Graph from another one, getting all its data
  //! Remark that status are copied from <agraph>, but the other
  //! lists (sharing/shared) are copied only if <copied> = True
  Standard_EXPORT Interface_Graph(const Interface_Graph& agraph, const Standard_Boolean copied = Standard_False);

  //! Assignment
  Standard_EXPORT Interface_Graph& operator= (const Interface_Graph& theOther);
  
  //! Erases data, making graph ready to rebegin from void
  //! (also resets Shared lists redefinitions)
  Standard_EXPORT void Reset();
  
  //! Erases Status (Values and Flags of Presence), making graph
  //! ready to rebegin from void. Does not concerns Shared lists
  Standard_EXPORT void ResetStatus();
  
  //! Returns size (max nb of entities, i.e. Model's nb of entities)
  Standard_EXPORT Standard_Integer Size() const;
  
  //! Returns size of array of statuses
  Standard_EXPORT Standard_Integer NbStatuses() const;
  
  //! Returns the Number of the entity in the Map, computed at
  //! creation time (Entities loaded from the Model)
  //! Returns 0 if <ent> not contained by Model used to create <me>
  //! (that is, <ent> is unknown from <me>)
  Standard_EXPORT Standard_Integer EntityNumber (const Handle(Standard_Transient)& ent) const;
  
  //! Returns True if an Entity is noted as present in the graph
  //! (See methods Get... which determine this status)
  //! Returns False if <num> is out of range too
  Standard_EXPORT Standard_Boolean IsPresent (const Standard_Integer num) const;
  
  //! Same as above but directly on an Entity <ent> : if it is not
  //! contained in the Model, returns False. Else calls
  //! IsPresent(num)  with <num> given by EntityNumber
  Standard_EXPORT Standard_Boolean IsPresent (const Handle(Standard_Transient)& ent) const;
  
  //! Returns mapped Entity given its no (if it is present)
  Standard_EXPORT const Handle(Standard_Transient)& Entity (const Standard_Integer num) const;
  
  //! Returns Status associated to a numero (only to read it)
  Standard_EXPORT Standard_Integer Status (const Standard_Integer num) const;
  
  //! Modifies Status associated to a numero
  Standard_EXPORT void SetStatus (const Standard_Integer num, const Standard_Integer stat);
  
  //! Clears Entity and sets Status to 0, for a numero
  Standard_EXPORT void RemoveItem (const Standard_Integer num);
  
  //! Changes all status which value is oldstat to new value newstat
  Standard_EXPORT void ChangeStatus (const Standard_Integer oldstat, const Standard_Integer newstat);
  
  //! Removes all items of which status has a given value stat
  Standard_EXPORT void RemoveStatus (const Standard_Integer stat);
  
  //! Returns the Bit Map in order to read or edit flag values
  Standard_EXPORT const Interface_BitMap& BitMap() const;
  
  //! Returns the Bit Map in order to edit it (add new flags)
  Standard_EXPORT Interface_BitMap& CBitMap();
  
  //! Returns the Model with which this Graph was created
  Standard_EXPORT const Handle(Interface_InterfaceModel)& Model() const;
  
  //! Loads Graph with all Entities contained in the Model
  Standard_EXPORT void GetFromModel();
  
  //! Gets an Entity, plus its shared ones (at every level) if
  //! "shared" is True. New items are set to status "newstat"
  //! Items already present in graph remain unchanged
  //! Of course, redefinitions of Shared lists are taken into
  //! account if there are some
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent, const Standard_Boolean shared, const Standard_Integer newstat = 0);
  
  //! Gets an Entity, plus its shared ones (at every level) if
  //! "shared" is True. New items are set to status "newstat".
  //! Items already present in graph are processed as follows :
  //! - if they already have status "newstat", they remain unchanged
  //! - if they have another status, this one is modified :
  //! if cumul is True,  to former status + overlapstat (cumul)
  //! if cumul is False, to overlapstat (enforce)
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent, const Standard_Boolean shared, const Standard_Integer newstat, const Standard_Integer overlapstat, const Standard_Boolean cumul);
  
  //! Gets Entities given by an EntityIterator. Entities which were
  //! not yet present in the graph are mapped with status "newstat"
  //! Entities already present remain unchanged
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter, const Standard_Integer newstat);
  
  //! Gets Entities given by an EntityIterator and distinguishes
  //! those already present in the Graph :
  //! - new entities added to the Graph with status "newstst"
  //! - entities already present with status = "newstat" remain
  //! unchanged
  //! - entities already present with status different form
  //! "newstat" have their status modified :
  //! if cumul is True,  to former status + overlapstat (cumul)
  //! if cumul is False, to overlapstat (enforce)
  //! (Note : works as GetEntity, shared = False, for each entity)
  Standard_EXPORT void GetFromIter (const Interface_EntityIterator& iter, const Standard_Integer newstat, const Standard_Integer overlapstat, const Standard_Boolean cumul);
  
  //! Gets all present items from another graph
  Standard_EXPORT void GetFromGraph (const Interface_Graph& agraph);
  
  //! Gets items from another graph which have a specific Status
  Standard_EXPORT void GetFromGraph (const Interface_Graph& agraph, const Standard_Integer stat);
  
  //! Returns True if <ent> or the list of entities shared by <ent>
  //! (not redefined) contains items unknown from this Graph
  //! Remark : apart from the status HasShareError, these items
  //! are ignored
  Standard_EXPORT Standard_Boolean HasShareErrors (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the sequence of Entities Shared by an Entity
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShareds (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the list of Entities Shared by an Entity, as recorded
  //! by the Graph. That is, by default Basic Shared List, else it
  //! can be redefined by methods SetShare, SetNoShare ... see below
  Standard_EXPORT Interface_EntityIterator Shareds (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the list of Entities which Share an Entity, computed
  //! from the Basic or Redefined Shared Lists
  Standard_EXPORT Interface_EntityIterator Sharings (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the sequence of Entities Sharings by an Entity
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetSharings (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the list of sharings entities, AT ANY LEVEL, which are
  //! kind of a given type. A sharing entity kind of this type
  //! ends the exploration of its branch
  Standard_EXPORT Interface_EntityIterator TypedSharings (const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& type) const;
  
  //! Returns the Entities which are not Shared (their Sharing List
  //! is empty) in the Model
  Standard_EXPORT Interface_EntityIterator RootEntities() const;
  
  //! Determines the name attached to an entity, by using the
  //! general service Name in GeneralModule
  //! Returns a null handle if no name could be computed or if
  //! the entity is not in the model
  Standard_EXPORT Handle(TCollection_HAsciiString) Name (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Table of Sharing lists. Used to Create
  //! another Graph from <me>
  Standard_EXPORT const Handle(TColStd_HArray1OfListOfInteger)& SharingTable() const;
  
  //! Returns mode responsible for computation of statuses;
  Standard_EXPORT Standard_Boolean ModeStat() const;




protected:

  
  //! Initialize statuses and flags
  Standard_EXPORT void InitStats();


  Handle(Interface_InterfaceModel) themodel;
  TCollection_AsciiString thepresents;
  Handle(TColStd_HArray1OfInteger) thestats;
  Handle(TColStd_HArray1OfListOfInteger) thesharings;


private:

  
  //! Performs the Evaluation of the Graph, from an initial Library,
  //! either defined through a Protocol, or given dierctly
  //! Called by the non-empty Constructors
  //!
  //! Normally, gtool suffices. But if a Graph is created from a
  //! GeneralLib directly, it cannot be used
  //! If <gtool> is defined, it has priority
  Standard_EXPORT void Evaluate();


  Interface_BitMap theflags;


};







#endif // _Interface_Graph_HeaderFile
