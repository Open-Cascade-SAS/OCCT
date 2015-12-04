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

#ifndef VrmlData_Coordinate_HeaderFile
#define VrmlData_Coordinate_HeaderFile

#include <VrmlData_ArrayVec3d.hxx>

/**
 *  Implementation of the node Coordinate
 */
class VrmlData_Coordinate : public VrmlData_ArrayVec3d
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty Constructor
   */
  inline VrmlData_Coordinate () {}

  /**
   * Constructor
   */
  inline VrmlData_Coordinate    (const VrmlData_Scene&  theScene,
                                 const char             * theName,
                                 const size_t           nPoints     = 0,
                                 const gp_XYZ           * arrPoints = 0L)
    : VrmlData_ArrayVec3d (theScene, theName, nPoints, arrPoints)
  {}

  /**
   * Query one point
   * @param i
   *   index in the array of points [0 .. N-1]
   * @return
   *   the coordinate for the index. If index irrelevant, returns (0., 0., 0.)
   */
  inline const gp_XYZ& Coordinate (const Standard_Integer i) const
  { return Value(i); }

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node)
                        Clone     (const Handle(VrmlData_Node)& theOther)const Standard_OVERRIDE;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Read      (VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to the Scene output.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus
                        Write     (const char * thePrefix) const Standard_OVERRIDE;

 private:
  // ---------- PRIVATE FIELDS ----------

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Coordinate,VrmlData_ArrayVec3d)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Coordinate, VrmlData_ArrayVec3d)


#endif
