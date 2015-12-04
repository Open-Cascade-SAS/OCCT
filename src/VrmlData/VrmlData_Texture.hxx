// Created on: 2006-05-25
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
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Texture,VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Texture, VrmlData_Node)


#endif
