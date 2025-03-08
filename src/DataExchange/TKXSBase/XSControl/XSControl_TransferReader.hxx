// Created on: 1995-12-05
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _XSControl_TransferReader_HeaderFile
#define _XSControl_TransferReader_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_DataMapOfIntegerTransient.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Interface_CheckStatus.hxx>
#include <Message_ProgressRange.hxx>

class XSControl_Controller;
class Interface_InterfaceModel;
class Interface_HGraph;
class Transfer_ActorOfTransientProcess;
class Transfer_TransientProcess;
class Transfer_ResultFromModel;
class TopoDS_Shape;
class Interface_CheckIterator;
class Interface_Graph;

class XSControl_TransferReader;
DEFINE_STANDARD_HANDLE(XSControl_TransferReader, Standard_Transient)

//! A TransferReader performs, manages, handles results of,
//! transfers done when reading a file (i.e. from entities of an
//! InterfaceModel, to objects for Imagine)
//!
//! Running is organised around basic tools : TransientProcess and
//! its Actor, results are Binders and CheckIterators. It implies
//! control by a Controller (which prepares the Actor as required)
//!
//! Getting results can be done directly on TransientProcess, but
//! these are immediate "last produced" results. Each transfer of
//! an entity gives a final result, but also possible intermediate
//! data, and checks, which can be attached to sub-entities.
//!
//! Hence, final results (which intermediates and checks) are
//! recorded as ResultFromModel and can be queried individually.
//!
//! Some more direct access are given for results which are
//! Transient or Shapes
class XSControl_TransferReader : public Standard_Transient
{
public:
  //! Creates a TransferReader, empty
  XSControl_TransferReader() {}

  //! Sets a Controller. It is required to generate the Actor.
  //! Elsewhere, the Actor must be provided directly
  Standard_EXPORT void SetController(const Handle(XSControl_Controller)& theControl);

  //! Sets the Actor directly : this value will be used if the
  //! Controller is not set
  void SetActor(const Handle(Transfer_ActorOfTransientProcess)& theActor) { myActor = theActor; }

  //! Returns the Actor, determined by the Controller, or if this
  //! one is unknown, directly set.
  //! Once it has been defined, it can then be edited.
  Standard_EXPORT Handle(Transfer_ActorOfTransientProcess) Actor();

  //! Sets an InterfaceModel. This causes former results, computed
  //! from another one, to be lost (see also Clear)
  Standard_EXPORT void SetModel(const Handle(Interface_InterfaceModel)& theModel);

  //! Sets a Graph and its InterfaceModel (calls SetModel)
  Standard_EXPORT void SetGraph(const Handle(Interface_HGraph)& theGraph);

  //! Returns the currently set InterfaceModel
  const Handle(Interface_InterfaceModel)& Model() const { return myModel; }

  //! Sets a Context : according to receiving appli, to be
  //! interpreted by the Actor
  Standard_EXPORT void SetContext(const Standard_CString            theName,
                                  const Handle(Standard_Transient)& theCtx);

  //! Returns the Context attached to a name, if set and if it is
  //! Kind of the type, else a Null Handle
  //! Returns True if OK, False if no Context
  Standard_EXPORT Standard_Boolean GetContext(const Standard_CString       theName,
                                              const Handle(Standard_Type)& theType,
                                              Handle(Standard_Transient)&  theCtx) const;

  //! Returns (modifiable) the whole definition of Context
  //! Rather for internal use (ex.: preparing and setting in once)
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& Context()
  {
    return myContext;
  }

  //! Sets a new value for (loaded) file name
  void SetFileName(const Standard_CString theName) { myFileName = theName; }

  //! Returns actual value of file name
  Standard_CString FileName() const { return myFileName.ToCString(); }

  //! Clears data, according mode :
  //! -1 all
  //! 0 nothing done
  //! +1 final results
  //! +2 working data (model, context, transfer process)
  Standard_EXPORT void Clear(const Standard_Integer theMode);

  //! Returns the currently used TransientProcess
  //! It is computed from the model by TransferReadRoots, or by
  //! BeginTransferRead
  const Handle(Transfer_TransientProcess)& TransientProcess() const { return myTP; }

