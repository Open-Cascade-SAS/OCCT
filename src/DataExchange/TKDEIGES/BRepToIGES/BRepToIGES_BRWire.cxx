// Created on: 1995-01-27
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
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

//: q4 abv 16.03.99: PRO17828 face 555: transform pcurves on SurfaceOfRevolution
// szv#4 S4163
// S4181 pdn implementing of writing IGES elementary surfaces.

#include <BRep_Tool.hxx>
#include <BRepToIGES_BRWire.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dToIGES_Geom2dCurve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomPoint.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_Point.hxx>
#include <MoniTool_Macros.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//=============================================================================
// BRepToIGES_BRWire
//=============================================================================
BRepToIGES_BRWire::BRepToIGES_BRWire() {}

//=============================================================================
// BRepToIGES_BRWire
//=============================================================================

BRepToIGES_BRWire::BRepToIGES_BRWire(const BRepToIGES_BREntity& BR)
    : BRepToIGES_BREntity(BR)
{
}

//=============================================================================
// TransferWire
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferWire(const TopoDS_Shape& start)
{
  occ::handle<IGESData_IGESEntity> res;

  if (start.IsNull())
    return res;

  if (start.ShapeType() == TopAbs_VERTEX)
  {
    TopoDS_Vertex V = TopoDS::Vertex(start);
    res             = TransferVertex(V);
  }
  else if (start.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Edge E = TopoDS::Edge(start);
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> anEmptyMap;
    res = TransferEdge(E, anEmptyMap, false);
  }
  else if (start.ShapeType() == TopAbs_WIRE)
  {
    TopoDS_Wire W = TopoDS::Wire(start);
    res           = TransferWire(W);
  }
  else
  {
    // error message
  }
  return res;
}

//=============================================================================
// TransferVertex
//
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferVertex(const TopoDS_Vertex& myvertex)
{
  occ::handle<IGESData_IGESEntity> res;
  if (myvertex.IsNull())
    return res;

  occ::handle<Geom_CartesianPoint> Point;
  Point = new Geom_CartesianPoint(BRep_Tool::Pnt(myvertex));
  occ::handle<IGESData_IGESEntity> IVertex;
  if (!Point.IsNull())
  {
    GeomToIGES_GeomPoint GP;
    GP.SetModel(GetModel());
    IVertex = GP.TransferPoint(Point);
  }

  if (!IVertex.IsNull())
    res = IVertex;
  return res;
}

//=============================================================================
// TransferVertex
//
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferVertex(const TopoDS_Vertex& myvertex,
                                                                    const TopoDS_Edge&   myedge,
                                                                    double&              parameter)
{
  occ::handle<IGESData_IGESEntity> res;
  if (myvertex.IsNull())
    return res;

  occ::handle<IGESData_IGESEntity> IVertex = TransferVertex(myvertex);

  // returns the parameter of myvertex on myedge
  parameter = BRep_Tool::Parameter(myvertex, myedge);

  if (!IVertex.IsNull())
    res = IVertex;
  return res;
}

//=============================================================================
// TransferVertex
//
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferVertex(const TopoDS_Vertex& myvertex,
                                                                    const TopoDS_Edge&   myedge,
                                                                    const TopoDS_Face&   myface,
                                                                    double&              parameter)
{
  occ::handle<IGESData_IGESEntity> res;
  if (myvertex.IsNull())
    return res;

  occ::handle<IGESData_IGESEntity> IVertex = TransferVertex(myvertex);

  // returns the parameter of myvertex on the pcurve of myedge on myface
  parameter = BRep_Tool::Parameter(myvertex, myedge, myface);

  if (!IVertex.IsNull())
    res = IVertex;
  return res;
}

//=============================================================================
// TransferVertex
//
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferVertex(
  const TopoDS_Vertex&             myvertex,
  const TopoDS_Edge&               myedge,
  const occ::handle<Geom_Surface>& mysurface,
  const TopLoc_Location&           myloc,
  double&                          parameter)
{
  occ::handle<IGESData_IGESEntity> res;
  if (myvertex.IsNull())
    return res;

  occ::handle<IGESData_IGESEntity> IVertex = TransferVertex(myvertex);

  // returns the parameter of myvertex on the pcurve of myedge on mysurface
  parameter = BRep_Tool::Parameter(myvertex, myedge, mysurface, myloc);

  if (!IVertex.IsNull())
    res = IVertex;
  return res;
}

