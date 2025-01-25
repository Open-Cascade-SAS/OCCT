// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BRepLib_ToolTriangulatedShape.hxx>

#include <BRep_Tool.hxx>
#include <GeomLib.hxx>
#include <Poly.hxx>
#include <Poly_Connect.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

// =======================================================================
// function : ComputeNormals
// purpose  :
// =======================================================================
void BRepLib_ToolTriangulatedShape::ComputeNormals(const TopoDS_Face&                theFace,
                                                   const Handle(Poly_Triangulation)& theTris,
                                                   Poly_Connect&                     thePolyConnect)
{
  if (theTris.IsNull() || theTris->HasNormals())
  {
    return;
  }

  // take in face the surface location
  const TopoDS_Face    aZeroFace = TopoDS::Face(theFace.Located(TopLoc_Location()));
  Handle(Geom_Surface) aSurf     = BRep_Tool::Surface(aZeroFace);
  if (!theTris->HasUVNodes() || aSurf.IsNull())
  {
    // compute normals by averaging triangulation normals sharing the same vertex
    Poly::ComputeNormals(theTris);
    return;
  }

  constexpr Standard_Real aTol = Precision::Confusion();
  Standard_Integer        aTri[3];
  gp_Dir                  aNorm;
  theTris->AddNormals();
  for (Standard_Integer aNodeIter = 1; aNodeIter <= theTris->NbNodes(); ++aNodeIter)
  {
    // try to retrieve normal from real surface first, when UV coordinates are available
    if (GeomLib::NormEstim(aSurf, theTris->UVNode(aNodeIter), aTol, aNorm) > 1)
    {
      if (thePolyConnect.Triangulation() != theTris)
      {
        thePolyConnect.Load(theTris);
      }

      // compute flat normals
      gp_XYZ eqPlan(0.0, 0.0, 0.0);
      for (thePolyConnect.Initialize(aNodeIter); thePolyConnect.More(); thePolyConnect.Next())
      {
        theTris->Triangle(thePolyConnect.Value()).Get(aTri[0], aTri[1], aTri[2]);
        const gp_XYZ        v1(theTris->Node(aTri[1]).Coord() - theTris->Node(aTri[0]).Coord());
        const gp_XYZ        v2(theTris->Node(aTri[2]).Coord() - theTris->Node(aTri[1]).Coord());
        const gp_XYZ        vv   = v1 ^ v2;
        const Standard_Real aMod = vv.Modulus();
        if (aMod >= aTol)
        {
          eqPlan += vv / aMod;
        }
      }
      const Standard_Real aModMax = eqPlan.Modulus();
      aNorm                       = (aModMax > aTol) ? gp_Dir(eqPlan) : gp::DZ();
    }

    theTris->SetNormal(aNodeIter, aNorm);
  }
}
