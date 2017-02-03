// Created on: 1993-10-25
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Geom2dHatch_Hatcher_HeaderFile
#define _Geom2dHatch_Hatcher_HeaderFile

#include <Standard.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dHatch_Elements.hxx>
#include <Geom2dHatch_Hatchings.hxx>
#include <TopAbs_Orientation.hxx>
#include <HatchGen_ErrorStatus.hxx>

class HatchGen_PointOnHatching;
class HatchGen_Domain;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class Geom2dHatch_Hatcher 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns an empty hatcher.
  Standard_EXPORT Geom2dHatch_Hatcher(const Geom2dHatch_Intersector& Intersector, const Standard_Real Confusion2d, const Standard_Real Confusion3d, const Standard_Boolean KeepPnt = Standard_False, const Standard_Boolean KeepSeg = Standard_False);
  
  //! Sets the associated intersector.
  Standard_EXPORT void Intersector (const Geom2dHatch_Intersector& Intersector);
  
  //! Returns the associated intersector.
    const Geom2dHatch_Intersector& Intersector();
  
  //! Returns the associated intersector.
    Geom2dHatch_Intersector& ChangeIntersector();
  
  //! Sets the confusion tolerance.
  Standard_EXPORT void Confusion2d (const Standard_Real Confusion);
  
  //! Returns the 2d confusion tolerance, i.e. the value under
  //! which two points are considered identical in the
  //! parametric space of the hatching.
    Standard_Real Confusion2d() const;
  
  //! Sets the confusion tolerance.
  Standard_EXPORT void Confusion3d (const Standard_Real Confusion);
  
  //! Returns the 3d confusion tolerance, i.e. the value under
  //! which two points are considered identical in the
  //! 3d space of the hatching.
    Standard_Real Confusion3d() const;
  
  //! Sets the above flag.
  Standard_EXPORT void KeepPoints (const Standard_Boolean Keep);
  
  //! Returns the flag about the points consideration.
    Standard_Boolean KeepPoints() const;
  
  //! Sets the above flag.
  Standard_EXPORT void KeepSegments (const Standard_Boolean Keep);
  
  //! Returns the flag about the segments consideration.
    Standard_Boolean KeepSegments() const;
  
  //! Removes all the hatchings and all the elements.
    void Clear();
  
  //! Returns the curve associated to the IndE-th element.
    const Geom2dAdaptor_Curve& ElementCurve (const Standard_Integer IndE) const;
  
  //! Adds an element to the hatcher and returns its index.
  Standard_EXPORT Standard_Integer AddElement (const Geom2dAdaptor_Curve& Curve, const TopAbs_Orientation Orientation = TopAbs_FORWARD);
  
  //! Adds an element to the hatcher and returns its index.
  Standard_Integer AddElement (const Handle(Geom2d_Curve)& Curve, const TopAbs_Orientation Orientation = TopAbs_FORWARD)
  {
    Geom2dAdaptor_Curve aGAC (Curve);
    return AddElement (aGAC, Orientation);
  }

  //! Removes the IndE-th element from the hatcher.
  Standard_EXPORT void RemElement (const Standard_Integer IndE);
  
  //! Removes all the elements from the hatcher.
  Standard_EXPORT void ClrElements();
  
  //! Returns the curve associated to the IndH-th hatching.
    const Geom2dAdaptor_Curve& HatchingCurve (const Standard_Integer IndH) const;
  
  //! Adds a hatching to the hatcher and returns its index.
  Standard_EXPORT Standard_Integer AddHatching (const Geom2dAdaptor_Curve& Curve);
  
  //! Removes the IndH-th hatching from the hatcher.
  Standard_EXPORT void RemHatching (const Standard_Integer IndH);
  
  //! Removes all the hatchings from the hatcher.
  Standard_EXPORT void ClrHatchings();
  
  //! Returns the number of intersection points of
  //! the IndH-th hatching.
    Standard_Integer NbPoints (const Standard_Integer IndH) const;
  
  //! Returns the IndP-th intersection point of the
  //! IndH-th hatching.
    const HatchGen_PointOnHatching& Point (const Standard_Integer IndH, const Standard_Integer IndP) const;
  
  //! Trims all the hatchings of the hatcher by all the
  //! elements of the hatcher.
  Standard_EXPORT void Trim();
  
  //! Adds a hatching to the hatcher and trims it by
  //! the elements already given and returns its index.
  Standard_EXPORT Standard_Integer Trim (const Geom2dAdaptor_Curve& Curve);
  
  //! Trims the IndH-th hatching by the elements
  //! already given.
  Standard_EXPORT void Trim (const Standard_Integer IndH);
  
  //! Computes the domains of all the hatchings.
  Standard_EXPORT void ComputeDomains();
  
  //! Computes the domains of the IndH-th hatching.
  Standard_EXPORT void ComputeDomains (const Standard_Integer IndH);
  
  //! Returns the fact that the intersections were computed
  //! for the IndH-th hatching.
    Standard_Boolean TrimDone (const Standard_Integer IndH) const;
  
  //! Returns the fact that the intersections failed
  //! for the IndH-th hatching.
    Standard_Boolean TrimFailed (const Standard_Integer IndH) const;
  
  //! Returns the fact that the domains were computed
  //! for all the hatchings.
    Standard_Boolean IsDone() const;
  
  //! Returns the fact that the domains were computed
  //! for the IndH-th hatching.
  Standard_EXPORT Standard_Boolean IsDone (const Standard_Integer IndH) const;
  
  //! Returns the status about the IndH-th hatching.
    HatchGen_ErrorStatus Status (const Standard_Integer IndH) const;
  
  //! Returns the number of domains of the IndH-th hatching.
  //! Only ONE "INFINITE" domain means that the hatching is
  //! fully included in the contour defined by the elements.
    Standard_Integer NbDomains (const Standard_Integer IndH) const;
  
  //! Returns the IDom-th domain of the IndH-th hatching.
  Standard_EXPORT const HatchGen_Domain& Domain (const Standard_Integer IndH, const Standard_Integer IDom) const;
  
  //! Dump the hatcher.
  Standard_EXPORT void Dump() const;




protected:

  
  //! Returns the IndE-th element.
    Geom2dHatch_Element& Element (const Standard_Integer IndE);
  
  //! Returns the IndH-th hatching.
    Geom2dHatch_Hatching& Hatching (const Standard_Integer IndH);




private:

  
  //! Trims the IndH-th hatching of the hatcher by the
  //! IndE-th element.
  Standard_EXPORT Standard_Boolean Trim (const Standard_Integer IndH, const Standard_Integer IndE);
  
  //! Sets the global transition (the before and after
  //! states and segment extremities flags) of the point.
  Standard_EXPORT Standard_Boolean GlobalTransition (HatchGen_PointOnHatching& Point);


  Geom2dHatch_Intersector myIntersector;
  Standard_Real myConfusion2d;
  Standard_Real myConfusion3d;
  Standard_Boolean myKeepPoints;
  Standard_Boolean myKeepSegments;
  Standard_Integer myNbElements;
  Geom2dHatch_Elements myElements;
  Standard_Integer myNbHatchings;
  Geom2dHatch_Hatchings myHatchings;


};


#include <Geom2dHatch_Hatcher.lxx>





#endif // _Geom2dHatch_Hatcher_HeaderFile
