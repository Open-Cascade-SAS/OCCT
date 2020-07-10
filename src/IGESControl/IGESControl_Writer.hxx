// Created on: 1996-01-30
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

#ifndef _IGESControl_Writer_HeaderFile
#define _IGESControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESData_BasicEditor.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_OStream.hxx>
#include <Message_ProgressRange.hxx>

class Transfer_FinderProcess;
class IGESData_IGESModel;
class TopoDS_Shape;
class Standard_Transient;
class IGESData_IGESEntity;


//! This class creates and writes
//! IGES files from CAS.CADE models. An IGES file can be written to
//! an existing IGES file or to a new one.
//! The translation can be performed in one or several
//! operations. Each translation operation
//! outputs a distinct root entity in the IGES file.
//! To write an IGES file it is possible to use the following sequence:
//! To modify the IGES file header or to change translation
//! parameters it is necessary to use class Interface_Static (see
//! IGESParameters and GeneralParameters).
class IGESControl_Writer 
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Creates a writer object with the
  //! default unit (millimeters) and write mode (Face).
  //! IGESControl_Writer (const Standard_CString unit,
  //! const Standard_Integer modecr = 0);
  Standard_EXPORT IGESControl_Writer();
  
  //! Creates a writer with given
  //! values for units and for write mode.
  //! unit may be any unit that is accepted by the IGES standard.
  //! By default, it is the millimeter.
  //! modecr defines the write mode and may be:
  //! - 0: Faces (default)
  //! - 1: BRep.
  Standard_EXPORT IGESControl_Writer(const Standard_CString unit, const Standard_Integer modecr = 0);
  
  //! Creates a writer object with the
  //! prepared IGES model model in write mode.
  //! modecr defines the write mode and may be:
  //! - 0: Faces (default)
  //! - 1: BRep.
  Standard_EXPORT IGESControl_Writer(const Handle(IGESData_IGESModel)& model, const Standard_Integer modecr = 0);
  
  //! Returns the IGES model to be written in output.
  const Handle(IGESData_IGESModel) & Model() const
  { return myModel; }

  const Handle(Transfer_FinderProcess) & TransferProcess() const
  { return myTP; }

  //! Returns/Sets the TransferProcess : it contains final results
  //! and if some, check messages
  void SetTransferProcess (const Handle(Transfer_FinderProcess)& TP)
  { myTP = TP; }
  
  //! Translates a Shape to IGES Entities and adds them to the model
  //! Returns True if done, False if Shape not suitable for IGES or null
  Standard_EXPORT Standard_Boolean AddShape (const TopoDS_Shape& sh,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates a Geometry (Surface or Curve) to IGES Entities and
  //! adds them to the model
  //! Returns True if done, False if geom is neither a Surface or
  //! a Curve suitable for IGES or is null
  Standard_EXPORT Standard_Boolean AddGeom (const Handle(Standard_Transient)& geom);
  
  //! Adds an IGES entity (and the ones it references) to the model
  Standard_EXPORT Standard_Boolean AddEntity (const Handle(IGESData_IGESEntity)& ent);
  
  //! Computes the entities found in
  //! the model, which is ready to be written.
  //! This contrasts with the default computation of headers only.
  Standard_EXPORT void ComputeModel();
  
  //! Computes then writes the model to an OStream
  //! Returns True when done, false in case of error
  Standard_EXPORT Standard_Boolean Write (Standard_OStream& S, const Standard_Boolean fnes = Standard_False);
  
  //! Prepares and writes an IGES model
  //! either to an OStream, S or to a file name,CString.
  //! Returns True if the operation was performed correctly and
  //! False if an error occurred (for instance,
  //! if the processor could not create the file).
  Standard_EXPORT Standard_Boolean Write (const Standard_CString file, const Standard_Boolean fnes = Standard_False);

 private:

  Handle(Transfer_FinderProcess) myTP;
  Handle(IGESData_IGESModel) myModel;
  IGESData_BasicEditor myEditor;
  Standard_Integer myWriteMode;
  Standard_Boolean myIsComputed;
};

#endif // _IGESControl_Writer_HeaderFile
