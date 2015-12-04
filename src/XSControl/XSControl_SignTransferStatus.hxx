// Created on: 1998-07-31
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _XSControl_SignTransferStatus_HeaderFile
#define _XSControl_SignTransferStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class XSControl_TransferReader;
class Transfer_TransientProcess;
class Standard_Transient;
class Interface_InterfaceModel;


class XSControl_SignTransferStatus;
DEFINE_STANDARD_HANDLE(XSControl_SignTransferStatus, IFSelect_Signature)

//! This Signatures gives the Transfer Status of an entity, as
//! recorded in a TransferProcess. It can be :
//! - Void : not recorded, or recorded as void with no message
//! (attributes are not taken into account)
//! - Warning : no result, warning message(s), no fail
//! - Fail : no result, fail messages (with or without warning)
//! - Result.. : result, no message (neither warning nor fail)
//! Result.. i.e. Result:TypeName of the result
//! - Result../Warning : result, with warning but no fail
//! - Result../Fail : result, with fail (.e. bad result)
//! - Fail on run : no result yet recorded, no message, but
//! an exception occurred while recording the result
//! (this should not appear and indicates a programming error)
class XSControl_SignTransferStatus : public IFSelect_Signature
{

public:

  
  //! Creates a SignTransferStatus, not initialised
  //! it gives nothing (empty string)
  Standard_EXPORT XSControl_SignTransferStatus();
  
  //! Creates a SignTransferStatus, which will work on the current
  //! TransientProcess brought by the TransferReader (its MapReader)
  Standard_EXPORT XSControl_SignTransferStatus(const Handle(XSControl_TransferReader)& TR);
  
  //! Sets a TransferReader to work
  Standard_EXPORT void SetReader (const Handle(XSControl_TransferReader)& TR);
  
  //! Sets a precise map to sign entities
  //! This definition oversedes the creation with a TransferReader
  Standard_EXPORT void SetMap (const Handle(Transfer_TransientProcess)& TP);
  
  //! Returns the TransientProcess used as precised one
  //! Returns a Null Handle for a creation from a TransferReader
  //! without any further setting
  Standard_EXPORT Handle(Transfer_TransientProcess) Map() const;
  
  //! Returns the Reader (if created with a Reader)
  //! Returns a Null Handle if not created with a Reader
  Standard_EXPORT Handle(XSControl_TransferReader) Reader() const;
  
  //! Returns the Signature for a Transient object, as its transfer
  //! status
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XSControl_SignTransferStatus,IFSelect_Signature)

protected:




private:


  Handle(XSControl_TransferReader) theTR;
  Handle(Transfer_TransientProcess) theTP;


};







#endif // _XSControl_SignTransferStatus_HeaderFile
