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

#ifndef VrmlData_Faceted_HeaderFile
#define VrmlData_Faceted_HeaderFile

#include <VrmlData_Geometry.hxx>

/**
 *  Common API of faceted Geometry nodes: IndexedFaceSet, ElevationGrid,
 *  Extrusion.
 */
class VrmlData_Faceted : public VrmlData_Geometry
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_Faceted ()
    : myCreaseAngle     (0.),
      myIsCCW           (Standard_True),
      myIsSolid         (Standard_True),
      myIsConvex        (Standard_True)
  {}

  /**
   * Empty constructor
   */
  inline VrmlData_Faceted (const VrmlData_Scene&  theScene,
                           const char             * theName,
                           const Standard_Boolean isCCW,
                           const Standard_Boolean isSolid,
                           const Standard_Boolean isConvex,
                           const Standard_Real    theCreaseAngle)
    : VrmlData_Geometry (theScene, theName),
      myCreaseAngle     (theCreaseAngle),
      myIsCCW           (isCCW),
      myIsSolid         (isSolid),
      myIsConvex        (isConvex)
  {}

  /**
   * Query "Is Counter-Clockwise" attribute
   */
  inline Standard_Boolean IsCCW         () const  { return myIsCCW; }

  /**
   * Query "Is Solid" attribute
   */
  inline Standard_Boolean IsSolid       () const  { return myIsSolid; }

  /**
   * Query "Is Convex" attribute
   */
  inline Standard_Boolean IsConvex      () const  { return myIsConvex; }

  /**
   * Query the Crease Angle
   */
  inline Standard_Real    CreaseAngle   () const  { return myCreaseAngle; }

  /**
   * Set "Is Counter-Clockwise" attribute
   */
  inline void             SetCCW        (const Standard_Boolean theValue)
  { myIsCCW = theValue; }

  /**
   * Set "Is Solid" attribute
   */
  inline void             SetSolid      (const Standard_Boolean theValue)
  { myIsSolid = theValue; }

  /**
   * Set "Is Convex" attribute
   */
  inline void             SetConvex     (const Standard_Boolean theValue)
  { myIsConvex = theValue; }

  /**
   * Set "Is Convex" attribute
   */
  inline void             SetCreaseAngle (const Standard_Real theValue)
  { myCreaseAngle = theValue; }

  // ---------- PROTECTED METHODS ----------
 protected:
  Standard_EXPORT VrmlData_ErrorStatus
                          readData       (VrmlData_InBuffer& theBuffer);

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myCreaseAngle;
  Standard_Boolean      myIsCCW    : 1;
  Standard_Boolean      myIsSolid  : 1;
  Standard_Boolean      myIsConvex : 1;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI_INLINE(VrmlData_Faceted,VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Faceted, VrmlData_Geometry)


#endif
