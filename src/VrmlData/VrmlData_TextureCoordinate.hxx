// Created on: 2006-05-26
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



#ifndef VrmlData_TextureCoordinate_HeaderFile
#define VrmlData_TextureCoordinate_HeaderFile

#include <VrmlData_Node.hxx>
class gp_XY;

/**
 *  Implementation of the node TextureCoordinate
 */
class VrmlData_TextureCoordinate : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_TextureCoordinate ()
    : myPoints (0L), myLength (0) {}

  /**
   * Constructor
   */
  inline VrmlData_TextureCoordinate (const VrmlData_Scene& theScene,
                                     const char          * theName,
                                     const size_t          nPoints   = 0,
                                     const gp_XY         * arrPoints = 0L)
    : VrmlData_Node     (theScene, theName),
      myPoints          (arrPoints),
      myLength          (nPoints)
  {}

  /**
   * Create a data array and assign the field myArray.
   * @return
   *   True if allocation was successful.
   */ 
  Standard_EXPORT Standard_Boolean
                        AllocateValues  (const Standard_Size theLength);

  /**
   * Query the number of points
   */
  inline size_t                 Length          () { return myLength; }

  /**
   * Query the points
   */
  inline const gp_XY *          Points          () { return myPoints; }

  /**
   * Set the points array
   */
  inline void                   SetPoints       (const size_t nPoints,
                                                 const gp_XY  * arrPoints)
  { myPoints = arrPoints; myLength = nPoints; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone     (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Read            (VrmlData_InBuffer& theBuffer);

 private:
  // ---------- PRIVATE FIELDS ----------

  const gp_XY * myPoints;
  size_t        myLength;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_TextureCoordinate)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_TextureCoordinate, VrmlData_Node)


#endif
