// Created on: 2007-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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
                        Clone   (const Handle(VrmlData_Node)& theOther) const;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const;

  /**
   * Returns True if the node is default, then it would not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                        IsDefault() const;

 private:
  // ---------- PRIVATE FIELDS ----------

  const char                    * myTitle;
  NCollection_List <const char *> myInfo;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_WorldInfo)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_WorldInfo, VrmlData_Node)


#endif
