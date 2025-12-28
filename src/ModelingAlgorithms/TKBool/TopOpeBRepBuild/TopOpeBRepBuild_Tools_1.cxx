// Created on: 2000-02-11
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRepCheck_Wire.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

static void CheckEdge(const TopoDS_Edge& E, const double aMaxTol);
static void CorrectEdgeTolerance(const TopoDS_Edge& myShape,
                                 const TopoDS_Face& S,
                                 const double       aMaxTol);
static bool Validate(const Adaptor3d_Curve& CRef,
                     const Adaptor3d_Curve& Other,
                     const double           Tol,
                     const bool             SameParameter,
                     double&                aNewTolerance);

//=================================================================================================

void TopOpeBRepBuild_Tools::CorrectTolerances(const TopoDS_Shape& aShape, const double aMaxTol)
{
  TopOpeBRepBuild_Tools::CorrectPointOnCurve(aShape, aMaxTol);
  TopOpeBRepBuild_Tools::CorrectCurveOnSurface(aShape, aMaxTol);
}

//=================================================================================================

void TopOpeBRepBuild_Tools::CorrectPointOnCurve(const TopoDS_Shape& S, const double aMaxTol)
{
  int                                                           i, aNb;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Edges;
  TopExp::MapShapes(S, TopAbs_EDGE, Edges);
  aNb = Edges.Extent();
  for (i = 1; i <= aNb; i++)
  {
    const TopoDS_Edge& E = TopoDS::Edge(Edges(i));
    CheckEdge(E, aMaxTol);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Tools::CorrectCurveOnSurface(const TopoDS_Shape& S, const double aMaxTol)
{
  int                                                           i, aNbFaces, j, aNbEdges;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Faces;
  TopExp::MapShapes(S, TopAbs_FACE, Faces);

  aNbFaces = Faces.Extent();
  for (i = 1; i <= aNbFaces; i++)
  {
    const TopoDS_Face&                                            F = TopoDS::Face(Faces(i));
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Edges;
    TopExp::MapShapes(F, TopAbs_EDGE, Edges);
    aNbEdges = Edges.Extent();
    for (j = 1; j <= aNbEdges; j++)
    {
      const TopoDS_Edge& E = TopoDS::Edge(Edges(j));
      CorrectEdgeTolerance(E, F, aMaxTol);
    }
  }
}

//=======================================================================
// Function : CorrectEdgeTolerance
// purpose :  Correct tolerances for Edge
//=======================================================================
void CorrectEdgeTolerance(const TopoDS_Edge& myShape, const TopoDS_Face& S, const double aMaxTol)
{
  //
  // 1. Minimum of conditions to Perform
  occ::handle<BRep_CurveRepresentation> myCref;
  occ::handle<Adaptor3d_Curve>          myHCurve;

  myCref.Nullify();

  occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
  bool Degenerated, SameParameter, SameRange;

  int unique = 0;

  Degenerated   = TE->Degenerated();
  SameParameter = TE->SameParameter();
  SameRange     = TE->SameRange();

  if (!SameRange && SameParameter)
  {
    return;
  }

  while (itcr.More())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
    if (cr->IsCurve3D())
    {
      unique++;
      if (myCref.IsNull() && !cr->Curve3D().IsNull())
      {
        myCref = cr;
      }
    }
    itcr.Next();
  }

  if (unique == 0)
  {
    return; //...No3DCurve
  }
  if (unique > 1)
  {
    return; //...Multiple3DCurve;
  }

  if (myCref.IsNull() && !Degenerated)
  {
    itcr.Initialize(TE->Curves());
    while (itcr.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
      if (cr->IsCurveOnSurface())
      {
        myCref = cr;
        break;
      }
      itcr.Next();
    }
  }

  else if (!myCref.IsNull() && Degenerated)
  {
    return; //...InvalidDegeneratedFlag;
  }

  if (!myCref.IsNull())
  {
    occ::handle<BRep_GCurve> GCref(occ::down_cast<BRep_GCurve>(myCref));
    double                   First, Last;
    GCref->Range(First, Last);
    if (Last <= First)
    {
      myCref.Nullify();
      return; // InvalidRange;
    }

    else
    {
      if (myCref->IsCurve3D())
      {
        occ::handle<Geom_Curve> C3d = occ::down_cast<Geom_Curve>(
          myCref->Curve3D()->Transformed(myCref->Location().Transformation()));
        GeomAdaptor_Curve GAC3d(C3d, First, Last);
        myHCurve = new GeomAdaptor_Curve(GAC3d);
      }
      else
      { // curve on surface
        occ::handle<Geom_Surface> Sref = myCref->Surface();
        Sref = occ::down_cast<Geom_Surface>(Sref->Transformed(myCref->Location().Transformation()));
        const occ::handle<Geom2d_Curve>& PCref   = myCref->PCurve();
        occ::handle<GeomAdaptor_Surface> GAHSref = new GeomAdaptor_Surface(Sref);
        occ::handle<Geom2dAdaptor_Curve> GHPCref = new Geom2dAdaptor_Curve(PCref, First, Last);
        Adaptor3d_CurveOnSurface         ACSref(GHPCref, GAHSref);
        myHCurve = new Adaptor3d_CurveOnSurface(ACSref);
      }
    }
  }

  //===============================================
  // 2. Tolerances in InContext
  {
    if (myCref.IsNull())
      return;
    bool ok = true;

    double Tol     = BRep_Tool::Tolerance(TopoDS::Edge(myShape));
    double aNewTol = Tol;

    double First = myHCurve->FirstParameter();
    double Last  = myHCurve->LastParameter();

    occ::handle<BRep_TFace>&         TF          = *((occ::handle<BRep_TFace>*)&S.TShape());
    const TopLoc_Location&           Floc        = S.Location();
    const TopLoc_Location&           TFloc       = TF->Location();
    const occ::handle<Geom_Surface>& Su          = TF->Surface();
    TopLoc_Location                  L           = (Floc * TFloc).Predivided(myShape.Location());
    bool                             pcurvefound = false;

    itcr.Initialize(TE->Curves());
    while (itcr.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
      if (cr != myCref && cr->IsCurveOnSurface(Su, L))
      {
        pcurvefound = true;
        occ::handle<BRep_GCurve> GC(occ::down_cast<BRep_GCurve>(cr));
        double                   f, l;
        GC->Range(f, l);
        if (SameRange && (f != First || l != Last))
        {
          return; // BRepCheck_InvalidSameRangeFlag;
        }

        occ::handle<Geom_Surface> Sb = cr->Surface();
        Sb = occ::down_cast<Geom_Surface>(Su->Transformed(L.Transformation()));
        occ::handle<Geom2d_Curve>        PC   = cr->PCurve();
        occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(Sb);
        occ::handle<Geom2dAdaptor_Curve> GHPC = new Geom2dAdaptor_Curve(PC, f, l);
        Adaptor3d_CurveOnSurface         ACS(GHPC, GAHS);
        ok = Validate(*myHCurve, ACS, Tol, SameParameter, aNewTol);
        if (ok)
        {
          // printf("(Edge,1) Tolerance=%15.10lg\n", aNewTol);
          if (aNewTol < aMaxTol)
            TE->UpdateTolerance(aNewTol);
        }
        if (cr->IsCurveOnClosedSurface())
        {
          // checkclosed = true;
          GHPC->Load(cr->PCurve2(), f, l); // same bounds
          ACS.Load(GHPC, GAHS);            // sans doute inutile
          ok = Validate(*myHCurve, ACS, Tol, SameParameter, aNewTol);
          if (ok)
          {
            if (aNewTol < aMaxTol)
              TE->UpdateTolerance(aNewTol);
          }
        }
      }
      itcr.Next();
    }

    if (!pcurvefound)
    {
      occ::handle<Geom_Plane>    P;
      occ::handle<Standard_Type> styp = Su->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        P = occ::down_cast<Geom_Plane>(
          occ::down_cast<Geom_RectangularTrimmedSurface>(Su)->BasisSurface());
      }
      else
      {
        P = occ::down_cast<Geom_Plane>(Su);
      }
      if (P.IsNull())
      {         // not a plane
        return; // BRepCheck::Add(lst,BRepCheck_NoCurveOnSurface);
      }
      else
      { // on fait la projection a la volee, comme BRep_Tool
        P = occ::down_cast<Geom_Plane>(P->Transformed(L.Transformation()));
        // on projette Cref sur ce plan
        occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(P);

        // Dub - Normalement myHCurve est une GeomAdaptor_Curve
        occ::handle<GeomAdaptor_Curve> Gac = occ::down_cast<GeomAdaptor_Curve>(myHCurve);
        occ::handle<Geom_Curve>        C3d = Gac->Curve();
        occ::handle<Geom_Curve>        ProjOnPlane =
          GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3d, First, Last),
                                      P,
                                      P->Position().Direction(),
                                      true);

        occ::handle<GeomAdaptor_Curve> aHCurve = new GeomAdaptor_Curve(ProjOnPlane);

        ProjLib_ProjectedCurve           proj(GAHS, aHCurve);
        occ::handle<Geom2d_Curve>        PC = Geom2dAdaptor::MakeCurve(proj);
        occ::handle<Geom2dAdaptor_Curve> GHPC =
          new Geom2dAdaptor_Curve(PC, myHCurve->FirstParameter(), myHCurve->LastParameter());

        Adaptor3d_CurveOnSurface ACS(GHPC, GAHS);

        ok = Validate(*myHCurve, ACS, Tol, true, aNewTol); // voir dub...
        if (ok)
        {
          if (aNewTol < aMaxTol)
            TE->UpdateTolerance(aNewTol);
        }
      }
    } // end of if (!pcurvefound) {
  } // end of  2. Tolerances in InContext
}

