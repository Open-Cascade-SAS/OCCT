// Created on: 1998-02-11
// Created by: Philippe MANGIN
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_SectionPlacement.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionPlacement.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

static Standard_Real SearchParam(const Handle(BRepFill_LocationLaw)& Law,
                                 const Standard_Integer              Ind,
                                 const TopoDS_Vertex&                TheV)
{
  Standard_Real t;
  TopoDS_Edge   E;
  E = Law->Edge(Ind);
  t = BRep_Tool::Parameter(TheV, E);
  if (E.Orientation() == TopAbs_REVERSED)
  {
    Standard_Real      f, l, Lf, Ll;
    Handle(Geom_Curve) C;
    C  = BRep_Tool::Curve(E, f, l);
    Lf = Law->Law(Ind)->GetCurve()->FirstParameter();
    Ll = Law->Law(Ind)->GetCurve()->LastParameter();
    t  = Ll - (t - f) * (Ll - Lf) / (l - f);
  }
  return t;
}

BRepFill_SectionPlacement::BRepFill_SectionPlacement(const Handle(BRepFill_LocationLaw)& Law,
                                                     const TopoDS_Shape&                 Section,
                                                     const Standard_Boolean WithContact,
                                                     const Standard_Boolean WithCorrection)
    : myLaw(Law),
      mySection(Section)
{
  TopoDS_Vertex VNull;
  VNull.Nullify();
  Perform(WithContact, WithCorrection, VNull);
}

BRepFill_SectionPlacement::BRepFill_SectionPlacement(const Handle(BRepFill_LocationLaw)& Law,
                                                     const TopoDS_Shape&                 Section,
                                                     const TopoDS_Shape&                 Vertex,
                                                     const Standard_Boolean WithContact,
                                                     const Standard_Boolean WithCorrection)
    : myLaw(Law),
      mySection(Section)
{
  Perform(WithContact, WithCorrection, Vertex);
}