//=============================================================================
// TransferVertex
//
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferVertex(const TopoDS_Vertex& myvertex,
                                                                    const TopoDS_Face&   myface,
                                                                    gp_Pnt2d&            mypoint)
{
  occ::handle<IGESData_IGESEntity> res;
  if (myvertex.IsNull())
    return res;

  occ::handle<IGESData_IGESEntity> IVertex = TransferVertex(myvertex);

  // returns the parameter of myvertex on myface
  mypoint = BRep_Tool::Parameters(myvertex, myface);

  if (!IVertex.IsNull())
    res = IVertex;
  return res;
}

//=============================================================================
// TransferEdge
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferEdge(
  const TopoDS_Edge&                                                              theEdge,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theOriginMap,
  const bool                                                                      theIsBRepMode)
{
  occ::handle<IGESData_IGESEntity> res;
  if (theEdge.IsNull())
    return res;

  // returns the 3d curve of the edge and the parameter range
  TopLoc_Location                  L;
  double                           First, Last, U1, U2;
  occ::handle<IGESData_IGESEntity> ICurve;
  occ::handle<Geom_Curve>          Curve3d = BRep_Tool::Curve(theEdge, L, First, Last);

  // #28 rln 19.10.98 UKI60155, etc.
  // Only Bezier will be converted into B-Spline, not Conic. This conertation
  // will be done only inside GeomToIGES package for simplifying the code.

  // #29 rln 19.10.98
  // Unnecessary duplication of curves is removed.

  if (!Curve3d.IsNull())
  {
    gp_Trsf Tr = L.Transformation();
    if (Tr.Form() != gp_Identity)
      Curve3d = occ::down_cast<Geom_Curve>(Curve3d->Transformed(Tr));
    else
      Curve3d = occ::down_cast<Geom_Curve>(Curve3d->Copy());

    if (theEdge.Orientation() == TopAbs_REVERSED && !theIsBRepMode)
    {
      U1 = Curve3d->ReversedParameter(Last);
      U2 = Curve3d->ReversedParameter(First);
      Curve3d->Reverse();
    }
    else
    {
      U1 = First;
      U2 = Last;
    }

    GeomToIGES_GeomCurve GC;
    GC.SetModel(GetModel());
    ICurve = GC.TransferCurve(Curve3d, U1, U2);
  }

  // #31 rln 19.10.98
  // Vertices are not translated into IGES anymore since they are not put into
  // the model

  if (!ICurve.IsNull())
    res = ICurve;

  // In the reverted face's case find an origin by the reverted
  TopoDS_Edge anEdge = !theOriginMap.IsEmpty() ? TopoDS::Edge(theOriginMap.Find(theEdge)) : theEdge;
  SetShapeResult(anEdge, res);

  return res;
}

