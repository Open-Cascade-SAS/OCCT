// File:      VrmlData_UnknownNode.hxx
// Created:   08.10.06 20:23:25
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


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
