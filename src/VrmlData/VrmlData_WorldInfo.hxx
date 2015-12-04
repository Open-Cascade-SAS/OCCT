// Created on: 2007-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_WorldInfo_HeaderFile
#define VrmlData_WorldInfo_HeaderFile

#include <VrmlData_Node.hxx>

/**
 * Data type for WorldInfo node
 */

class VrmlData_WorldInfo : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty Constructor.
   */
  inline VrmlData_WorldInfo () : myTitle (0L) {}

  /**
   * Constructor.
   */
  Standard_EXPORT VrmlData_WorldInfo (const VrmlData_Scene&  theScene,
                                      const char             * theName = 0L,
                                      const char             * theTitle = 0L);

  /**
   * Set or modify the title.
   */
  Standard_EXPORT void SetTitle (const char * theString);

  /**
   * Add a string to the list of info strings.
   */
  Standard_EXPORT void  AddInfo (const char * theString);

  /**
   * Query the title string.
   */
  inline const char *   Title   () const
  { return myTitle; }

  /**
   * Return the iterator of Info strings.
   */
  inline NCollection_List <const char *>::Iterator
                        InfoIterator () const
  { return myInfo; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone   (const Handle(VrmlData_Node)& theOther) const Standard_OVERRIDE;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const Standard_OVERRIDE;

  /**
   * Returns True if the node is default, then it would not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                        IsDefault() const Standard_OVERRIDE;

 private:
  // ---------- PRIVATE FIELDS ----------

  const char                    * myTitle;
  NCollection_List <const char *> myInfo;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTIEXT(VrmlData_WorldInfo,VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_WorldInfo, VrmlData_Node)


#endif
