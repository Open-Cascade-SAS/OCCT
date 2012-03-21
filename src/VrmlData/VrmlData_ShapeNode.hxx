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



#ifndef VrmlData_ShapeNode_HeaderFile
#define VrmlData_ShapeNode_HeaderFile

#include <VrmlData_Appearance.hxx>
#include <VrmlData_Geometry.hxx>

/**
 *  Implementation of the Shape node type
 */
class VrmlData_ShapeNode : public VrmlData_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline        VrmlData_ShapeNode () {}

  /**
   * Constructor
   */
  inline        VrmlData_ShapeNode (const VrmlData_Scene& theScene,
                                    const char            * theName)
    : VrmlData_Node (theScene, theName) {}

  /**
   * Query the Appearance.
   */
  inline const Handle(VrmlData_Appearance)&
                Appearance      () const        { return myAppearance; }

  /**
   * Query the Geometry.
   */
  inline const Handle(VrmlData_Geometry)&
                Geometry        () const        { return myGeometry; }

  /**
   * Set the Appearance
   */
  inline void   SetAppearance   (const Handle(VrmlData_Appearance)& theAppear)
  { myAppearance = theAppear; }

  /**
   * Set the Geometry
   */
  inline void   SetGeometry     (const Handle(VrmlData_Geometry)& theGeometry)
  { myGeometry = theGeometry; }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                Clone           (const Handle(VrmlData_Node)& theOther)const;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Read            (VrmlData_InBuffer& theBuffer);

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                Write           (const char * thePrefix) const;

  /**
   * Check if the Shape Node is writeable.
   */
  Standard_EXPORT virtual Standard_Boolean
                IsDefault       () const;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Handle(VrmlData_Appearance)   myAppearance;
  Handle(VrmlData_Geometry)     myGeometry;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (VrmlData_ShapeNode)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_ShapeNode, VrmlData_Node)


#endif
