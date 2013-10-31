// Created on: 1993-10-27
// Created by: Jean-LOuis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StdPrs_ToolShadedShape.ixx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomLib.hxx>
#include <gp_Vec.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <TShort_Array1OfShortReal.hxx>

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================
Standard_Boolean StdPrs_ToolShadedShape::IsClosed (const TopoDS_Shape& theShape)
{
  return theShape.Closed();
}

//=======================================================================
//function : Triangulation
//purpose  :
//=======================================================================
Handle(Poly_Triangulation) StdPrs_ToolShadedShape::Triangulation (const TopoDS_Face& theFace,
                                                                  TopLoc_Location&   theLoc)
{
  return BRep_Tool::Triangulation (theFace, theLoc);
}

//=======================================================================
//function : Normal
//purpose  :
//=======================================================================
void StdPrs_ToolShadedShape::Normal (const TopoDS_Face&  theFace,
                                     Poly_Connect&       thePolyConnect,
                                     TColgp_Array1OfDir& theNormals)
{
  const Handle(Poly_Triangulation)& aPolyTri = thePolyConnect.Triangulation();
  const TColgp_Array1OfPnt&         aNodes   = aPolyTri->Nodes();
  if (aPolyTri->HasNormals())
  {
    // normals pre-computed in triangulation structure
    const TShort_Array1OfShortReal& aNormals = aPolyTri->Normals();
    const Standard_ShortReal*       aNormArr = &(aNormals.Value (aNormals.Lower()));
    for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
    {
      const Standard_Integer anId = 3 * (aNodeIter - aNodes.Lower());
      const gp_Dir aNorm (aNormArr[anId + 0],
                          aNormArr[anId + 1],
                          aNormArr[anId + 2]);
      theNormals (aNodeIter) = aNorm;
    }

    if (theFace.Orientation() == TopAbs_REVERSED)
    {
      for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
      {
        theNormals.ChangeValue (aNodeIter).Reverse();
      }
    }
    return;
  }

  // take in face the surface location
  const TopoDS_Face      aZeroFace = TopoDS::Face (theFace.Located (TopLoc_Location()));
  Handle(Geom_Surface)   aSurf     = BRep_Tool::Surface (aZeroFace);
  const Standard_Real    aTol      = Precision::Confusion();
  Handle(TShort_HArray1OfShortReal) aNormals = new TShort_HArray1OfShortReal (1, aPolyTri->NbNodes() * 3);
  const Poly_Array1OfTriangle& aTriangles = aPolyTri->Triangles();
  const TColgp_Array1OfPnt2d*  aNodesUV   = aPolyTri->HasUVNodes() && !aSurf.IsNull()
                                          ? &aPolyTri->UVNodes()
                                          : NULL;
  Standard_Integer aTri[3];
  for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
  {
    // try to retrieve normal from real surface first, when UV coordinates are available
    if (aNodesUV == NULL
     || GeomLib::NormEstim (aSurf, aNodesUV->Value (aNodeIter), aTol, theNormals (aNodeIter)) > 1)
    {
      // compute flat normals
      gp_XYZ eqPlan (0.0, 0.0, 0.0);
      for (thePolyConnect.Initialize (aNodeIter); thePolyConnect.More(); thePolyConnect.Next())
      {
        aTriangles (thePolyConnect.Value()).Get (aTri[0], aTri[1], aTri[2]);
        const gp_XYZ v1 (aNodes (aTri[1]).Coord() - aNodes (aTri[0]).Coord());
        const gp_XYZ v2 (aNodes (aTri[2]).Coord() - aNodes (aTri[1]).Coord());
        const gp_XYZ vv = v1 ^ v2;
        const Standard_Real aMod = vv.Modulus();
        if (aMod >= aTol)
        {
          eqPlan += vv / aMod;
        }
      }
      const Standard_Real aModMax = eqPlan.Modulus();
      theNormals (aNodeIter) = (aModMax > aTol) ? gp_Dir (eqPlan) : gp::DZ();
    }

    const Standard_Integer anId = (aNodeIter - aNodes.Lower()) * 3;
    aNormals->SetValue (anId + 1, (Standard_ShortReal )theNormals (aNodeIter).X());
    aNormals->SetValue (anId + 2, (Standard_ShortReal )theNormals (aNodeIter).Y());
    aNormals->SetValue (anId + 3, (Standard_ShortReal )theNormals (aNodeIter).Z());
  }
  aPolyTri->SetNormals (aNormals);

  if (theFace.Orientation() == TopAbs_REVERSED)
  {
    for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
    {
      theNormals.ChangeValue (aNodeIter).Reverse();
    }
  }
}
