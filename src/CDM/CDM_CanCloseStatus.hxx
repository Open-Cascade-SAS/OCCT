// Created on: 1997-05-06
// Created by: Jean-Louis Frenkel, Remi Lequette
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

#ifndef _CDM_CanCloseStatus_HeaderFile
#define _CDM_CanCloseStatus_HeaderFile


enum CDM_CanCloseStatus
{
CDM_CCS_OK,
CDM_CCS_NotOpen,
CDM_CCS_UnstoredReferenced,
CDM_CCS_ModifiedReferenced,
CDM_CCS_ReferenceRejection
};

#endif // _CDM_CanCloseStatus_HeaderFile
