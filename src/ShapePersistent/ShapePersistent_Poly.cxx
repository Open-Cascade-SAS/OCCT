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
#include <ShapePersistent_HArray1.hxx>

#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>


void ShapePersistent_Poly::pPolygon2D::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myNodes);
}

Handle(Poly_Polygon2D) ShapePersistent_Poly::pPolygon2D::Import() const
{
  if (myNodes.IsNull())
    return NULL;

  Handle(Poly_Polygon2D) aPolygon = new Poly_Polygon2D (*myNodes->Array());
  aPolygon->Deflection (myDeflection);
  return aPolygon;
}

void ShapePersistent_Poly::pPolygon3D::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myNodes);
  theChildren.Append(myParameters);
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

void ShapePersistent_Poly::pPolygonOnTriangulation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myNodes);
  theChildren.Append(myParameters);
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

void ShapePersistent_Poly::pTriangulation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myNodes);
  theChildren.Append(myUVNodes);
  theChildren.Append(myTriangles);
}

Handle(Poly_Triangulation) ShapePersistent_Poly::pTriangulation::Import() const
{
  Handle(Poly_Triangulation) aTriangulation;

// Triangulation is not used
#if 1
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

Handle(ShapePersistent_Poly::Polygon2D) 
ShapePersistent_Poly::Translate(const Handle(Poly_Polygon2D)& thePoly,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Polygon2D) aPP;
  if (!thePoly.IsNull()) 
  {
    if (theMap.IsBound(thePoly))
      aPP = Handle(Polygon2D)::DownCast(theMap.Find(thePoly));
    else 
    {
      aPP = new Polygon2D;
      aPP->myPersistent = new pPolygon2D;
      aPP->myPersistent->myDeflection = thePoly->Deflection();
      aPP->myPersistent->myNodes =
        StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt2d>("PColgp_HArray1OfPnt2d", thePoly->Nodes());
      theMap.Bind(thePoly, aPP);
    }
  }
  return aPP;
}

Handle(ShapePersistent_Poly::Polygon3D) 
ShapePersistent_Poly::Translate(const Handle(Poly_Polygon3D)& thePoly,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Polygon3D) aPP = new Polygon3D;
  if (!thePoly.IsNull()) 
  {
    if (theMap.IsBound(thePoly))
      aPP = Handle(Polygon3D)::DownCast(theMap.Find(thePoly));
    else 
    {
      aPP = new Polygon3D;
      aPP->myPersistent = new pPolygon3D;
      aPP->myPersistent->myDeflection = thePoly->Deflection();
      aPP->myPersistent->myNodes =
        StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt>("PColgp_HArray1OfPnt", thePoly->Nodes());
      if (thePoly->HasParameters()) {
        aPP->myPersistent->myParameters =
          StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(thePoly->Parameters());
      }
      theMap.Bind(thePoly, aPP);
    }
  }
  return aPP;
}

Handle(ShapePersistent_Poly::PolygonOnTriangulation) 
ShapePersistent_Poly::Translate(const Handle(Poly_PolygonOnTriangulation)& thePolyOnTriang,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(PolygonOnTriangulation) aPPonT;
  if (!thePolyOnTriang.IsNull())
  {
    if (theMap.IsBound(thePolyOnTriang))
      aPPonT = Handle(PolygonOnTriangulation)::DownCast(theMap.Find(thePolyOnTriang));
    else
    {
      aPPonT = new PolygonOnTriangulation;
      aPPonT->myPersistent = new pPolygonOnTriangulation;
      aPPonT->myPersistent->myDeflection = thePolyOnTriang->Deflection();
      aPPonT->myPersistent->myNodes =
        StdLPersistent_HArray1::Translate<TColStd_HArray1OfInteger>(thePolyOnTriang->Nodes());
      if (thePolyOnTriang->HasParameters()) {
        aPPonT->myPersistent->myParameters =
          StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(thePolyOnTriang->Parameters()->Array1());
      }
      theMap.Bind(thePolyOnTriang, aPPonT);
    }
  }
  return aPPonT;
}

Handle(ShapePersistent_Poly::Triangulation) 
ShapePersistent_Poly::Translate(const Handle(Poly_Triangulation)& thePolyTriang,
                                StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Triangulation) aPT;
  if (!thePolyTriang.IsNull()) 
  {
    if (theMap.IsBound(thePolyTriang))
      aPT = Handle(Triangulation)::DownCast(theMap.Find(thePolyTriang));
    else 
    {
      aPT = new Triangulation;
      aPT->myPersistent = new pTriangulation;
      aPT->myPersistent->myNodes = 
        StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt>("PColgp_HArray1OfPnt", thePolyTriang->Nodes());
      aPT->myPersistent->myTriangles = 
        StdLPersistent_HArray1::Translate<Poly_HArray1OfTriangle>("PPoly_HArray1OfTriangle", thePolyTriang->Triangles());
      if (thePolyTriang->HasUVNodes()) {
        aPT->myPersistent->myUVNodes = 
          StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt2d>("PColgp_HArray1OfPnt2d", thePolyTriang->UVNodes());
      }
      theMap.Bind(thePolyTriang, aPT);
    }
  }
  return aPT;
}
