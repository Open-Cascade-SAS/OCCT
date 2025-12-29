// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

#include <BRepClass3d_BndBoxTree.hxx>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtCC.hxx>

//=================================================================================================

bool BRepClass3d_BndBoxTreeSelectorPoint::Accept(const int& theObj)
{
  // Box-point collision.
  if (theObj > myMapOfShape.Extent())
    return false;
  const TopoDS_Shape& shp = myMapOfShape(theObj);
  TopAbs_ShapeEnum    sht = shp.ShapeType();
  if (sht == TopAbs_EDGE)
  {
    const TopoDS_Edge& E       = TopoDS::Edge(shp);
    double             EdgeTSq = BRep_Tool::Tolerance(E);
    EdgeTSq *= EdgeTSq;
    double            f, l;
    BRepAdaptor_Curve C(E);
    BRep_Tool::Range(E, f, l);

    // Edge-Point interference.
    Extrema_ExtPC ExtPC(myP, C, f, l);
    if (ExtPC.IsDone() && ExtPC.NbExt() > 0)
    {
      for (int i = 1; i <= ExtPC.NbExt(); i++)
        if (ExtPC.SquareDistance(i) < EdgeTSq)
        {
          myStop = true; // exit from selector
          return true;
        }
    }
  }
  else if (sht == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& V       = TopoDS::Vertex(shp);
    gp_Pnt               VPnt    = BRep_Tool::Pnt(V);
    double               VertTSq = BRep_Tool::Tolerance(V);
    VertTSq *= VertTSq;
    // Vertex-Point interference.
    if (VPnt.SquareDistance(myP) < VertTSq)
    {
      myStop = true;
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool BRepClass3d_BndBoxTreeSelectorLine::Accept(const int& theObj)
{
  // box-line collision
  if (theObj > myMapOfShape.Extent())
    return false;
  const TopoDS_Shape& shp = myMapOfShape(theObj);
  TopAbs_ShapeEnum    sht = shp.ShapeType();
  if (sht == TopAbs_EDGE)
  {
    const TopoDS_Edge& E       = TopoDS::Edge(shp);
    double             EdgeTSq = BRep_Tool::Tolerance(E);
    EdgeTSq *= EdgeTSq;
    double            f, l;
    BRepAdaptor_Curve C(E);
    BRep_Tool::Range(E, f, l);

    // Edge-Line interference.
    Extrema_ExtCC ExtCC(C, myLC, f, l, myLC.FirstParameter(), myLC.LastParameter());
    if (ExtCC.IsDone())
    {
      if (ExtCC.IsParallel())
      {
        // Tangent case is invalid for classification
        myIsValid = false;
      }
      else if (ExtCC.NbExt() > 0)
      {
        bool IsInside = false;
        for (int i = 1; i <= ExtCC.NbExt(); i++)
        {
          if (ExtCC.SquareDistance(i) < EdgeTSq)
          {
            Extrema_POnCurv P1, P2;
            ExtCC.Points(i, P1, P2);

            EdgeParam EP;
            EP.myE      = E;
            EP.myParam  = P1.Parameter(); // Original curve is the first parameter.
            EP.myLParam = P2.Parameter(); // Linear curve is the second parameter.

            myEP.Append(EP);
            IsInside = true;
          }
        }
        if (IsInside)
          return true;
      }
    }
  }
  else if (sht == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& V       = TopoDS::Vertex(shp);
    double               VertTSq = BRep_Tool::Tolerance(V);
    VertTSq *= VertTSq;
    // Vertex-Line interference.
    Extrema_ExtPElC ExtPL(BRep_Tool::Pnt(V),
                          myL,
                          Precision::Confusion(),
                          -Precision::Infinite(),
                          Precision::Infinite());
    if (ExtPL.IsDone() && ExtPL.NbExt() > 0)
      if (ExtPL.SquareDistance(1) < VertTSq)
      {
        Extrema_POnCurv PP;
        double          paramL;
        PP     = ExtPL.Point(1);
        paramL = PP.Parameter();
        VertParam VP;
        VP.myV      = V;
        VP.myLParam = paramL;
        myVP.Append(VP);
        return true;
      }
  }
  return false;
}