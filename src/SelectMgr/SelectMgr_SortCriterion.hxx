// Created on: 1998-03-26
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_SortCriterion_HeaderFile
#define _SelectMgr_SortCriterion_HeaderFile

#include <Graphic3d_ZLayerId.hxx>
#include <Precision.hxx>
#include <SelectBasics_SensitiveEntity.hxx>

//! This class provides data and criterion for sorting candidate
//! entities in the process of interactive selection by mouse click
class SelectMgr_SortCriterion 
{
public:

  Handle(SelectBasics_SensitiveEntity) Entity; //!< detected entity
  gp_Pnt             Point;           //!< 3D point
  Standard_Real      Depth;           //!< distance from the view plane to the entity
  Standard_Real      MinDist;         //!< distance from the clicked point to the entity on the view plane
  Standard_Real      Tolerance;       //!< tolerance used for selecting candidates
  Standard_Integer   Priority;        //!< selection priority
  Standard_Integer   ZLayerPosition;  //!< ZLayer rendering order index, stronger than a depth
  Standard_Boolean   ToPreferClosest; //!< whether closest object is preferred even if has less priority

public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  SelectMgr_SortCriterion()
  : Depth    (0.0),
    MinDist  (0.0),
    Tolerance(0.0),
    Priority (0),
    ZLayerPosition  (0),
    ToPreferClosest (Standard_True) {}

  //! Comparison operator.
  bool operator> (const SelectMgr_SortCriterion& theOther) const { return IsGreater (theOther); }

  //! Comparison operator.
  bool operator< (const SelectMgr_SortCriterion& theOther) const { return IsLower   (theOther); }

  //! Compare with another item.
  bool IsGreater (const SelectMgr_SortCriterion& theOther) const
  {
    // the object within different ZLayer groups can not be compared by depth
    if (ZLayerPosition != theOther.ZLayerPosition)
    {
      return ZLayerPosition > theOther.ZLayerPosition;
    }

    if (ToPreferClosest)
    {
      // closest object is selected unless difference is within tolerance
      if (Abs (Depth - theOther.Depth) > (Tolerance + theOther.Tolerance))
      {
        return Depth < theOther.Depth;
      }

      // if two objects have similar depth, select the one with higher priority
      if (Priority > theOther.Priority)
      {
        return true;
      }

      // if priorities are equal, one closest to the mouse
      return Priority == theOther.Priority
          && MinDist  <  theOther.MinDist;
    }

    // old logic (OCCT version <= 6.3.1)
    if (Priority > theOther.Priority)
    {
      return true;
    }
    else if (Priority != theOther.Priority)
    {
      return false;
    }

    if (Abs (Depth - theOther.Depth) <= Precision::Confusion())
    {
      return MinDist < theOther.MinDist;
    }

    return Depth < theOther.Depth;
  }

  //! Compare with another item.
  bool IsLower (const SelectMgr_SortCriterion& theOther) const
  {
    // the object within different ZLayer groups can not be compared by depth
    if (ZLayerPosition != theOther.ZLayerPosition)
    {
      return ZLayerPosition < theOther.ZLayerPosition;
    }

    if (ToPreferClosest)
    {
      // closest object is selected unless difference is within tolerance
      if (ToPreferClosest
       && Abs (Depth - theOther.Depth) > (Tolerance + theOther.Tolerance))
      {
        return Depth > theOther.Depth;
      }

      // if two objects have similar depth, select the one with higher priority
      if (Priority < theOther.Priority)
      {
        return true;
      }

      // if priorities are equal, one closest to the mouse
      return Priority == theOther.Priority
          && MinDist  >  theOther.MinDist;
    }

    // old logic (OCCT version <= 6.3.1)
    if (Priority > theOther.Priority)
    {
      return false;
    }
    else if (Priority != theOther.Priority)
    {
      return true;
    }

    if (Abs (Depth - theOther.Depth) <= Precision::Confusion())
    {
      return MinDist > theOther.MinDist;
    }

    return Depth > theOther.Depth;
  }

};

#endif // _SelectMgr_SortCriterion_HeaderFile
