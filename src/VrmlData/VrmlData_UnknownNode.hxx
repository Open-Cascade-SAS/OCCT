// Created on: 2006-10-08
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
                        Read            (VrmlData_InBuffer& theBuffer);

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
                        IsDefault       () const;

 private:
  // ---------- PRIVATE FIELDS ----------

  TCollection_AsciiString myTitle;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_UnknownNode)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_UnknownNode, VrmlData_Node)


#endif
