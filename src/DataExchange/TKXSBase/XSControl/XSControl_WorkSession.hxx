// Created on: 1995-06-01
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

#ifndef _XSControl_WorkSession_HeaderFile
#define _XSControl_WorkSession_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_WorkSession.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <XSControl_TransferWriter.hxx>
class XSControl_Controller;
class XSControl_TransferReader;
class XSControl_Vars;
class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Transfer_FinderProcess;
class TopoDS_Shape;
class Interface_CheckIterator;

class XSControl_WorkSession;
DEFINE_STANDARD_HANDLE(XSControl_WorkSession, IFSelect_WorkSession)

using XSControl_WorkSessionMap =
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>;

//! This WorkSession completes the basic one, by adding :
//! - use of Controller, with norm selection...
//! - management of transfers (both ways) with auxiliary classes
//! TransferReader and TransferWriter
//! -> these transfers may work with a Context List : its items
//! are given by the user, according to the transfer to be
//! i.e. it is interpreted by the Actors
//! Each item is accessed by a Name
class XSControl_WorkSession : public IFSelect_WorkSession
{
public:
  Standard_EXPORT XSControl_WorkSession();

  ~XSControl_WorkSession() { ClearBinders(); }

  //! In addition to basic ClearData, clears Transfer and Management
  //! for interactive use, for mode = 0,1,2 and over 4
  //! Plus : mode = 5 to clear Transfers (both ways) only
  //! mode = 6 to clear enforced results
  //! mode = 7 to clear transfers, results
  Standard_EXPORT virtual void ClearData(const Standard_Integer theMode) Standard_OVERRIDE;

  //! Selects a Norm defined by its name.
  //! A Norm is described and handled by a Controller
  //! Returns True if done, False if <normname> is unknown
  //!
  //! The current Profile for this Norm is taken.
  Standard_EXPORT Standard_Boolean SelectNorm(const Standard_CString theNormName);

  //! Selects a Norm defined by its Controller itself
  Standard_EXPORT void SetController(const Handle(XSControl_Controller)& theCtl);

  //! Returns the name of the last Selected Norm. If none is
  //! defined, returns an empty string
  //! By default, returns the complete name of the norm
  //! If <rsc> is True, returns the short name used for resource
  Standard_EXPORT Standard_CString
    SelectedNorm(const Standard_Boolean theRsc = Standard_False) const;

  //! Returns the norm controller itself
  const Handle(XSControl_Controller)& NormAdaptor() const { return myController; }

  //! Returns the current Context List, Null if not defined
  //! The Context is given to the TransientProcess for TransferRead
  const XSControl_WorkSessionMap& Context() const { return myContext; }

  //! Sets the current Context List, as a whole
  //! Sets it to the TransferReader
  Standard_EXPORT void SetAllContext(const XSControl_WorkSessionMap& theContext);

  //! Clears the whole current Context (nullifies it)
  Standard_EXPORT void ClearContext();

  //! Prints the transfer status of a transferred item, as being
  //! the Mapped n0 <num>, from MapWriter if <wri> is True, or
  //! from MapReader if <wri> is False
  //! Returns True when done, False else (i.e. num out of range)
  Standard_EXPORT Standard_Boolean PrintTransferStatus(const Standard_Integer theNum,
                                                       const Standard_Boolean theWri,
                                                       Standard_OStream&      theS) const;

  //! Sets a Transfer Reader, by internal ways, according mode :
  //! 0 recreates it clear,  1 clears it (does not recreate)
  //! 2 aligns Roots of TransientProcess from final Results
  //! 3 aligns final Results from Roots of TransientProcess
  //! 4 begins a new transfer (by BeginTransfer)
  //! 5 recreates TransferReader then begins a new transfer
  Standard_EXPORT void InitTransferReader(const Standard_Integer theMode);

  //! Sets a Transfer Reader, which manages transfers on reading
  Standard_EXPORT void SetTransferReader(const Handle(XSControl_TransferReader)& theTR);

  //! Returns the Transfer Reader, Null if not set
  const Handle(XSControl_TransferReader)& TransferReader() const { return myTransferReader; }

