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
  inline VrmlData_ShapeNode() {}

  /**
   * Constructor
   */
  inline VrmlData_ShapeNode(const VrmlData_Scene& theScene, const char* theName)
      : VrmlData_Node(theScene, theName)
  {
  }

  /**
   * Query the Appearance.
   */
  inline const occ::handle<VrmlData_Appearance>& Appearance() const { return myAppearance; }

  /**
   * Query the Geometry.
   */
  inline const occ::handle<VrmlData_Geometry>& Geometry() const { return myGeometry; }

  /**
   * Set the Appearance
   */
  inline void SetAppearance(const occ::handle<VrmlData_Appearance>& theAppear)
  {
    myAppearance = theAppear;
  }

  /**
   * Set the Geometry
   */
  inline void SetGeometry(const occ::handle<VrmlData_Geometry>& theGeometry)
  {
    myGeometry = theGeometry;
  }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual occ::handle<VrmlData_Node> Clone(
    const occ::handle<VrmlData_Node>& theOther) const override;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Read(VrmlData_InBuffer& theBuffer) override;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Write(const char* thePrefix) const override;

  /**
   * Check if the Shape Node is writeable.
   */
  Standard_EXPORT virtual bool IsDefault() const override;

private:
  // ---------- PRIVATE FIELDS ----------

  occ::handle<VrmlData_Appearance> myAppearance;
  occ::handle<VrmlData_Geometry>   myGeometry;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(VrmlData_ShapeNode, VrmlData_Node)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif
