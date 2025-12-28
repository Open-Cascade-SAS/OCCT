// Created on: 1993-02-19
// Created by: Remi LEQUETTE
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

#include <BRepAdaptor_Surface.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Surface.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAdaptor_Surface, GeomAdaptor_TransformedSurface)

//=================================================================================================

BRepAdaptor_Surface::BRepAdaptor_Surface() {}

//=================================================================================================

BRepAdaptor_Surface::BRepAdaptor_Surface(const TopoDS_Face& F, const bool R)
{
  Initialize(F, R);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> BRepAdaptor_Surface::ShallowCopy() const
{
  occ::handle<BRepAdaptor_Surface> aCopy = new BRepAdaptor_Surface();

  const occ::handle<Adaptor3d_Surface> aSurface = mySurf.ShallowCopy();
  const GeomAdaptor_Surface& aGeomSurface       = *occ::down_cast<GeomAdaptor_Surface>(aSurface);
  aCopy->mySurf                                 = aGeomSurface;

  aCopy->myTrsf = myTrsf;
  aCopy->myFace = myFace;

  return aCopy;
}

//=================================================================================================

void BRepAdaptor_Surface::Initialize(const TopoDS_Face& F, const bool Restriction)
{
  if (F.IsNull())
  {
    return;
  }

  myFace = F;
  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& aSurface = BRep_Tool::Surface(F, L);
  if (aSurface.IsNull())
    return;

  if (Restriction)
  {
    double umin, umax, vmin, vmax;
    BRepTools::UVBounds(F, umin, umax, vmin, vmax);
    mySurf.Load(aSurface, umin, umax, vmin, vmax);
  }
  else
    mySurf.Load(aSurface);
  myTrsf = L.Transformation();
}

//=================================================================================================

const TopoDS_Face& BRepAdaptor_Surface::Face() const
{
  return myFace;
}

//=================================================================================================

double BRepAdaptor_Surface::Tolerance() const
{
  return BRep_Tool::Tolerance(myFace);
}
