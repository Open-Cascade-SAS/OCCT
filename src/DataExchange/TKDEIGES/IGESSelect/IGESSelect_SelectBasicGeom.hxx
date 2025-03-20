// Created on: 1997-04-14
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _IGESSelect_SelectBasicGeom_HeaderFile
#define _IGESSelect_SelectBasicGeom_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_SelectExplore.hxx>
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;
class IGESData_IGESEntity;

class IGESSelect_SelectBasicGeom;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectBasicGeom, IFSelect_SelectExplore)

//! This selection returns the basic geometric elements
//! contained in an IGES Entity
//! Intended to run a "quick" transfer. I.E. :
//! - for a Group, considers its Elements
//! - for a Trimmed or Bounded Surface or a Face (BREP),
//! considers the 3D curves of each of its loops
//! - for a Plane (108), considers its Bounding Curve
//! - for a Curve itself, takes it
//!
//! Also, FREE surfaces are taken, because curve 3d is known for
//! them. (the ideal should be to have their natural bounds)
//!
//! If <curvesonly> is set, ONLY curves-3d are returned
class IGESSelect_SelectBasicGeom : public IFSelect_SelectExplore
{

public:
  //! Creates a SelectBasicGeom, which always works recursively
  //! mode = -1 : Returns Surfaces (without trimming)
  //! mode = +1 : Returns Curves 3D (free or bound of surface)
  //! mode = +2 : Returns Basic Curves 3D : as 1 but CompositeCurves
  //! are returned in detail
  //! mode = 0  : both
  Standard_EXPORT IGESSelect_SelectBasicGeom(const Standard_Integer mode);

  Standard_EXPORT Standard_Boolean CurvesOnly() const;

  //! Explores an entity, to take its contained Curves 3d
  //! Works recursively
  Standard_EXPORT Standard_Boolean
    Explore(const Standard_Integer            level,
            const Handle(Standard_Transient)& ent,
            const Interface_Graph&            G,
            Interface_EntityIterator&         explored) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Curves 3d" or
  //! "Basic Geometry"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;

  //! This method can be called from everywhere to get the curves
  //! as sub-elements of a given curve :
  //! CompositeCurve : explored lists its subs + returns True
  //! Any Curve : explored is not filled but returned is True
  //! Other : returned is False
  Standard_EXPORT static Standard_Boolean SubCurves(const Handle(IGESData_IGESEntity)& ent,
                                                    Interface_EntityIterator&          explored);

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectBasicGeom, IFSelect_SelectExplore)

protected:
private:
  Standard_Integer thegeom;
};

#endif // _IGESSelect_SelectBasicGeom_HeaderFile