void BRepFill_SectionPlacement::Perform(const Standard_Boolean WithContact,
                                        const Standard_Boolean WithCorrection,
                                        const TopoDS_Shape&    Vertex)
{
  Standard_Real      anEdgeStartParam = 0.;
  Standard_Real      anEdgeEndParam   = 0.;
  Handle(Geom_Curve) aCurve;

  // Here we are simply looking for the first valid curve in the section.
  TopExp_Explorer anEdgeExplorer(mySection, TopAbs_EDGE);
  for (; anEdgeExplorer.More(); anEdgeExplorer.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    if (anEdge.IsNull() || BRep_Tool::Degenerated(anEdge))
      continue;

    aCurve = BRep_Tool::Curve(anEdge, anEdgeStartParam, anEdgeEndParam);
    if (aCurve.IsNull())
      continue;

    break;
  }

  Handle(Geom_Geometry) aSection;
  if (aCurve.IsNull())
  {
    // No edge found : the section is a vertex
    TopExp_Explorer     aVertexExplorer(mySection, TopAbs_VERTEX);
    const TopoDS_Vertex aFirstVertex = TopoDS::Vertex(aVertexExplorer.Current());
    const gp_Pnt        aPoint       = BRep_Tool::Pnt(aFirstVertex);
    aSection                         = new Geom_CartesianPoint(aPoint);
  }
  else
  {
    Handle(Geom_TrimmedCurve) aTrimmedCurve =
      new Geom_TrimmedCurve(aCurve, anEdgeStartParam, anEdgeEndParam);
    anEdgeExplorer.Next();

    if (anEdgeExplorer.More())
    {
      constexpr Standard_Real             aPrecisionTolerance = Precision::Confusion();
      GeomConvert_CompCurveToBSplineCurve aBSplineConverter(aTrimmedCurve);
      for (; anEdgeExplorer.More(); anEdgeExplorer.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
        // avoid null, degenerated edges
        if (anEdge.IsNull() || BRep_Tool::Degenerated(anEdge))
          continue;

        aCurve = BRep_Tool::Curve(anEdge, anEdgeStartParam, anEdgeEndParam);
        if (aCurve.IsNull())
          continue;

        TopoDS_Vertex aFirstVertex;
        TopoDS_Vertex aLastVertex;
        TopExp::Vertices(anEdge, aFirstVertex, aLastVertex);
        const Standard_Real aVertexTolerance =
          std::max(BRep_Tool::Tolerance(aFirstVertex), BRep_Tool::Tolerance(aLastVertex));

        aTrimmedCurve = new Geom_TrimmedCurve(aCurve, anEdgeStartParam, anEdgeEndParam);
        if (!aBSplineConverter.Add(aTrimmedCurve, std::min(aPrecisionTolerance, aVertexTolerance)))
        {
          aBSplineConverter.Add(aTrimmedCurve, std::max(aPrecisionTolerance, aVertexTolerance));
        }
      }
      aCurve = aBSplineConverter.BSplineCurve();
    }
    else
    {
      aCurve = aTrimmedCurve;
    }

    aSection = aCurve;
  }

  GeomFill_SectionPlacement     aSectionPlacement(myLaw->Law(1), aSection);
  Handle(BRepAdaptor_CompCurve) aWireAdaptor = new BRepAdaptor_CompCurve(myLaw->Wire());
  aSectionPlacement.Perform(aWireAdaptor, Precision::Confusion());

  const Standard_Real     aSectionParam   = aSectionPlacement.ParameterOnPath();
  constexpr Standard_Real aParamConfusion = Precision::PConfusion();

  Standard_Integer aLawIndex1 = 0;
  Standard_Integer aLawIndex2 = 0;
  // In the general case : Localisation via concatenation of the spine
  Standard_Boolean anIsIntervalFound = Standard_False;
  for (int aLawIndex = 1; aLawIndex <= myLaw->NbLaw() && !anIsIntervalFound; ++aLawIndex)
  {
    const Standard_Real aCurrKnotParam = aLawIndex - 1;
    const Standard_Real aNextKnotParam = aLawIndex;

    // Check if the section parameter is in the interval [aCurrKnotParam, aNextKnotParam]
    anIsIntervalFound = (aCurrKnotParam - aParamConfusion <= aSectionParam)
                        && (aNextKnotParam + aParamConfusion >= aSectionParam);
    if (!anIsIntervalFound)
    {
      continue;
    }

    aLawIndex1 = aLawIndex;
    if ((std::abs(aSectionParam - aCurrKnotParam) < aParamConfusion) && (aLawIndex > 1))
    {
      aLawIndex2 = aLawIndex - 1;
    }
    else if ((std::abs(aSectionParam - aNextKnotParam) < aParamConfusion)
             && (aLawIndex < myLaw->NbLaw()))
    {
      aLawIndex2 = aLawIndex + 1;
    }
  }

  if (!anIsIntervalFound)
  {
    throw Standard_ConstructionError("Interval is not found");
  }

  // Search of the <Ind1> by vertex <TheV>
  bool          anIsVertexOnLaw = false;
  TopoDS_Vertex aVertex         = TopoDS::Vertex(Vertex);
  if (!aVertex.IsNull())
  {
    for (int aCurrentLawIndex = 1; aCurrentLawIndex <= myLaw->NbLaw(); ++aCurrentLawIndex)
    {
      TopoDS_Edge   anEdge = myLaw->Edge(aCurrentLawIndex);
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      if (V1.IsSame(aVertex) || V2.IsSame(aVertex))
      {
        anIsVertexOnLaw = true;
        aLawIndex1      = aCurrentLawIndex;
        aLawIndex2      = 0;
        break;
      }
    }
  }
  ////////////////////

  // Positioning on the localized edge (or 2 Edges)

  aSectionPlacement.SetLocation(myLaw->Law(aLawIndex1));
  if (!anIsVertexOnLaw)
  {
    aSectionPlacement.Perform(Precision::Confusion());
  }
  else
  {
    aSectionPlacement.Perform(SearchParam(myLaw, aLawIndex1, aVertex), Precision::Confusion());
  }

  myTrsf              = aSectionPlacement.Transformation(WithContact, WithCorrection);
  myIndex             = aLawIndex1;
  myParam             = aSectionPlacement.ParameterOnPath();
  Standard_Real Angle = aSectionPlacement.Angle();

  if (aLawIndex2)
  {
    aSectionPlacement.SetLocation(myLaw->Law(aLawIndex2));
    if (!anIsVertexOnLaw)
    {
      aSectionPlacement.Perform(Precision::Confusion());
    }
    else
    {
      if (aLawIndex1 == aLawIndex2)
        aVertex.Reverse();
      aSectionPlacement.Perform(SearchParam(myLaw, aLawIndex2, aVertex), Precision::Confusion());
    }
    if (aSectionPlacement.Angle() > Angle)
    {
      myTrsf  = aSectionPlacement.Transformation(WithContact, WithCorrection);
      myIndex = aLawIndex2;
      myParam = aSectionPlacement.ParameterOnPath();
    }
  }
}

const gp_Trsf& BRepFill_SectionPlacement::Transformation() const
{
  return myTrsf;
}

Standard_Real BRepFill_SectionPlacement::AbscissaOnPath()
{
  return myLaw->Abscissa(myIndex, myParam);
}