#define NCONTROL 23

//=================================================================================================

bool Validate(const Adaptor3d_Curve& CRef,
              const Adaptor3d_Curve& Other,
              const double           Tol,
              const bool             SameParameter,
              double&                aNewTolerance)
{
  double First, Last, MaxDistance, aD;

  First       = CRef.FirstParameter();
  Last        = CRef.LastParameter();
  MaxDistance = Tol * Tol;

  int i, aNC1 = NCONTROL - 1;

  bool aFlag = false;
  bool proj  = (!SameParameter || First != Other.FirstParameter() || Last != Other.LastParameter());
  //
  // 1.
  if (!proj)
  {
    for (i = 0; i < NCONTROL; i++)
    {
      double prm    = ((aNC1 - i) * First + i * Last) / aNC1;
      gp_Pnt pref   = CRef.Value(prm);
      gp_Pnt pother = Other.Value(prm);

      aD = pref.SquareDistance(pother);

      if (aD > MaxDistance)
      {
        MaxDistance = aD;
        aFlag       = true;
      }
    }
  }

  //
  // 2.
  else
  {
    Extrema_LocateExtPC refd, otherd;
    double              OFirst, OLast;
    OFirst = Other.FirstParameter();
    OLast  = Other.LastParameter();

    gp_Pnt pd  = CRef.Value(First);
    gp_Pnt pdo = Other.Value(OFirst);

    aD = pd.SquareDistance(pdo);
    if (aD > MaxDistance)
    {
      MaxDistance = aD;
      aFlag       = true;
    }

    pd  = CRef.Value(Last);
    pdo = Other.Value(OLast);
    aD  = pd.SquareDistance(pdo);
    if (aD > MaxDistance)
    {
      MaxDistance = aD;
      aFlag       = true;
    }

    refd.Initialize(CRef, First, Last, CRef.Resolution(Tol));
    otherd.Initialize(Other, OFirst, OLast, Other.Resolution(Tol));

    for (i = 2; i < aNC1; i++)
    {
      double rprm = ((aNC1 - i) * First + i * Last) / aNC1;
      gp_Pnt pref = CRef.Value(rprm);

      double oprm   = ((aNC1 - i) * OFirst + i * OLast) / aNC1;
      gp_Pnt pother = Other.Value(oprm);

      refd.Perform(pother, rprm);
      if (!refd.IsDone() || refd.SquareDistance() > Tol * Tol)
      {
        if (refd.IsDone())
        {
          aD = refd.SquareDistance();
          if (aD > MaxDistance)
          {
            aFlag       = true;
            MaxDistance = aD;
          }
        }
      }

      otherd.Perform(pref, oprm);
      if (!otherd.IsDone() || otherd.SquareDistance() > Tol * Tol)
      {

        if (otherd.IsDone())
        {
          aD = otherd.SquareDistance();
          if (aD > MaxDistance)
          {
            aFlag       = true;
            MaxDistance = aD;
          }
        }
      }
    }
  }

  if (aFlag)
  {
    aD            = sqrt(MaxDistance);
    aNewTolerance = aD * 1.05;
  }

  return aFlag;
}