  //! Forces the TransientProcess
  //! Remark : it also changes the Model and the Actor, from those
  //! recorded in the new TransientProcess
  void SetTransientProcess(const Handle(Transfer_TransientProcess)& theTP) { myTP = theTP; }

  //! Records a final result of transferring an entity
  //! This result is recorded as a ResultFromModel, taken from
  //! the TransientProcess
  //! Returns True if a result is available, False else
  Standard_EXPORT Standard_Boolean RecordResult(const Handle(Standard_Transient)& theEnt);

  //! Returns True if a final result is recorded for an entity
  //! Remark that it can bring no effective result if transfer has
  //! completely failed (FinalResult brings only fail messages ...)
  Standard_EXPORT Standard_Boolean IsRecorded(const Handle(Standard_Transient)& theEnt) const;

  //! Returns True if a final result is recorded AND BRINGS AN
  //! EFFECTIVE RESULT (else, it brings only fail messages)
  Standard_EXPORT Standard_Boolean HasResult(const Handle(Standard_Transient)& theEnt) const;

  //! Returns the list of entities to which a final result is
  //! attached (i.e. processed by RecordResult)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) RecordedList() const;

  //! Note that an entity has been required for transfer but no
  //! result at all is available (typically : case not implemented)
  //! It is not an error, but it gives a specific status : Skipped
  //! Returns True if done, False if <ent> is not in starting model
  Standard_EXPORT Standard_Boolean Skip(const Handle(Standard_Transient)& theEnt);

  //! Returns True if an entity is noted as skipped
  Standard_EXPORT Standard_Boolean IsSkipped(const Handle(Standard_Transient)& theEnt) const;

  //! Returns True if an entity has been asked for transfert, hence
  //! it is marked, as : Recorded (a computation has ran, with or
  //! without an effective result), or Skipped (case ignored)
  Standard_EXPORT Standard_Boolean IsMarked(const Handle(Standard_Transient)& theEnt) const;

  //! Returns the final result recorded for an entity, as such
  Standard_EXPORT Handle(Transfer_ResultFromModel) FinalResult(
    const Handle(Standard_Transient)& theEnt) const;

  //! Returns the label attached to an entity recorded for final,
  //! or an empty string if not recorded
  Standard_EXPORT Standard_CString FinalEntityLabel(const Handle(Standard_Transient)& theEnt) const;

  //! Returns the number attached to the entity recorded for final,
  //! or zero if not recorded (looks in the ResultFromModel)
  Standard_EXPORT Standard_Integer
    FinalEntityNumber(const Handle(Standard_Transient)& theEnt) const;

  //! Returns the final result recorded for a NUMBER of entity
  //! (internal use). Null if out of range
  Standard_EXPORT Handle(Transfer_ResultFromModel) ResultFromNumber(
    const Standard_Integer theNum) const;

  //! Returns the resulting object as a Transient
  //! Null Handle if no result or result not transient
  Standard_EXPORT Handle(Standard_Transient) TransientResult(
    const Handle(Standard_Transient)& theEnt) const;

  //! Returns the resulting object as a Shape
  //! Null Shape if no result or result not a shape
  Standard_EXPORT TopoDS_Shape ShapeResult(const Handle(Standard_Transient)& theEnt) const;

  //! Clears recorded result for an entity, according mode
  //! <mode> = -1 : true, complete, clearing (erasing result)
  //! <mode> >= 0 : simple "stripping", see ResultFromModel,
  //! in particular, 0 for simple internal strip,
  //! 10 for all but final result,
  //! 11 for all : just label, status and filename are kept
  //! Returns True when done, False if nothing was to clear
  Standard_EXPORT Standard_Boolean ClearResult(const Handle(Standard_Transient)& theEnt,
                                               const Standard_Integer            theMode);

  //! Returns an entity from which a given result was produced.
  //! If <mode> = 0 (D), searches in last root transfers
  //! If <mode> = 1,     searches in last (root & sub) transfers
  //! If <mode> = 2,     searches in root recorded results
  //! If <mode> = 3,     searches in all (root & sub) recordeds
  //! <res> can be, either a transient object (result itself) or
  //! a binder. For a binder of shape, calls EntityFromShapeResult
  //! Returns a Null Handle if <res> not recorded
  Standard_EXPORT Handle(Standard_Transient) EntityFromResult(
    const Handle(Standard_Transient)& theRes,
    const Standard_Integer            theMode = 0) const;

