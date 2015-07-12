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


enum PCDM_ReaderStatus
{
PCDM_RS_OK,
PCDM_RS_NoDriver,
PCDM_RS_UnknownFileDriver,
PCDM_RS_OpenError,
PCDM_RS_NoVersion,
PCDM_RS_NoSchema,
PCDM_RS_NoDocument,
PCDM_RS_ExtensionFailure,
PCDM_RS_WrongStreamMode,
PCDM_RS_FormatFailure,
PCDM_RS_TypeFailure,
PCDM_RS_TypeNotFoundInSchema,
PCDM_RS_UnrecognizedFileFormat,
PCDM_RS_MakeFailure,
PCDM_RS_PermissionDenied,
PCDM_RS_DriverFailure,
PCDM_RS_AlreadyRetrievedAndModified,
PCDM_RS_AlreadyRetrieved,
PCDM_RS_UnknownDocument,
PCDM_RS_WrongResource,
PCDM_RS_ReaderException,
PCDM_RS_NoModel
};

#endif // _PCDM_ReaderStatus_HeaderFile
