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

#include <Graphic3d_Vec3.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveEntity.hxx>

//! This class provides data and criterion for sorting candidate
//! entities in the process of interactive selection by mouse click
class SelectMgr_SortCriterion
{
public:
  Handle(Select3D_SensitiveEntity) Entity; //!< detected entity
  gp_Pnt                           Point;  //!< 3D point
  Graphic3d_Vec3                   Normal; //!< surface normal or 0 vector if undefined
  Standard_Real                    Depth;  //!< distance from the view plane to the entity
                                           // clang-format off
  Standard_Real      MinDist;           //!< distance from the clicked point to the entity on the view plane
  Standard_Real      Tolerance;         //!< tolerance used for selecting candidates
  Standard_Integer   SelectionPriority; //!< selection priority
  Standard_Integer   DisplayPriority;   //!< display priority
  Standard_Integer   ZLayerPosition;    //!< ZLayer rendering order index, stronger than a depth
  Standard_Integer   NbOwnerMatches;    //!< overall number of entities collected for the same owner
                                           // clang-format on
  Standard_Boolean IsPreferPriority;       //!< flag to signal comparison to be done over priority

public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  SelectMgr_SortCriterion()
      : Depth(0.0),
        MinDist(0.0),
        Tolerance(0.0),
        SelectionPriority(0),
        DisplayPriority(0),
        ZLayerPosition(0),
        NbOwnerMatches(0),
        IsPreferPriority(Standard_False)
  {
  }

  //! Compare with another item by depth, priority and minDist.
  bool IsCloserDepth(const SelectMgr_SortCriterion& theOther) const
  {
    // the object within different ZLayer groups can not be compared by depth
    if (ZLayerPosition != theOther.ZLayerPosition)
    {
      return ZLayerPosition > theOther.ZLayerPosition;
    }

    // closest object is selected if their depths are not equal within tolerance
    if (Abs(Depth - theOther.Depth) > Tolerance + theOther.Tolerance)
    {
      return Depth < theOther.Depth;
    }

    Standard_Real aCos = 1.0;
    if (Normal.Modulus() > 0 && theOther.Normal.Modulus() > 0)
    {
      gp_Dir aNormal(Normal.x(), Normal.y(), Normal.z());
      gp_Dir anOtherNormal(theOther.Normal.x(), theOther.Normal.y(), theOther.Normal.z());
      aCos = Abs(Cos(aNormal.Angle(anOtherNormal)));
    }

    Standard_Real aDepth       = Depth - Tolerance;
    Standard_Real anOtherDepth = theOther.Depth - theOther.Tolerance;
    // Comparison depths taking into account tolerances occurs when the surfaces are parallel
    // or have the same sensitivity and the angle between them is less than 60 degrees.
    if (Abs(aDepth - anOtherDepth) > Precision::Confusion())
    {
      if ((aCos > 0.5 && Abs(Tolerance - theOther.Tolerance) < Precision::Confusion())
          || Abs(aCos - 1.0) < Precision::Confusion())
      {
        return aDepth < anOtherDepth;
      }
    }

    // if two objects have similar depth, select the one with higher priority
    if (SelectionPriority > theOther.SelectionPriority)
    {
      return true;
    }

    if (DisplayPriority > theOther.DisplayPriority)
    {
      return true;
    }

    // if priorities are equal, one closest to the mouse
    return SelectionPriority == theOther.SelectionPriority && MinDist < theOther.MinDist;
  }

  //! Compare with another item using old logic (OCCT version <= 6.3.1) with priority considered
  //! preceding depth.
  bool IsHigherPriority(const SelectMgr_SortCriterion& theOther) const
  {
    // the object within different ZLayer groups can not be compared by depth
    if (ZLayerPosition != theOther.ZLayerPosition)
    {
      return ZLayerPosition > theOther.ZLayerPosition;
    }

    if (SelectionPriority != theOther.SelectionPriority)
    {
      return SelectionPriority > theOther.SelectionPriority;
    }

    if (DisplayPriority != theOther.DisplayPriority)
    {
      return DisplayPriority > theOther.DisplayPriority;
    }

    // if (Abs (Depth - theOther.Depth) <= (Tolerance + theOther.Tolerance))
    if (Abs(Depth - theOther.Depth) <= Precision::Confusion())
    {
      return MinDist < theOther.MinDist;
    }

    return Depth < theOther.Depth;
  }
};

#endif // _SelectMgr_SortCriterion_HeaderFile
