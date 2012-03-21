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
                Clone           (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Read            (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Write           (const char * thePrefix) const;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean
                IsDefault       () const;

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
DEFINE_STANDARD_RTTI (VrmlData_Material)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Material, VrmlData_Node)


#endif