//=============================================================================
// TransferEdge
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferEdge(
  const TopoDS_Edge&                                                              theEdge,
  const TopoDS_Face&                                                              theFace,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theOriginMap,
  const double                                                                    theLength,
  const bool                                                                      theIsBRepMode)
{
  occ::handle<IGESData_IGESEntity> res;
  if (theEdge.IsNull() || GetPCurveMode() == 0
      || (!theIsBRepMode && BRep_Tool::Degenerated(theEdge)))
    return res;

  // S4181 pdn 23.04.99 adjusting length factor according to analytic mode.
  double myLen        = theLength;
  bool   analyticMode = (GetConvertSurfaceMode() == 0 && theIsBRepMode);

  // returns the 2d curve associated to theEdge in the parametric space of theFace
  double                    First, Last;
  occ::handle<Geom2d_Curve> Curve2d = BRep_Tool::CurveOnSurface(theEdge, theFace, First, Last);
  occ::handle<IGESData_IGESEntity> ICurve2d;
  // #29 rln 19.10.98

  if (!Curve2d.IsNull())
  {
    // For "revolution" and "LinearExtrusion" surfaces, it is necessary
    // to apply a translation of 2D curves to agree on the
    // origin (U,V) between IGES and BRep (for Cylindrical,
    // Conical and SurfaceOfLinearExtrusion)
    // It is necessary to invert (u,v) surfaces of revolution.

    TopLoc_Location           L;
    occ::handle<Geom_Surface> st = BRep_Tool::Surface(theFace, L);
    if (st->IsKind(STANDARD_TYPE(Geom_Plane)))
    {
      return res;
    }
    double Ufirst, Ulast, Vfirst, Vlast;
    BRepTools::UVBounds(theFace, Ufirst, Ulast, Vfirst, Vlast);
    occ::handle<Geom_Surface> Surf;

    if (st->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      DeclareAndCast(Geom_RectangularTrimmedSurface, rectang, st);
      Surf = rectang->BasisSurface();
    }
    else
      Surf = st;

    //: abv 19.06.02: writing (loopback) on file offseted_sphere.rle in BRep mode
    if (st->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      DeclareAndCast(Geom_OffsetSurface, offset, Surf);
      Surf = offset->BasisSurface();
    }

    // S4181 pdn 20.04.99 transformation of pcurves id defined by type of surface
    //  and analytic mode.
    //  skl 18.07.2005 for OCC9490 : in following if() commented
    //  condition for SurfaceOfLinearExtrusion
    bool needShift = (!analyticMode&&
                                  ((Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) ||
                                   (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))/* ||
                                   (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))*/));
    //: q4 abv 16 Mar 99: PRO17828 face 555: shift pcurves on SurfaceOfRevolution
    if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      occ::handle<Geom_SurfaceOfRevolution> rev   = occ::down_cast<Geom_SurfaceOfRevolution>(Surf);
      occ::handle<Geom_Curve>               curve = rev->BasisCurve();
      // skl 31.05.2004 for OCC6004
      if (curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
      {
        occ::handle<Geom_TrimmedCurve> tc = occ::down_cast<Geom_TrimmedCurve>(curve);
        curve                             = tc->BasisCurve();
      }
      if (curve->IsKind(STANDARD_TYPE(Geom_Line)))
        needShift = true;
    }
    if (needShift)
    {
      gp_Trsf2d TR;
      TR.SetTranslation(gp_Pnt2d(0., 0.), gp_Pnt2d(0., -Vfirst));
      Curve2d = occ::down_cast<Geom2d_Curve>(Curve2d->Transformed(TR));
    }
    else
      Curve2d = occ::down_cast<Geom2d_Curve>(Curve2d->Copy());

    // shift pcurves on periodic BSpline surfaces (issue 26138)
    if (Surf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      occ::handle<Geom_BSplineSurface> aBSpline = occ::down_cast<Geom_BSplineSurface>(Surf);
      double                           uShift = 0., vShift = 0.;
      double                           U0, U1, V0, V1;
      Surf->Bounds(U0, U1, V0, V1);
      if (aBSpline->IsUPeriodic() && std::abs(Ufirst - U0) > Precision::PConfusion())
      {
        uShift = ShapeAnalysis::AdjustToPeriod(Ufirst, U0, U1);
      }
      if (aBSpline->IsVPeriodic() && std::abs(Vfirst - V0) > Precision::PConfusion())
      {
        vShift = ShapeAnalysis::AdjustToPeriod(Vfirst, V0, V1);
      }
      if (std::abs(uShift) > Precision::PConfusion() || std::abs(vShift) > Precision::PConfusion())
      {
        gp_Trsf2d TR;
        TR.SetTranslation(gp_Pnt2d(0., 0.), gp_Pnt2d(uShift, vShift));
        Curve2d = occ::down_cast<Geom2d_Curve>(Curve2d->Transformed(TR));
      }
    }

    if (!analyticMode
        && ((Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
            || (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
            || (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))))
    {
      // #30 rln 19.10.98 transformation of pcurves for IGES Surface of Revolution
      Curve2d->Mirror(gp_Ax2d(gp::Origin2d(), gp_Dir2d(1., 1.)));
      Curve2d->Mirror(gp::OX2d());
      Curve2d->Translate(gp_Vec2d(0, 2 * M_PI));
    }

    if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))
        || (Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))))
    {
      Curve2d->Mirror(gp_Ax2d(gp::Origin2d(), gp_Dir2d(1., 1.)));
      Curve2d->Mirror(gp::OX2d());
      Curve2d->Translate(gp_Vec2d(0, 2 * M_PI));
    }

    if (analyticMode
        && (Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
            || Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))))
      myLen = M_PI / 180.;

    if (analyticMode
        && (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
            || Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))))
    {
      gp_Trsf2d trans;
      trans.SetScale(gp_Pnt2d(0, 0), 180. / M_PI);
      Curve2d->Transform(trans);
      First = Curve2d->TransformedParameter(First, trans);
      Last  = Curve2d->TransformedParameter(Last, trans);
    }

    if (analyticMode && (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))))
    {
      occ::handle<Geom_ConicalSurface> con = occ::down_cast<Geom_ConicalSurface>(Surf);
      if (con->SemiAngle() < 0)
      {
        double vApex = 2 * con->RefRadius() / std::sin(con->SemiAngle());
        Curve2d->Translate(gp_Vec2d(0, vApex));
      }
    }
    // S4181 transfer functionality
    gp_Trsf2d trans;
    double    uFact = 1.;
    if (theIsBRepMode && Surf->IsKind(STANDARD_TYPE(Geom_Plane)))
    {
      trans.SetScale(gp_Pnt2d(0, 0), 1. / GetUnit());
    }
    if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      double aDiv = myLen;
      if (aDiv < gp::Resolution())
        aDiv = 1.;
      // emv: changed for bug OCC22126 17.12.2010
      trans.SetScale(gp_Pnt2d(0, 0), 1. / (Vlast - Vfirst));
      // uFact = myLen;

      // added by skl 18.07.2005 for OCC9490
      //      trans.SetScale(gp_Pnt2d(0,0),1./Vlast);

      double du = 1.;
      double us1, us2, vs1, vs2;
      // scaling parameterization to [0,1]
      Surf->Bounds(us1, us2, vs1, vs2);
      du = us2 - us1;
      // emv: changed for bug OCC22126 17.12.2010
      uFact = (Vlast - Vfirst) / du;
      // uFact = aDiv/du;
    }
    if (Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
        || Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))
        || Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    { //: q4
      uFact = 1. / myLen;
    }

    ShapeBuild_Edge sbe;
    Curve2d = sbe.TransformPCurve(Curve2d, trans, uFact, First, Last);
    //      (Curve2d, Surf, First, Last, myLen, isBRepMode);
    // if the edge is REVERSED, it is necessary to "REVERSE" the curve 2d.

    // added by skl 18.07.2005 for OCC9490
    if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      // emv: changed for bug OCC22126 17.12.2010
      gp_Trsf2d trans1;
      double    us1, us2, vs1, vs2, du;
      // computing shift of pcurves
      Surf->Bounds(us1, us2, vs1, vs2);
      du = us2 - us1;
      trans1.SetTranslation(gp_Vec2d(-us1 / du, -Vfirst / (Vlast - Vfirst)));
      Curve2d = sbe.TransformPCurve(Curve2d, trans1, 1., First, Last);
    }

    if (theEdge.Orientation() == TopAbs_REVERSED)
    {
      double tmpFirst = Curve2d->ReversedParameter(Last),
             tmpLast  = Curve2d->ReversedParameter(First);
      Curve2d->Reverse();
      First = tmpFirst;
      Last  = tmpLast;
    }
    Geom2dToIGES_Geom2dCurve GC;
    GC.SetModel(GetModel());
    ICurve2d = GC.Transfer2dCurve(Curve2d, First, Last);
  }

  // #31 rln 19.10.98
  // Vertices are not translated into IGES anymore since they are not put into
  // the model

  if (!ICurve2d.IsNull())
    res = ICurve2d;

  // In the reverted face's case find an origin by the reverted
  TopoDS_Edge anEdge = !theOriginMap.IsEmpty() ? TopoDS::Edge(theOriginMap.Find(theEdge)) : theEdge;
  SetShapeResult(anEdge, res);

  return res;
}

