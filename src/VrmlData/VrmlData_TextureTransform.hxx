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



#ifndef VrmlData_TextureTransform_HeaderFile
#define VrmlData_TextureTransform_HeaderFile

#include <VrmlData_Node.hxx>
#include <gp_XY.hxx>

///  Implementation of the TextureTransform node

class VrmlData_TextureTransform : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_TextureTransform () {}

  /**
   * Constructor
   */
  inline VrmlData_TextureTransform (const VrmlData_Scene& theScene,
                                    const char            * theName)
    : VrmlData_Node (theScene, theName)
  {}

  /**
   * Query the Center
   */
  inline const gp_XY&   Center          () const { return myCenter; }
  
  /**
   * Query the Rotation
   */
  inline Standard_Real  Rotation        () const { return myRotation; }
  
  /**
   * Query the Scale
   */
  inline const gp_XY&   Scale           () const { return myScale; }

  /**
   * Query the Translation
   */
  inline const gp_XY&   Translation     () const { return myTranslation; }

  /**
   * Set the Center
   */
  inline void           SetCenter       (const gp_XY& V) { myCenter = V; }

  /**
   * Set the Rotation
   */
  inline void           SetRotation     (const Standard_Real V)
  { myRotation = V; }

  /**
   * Set the Scale
   */
  inline void           SetScale        (const gp_XY& V) { myScale = V; }

  /**
   * Set the Translation
   */
  inline void           SetTranslation  (const gp_XY& V) { myTranslation = V; }

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XY         myCenter;
  Standard_Real myRotation;
  gp_XY         myScale;
  gp_XY         myTranslation;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_TextureTransform)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_TextureTransform, VrmlData_Node)


#endif
