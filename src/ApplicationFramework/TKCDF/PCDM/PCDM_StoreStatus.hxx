// Created on: 1997-08-01
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _PCDM_StoreStatus_HeaderFile
#define _PCDM_StoreStatus_HeaderFile

//! Status of storage of a document on disk.
//! If it is PCDM_SS_OK, the document is successfully saved on disk.
//! Else - there is an error.
enum PCDM_StoreStatus
{
  PCDM_SS_OK,                 //!< Document is saved successfully
  PCDM_SS_DriverFailure,      //!< Storage driver is not found
  PCDM_SS_WriteFailure,       //!< Attempt to write a file on disk failed
  PCDM_SS_Failure,            //!< A general error occurred (unexpected)
  PCDM_SS_Doc_IsNull,         //!< Attempt to save a null document
  PCDM_SS_No_Obj,             //!< Document has no objects to be saved
  PCDM_SS_Info_Section_Error, //!< Error occurred on writing of an information-section
  PCDM_SS_UserBreak,          //!< User interrupted the process of storage of the document on disk
  PCDM_SS_UnrecognizedFormat  //!< No storage driver exist for this document format
};

#endif // _PCDM_StoreStatus_HeaderFile
