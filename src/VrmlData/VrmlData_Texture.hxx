// Created on: 2006-05-25
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



#ifndef VrmlData_Texture_HeaderFile
#define VrmlData_Texture_HeaderFile

#include <VrmlData_Node.hxx>

/**
 *  Implementation of the Texture node
 */
class VrmlData_Texture : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Texture ()
    : myRepeatS (Standard_False),
      myRepeatT (Standard_False)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Texture (const VrmlData_Scene&  theScene,
                           const char             * theName,
                           const Standard_Boolean theRepeatS = Standard_False,
                           const Standard_Boolean theRepeatT = Standard_False)
    : VrmlData_Node     (theScene, theName),
      myRepeatS         (theRepeatS),
      myRepeatT         (theRepeatT)
  {}

  /**
   * Query the RepeatS value
   */
  inline Standard_Boolean
                RepeatS         () const { return myRepeatS; }

  /**
   * Query the RepeatT value
   */
  inline Standard_Boolean
                RepeatT         () const { return myRepeatT; }

  /**
   * Set the RepeatS flag
   */
  inline void   SetRepeatS      (const Standard_Boolean theFlag)
  { myRepeatS = theFlag; }

  /**
   * Set the RepeatT flag
   */
  inline void   SetRepeatT      (const Standard_Boolean theFlag)
  { myRepeatT = theFlag; }

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Boolean      myRepeatS;
  Standard_Boolean      myRepeatT;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Texture)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Texture, VrmlData_Node)


#endif
