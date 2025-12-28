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
  inline VrmlData_Faceted()
      : myCreaseAngle(0.),
        myIsCCW(true),
        myIsSolid(true),
        myIsConvex(true)
  {
  }

  /**
   * Empty constructor
   */
  inline VrmlData_Faceted(const VrmlData_Scene& theScene,
                          const char*           theName,
                          const bool            isCCW,
                          const bool            isSolid,
                          const bool            isConvex,
                          const double          theCreaseAngle)
      : VrmlData_Geometry(theScene, theName),
        myCreaseAngle(theCreaseAngle),
        myIsCCW(isCCW),
        myIsSolid(isSolid),
        myIsConvex(isConvex)
  {
  }

  /**
   * Query "Is Counter-Clockwise" attribute
   */
  inline bool IsCCW() const { return myIsCCW; }

  /**
   * Query "Is Solid" attribute
   */
  inline bool IsSolid() const { return myIsSolid; }

  /**
   * Query "Is Convex" attribute
   */
  inline bool IsConvex() const { return myIsConvex; }

  /**
   * Query the Crease Angle
   */
  inline double CreaseAngle() const { return myCreaseAngle; }

  /**
   * Set "Is Counter-Clockwise" attribute
   */
  inline void SetCCW(const bool theValue) { myIsCCW = theValue; }

  /**
   * Set "Is Solid" attribute
   */
  inline void SetSolid(const bool theValue) { myIsSolid = theValue; }

  /**
   * Set "Is Convex" attribute
   */
  inline void SetConvex(const bool theValue) { myIsConvex = theValue; }

  /**
   * Set "Is Convex" attribute
   */
  inline void SetCreaseAngle(const double theValue) { myCreaseAngle = theValue; }

  // ---------- PROTECTED METHODS ----------
protected:
  Standard_EXPORT VrmlData_ErrorStatus readData(VrmlData_InBuffer& theBuffer);

private:
  // ---------- PRIVATE FIELDS ----------

  double myCreaseAngle;
  bool   myIsCCW : 1;
  bool   myIsSolid : 1;
  bool   myIsConvex : 1;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(VrmlData_Faceted, VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif
