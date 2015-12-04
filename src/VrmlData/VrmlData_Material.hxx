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

#ifndef VrmlData_Material_HeaderFile
#define VrmlData_Material_HeaderFile

#include <VrmlData_Node.hxx>
#include <Quantity_Color.hxx>

/**
 *  Implementation of the Material node
 */
class VrmlData_Material : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  Standard_EXPORT VrmlData_Material ();

  /**
   * Constructor
   */
  Standard_EXPORT VrmlData_Material
                                (const VrmlData_Scene&  theScene,
                                 const char             * theName,
                                 const Standard_Real theAmbientIntensity = -1.,
                                 const Standard_Real theShininess        = -1.,
                                 const Standard_Real theTransparency     = -1.);

  /**
   * Query the Ambient Intensity value
   */
  inline Standard_Real
                AmbientIntensity() const        { return myAmbientIntensity; }

  /**
   * Query the Shininess value
   */
  inline Standard_Real
                Shininess       () const        { return myShininess; }

  /**
   * Query the Transparency value
   */
  inline Standard_Real
                Transparency    () const        { return myTransparency; }

  /**
   * Query the Diffuse color
   */
  inline const Quantity_Color&
                DiffuseColor    () const        { return myDiffuseColor; }

  /**
   * Query the Emissive color
   */
  inline const Quantity_Color&
                EmissiveColor   () const        { return myEmissiveColor; }

  /**
   * Query the Specular color
   */
  inline const Quantity_Color&
                SpecularColor   () const        { return mySpecularColor; }

  /**
   * Set the Ambient Intensity value
   */
  inline void   SetAmbientIntensity
                                (const Standard_Real theAmbientIntensity)
  { myAmbientIntensity = theAmbientIntensity; }

  /**
   * Set the Shininess value
   */
  inline void   SetShininess    (const Standard_Real theShininess)
  { myShininess = theShininess; }

  /**
   * Set the Transparency value
   */
  inline void   SetTransparency (const Standard_Real theTransparency)
  { myTransparency = theTransparency; }

  /**
   * Query the Diffuse color
   */
  inline void   SetDiffuseColor (const Quantity_Color& theColor)
  { myDiffuseColor = theColor; }

  /**
   * Query the Emissive color
   */
  inline void   SetEmissiveColor (const Quantity_Color& theColor)
  { myEmissiveColor = theColor; }

  /**
   * Query the Specular color
   */
  inline void   SetSpecularColor (const Quantity_Color& theColor)
  { mySpecularColor = theColor; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                Clone           (const Handle(VrmlData_Node)& theOther)const Standard_OVERRIDE;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Read            (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Write           (const char * thePrefix) const Standard_OVERRIDE;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                IsDefault       () const Standard_OVERRIDE;

 protected:
  // ---------- PROTECTED METHODS ----------

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myAmbientIntensity;
  Standard_Real         myShininess;
  Standard_Real         myTransparency;
  Quantity_Color        myDiffuseColor;
  Quantity_Color        myEmissiveColor;
  Quantity_Color        mySpecularColor;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTIEXT(VrmlData_Material,VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Material, VrmlData_Node)


#endif
