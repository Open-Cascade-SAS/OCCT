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
  Standard_EXPORT VrmlData_Material();

  /**
   * Constructor
   */
  Standard_EXPORT VrmlData_Material(const VrmlData_Scene& theScene,
                                    const char*           theName,
                                    const double          theAmbientIntensity = -1.,
                                    const double          theShininess        = -1.,
                                    const double          theTransparency     = -1.);

  /**
   * Query the Ambient Intensity value
   */
  inline double AmbientIntensity() const { return myAmbientIntensity; }

  /**
   * Query the Shininess value
   */
  inline double Shininess() const { return myShininess; }

  /**
   * Query the Transparency value
   */
  inline double Transparency() const { return myTransparency; }

  /**
   * Query the Ambient color
   */
  inline const Quantity_Color& AmbientColor() const { return myAmbientColor; }

  /**
   * Query the Diffuse color
   */
  inline const Quantity_Color& DiffuseColor() const { return myDiffuseColor; }

  /**
   * Query the Emissive color
   */
  inline const Quantity_Color& EmissiveColor() const { return myEmissiveColor; }

  /**
   * Query the Specular color
   */
  inline const Quantity_Color& SpecularColor() const { return mySpecularColor; }

  /**
   * Set the Ambient Intensity value
   */
  inline void SetAmbientIntensity(const double theAmbientIntensity)
  {
    myAmbientIntensity = theAmbientIntensity;
  }

  /**
   * Set the Shininess value
   */
  inline void SetShininess(const double theShininess) { myShininess = theShininess; }

  /**
   * Set the Transparency value
   */
  inline void SetTransparency(const double theTransparency) { myTransparency = theTransparency; }

  /**
   * Query the Ambient color
   */
  inline void SetAmbientColor(const Quantity_Color& theColor) { myAmbientColor = theColor; }

  /**
   * Query the Diffuse color
   */
  inline void SetDiffuseColor(const Quantity_Color& theColor) { myDiffuseColor = theColor; }

  /**
   * Query the Emissive color
   */
  inline void SetEmissiveColor(const Quantity_Color& theColor) { myEmissiveColor = theColor; }

  /**
   * Query the Specular color
   */
  inline void SetSpecularColor(const Quantity_Color& theColor) { mySpecularColor = theColor; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT occ::handle<VrmlData_Node> Clone(
    const occ::handle<VrmlData_Node>& theOther) const override;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT VrmlData_ErrorStatus Read(VrmlData_InBuffer& theBuffer) override;

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT VrmlData_ErrorStatus Write(const char* thePrefix) const override;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT bool IsDefault() const override;

private:
  // ---------- PRIVATE FIELDS ----------

  double         myAmbientIntensity;
  double         myShininess;
  double         myTransparency;
  Quantity_Color myAmbientColor;
  Quantity_Color myDiffuseColor;
  Quantity_Color myEmissiveColor;
  Quantity_Color mySpecularColor;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(VrmlData_Material, VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif
