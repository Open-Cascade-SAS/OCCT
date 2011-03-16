// File:      VrmlData_Texture.hxx
// Created:   25.05.06 15:59:43
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2006


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
