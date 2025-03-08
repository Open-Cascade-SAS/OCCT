// Created on: 2006-10-28
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_ErrorStatus_HeaderFile
#define VrmlData_ErrorStatus_HeaderFile

/**
 * Status of read/write or other operation.
 */
enum VrmlData_ErrorStatus
{
  VrmlData_StatusOK = 0,
  VrmlData_EmptyData,
  VrmlData_UnrecoverableError,
  VrmlData_GeneralError,
  VrmlData_EndOfFile,
  VrmlData_NotVrmlFile,
  VrmlData_CannotOpenFile,
  VrmlData_VrmlFormatError,
  VrmlData_NumericInputError,
  VrmlData_IrrelevantNumber,
  VrmlData_BooleanInputError,
  VrmlData_StringInputError,
  VrmlData_NodeNameUnknown,
  VrmlData_NonPositiveSize,
  VrmlData_ReadUnknownNode,
  VrmlData_NonSupportedFeature,
  VrmlData_OutputStreamUndefined,
  VrmlData_NotImplemented
};

#endif
