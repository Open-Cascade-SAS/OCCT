// Created on: 2014-11-14
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectBasics_PickResult_HeaderFile
#define _SelectBasics_PickResult_HeaderFile

#include <gp_Pnt.hxx>

//! This structure provides unified access to the results of Matches() method in all sensitive entities,
//! so that it defines a Depth (distance to the entity along picking ray) and a closest Point on entity.
struct SelectBasics_PickResult
{
public:
  //! Return closest result between two Pick Results according to Depth value.
  static const SelectBasics_PickResult& Min (const SelectBasics_PickResult& thePickResult1,
                                             const SelectBasics_PickResult& thePickResult2)
  {
    return thePickResult1.Depth() <= thePickResult2.Depth() ? thePickResult1 : thePickResult2;
  }

public:
  //! Empty constructor defining an invalid result.
  SelectBasics_PickResult()
  : myObjPickedPnt (RealLast(), 0.0, 0.0),
    myDepth (RealLast()),
    myDistToCenter (RealLast()) {}

  //! Constructor with initialization.
  SelectBasics_PickResult (Standard_Real theDepth,
                           Standard_Real theDistToCenter,
                           const gp_Pnt& theObjPickedPnt)
  : myObjPickedPnt (theObjPickedPnt),
    myDepth (theDepth),
    myDistToCenter (theDistToCenter) {}

public:

  //! Return TRUE if result was been defined.
  Standard_Boolean IsValid() const { return myDepth != RealLast(); }

  //! Reset depth value.
  void Invalidate()
  {
    myDepth = RealLast();
    myObjPickedPnt = gp_Pnt (RealLast(), 0.0, 0.0);
    myNormal.SetValues (0.0f, 0.0f, 0.0f);
  }

  //! Return depth along picking ray.
  Standard_Real Depth() const { return myDepth; }

  //! Set depth along picking ray.
  void SetDepth (Standard_Real theDepth) { myDepth = theDepth; }

  //! Return TRUE if Picked Point lying on detected entity was set.
  Standard_Boolean HasPickedPoint() const { return myObjPickedPnt.X() != RealLast(); }

  //! Return picked point lying on detected entity.
  //! WARNING! Point is defined in local coordinate system and should be translated into World System before usage!
  const gp_Pnt& PickedPoint() const { return myObjPickedPnt; }

  //! Set picked point.
  void SetPickedPoint (const gp_Pnt& theObjPickedPnt) { myObjPickedPnt = theObjPickedPnt; }

  //! Return distance to geometry center (auxiliary value for comparing results).
  Standard_Real DistToGeomCenter() const { return myDistToCenter; }

  //! Set distance to geometry center.
  void SetDistToGeomCenter (Standard_Real theDistToCenter) { myDistToCenter = theDistToCenter; }

  //! Return (unnormalized) surface normal at picked point or zero vector if undefined.
  //! WARNING! Normal is defined in local coordinate system and should be translated into World System before usage!
  const NCollection_Vec3<float>& SurfaceNormal() const { return myNormal; }

  //! Set surface normal at picked point.
  void SetSurfaceNormal (const NCollection_Vec3<float>& theNormal) { myNormal = theNormal; }

  //! Set surface normal at picked point.
  void SetSurfaceNormal (const gp_Vec& theNormal)
  {
    myNormal.SetValues ((float )theNormal.X(), (float )theNormal.Y(), (float )theNormal.Z());
  }

private:
  gp_Pnt                  myObjPickedPnt; //!< User-picked selection point onto object
  NCollection_Vec3<float> myNormal;       //!< surface normal
  Standard_Real           myDepth;        //!< Depth to detected point
  Standard_Real           myDistToCenter; //!< Distance from 3d projection user-picked selection point to entity's geometry center
};

#endif // _SelectBasics_PickResult_HeaderFile