  //! Returns the TransientProcess(internal data for TransferReader)
  Standard_EXPORT Handle(Transfer_TransientProcess) MapReader() const;

  //! Changes the Map Reader, i.e. considers that the new one
  //! defines the relevant read results (forgets the former ones)
  //! Returns True when done, False in case of bad definition, i.e.
  //! if Model from TP differs from that of Session
  Standard_EXPORT Standard_Boolean SetMapReader(const Handle(Transfer_TransientProcess)& theTP);

  //! Returns the result attached to a starting entity
  //! If <mode> = 0, returns Final Result
  //! If <mode> = 1, considers Last Result
  //! If <mode> = 2, considers Final, else if absent, Last
  //! returns it as Transient, if result is not transient returns
  //! the Binder
  //! <mode> = 10,11,12 idem but returns the Binder itself
  //! (if it is not, e.g. Shape, returns the Binder)
  //! <mode> = 20, returns the ResultFromModel
  Standard_EXPORT Handle(Standard_Transient) Result(const Handle(Standard_Transient)& theEnt,
                                                    const Standard_Integer theMode) const;

  //! Commands the transfer of, either one entity, or a list
  //! I.E. calls the TransferReader after having analysed <ents>
  //! It is cumulated from the last BeginTransfer
  //! <ents> is processed by GiveList, hence :
  //! - <ents> a Selection : its SelectionResult
  //! - <ents> a HSequenceOfTransient : this list
  //! - <ents> the Model : in this specific case, all the roots,
  //! with no cumulation of former transfers (TransferReadRoots)
  Standard_EXPORT Standard_Integer
    TransferReadOne(const Handle(Standard_Transient)& theEnts,
                    const Message_ProgressRange&      theProgress = Message_ProgressRange());

  //! Commands the transfer of all the root entities of the model
  //! i.e. calls TransferRoot from the TransferReader with the Graph
  //! No cumulation with former calls to TransferReadOne
  Standard_EXPORT Standard_Integer
    TransferReadRoots(const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! produces and returns a new Model well conditioned
  //! It is produced by the Norm Controller
  //! It can be Null (if this function is not implemented)
  Standard_EXPORT Handle(Interface_InterfaceModel) NewModel();

  //! Returns the Transfer Reader, Null if not set
  const Handle(XSControl_TransferWriter)& TransferWriter() const { return myTransferWriter; }

  //! Changes the Map Reader, i.e. considers that the new one
  //! defines the relevant read results (forgets the former ones)
  //! Returns True when done, False if <FP> is Null
  Standard_Boolean SetMapWriter(const Handle(Transfer_FinderProcess)& theFP)
  {
    if (theFP.IsNull())
      return Standard_False;
    myTransferWriter->SetFinderProcess(theFP);
    return Standard_True;
  }

  //! Transfers a Shape from CasCade to a model of current norm,
  //! according to the last call to SetModeWriteShape
  //! Returns status :Done if OK, Fail if error during transfer,
  //! Error if transfer badly initialised
  Standard_EXPORT IFSelect_ReturnStatus
    TransferWriteShape(const TopoDS_Shape&          theShape,
                       const Standard_Boolean       theCompGraph = Standard_True,
                       const Message_ProgressRange& theProgress  = Message_ProgressRange());

  //! Returns the check-list of last transfer (write)
  //! It is recorded in the FinderProcess, but it must be bound with
  //! resulting entities (in the resulting file model) rather than
  //! with original objects (in fact, their mappers)
  Standard_EXPORT Interface_CheckIterator TransferWriteCheckList() const;

  const Handle(XSControl_Vars)& Vars() const { return myVars; }

  void SetVars(const Handle(XSControl_Vars)& theVars) { myVars = theVars; }

  DEFINE_STANDARD_RTTIEXT(XSControl_WorkSession, IFSelect_WorkSession)

private:
  //! Clears binders
  Standard_EXPORT void ClearBinders();

private:
  Handle(XSControl_Controller)     myController;
  Handle(XSControl_TransferReader) myTransferReader;
  Handle(XSControl_TransferWriter) myTransferWriter;
  XSControl_WorkSessionMap         myContext;
  Handle(XSControl_Vars)           myVars;
};

#endif // _XSControl_WorkSession_HeaderFile