//=======================================================================
// Function : CheckEdge
// purpose :  Correct tolerances for Vertices on Edge
//=======================================================================
void CheckEdge(const TopoDS_Edge& Ed, const double aMaxTol)
{
  TopoDS_Edge E = Ed;
  E.Orientation(TopAbs_FORWARD);

  gp_Pnt Controlp;

  TopExp_Explorer aVExp;
  aVExp.Init(E, TopAbs_VERTEX);
  for (; aVExp.More(); aVExp.Next())
  {
    TopoDS_Vertex aVertex = TopoDS::Vertex(aVExp.Current());

    occ::handle<BRep_TVertex>& TV   = *((occ::handle<BRep_TVertex>*)&aVertex.TShape());
    const gp_Pnt&              prep = TV->Pnt();

    double Tol, aD2, aNewTolerance, dd;

    Tol = BRep_Tool::Tolerance(aVertex);
    Tol = std::max(Tol, BRep_Tool::Tolerance(E));
    dd  = 0.1 * Tol;
    Tol *= Tol;

    const TopLoc_Location&                                            Eloc = E.Location();
    NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr;

    occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
    while (itcr.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr  = itcr.Value();
      const TopLoc_Location&                       loc = cr->Location();
      TopLoc_Location                              L = (Eloc * loc).Predivided(aVertex.Location());

      if (cr->IsCurve3D())
      {
        const occ::handle<Geom_Curve>& C = cr->Curve3D();
        if (!C.IsNull())
        {
          itpr.Initialize(TV->Points());
          while (itpr.More())
          {
            const occ::handle<BRep_PointRepresentation>& pr = itpr.Value();
            if (pr->IsPointOnCurve(C, L))
            {
              Controlp = C->Value(pr->Parameter());
              Controlp.Transform(L.Transformation());
              aD2 = prep.SquareDistance(Controlp);
              if (aD2 > Tol)
              {
                aNewTolerance = sqrt(aD2) + dd;
                if (aNewTolerance < aMaxTol)
                  TV->UpdateTolerance(aNewTolerance);
              }
            }
            itpr.Next();
          }

          TopAbs_Orientation orv = aVertex.Orientation();
          if (orv == TopAbs_FORWARD || orv == TopAbs_REVERSED)
          {
            occ::handle<BRep_GCurve> GC(occ::down_cast<BRep_GCurve>(cr));

            if (orv == TopAbs_FORWARD)
              Controlp = C->Value(GC->First());
            else
              Controlp = C->Value(GC->Last());

            Controlp.Transform(L.Transformation());
            aD2 = prep.SquareDistance(Controlp);

            if (aD2 > Tol)
            {
              aNewTolerance = sqrt(aD2) + dd;
              if (aNewTolerance < aMaxTol)
                TV->UpdateTolerance(aNewTolerance);
            }
          }
        }
      }
      itcr.Next();
    }
  }
}

//=================================================================================================

bool TopOpeBRepBuild_Tools::CheckFaceClosed2d(const TopoDS_Face& theFace)
{
  bool            isClosed = true;
  TopExp_Explorer ex(theFace, TopAbs_WIRE);
  for (; ex.More() && isClosed; ex.Next())
  {
    const TopoDS_Wire& aW = TopoDS::Wire(ex.Current());
    BRepCheck_Wire     aWChk(aW);
    BRepCheck_Status   aStatus = aWChk.Orientation(theFace);
    if (aStatus != BRepCheck_NoError)
      isClosed = false;
  }
  return isClosed;
}