  //! Returns an entity from which a given shape result was produced
  //! Returns a Null Handle if <res> not recorded or not a Shape
  Standard_EXPORT Handle(Standard_Transient) EntityFromShapeResult(
    const TopoDS_Shape&    theRes,
    const Standard_Integer theMode = 0) const;

  //! Returns the list of entities from which some shapes were
  //! produced : it corresponds to a loop on EntityFromShapeResult,
  //! but is optimised
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) EntitiesFromShapeList(
    const Handle(TopTools_HSequenceOfShape)& theRes,
    const Standard_Integer                   theMode = 0) const;

  //! Returns the CheckList resulting from transferring <ent>, i.e.
  //! stored in its recorded form ResultFromModel
  //! (empty if transfer successful or not recorded ...)
  //!
  //! If <ent> is the Model, returns the complete cumulated
  //! check-list, <level> is ignored
  //!
  //! If <ent> is an entity of the Model, <level> applies as follows
  //! <level> : -1 for <ent> only, LAST transfer (TransientProcess)
  //! <level> : 0  for <ent> only (D)
  //! 1  for <ent> and its immediate subtransfers, if any
  //! 2  for <ent> and subtransferts at all levels
  Standard_EXPORT Interface_CheckIterator CheckList(const Handle(Standard_Transient)& theEnt,
                                                    const Standard_Integer theLevel = 0) const;

  //! Returns True if an entity (with a final result) has checks :
  //! - failsonly = False : any kind of check message
  //! - failsonly = True  : fails only
  //! Returns False if <ent> is not recorded
  Standard_EXPORT Standard_Boolean HasChecks(const Handle(Standard_Transient)& theEnt,
                                             const Standard_Boolean            FailsOnly) const;

  //! Returns the list of starting entities to which a given check
  //! status is attached, IN FINAL RESULTS
  //! <ent> can be an entity, or the model to query all entities
  //! Below, "entities" are, either <ent> plus its sub-transferred,
  //! or all the entities of the model
  //!
  //! <check> = -2 , all entities whatever the check (see result)
  //! <check> = -1 , entities with no fail (warning allowed)
  //! <check> =  0 , entities with no check at all
  //! <check> =  1 , entities with warning but no fail
  //! <check> =  2 , entities with fail
  //! <result> : if True, only entities with an attached result
  //! Remark : result True and check=0 will give an empty list
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) CheckedList(
    const Handle(Standard_Transient)& theEnt,
    const Interface_CheckStatus       WithCheck = Interface_CheckAny,
    const Standard_Boolean            theResult = Standard_True) const;

  //! Defines a new TransferProcess for reading transfer
  //! Returns True if done, False if data are not properly defined
  //! (the Model, the Actor for Read)
  Standard_EXPORT Standard_Boolean BeginTransfer();

  //! Tells if an entity is recognized as a valid candidate for
  //! Transfer. Calls method Recognize from the Actor (if known)
  Standard_EXPORT Standard_Boolean Recognize(const Handle(Standard_Transient)& theEnt);

  //! Commands the transfer on reading for an entity to data for
  //! Imagine, using the selected Actor for Read
  //! Returns count of transferred entities, ok or with fails (0/1)
  //! If <rec> is True (D), the result is recorded by RecordResult
  Standard_EXPORT Standard_Integer
    TransferOne(const Handle(Standard_Transient)& theEnt,
                const Standard_Boolean            theRec      = Standard_True,
                const Message_ProgressRange&      theProgress = Message_ProgressRange());

  //! Commands the transfer on reading for a list of entities to
  //! data for Imagine, using the selected Actor for Read
  //! Returns count of transferred entities, ok or with fails (0/1)
  //! If <rec> is True (D), the results are recorded by RecordResult
  Standard_EXPORT Standard_Integer
    TransferList(const Handle(TColStd_HSequenceOfTransient)& theList,
                 const Standard_Boolean                      theRec      = Standard_True,
                 const Message_ProgressRange&                theProgress = Message_ProgressRange());

  //! Transfers the content of the current Interface Model to
  //! data handled by Imagine, starting from its Roots (determined
  //! by the Graph <G>),  using the selected Actor for Read
  //! Returns the count of performed root transfers (i.e. 0 if none)
  //! or -1 if no actor is defined
  Standard_EXPORT Standard_Integer
    TransferRoots(const Interface_Graph&       theGraph,
                  const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Clears the results attached to an entity
  //! if <ents> equates the starting model, clears all results
  Standard_EXPORT void TransferClear(const Handle(Standard_Transient)& theEnt,
                                     const Standard_Integer            theLevel = 0);

  //! Prints statistics on current Trace File, according <what> and
  //! <mode>.  See PrintStatsProcess for details
  Standard_EXPORT void PrintStats(Standard_OStream&      theStream,
                                  const Standard_Integer theWhat,
                                  const Standard_Integer theMode = 0) const;

  //! Returns the CheckList resulting from last TransferRead
  //! i.e. from TransientProcess itself, recorded from last Clear
  Standard_EXPORT Interface_CheckIterator LastCheckList() const;

  //! Returns the list of entities recorded as lastly transferred
  //! i.e. from TransientProcess itself, recorded from last Clear
  //! If <roots> is True , considers only roots of transfer
  //! If <roots> is False, considers all entities bound with result
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) LastTransferList(
    const Standard_Boolean theRoots) const;

  //! Returns a list of result Shapes
  //! If <rec> is True , sees RecordedList
  //! If <rec> is False, sees LastTransferList (last ROOT transfers)
  //! For each one, if it is a Shape, it is cumulated to the list
  //! If no Shape is found, returns an empty Sequence
  Standard_EXPORT const Handle(TopTools_HSequenceOfShape)& ShapeResultList(
    const Standard_Boolean theRec);

  //! This routines prints statistics about a TransientProcess
  //! It can be called, by a TransferReader, or isolately
  //! Prints are done on the default trace file
  //! <what> defines what kind of statistics are to be printed :
  //! 0 : basic figures
  //! 1 : root results
  //! 2 : all recorded (roots, intermediate, checked entities)
  //! 3 : abnormal records
  //! 4 : check messages (warnings and fails)
  //! 5 : fail messages
  //!
  //! <mode> is used according <what> :
  //! <what> = 0 : <mode> is ignored
  //! <what> = 1,2,3 : <mode> as follows :
  //! 0 (D) : just lists numbers of concerned entities in the model
  //! 1 : for each entity, gives number,label, type and result
  //! type and/or status (fail/warning...)
  //! 2 : for each entity, gives maximal information (i.e. checks)
  //! 3 : counts per type of starting entity (class type)
  //! 4 : counts per result type and/or status
  //! 5 : counts per couple (starting type / result type/status)
  //! 6 : idem plus gives for each item, the list of numbers of
  //! entities in the starting model
  //!
  //! <what> = 4,5 : modes relays on an enum PrintCount :
  //! 0 (D) : ItemsByEntity (sequential list by entity)
  //! 1 : CountByItem
  //! 2 : ShortByItem       (count + 5 first numbers)
  //! 3 : ListByItem        (count + entity numbers)
  //! 4 : EntitiesByItem    (count + entity numbers and labels)
  Standard_EXPORT static void PrintStatsProcess(const Handle(Transfer_TransientProcess)& theTP,
                                                const Standard_Integer                   theWhat,
                                                const Standard_Integer theMode = 0);

  //! Works as PrintStatsProcess, but displays data only on the
  //! entities which are in <list> (filter)
  Standard_EXPORT static void PrintStatsOnList(const Handle(Transfer_TransientProcess)&    theTP,
                                               const Handle(TColStd_HSequenceOfTransient)& theList,
                                               const Standard_Integer                      theWhat,
                                               const Standard_Integer theMode = 0);

  DEFINE_STANDARD_RTTIEXT(XSControl_TransferReader, Standard_Transient)

private:
  Handle(XSControl_Controller)                                             myController;
  TCollection_AsciiString                                                  myFileName;
  Handle(Interface_InterfaceModel)                                         myModel;
  Handle(Interface_HGraph)                                                 myGraph;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> myContext;
  Handle(Transfer_ActorOfTransientProcess)                                 myActor;
  Handle(Transfer_TransientProcess)                                        myTP;
  TColStd_DataMapOfIntegerTransient                                        myResults;
  Handle(TopTools_HSequenceOfShape)                                        myShapeResult;
};

#endif // _XSControl_TransferReader_HeaderFile
