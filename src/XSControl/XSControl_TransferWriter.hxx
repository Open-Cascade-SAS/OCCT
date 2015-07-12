// Created on: 1996-03-13
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _XSControl_TransferWriter_HeaderFile
#define _XSControl_TransferWriter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
#include <IFSelect_ReturnStatus.hxx>
class XSControl_Controller;
class Transfer_FinderProcess;
class Standard_Transient;
class Interface_InterfaceModel;
class TopoDS_Shape;
class Interface_CheckIterator;


class XSControl_TransferWriter;
DEFINE_STANDARD_HANDLE(XSControl_TransferWriter, MMgt_TShared)

//! TransferWriter gives help to control transfer to write a file
//! after having converted data from Cascade/Imagine
//!
//! It works with a Controller (which itself can work with an
//! Actor to Write) and a FinderProcess. It records results and
//! checks
class XSControl_TransferWriter : public MMgt_TShared
{

public:

  
  //! Creates a TransferWriter, empty, ready to run
  //! with an empty FinderProcess (but no controller, etc)
  Standard_EXPORT XSControl_TransferWriter();
  
  //! Returns the FinderProcess itself
  Standard_EXPORT Handle(Transfer_FinderProcess) FinderProcess() const;
  
  //! Sets a new FinderProcess and forgets the former one
  Standard_EXPORT void SetFinderProcess (const Handle(Transfer_FinderProcess)& FP);
  
  //! Returns the currently used Controller
  Standard_EXPORT Handle(XSControl_Controller) Controller() const;
  
  //! Sets a new Controller, also sets a new FinderProcess
  Standard_EXPORT void SetController (const Handle(XSControl_Controller)& ctl);
  
  //! Clears recorded data according a mode
  //! 0 clears FinderProcess (results, checks)
  //! -1 create a new FinderProcess
  Standard_EXPORT void Clear (const Standard_Integer mode);
  
  //! Returns the current Transfer Mode (an Integer)
  //! It will be interpreted by the Controller to run Transfers
  //! This call form could be later replaced by more specific ones
  //! (parameters suited for each norm / transfer case)
  Standard_EXPORT Standard_Integer TransferMode() const;
  
  //! Changes the Transfer Mode
  Standard_EXPORT void SetTransferMode (const Standard_Integer mode);
  
  //! Prints statistics on current Trace File, according what,mode
  //! See PrintStatsProcess for details
  Standard_EXPORT void PrintStats (const Standard_Integer what, const Standard_Integer mode = 0) const;
  
  //! Tells if a transient object (from an application) is a valid
  //! candidate for a transfer to a model
  //! Asks the Controller (RecognizeWriteTransient)
  //! If <obj> is a HShape, calls RecognizeShape
  Standard_EXPORT Standard_Boolean RecognizeTransient (const Handle(Standard_Transient)& obj);
  
  //! Transfers a Transient object (from an application) to a model
  //! of current norm, according to the last call to SetTransferMode
  //! Works by calling the Controller
  //! Returns status : =0 if OK, >0 if error during transfer, <0 if
  //! transfer badly initialised
  Standard_EXPORT IFSelect_ReturnStatus TransferWriteTransient (const Handle(Interface_InterfaceModel)& model, const Handle(Standard_Transient)& obj);
  
  //! Tells if a Shape is valid for a transfer to a model
  //! Asks the Controller (RecognizeWriteShape)
  Standard_EXPORT Standard_Boolean RecognizeShape (const TopoDS_Shape& shape);
  
  //! Transfers a Shape from CasCade to a model of current norm,
  //! according to the last call to SetTransferMode
  //! Works by calling the Controller
  //! Returns status : =0 if OK, >0 if error during transfer, <0 if
  //! transfer badly initialised
  Standard_EXPORT IFSelect_ReturnStatus TransferWriteShape (const Handle(Interface_InterfaceModel)& model, const TopoDS_Shape& shape);
  
  //! Returns the check-list of last transfer (write), i.e. the
  //! check-list currently recorded in the FinderProcess
  Standard_EXPORT Interface_CheckIterator CheckList() const;
  
  //! Returns the check-list of last transfer (write), but tries
  //! to bind to each check, the resulting entity in the model
  //! instead of keeping the original Mapper, whenever known
  Standard_EXPORT Interface_CheckIterator ResultCheckList (const Handle(Interface_InterfaceModel)& model) const;
  
  //! Forecast to print statitics about a FinderProcess
  Standard_EXPORT static void PrintStatsProcess (const Handle(Transfer_FinderProcess)& TP, const Standard_Integer what, const Standard_Integer mode = 0);




  DEFINE_STANDARD_RTTI(XSControl_TransferWriter,MMgt_TShared)

protected:




private:


  Handle(XSControl_Controller) theController;
  Handle(Transfer_FinderProcess) theTransferWrite;
  Standard_Integer theTransferMode;


};







#endif // _XSControl_TransferWriter_HeaderFile
