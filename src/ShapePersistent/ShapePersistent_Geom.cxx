// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Standard_NullObject.hxx>

#include <ShapePersistent_Geom.hxx>
#include <ShapePersistent_Geom_Curve.hxx>
#include <ShapePersistent_Geom_Surface.hxx>
#include <StdObject_gp_Axes.hxx>
#include <StdObject_gp_Vectors.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_Geom::Geometry::Read (StdObjMgt_ReadData&) {}

//=======================================================================
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
void ShapePersistent_Geom::Geometry::Write (StdObjMgt_WriteData&) const {}

//=======================================================================
//function : PChildren
//purpose  : Gets persistent objects
//=======================================================================
void ShapePersistent_Geom::Geometry::PChildren (SequenceOfPersistent&) const
{ 
}

//=======================================================================
//function : Translate
//purpose  : Create a persistent object for a curve
//=======================================================================
Handle(ShapePersistent_Geom::Curve) 
ShapePersistent_Geom::Translate (const Handle(Geom_Curve)& theCurve,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(Curve)::DownCast(theMap.Find(theCurve));
    else
    {
      Handle(Standard_Type) aCT = theCurve->DynamicType();
      if (aCT == STANDARD_TYPE(Geom_Line)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_Line)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_Circle)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_Circle)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_Ellipse)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_Ellipse)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_Hyperbola)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_Hyperbola)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_Parabola)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_Parabola)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_BezierCurve)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_BezierCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_BSplineCurve)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_BSplineCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_TrimmedCurve)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_TrimmedCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom_OffsetCurve)) {
        aPC = ShapePersistent_Geom_Curve::Translate(Handle(Geom_OffsetCurve)::DownCast(theCurve), theMap);
      }
      else {
        Standard_NullObject::Raise("No mapping for the current Transient Curve");
      }
      theMap.Bind(theCurve, aPC);
    }
  }
  return aPC;
}

//=======================================================================
//function : Translate
//purpose  : Create a persistent object for a surface
//=======================================================================
Handle(ShapePersistent_Geom::Surface) 
ShapePersistent_Geom::Translate(const Handle(Geom_Surface)& theSurf,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Surface) aPS;
  if (!theSurf.IsNull())
  {
    if (theMap.IsBound(theSurf))
      aPS = Handle(Surface)::DownCast(theMap.Find(theSurf));
    else
    {
      Handle(Standard_Type) aST = theSurf->DynamicType();
      if (aST == STANDARD_TYPE(Geom_Plane)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_Plane)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_CylindricalSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_CylindricalSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_ConicalSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_ConicalSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_SphericalSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_SphericalSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_ToroidalSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_ToroidalSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_SurfaceOfRevolution)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_BezierSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_BezierSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_BSplineSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_BSplineSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_RectangularTrimmedSurface)::DownCast(theSurf), theMap);
      }
      else if (aST == STANDARD_TYPE(Geom_OffsetSurface)) {
        aPS = ShapePersistent_Geom_Surface::Translate(Handle(Geom_OffsetSurface)::DownCast(theSurf), theMap);
      }
      else {
        Standard_NullObject::Raise("No mapping for the current Transient Surface");
      }
      theMap.Bind(theSurf, aPS);
    }
  }
  return aPS;
}
