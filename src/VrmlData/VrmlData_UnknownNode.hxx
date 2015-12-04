// Created on: 2006-10-08
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

#ifndef VrmlData_UnknownNode_HeaderFile
#define VrmlData_UnknownNode_HeaderFile

#include <VrmlData_Node.hxx>
#include <TCollection_AsciiString.hxx>

/**
 * Definition of UnknownNode -- placeholder for node types that
 * are not processed now.
 */

class VrmlData_UnknownNode : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty Constructor.
   */
  inline VrmlData_UnknownNode () {}

  /**
   * Constructor.
   */
  inline VrmlData_UnknownNode           (const VrmlData_Scene& theScene,
                                         const char            * theName = 0L,
                                         const char            * theTitle= 0L)
    : VrmlData_Node     (theScene, theName)
  { if (theTitle) myTitle = (Standard_CString)theTitle; }

  /**
   * Read the unknown node, till the last closing brace of it.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read            (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Query the title of the unknown node.
   */
  inline const TCollection_AsciiString&
                        GetTitle        () const
  { return myTitle; }

  /**
   * Check if the Node is non-writeable -- always returns true.
   */
  Standard_EXPORT virtual Standard_Boolean
                        IsDefault       () const Standard_OVERRIDE;

 private:
  // ---------- PRIVATE FIELDS ----------

  TCollection_AsciiString myTitle;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_UnknownNode,VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_UnknownNode, VrmlData_Node)


#endif
