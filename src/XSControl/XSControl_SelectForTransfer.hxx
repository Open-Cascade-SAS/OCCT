// Created on: 1996-03-26
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

#ifndef _XSControl_SelectForTransfer_HeaderFile
#define _XSControl_SelectForTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Integer.hxx>
class XSControl_TransferReader;
class Transfer_ActorOfTransientProcess;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class XSControl_SelectForTransfer;
DEFINE_STANDARD_HANDLE(XSControl_SelectForTransfer, IFSelect_SelectExtract)

//! This selection selects the entities which are recognised for
//! transfer by an Actor for Read : current one or another one.
//!
//! An Actor is an operator which runs transfers from interface
//! entities to objects for Imagine. It has a method to recognize
//! the entities it can process (by default, it recognises all,
//! this method can be redefined).
//!
//! A TransferReader brings an Actor, according to the currently
//! selected norm and transfer conditions.
//!
//! This selection considers, either the current Actor (brought by
//! the TransferReader, updated as required), or a precise one.
class XSControl_SelectForTransfer : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectForTransfer, non initialised
  //! it sorts nothing, unless an Actor has been defined
  Standard_EXPORT XSControl_SelectForTransfer();
  
  //! Creates a SelectForTransfer, which will work with the
  //! currently defined Actor brought by the TransferReader
  Standard_EXPORT XSControl_SelectForTransfer(const Handle(XSControl_TransferReader)& TR);
  
  //! Sets a TransferReader to sort entities : it brings the Actor,
  //! which may change, while the TransferReader does not
  Standard_EXPORT void SetReader (const Handle(XSControl_TransferReader)& TR);
  
  //! Sets a precise actor to sort entities
  //! This definition oversedes the creation with a TransferReader
  Standard_EXPORT void SetActor (const Handle(Transfer_ActorOfTransientProcess)& act);
  
  //! Returns the Actor used as precised one.
  //! Returns a Null Handle for a creation from a TransferReader
  //! without any further setting
  Standard_EXPORT Handle(Transfer_ActorOfTransientProcess) Actor() const;
  
  //! Returns the Reader (if created with a Reader)
  //! Returns a Null Handle if not created with a Reader
  Standard_EXPORT Handle(XSControl_TransferReader) Reader() const;
  
  //! Returns True for an Entity which is recognized by the Actor,
  //! either the precised one, or the one defined by TransferReader
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Recognized for Transfer [(current actor)]"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XSControl_SelectForTransfer,IFSelect_SelectExtract)

protected:




private:


  Handle(XSControl_TransferReader) theTR;
  Handle(Transfer_ActorOfTransientProcess) theAC;


};







#endif // _XSControl_SelectForTransfer_HeaderFile
