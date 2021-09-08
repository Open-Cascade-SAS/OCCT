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

#ifndef _PCDM_ReaderStatus_HeaderFile
#define _PCDM_ReaderStatus_HeaderFile

//! Status of reading of a document.
//! The following values are accessible:
//! - PCDM_RS_OK: the document was successfully read;
//! - PCDM_RS_NoDriver: driver is not found for the defined file format;
//! - PCDM_RS_UnknownFileDriver: check of the file failed (file doesn't exist, for example);
//! - PCDM_RS_OpenError: attempt to open the file failed;
//! - PCDM_RS_NoVersion: document version of the file is out of scope;
//! - PCDM_RS_NoSchema: NOT USED;
//! - PCDM_RS_NoDocument: document is empty (failed to be read correctly);
//! - PCDM_RS_ExtensionFailure: NOT USED;
//! - PCDM_RS_WrongStreamMode: file is not open for reading (a mistaken mode);
//! - PCDM_RS_FormatFailure: mistake in document data structure;
//! - PCDM_RS_TypeFailure: data type is unknown;
//! - PCDM_RS_TypeNotFoundInSchema: data type is not found in schema (STD file format);
//! - PCDM_RS_UnrecognizedFileFormat: document data structure is wrong (binary file format);
//! - PCDM_RS_MakeFailure: conversion of data from persistent to transient attributes failed (XML file format);
//! - PCDM_RS_PermissionDenied: file can't be opened because permission is denied;
//! - PCDM_RS_DriverFailure: something went wrong (a general mistake of reading of a document);
//! - PCDM_RS_AlreadyRetrievedAndModified: document is already retrieved and modified in current session;
//! - PCDM_RS_AlreadyRetrieved: document is already in current session (already retrieved);
//! - PCDM_RS_UnknownDocument: file doesn't exist on disk;
//! - PCDM_RS_WrongResource: wrong resource file (.RetrievalPlugin);
//! - PCDM_RS_ReaderException: no shape section in the document file (binary file format);
//! - PCDM_RS_NoModel: NOT USED;
//! - PCDM_RS_UserBreak: user stopped reading of the document;
enum PCDM_ReaderStatus
{
PCDM_RS_OK,                          //!< Success
PCDM_RS_NoDriver,                    //!< No driver for file format
PCDM_RS_UnknownFileDriver,           //!< File is bad
PCDM_RS_OpenError,                   //!< Can't open file
PCDM_RS_NoVersion,                   //!< Unknown document version
PCDM_RS_NoSchema,                    //!< NOT USED
PCDM_RS_NoDocument,                  //!< Document is empty
PCDM_RS_ExtensionFailure,            //!< NOT USED
PCDM_RS_WrongStreamMode,             //!< Open mode is mistaken
PCDM_RS_FormatFailure,               //!< Document data structure is wrong
PCDM_RS_TypeFailure,                 //!< Data type is unknown
PCDM_RS_TypeNotFoundInSchema,        //!< Data type is not found in schema
PCDM_RS_UnrecognizedFileFormat,      //!< Document data structure is wrong
PCDM_RS_MakeFailure,                 //!< Conversion of data failed
PCDM_RS_PermissionDenied,            //!< Permission denied to open file
PCDM_RS_DriverFailure,               //!< General mistake of reading
PCDM_RS_AlreadyRetrievedAndModified, //!< Document is already retrieved and modified
PCDM_RS_AlreadyRetrieved,            //!< Document is already retrieved
PCDM_RS_UnknownDocument,             //!< File doesn't exist
PCDM_RS_WrongResource,               //!< Wrong resource file
PCDM_RS_ReaderException,             //!< Wrong data structure
PCDM_RS_NoModel,                     //!< NOT USED
PCDM_RS_UserBreak                    //!< User interrupted reading
};

#endif // _PCDM_ReaderStatus_HeaderFile
