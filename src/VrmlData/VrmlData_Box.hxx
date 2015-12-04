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

#ifndef VrmlData_Box_HeaderFile
#define VrmlData_Box_HeaderFile

#include <VrmlData_Geometry.hxx>
#include <gp_XYZ.hxx>

/**
 *  Inplementation of the Box node.
 *  This node is defined by Size vector, assumong that the box center is located
 *  in (0., 0., 0.) and that each corner is 0.5*|Size| distance from the center.
 */
class VrmlData_Box : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Box           ()
    : mySize (2., 2., 2.)
  {}

  /**
   * Constructor
   */
  inline VrmlData_Box           (const VrmlData_Scene&  theScene,
                                 const char             * theName,
                                 const Standard_Real    sizeX = 2.,
                                 const Standard_Real    sizeY = 2.,
                                 const Standard_Real    sizeZ = 2.)
    : VrmlData_Geometry (theScene, theName),
      mySize            (sizeX, sizeY, sizeZ)
  {}

  /**
   * Query the Box size
   */
  inline const gp_XYZ&  Size    () const                { return mySize; }

  /**
   * Set the Box Size
   */
  inline void           SetSize (const gp_XYZ& theSize)
  { mySize = theSize; SetModified(); }

  /**
   * Query the primitive topology. This method returns a Null shape if there
   * is an internal error during the primitive creation (zero radius, etc.)
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)&
                        TShape  () Standard_OVERRIDE;

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone   (const Handle(VrmlData_Node)& theOther)const Standard_OVERRIDE;

  /**
   * Fill the Node internal data from the given input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read    (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write   (const char * thePrefix) const Standard_OVERRIDE;

 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XYZ        mySize;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Box,VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Box, VrmlData_Geometry)


#endif