//=============================================================================
// TransferWire
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferWire(const TopoDS_Wire& mywire)
{
  occ::handle<IGESData_IGESEntity> res;
  if (mywire.IsNull())
    return res;

  // A composite curve is defined as an ordered list of entities
  // consisting of a point, connect point and parametrised curve
  // entities (excluding the CompositeCurve entity).

  occ::handle<IGESData_IGESEntity>                                    ent;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Seq =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();

  TopExp_Explorer TE(mywire, TopAbs_VERTEX);
  if (TE.More())
  {
    BRepTools_WireExplorer WE;
    for (WE.Init(mywire); WE.More(); WE.Next())
    {
      const TopoDS_Edge& E = WE.Current();
      if (E.IsNull())
      {
        AddWarning(mywire, "an Edge is a null entity");
      }
      else
      {
        NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> anEmptyMap;
        ent = TransferEdge(E, anEmptyMap, false);
        if (!ent.IsNull())
          Seq->Append(ent);
      }
    }
  }
  else
    AddWarning(mywire, " no Vertex associated to the Wire");

  int                                                                nbedges = Seq->Length();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> Tab;
  if (nbedges == 1)
  {
    res = GetCasted(IGESData_IGESEntity, Seq->Value(1));
  }
  else if (nbedges >= 2)
  {
    Tab = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbedges);
    for (int itab = 1; itab <= nbedges; itab++)
    {
      occ::handle<IGESData_IGESEntity> item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab, item);
    }
    occ::handle<IGESGeom_CompositeCurve> Comp = new IGESGeom_CompositeCurve;
    Comp->Init(Tab);
    res = Comp;
  }

  SetShapeResult(mywire, res);

  return res;
}

