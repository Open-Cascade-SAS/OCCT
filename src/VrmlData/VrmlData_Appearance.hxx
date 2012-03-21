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



#ifndef VrmlData_Appearance_HeaderFile
#define VrmlData_Appearance_HeaderFile

#include <VrmlData_Material.hxx>
#include <VrmlData_Texture.hxx>
#include <VrmlData_TextureTransform.hxx>

/**
 *  Implementation of the Appearance node type
 */
class VrmlData_Appearance : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Appearance () {}

  /**
   * Constructor
   */
  inline VrmlData_Appearance (const VrmlData_Scene& theScene,
                              const char * theName)
    : VrmlData_Node (theScene, theName) {}

  /**
   * Query the Material
   */
  inline const Handle(VrmlData_Material)&
                Material        () const        { return myMaterial; }

  /**
   * Query the Texture
   */
  inline const Handle(VrmlData_Texture)&
                Texture         () const        { return myTexture; }

  /**
   * Query the TextureTransform
   */
  inline const Handle(VrmlData_TextureTransform)&
                TextureTransform () const       { return myTTransform; }

  /**
   * Set the Material
   */
  inline void   SetMaterial     (const Handle(VrmlData_Material)& theMat)
  { myMaterial = theMat; }

  /**
   * Set the Texture
   */
  inline void   SetTexture      (const Handle(VrmlData_Texture)& theTexture)
  { myTexture = theTexture; }

  /**
   * Set the Texture Transform
   */
  inline void   SetTextureTransform
                                (const Handle(VrmlData_TextureTransform)& theTT)
  { myTTransform = theTT; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.<p>
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                                Clone       (const Handle(VrmlData_Node)&)const;
  /**
   * Read the node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Read        (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Write       (const char * thePrefix) const;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsDefault       () const;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Handle(VrmlData_Material)             myMaterial;
  Handle(VrmlData_Texture)              myTexture;
  Handle(VrmlData_TextureTransform)     myTTransform;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_Appearance)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Appearance, VrmlData_Node)

#endif
