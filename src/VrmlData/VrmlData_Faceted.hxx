// Created on: 2006-05-26
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
DEFINE_STANDARD_RTTI (VrmlData_Faceted)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (VrmlData_Faceted, VrmlData_Geometry)


#endif
