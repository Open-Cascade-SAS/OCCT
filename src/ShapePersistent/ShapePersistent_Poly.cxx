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

#include <ShapePersistent_Poly.hxx>

#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>


Handle(Poly_Polygon2D) ShapePersistent_Poly::pPolygon2D::Import() const
{
  if (myNodes.IsNull())
    return NULL;

  Handle(Poly_Polygon2D) aPolygon = new Poly_Polygon2D (*myNodes->Array());
  aPolygon->Deflection (myDeflection);
  return aPolygon;
}

Handle(Poly_Polygon3D) ShapePersistent_Poly::pPolygon3D::Import() const
{
  if (myNodes.IsNull() || myParameters.IsNull())
    return NULL;

  Handle(Poly_Polygon3D) aPolygon = new Poly_Polygon3D (*myNodes->Array(),
                                                        *myParameters->Array());
  aPolygon->Deflection (myDeflection);
  return aPolygon;
}

Handle(Poly_PolygonOnTriangulation)
  ShapePersistent_Poly::pPolygonOnTriangulation::Import() const
{
  Handle(Poly_PolygonOnTriangulation) aPolygon;

  if (myNodes)
  {
    if (myParameters)
      aPolygon = new Poly_PolygonOnTriangulation (*myNodes->Array(),
                                                  *myParameters->Array());
    else
      aPolygon = new Poly_PolygonOnTriangulation (*myNodes->Array());

    aPolygon->Deflection (myDeflection);
  }

  return aPolygon;
}

Handle(Poly_Triangulation) ShapePersistent_Poly::pTriangulation::Import() const
{
  Handle(Poly_Triangulation) aTriangulation;

// Triangulation is not used
#if 0
  if (myNodes && myTriangles)
  {
    if (myUVNodes)
      aTriangulation = new Poly_Triangulation (*myNodes->Array(),
                                               *myUVNodes->Array(),
                                               *myTriangles->Array());
    else
      aTriangulation = new Poly_Triangulation (*myNodes->Array(),
                                               *myTriangles->Array());

    aTriangulation->Deflection (myDeflection);
  }
#endif

  return aTriangulation;
}