//=============================================================================
// TransferWire
//=============================================================================

occ::handle<IGESData_IGESEntity> BRepToIGES_BRWire ::TransferWire(
  const TopoDS_Wire&                                                              theWire,
  const TopoDS_Face&                                                              theFace,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theOriginMap,
  occ::handle<IGESData_IGESEntity>&                                               theCurve2d,
  const double                                                                    theLength)
{
  occ::handle<IGESData_IGESEntity> res;
  if (theWire.IsNull())
    return res;

  occ::handle<IGESData_IGESEntity>                                    ent3d;
  occ::handle<IGESData_IGESEntity>                                    ent2d;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Seq3d =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Seq2d =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();

  // create a 3d CompositeCurve and a 2d CompositeCurve
  TopExp_Explorer TE(theWire, TopAbs_VERTEX);
  if (TE.More())
  {
    // PTV OCC908 workaround for KAS:dev version
    /*
    BRepTools_WireExplorer WE;
    for ( WE.Init(theWire,theFace); WE.More(); WE.Next()) {
      TopoDS_Edge E = WE.Current();
      if (E.IsNull()) {
    AddWarning(theWire, "an Edge is a null entity");
      }
      else {
    ent3d = TransferEdge(E, false);
    if (!ent3d.IsNull()) Seq3d->Append(ent3d);
    ent2d = TransferEdge(E, theFace, theLength, false);
    if (!ent2d.IsNull()) Seq2d->Append(ent2d);
      }
    }
    */
    occ::handle<ShapeFix_Wire> aSFW = new ShapeFix_Wire(theWire, theFace, Precision::Confusion());
    aSFW->FixReorder();
    occ::handle<ShapeExtend_WireData> aSEWD = aSFW->WireData();
    int                               nbE   = aSEWD->NbEdges();
    for (int windex = 1; windex <= nbE; windex++)
    {
      TopoDS_Edge E = aSEWD->Edge(windex);
      if (E.IsNull())
      {
        AddWarning(theWire, "an Edge is a null entity");
      }
      else
      {
        ent3d = TransferEdge(E, theOriginMap, false);
        if (!ent3d.IsNull())
          Seq3d->Append(ent3d);
        ent2d = TransferEdge(E, theFace, theOriginMap, theLength, false);
        if (!ent2d.IsNull())
          Seq2d->Append(ent2d);
      }
    }
    // OCC908 end of workaround
  }
  else
    AddWarning(theWire, " no Vertex associated to the Wire");

  // Composite Curve 3D
  int                                                                nb3d = Seq3d->Length();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> Tab3d;
  if (nb3d == 1)
  {
    res = ent3d;
  }
  else if (nb3d >= 2)
  {
    Tab3d = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb3d);
    // int tabval = nb3d; //szv#4:S4163:12Mar99 unused
    for (int itab = 1; itab <= nb3d; itab++)
    {
      occ::handle<IGESData_IGESEntity> item = GetCasted(IGESData_IGESEntity, Seq3d->Value(itab));
      Tab3d->SetValue(itab, item);
    }
    occ::handle<IGESGeom_CompositeCurve> Comp = new IGESGeom_CompositeCurve;
    Comp->Init(Tab3d);
    res = Comp;
  }

  // Composite Curve 2D
  int                                                                nb2d = Seq2d->Length();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> Tab2d;
  if (nb2d == 1)
  {
    theCurve2d = ent2d;
  }
  else if (nb2d >= 2)
  {
    Tab2d = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb2d);
    // int tabval = nb2d; //szv#4:S4163:12Mar99 unused
    for (int itab = 1; itab <= nb2d; itab++)
    {
      occ::handle<IGESData_IGESEntity> item = GetCasted(IGESData_IGESEntity, Seq2d->Value(itab));
      Tab2d->SetValue(itab, item);
    }
    occ::handle<IGESGeom_CompositeCurve> Comp = new IGESGeom_CompositeCurve;
    Comp->Init(Tab2d);
    theCurve2d = Comp;
  }

  // In the reverted face's case find an origin by the reverted
  TopoDS_Wire aWire = !theOriginMap.IsEmpty() ? TopoDS::Wire(theOriginMap.Find(theWire)) : theWire;
  SetShapeResult(aWire, res);

  return res;
}
