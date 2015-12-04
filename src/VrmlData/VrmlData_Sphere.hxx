// Created on: 2006-05-26
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

#ifndef VrmlData_Sphere_HeaderFile
#define VrmlData_Sphere_HeaderFile

#include <VrmlData_Geometry.hxx>

/**
 *  Implementation of the Sphere node.
 */
class VrmlData_Sphere : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Sphere ()
    : myRadius (1.)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Sphere (const VrmlData_Scene& theScene,
                          const char            * theName,
                          const Standard_Real   theRadius = 1.)
    : VrmlData_Geometry (theScene, theName),
      myRadius          (theRadius)
  {}

  /**
   * Query the sphere radius
   */
  inline Standard_Real  Radius          () const        { return myRadius; } 

  /**
   * Set the spere radius
   */
  inline void           SetRadius       (const Standard_Real  theRadius)
  { myRadius = theRadius; SetModified(); }

  /**
   * Query the primitive topology. This method returns a Null shape if there
   * is an internal error during the primitive creation (zero radius, etc.)
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)&  TShape () Standard_OVERRIDE;

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone     (const Handle(VrmlData_Node)& theOther)const Standard_OVERRIDE;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read            (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write           (const char * thePrefix) const Standard_OVERRIDE;

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myRadius;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Sphere,VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Sphere, VrmlData_Geometry)


#endif
