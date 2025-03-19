// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TransferBRep_Reader_HeaderFile
#define _TransferBRep_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Message_ProgressRange.hxx>

class Interface_Protocol;
class Transfer_ActorOfTransientProcess;
class Interface_InterfaceModel;
class Transfer_TransientProcess;
class Interface_CheckIterator;
class TopoDS_Shape;
class Standard_Transient;

//! This class offers a simple, easy to call, way of transferring
//! data from interface files to Shapes from CasCade
//! It must be specialized according to each norm/protocol, by :
//! - defining how to read a file (specific method with protocol)
//! - definig transfer, by providing an Actor
class TransferBRep_Reader
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes a non-specialised Reader. Typically, for each norm
  //! or protocol, is will be required to define a specific Create
  //! to load a file and transfer it
  Standard_EXPORT TransferBRep_Reader();

  //! Records the protocol to be used for read and transfer roots
  Standard_EXPORT void SetProtocol(const Handle(Interface_Protocol)& protocol);

  //! Returns the recorded Protocol
  Standard_EXPORT virtual Handle(Interface_Protocol) Protocol() const;

  //! Records the actor to be used for transfers
  Standard_EXPORT void SetActor(const Handle(Transfer_ActorOfTransientProcess)& actor);

  //! Returns the recorded Actor
  Standard_EXPORT virtual Handle(Transfer_ActorOfTransientProcess) Actor() const;

  //! Sets File Status to be interpreted as follows :
  //! = 0 OK
  //! < 0 file not found
  //! > 0 read error, no Model could be created
  Standard_EXPORT void SetFileStatus(const Standard_Integer status);

  //! Returns the File Status
  Standard_EXPORT Standard_Integer FileStatus() const;

  //! Returns True if FileStatus is for FileNotFound
  Standard_EXPORT Standard_Boolean FileNotFound() const;

  //! Returns True if FileStatus is for Error during read
  //! (major error; for local error, see CheckModel)
  Standard_EXPORT Standard_Boolean SyntaxError() const;

  //! Specifies a Model to work on
  //! Also clears the result and Done status
  Standard_EXPORT void SetModel(const Handle(Interface_InterfaceModel)& model);

  //! Returns the Model to be worked on
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;

  //! clears the result and Done status. But not the Model.
  Standard_EXPORT void Clear();

  //! Checks the Model. Returns True if there is NO FAIL at all
  //! (regardless Warnings)
  //! If <withprint> is True, also sends Checks on standard output
  Standard_EXPORT Standard_Boolean CheckStatusModel(const Standard_Boolean withprint) const;

  //! Checks the Model (complete : syntax + semantic) and returns
  //! the produced Check List
  Standard_EXPORT Interface_CheckIterator CheckListModel() const;

  //! Returns (by Reference, hence can be changed) the Mode for new
  //! Transfer : True (D) means that each new Transfer produces a
  //! new TransferProcess. Else keeps the original one but each
  //! Transfer clears its (former results are not kept)
  Standard_EXPORT Standard_Boolean& ModeNewTransfer();

  //! Initializes the Reader for a Transfer (one,roots, or list)
  //! Also calls PrepareTransfer
  //! Returns True when done, False if could not be done
  Standard_EXPORT Standard_Boolean BeginTransfer();

  //! Ebds a Transfer (one, roots or list) by recording its result
  Standard_EXPORT void EndTransfer();

  //! Prepares the Transfer. Also can act on the Actor or change the
  //! TransientProcess if required.
  //! Should not set the Actor into the TransientProcess, it is done
  //! by caller. The provided default does nothing.
  Standard_EXPORT virtual void PrepareTransfer();

  //! Transfers all Root Entities which are recognized as Geom-Topol
  //! The result will be a list of Shapes.
  //! This method calls user redefinable PrepareTransfer
  //! Remark : former result is cleared
  Standard_EXPORT virtual void TransferRoots(
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfers an Entity given its rank in the Model (Root or not)
  //! Returns True if it is recognized as Geom-Topol.
  //! (But it can have failed : see IsDone)
  Standard_EXPORT virtual Standard_Boolean Transfer(
    const Standard_Integer       num,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfers a list of Entities (only the ones also in the Model)
  //! Remark : former result is cleared
  Standard_EXPORT virtual void TransferList(
    const Handle(TColStd_HSequenceOfTransient)& list,
    const Message_ProgressRange&                theProgress = Message_ProgressRange());

  //! Returns True if the LAST Transfer/TransferRoots was a success
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the count of produced Shapes (roots)
  Standard_EXPORT Standard_Integer NbShapes() const;

  //! Returns the complete list of produced Shapes
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) Shapes() const;

  //! Returns a Shape given its rank, by default the first one
  Standard_EXPORT const TopoDS_Shape& Shape(const Standard_Integer num = 1) const;

  //! Returns a Shape produced from a given entity (if it was
  //! individually transferred or if an intermediate result is
  //! known). If no Shape is bound with <ent>, returns a Null Shape
  //! Warning : Runs on the last call to Transfer,TransferRoots,TransferList
  Standard_EXPORT TopoDS_Shape ShapeResult(const Handle(Standard_Transient)& ent) const;

  //! Returns a unique Shape for the result :
  //! - a void Shape (type = SHAPE) if result is empty
  //! - a simple Shape if result has only one : returns this one
  //! - a Compound if result has more than one Shape
  Standard_EXPORT TopoDS_Shape OneShape() const;

  //! Returns the count of produced Transient Results (roots)
  Standard_EXPORT Standard_Integer NbTransients() const;

  //! Returns the complete list of produced Transient Results
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Transients() const;

  //! Returns a Transient Root Result, given its rank (by default
  //! the first one)
  Standard_EXPORT Handle(Standard_Transient) Transient(const Standard_Integer num = 1) const;

  //! Checks the Result of last Transfer (individual or roots, no
  //! cumulation on several transfers). Returns True if NO fail
  //! occurred during Transfer (queries the TransientProcess)
  Standard_EXPORT Standard_Boolean CheckStatusResult(const Standard_Boolean withprints) const;

  //! Checks the Result of last Transfer (individual or roots, no
  //! cumulation on several transfers) and returns the produced list
  Standard_EXPORT Interface_CheckIterator CheckListResult() const;

  //! Returns the TransientProcess. It records information about
  //! the very last transfer done. Null if no transfer yet done.
  //! Can be used for queries more accurate than the default ones.
  Standard_EXPORT Handle(Transfer_TransientProcess) TransientProcess() const;

  Standard_EXPORT virtual ~TransferBRep_Reader();

protected:
  Standard_Boolean                  theDone;
  Handle(Transfer_TransientProcess) theProc;

private:
  Handle(Interface_Protocol)               theProto;
  Handle(Transfer_ActorOfTransientProcess) theActor;
  Handle(Interface_InterfaceModel)         theModel;
  Standard_Integer                         theFilest;
  Standard_Boolean                         theNewpr;
  Handle(TopTools_HSequenceOfShape)        theShapes;
  Handle(TColStd_HSequenceOfTransient)     theTransi;
};

#endif // _TransferBRep_Reader_HeaderFile
