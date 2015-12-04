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
                                Clone       (const Handle(VrmlData_Node)&)const Standard_OVERRIDE;
  /**
   * Read the node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Read        (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                                Write       (const char * thePrefix) const Standard_OVERRIDE;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                                IsDefault       () const Standard_OVERRIDE;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Handle(VrmlData_Material)             myMaterial;
  Handle(VrmlData_Texture)              myTexture;
  Handle(VrmlData_TextureTransform)     myTTransform;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Appearance,VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Appearance, VrmlData_Node)

#endif
