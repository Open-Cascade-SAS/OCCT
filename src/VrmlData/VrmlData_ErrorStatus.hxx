// Created on: 2006-10-28
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef VrmlData_ErrorStatus_HeaderFile
#define VrmlData_ErrorStatus_HeaderFile

/**
   * Status of read/write or other operation.
   */
  enum VrmlData_ErrorStatus {
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
