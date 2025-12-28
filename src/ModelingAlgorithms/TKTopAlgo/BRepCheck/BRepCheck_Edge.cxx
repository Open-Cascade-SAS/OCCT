// Created on: 1995-12-11
// Created by: Jacques GOUSSARD
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <BRep_GCurve.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProjLib.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Edge, BRepCheck_Result)

static const int NCONTROL = 23;

//=================================================================================================

BRepCheck_Edge::BRepCheck_Edge(const TopoDS_Edge& E)
{
  Init(E);
  myGctrl         = true;
  myIsExactMethod = false;
}

//=================================================================================================

void BRepCheck_Edge::Minimum()
{
  if (!myMin)
  {
    occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aNewList = new NCollection_Shared<NCollection_List<BRepCheck_Status>>();
    NCollection_List<BRepCheck_Status>&         lst      = **myMap.Bound(myShape, aNewList);
    myCref.Nullify();

    // Existence et unicite d`une representation 3D
    occ::handle<BRep_TEdge>&                          TE = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
    bool                             exist  = false;
    bool                             unique = true;
    // Search for a 3D reference. If no existent one, creates it with the
    // first encountered CurveOnSurf; if multiple, chooses the first one...

    bool Degenerated   = TE->Degenerated();
    bool SameParameter = TE->SameParameter();
    bool SameRange     = TE->SameRange();
    if (!SameRange && SameParameter)
    {
      BRepCheck::Add(lst, BRepCheck_InvalidSameParameterFlag);
    }
    //    occ::handle<Geom_Curve> C3d;

    while (itcr.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
      if (cr->IsCurve3D())
      {
        if (!exist)
        {
          exist = true;
        }
        else
        {
          unique = false;
        }
        if (myCref.IsNull() && !cr->Curve3D().IsNull())
        {
          myCref = cr;
        }
      }
      itcr.Next();
    }

    if (!exist)
    {
      BRepCheck::Add(lst, BRepCheck_No3DCurve);
      // myCref est nulle
    }
    else if (!unique)
    {
      BRepCheck::Add(lst, BRepCheck_Multiple3DCurve);
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
      BRepCheck::Add(lst, BRepCheck_InvalidDegeneratedFlag);
    }

    if (!myCref.IsNull())
    {
      occ::handle<BRep_GCurve>     GCref(occ::down_cast<BRep_GCurve>(myCref));
      constexpr double eps = Precision::PConfusion();
      double           First, Last;
      GCref->Range(First, Last);
      if (Last <= First)
      {
        myCref.Nullify();
        BRepCheck::Add(lst, BRepCheck_InvalidRange);
      }
      else
      {
        if (myCref->IsCurve3D())
        {
          // eap 6 Jun 2002 occ332
          // better transform C3d instead of transforming Surf upto C3d initial location,
          // on transformed BSpline surface 'same parameter' may seem wrong
          TopLoc_Location    L   = myShape.Location() * myCref->Location();
          occ::handle<Geom_Curve> C3d = occ::down_cast<Geom_Curve>(
            myCref->Curve3D()->Transformed(/*myCref->Location()*/ L.Transformation()));
          bool IsPeriodic = C3d->IsPeriodic();
          double    aPeriod    = RealLast();
          if (IsPeriodic)
          {
            aPeriod = C3d->Period();
          }
          double f = C3d->FirstParameter(), l = C3d->LastParameter();
          if (C3d->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
          {
            const occ::handle<Geom_Curve>& aC = occ::down_cast<Geom_TrimmedCurve>(C3d)->BasisCurve();
            f                            = aC->FirstParameter();
            l                            = aC->LastParameter();
            IsPeriodic                   = aC->IsPeriodic();
            if (IsPeriodic)
            {
              aPeriod = aC->Period();
            }
          }
          if (IsPeriodic && (Last - First > aPeriod + eps))
          {
            myCref.Nullify();
            BRepCheck::Add(lst, BRepCheck_InvalidRange);
          }
          else if (!IsPeriodic && (First < f - eps || Last > l + eps))
          {
            myCref.Nullify();
            BRepCheck::Add(lst, BRepCheck_InvalidRange);
          }
          else
          {
            GeomAdaptor_Curve GAC3d(C3d,
                                    C3d->TransformedParameter(First, L.Transformation()),
                                    C3d->TransformedParameter(Last, L.Transformation()));
            myHCurve = new GeomAdaptor_Curve(GAC3d);
          }
        }
        else
        { // curve on surface
          occ::handle<Geom_Surface> Sref = myCref->Surface();
          Sref =
            occ::down_cast<Geom_Surface>(Sref->Transformed(myCref->Location().Transformation()));
          const occ::handle<Geom2d_Curve>& PCref      = myCref->PCurve();
          bool            IsPeriodic = PCref->IsPeriodic();
          double               aPeriod    = RealLast();
          if (IsPeriodic)
          {
            aPeriod = PCref->Period();
          }
          double f = PCref->FirstParameter(), l = PCref->LastParameter();
          if (PCref->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
          {
            const occ::handle<Geom2d_Curve>& aC =
              occ::down_cast<Geom2d_TrimmedCurve>(PCref)->BasisCurve();
            f          = aC->FirstParameter();
            l          = aC->LastParameter();
            IsPeriodic = aC->IsPeriodic();
            if (IsPeriodic)
            {
              aPeriod = aC->Period();
            }
          }
          if (IsPeriodic && (Last - First > aPeriod + eps))
          {
            myCref.Nullify();
            BRepCheck::Add(lst, BRepCheck_InvalidRange);
          }
          else if (!IsPeriodic && (First < f - eps || Last > l + eps))
          {
            myCref.Nullify();
            BRepCheck::Add(lst, BRepCheck_InvalidRange);
          }
          else
          {
            occ::handle<GeomAdaptor_Surface> GAHSref = new GeomAdaptor_Surface(Sref);
            occ::handle<Geom2dAdaptor_Curve> GHPCref = new Geom2dAdaptor_Curve(PCref, First, Last);
            Adaptor3d_CurveOnSurface    ACSref(GHPCref, GAHSref);
            myHCurve = new Adaptor3d_CurveOnSurface(ACSref);
          }
        }
      }
    }
    if (lst.IsEmpty())
    {
      lst.Append(BRepCheck_NoError);
    }
    myMin = true;
  }
}

//=================================================================================================

void BRepCheck_Edge::InContext(const TopoDS_Shape& S)
{
  occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aHList;
  {
    std::unique_lock<std::mutex> aLock =
      myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
    if (myMap.IsBound(S))
    {
      return;
    }

    occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aNewList = new NCollection_Shared<NCollection_List<BRepCheck_Status>>();
    aHList                                   = *myMap.Bound(S, aNewList);
  }

  NCollection_List<BRepCheck_Status>& lst = *aHList;

  occ::handle<BRep_TEdge>& TE  = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
  double       Tol = BRep_Tool::Tolerance(TopoDS::Edge(myShape));

  TopAbs_ShapeEnum styp = S.ShapeType();
  //  for (TopExp_Explorer exp(S,TopAbs_EDGE); exp.More(); exp.Next()) {
  TopExp_Explorer exp(S, TopAbs_EDGE);
  for (; exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(myShape))
    {
      break;
    }
  }
  if (!exp.More())
  {
    BRepCheck::Add(lst, BRepCheck_SubshapeNotInShape);
    return;
  }

  switch (styp)
  {
    case TopAbs_WIRE: {
    }
    break;

    case TopAbs_FACE:
      if (!myCref.IsNull())
      {

        bool SameParameter = TE->SameParameter();
        bool SameRange     = TE->SameRange();
        //  Modified by skv - Tue Apr 27 11:48:13 2004 Begin
        if (!SameParameter || !SameRange)
        {
          if (!SameParameter)
            BRepCheck::Add(lst, BRepCheck_InvalidSameParameterFlag);
          if (!SameRange)
            BRepCheck::Add(lst, BRepCheck_InvalidSameRangeFlag);

          return;
        }
        //  Modified by skv - Tue Apr 27 11:48:14 2004 End
        double First = myHCurve->FirstParameter();
        double Last  = myHCurve->LastParameter();

        occ::handle<BRep_TFace>&         TF          = *((occ::handle<BRep_TFace>*)&S.TShape());
        const TopLoc_Location&      Floc        = S.Location();
        const TopLoc_Location&      TFloc       = TF->Location();
        const occ::handle<Geom_Surface>& Su          = TF->Surface();
        TopLoc_Location             L           = (Floc * TFloc).Predivided(myShape.Location());
        TopLoc_Location             LE          = myShape.Location() * myCref->Location();
        const gp_Trsf&              Etrsf       = LE.Transformation();
        bool            pcurvefound = false;

        NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
        constexpr double                      eps           = Precision::PConfusion();
        const bool                       toRunParallel = myMutex != nullptr;
        while (itcr.More())
        {
          const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
          if (cr != myCref && cr->IsCurveOnSurface(Su, L))
          {
            pcurvefound = true;
            occ::handle<BRep_GCurve> GC(occ::down_cast<BRep_GCurve>(cr));
            double       f, l;
            GC->Range(f, l);
            double ff = f, ll = l;
            if (myCref->IsCurve3D())
            {
              ff = myCref->Curve3D()->TransformedParameter(f, Etrsf);
              ll = myCref->Curve3D()->TransformedParameter(l, Etrsf);
            }
            // gka OCC
            //  Modified by skv - Tue Apr 27 11:50:35 2004 Begin
            if (std::abs(ff - First) > eps || std::abs(ll - Last) > eps)
            {
              BRepCheck::Add(lst, BRepCheck_InvalidSameRangeFlag);
              BRepCheck::Add(lst, BRepCheck_InvalidSameParameterFlag);
            }
            //  Modified by skv - Tue Apr 27 11:50:37 2004 End
            //
            const occ::handle<Geom2d_Curve>& pc         = cr->PCurve();
            bool            IsPeriodic = pc->IsPeriodic();
            double               aPeriod    = RealLast();
            if (IsPeriodic)
            {
              aPeriod = pc->Period();
            }
            double fp = pc->FirstParameter(), lp = pc->LastParameter();
            if (pc->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
            {
              const occ::handle<Geom2d_Curve> aC =
                occ::down_cast<Geom2d_TrimmedCurve>(pc)->BasisCurve();
              fp         = aC->FirstParameter();
              lp         = aC->LastParameter();
              IsPeriodic = aC->IsPeriodic();
              if (IsPeriodic)
              {
                aPeriod = aC->Period();
              }
            }
            if (IsPeriodic && (l - f > aPeriod + eps))
            {
              BRepCheck::Add(lst, BRepCheck_InvalidRange);
              return;
            }
            else if (!IsPeriodic && (f < fp - eps || l > lp + eps))
            {
              BRepCheck::Add(lst, BRepCheck_InvalidRange);
              return;
            }

            if (myGctrl)
            {
              occ::handle<Geom_Surface> Sb = cr->Surface();
              Sb                      = Handle(Geom_Surface)::DownCast
                //	      (Su->Transformed(L.Transformation()));
                (Su->Transformed(/*L*/ (Floc * TFloc).Transformation()));
              occ::handle<Geom2d_Curve>             PC   = cr->PCurve();
              occ::handle<GeomAdaptor_Surface>      GAHS = new GeomAdaptor_Surface(Sb);
              occ::handle<Geom2dAdaptor_Curve>      GHPC = new Geom2dAdaptor_Curve(PC, f, l);
              occ::handle<Adaptor3d_CurveOnSurface> ACS  = new Adaptor3d_CurveOnSurface(GHPC, GAHS);

              BRepLib_ValidateEdge aValidateEdge(myHCurve, ACS, SameParameter);
              aValidateEdge.SetExitIfToleranceExceeded(Tol);
              aValidateEdge.SetExactMethod(myIsExactMethod);
              aValidateEdge.SetParallel(toRunParallel);
              aValidateEdge.Process();
              if (!aValidateEdge.IsDone() || !aValidateEdge.CheckTolerance(Tol))
              {
                if (cr->IsCurveOnClosedSurface())
                {
                  BRepCheck::Add(lst, BRepCheck_InvalidCurveOnClosedSurface);
                }
                else
                {
                  BRepCheck::Add(lst, BRepCheck_InvalidCurveOnSurface);
                }
                //  Modified by skv - Tue Apr 27 11:53:00 2004 Begin
                BRepCheck::Add(lst, BRepCheck_InvalidSameParameterFlag);
                // 	      if (SameParameter) {
                // 		BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
                // 	      }
                //  Modified by skv - Tue Apr 27 11:53:01 2004 End
              }
              if (cr->IsCurveOnClosedSurface())
              {
                GHPC->Load(cr->PCurve2(), f, l); // same bounds
                ACS->Load(GHPC, GAHS);           // sans doute inutile

                BRepLib_ValidateEdge aValidateEdgeOnClosedSurf(myHCurve, ACS, SameParameter);
                aValidateEdgeOnClosedSurf.SetExitIfToleranceExceeded(Tol);
                aValidateEdgeOnClosedSurf.SetExactMethod(myIsExactMethod);
                aValidateEdgeOnClosedSurf.SetParallel(toRunParallel);
                aValidateEdgeOnClosedSurf.Process();
                if (!aValidateEdgeOnClosedSurf.IsDone()
                    || !aValidateEdgeOnClosedSurf.CheckTolerance(Tol))
                {
                  BRepCheck::Add(lst, BRepCheck_InvalidCurveOnClosedSurface);
                  //  Modified by skv - Tue Apr 27 11:53:20 2004 Begin
                  if (SameParameter)
                  {
                    BRepCheck::Add(lst, BRepCheck_InvalidSameParameterFlag);
                  }
                  //  Modified by skv - Tue Apr 27 11:53:23 2004 End
                }
              }
            }
          }
          itcr.Next();
        }

        if (!pcurvefound)
        {
          occ::handle<Geom_Plane>    P;
          occ::handle<Standard_Type> dtyp = Su->DynamicType();
          if (dtyp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
          {
            P = occ::down_cast<Geom_Plane>(
              occ::down_cast<Geom_RectangularTrimmedSurface>(Su)->BasisSurface());
          }
          else
          {
            P = occ::down_cast<Geom_Plane>(Su);
          }
          if (P.IsNull())
          { // not a plane
            BRepCheck::Add(lst, BRepCheck_NoCurveOnSurface);
          }
          else
          { // on fait la projection a la volee, comme BRep_Tool
            // plan en position
            if (myGctrl)
            {
              P = occ::down_cast<Geom_Plane>(
                P->Transformed(/*L*/ (Floc * TFloc).Transformation())); // eap occ332
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

              ProjLib_ProjectedCurve      proj(GAHS, aHCurve);
              occ::handle<Geom2d_Curve>        PC = Geom2dAdaptor::MakeCurve(proj);
              occ::handle<Geom2dAdaptor_Curve> GHPC =
                new Geom2dAdaptor_Curve(PC, myHCurve->FirstParameter(), myHCurve->LastParameter());

              occ::handle<Adaptor3d_CurveOnSurface> ACS = new Adaptor3d_CurveOnSurface(GHPC, GAHS);

              BRepLib_ValidateEdge aValidateEdgeProj(myHCurve, ACS, SameParameter);
              aValidateEdgeProj.SetExitIfToleranceExceeded(Tol);
              aValidateEdgeProj.SetExactMethod(myIsExactMethod);
              aValidateEdgeProj.SetParallel(toRunParallel);
              aValidateEdgeProj.Process();
              if (!aValidateEdgeProj.IsDone() || !aValidateEdgeProj.CheckTolerance(Tol))
              {
                BRepCheck::Add(lst, BRepCheck_InvalidCurveOnSurface);
              }
            }
          }
        }
      }
      break;
    case TopAbs_SOLID: {
      // on verifie que l`edge est bien connectee 2 fois (pas de bord libre)
      int nbconnection = 0;
      // TopExp_Explorer exp;
      for (exp.Init(S, TopAbs_FACE); exp.More(); exp.Next())
      {
        const TopoDS_Face& fac = TopoDS::Face(exp.Current());
        TopExp_Explorer    exp2;
        for (exp2.Init(fac, TopAbs_EDGE); exp2.More(); exp2.Next())
        {
          if (exp2.Current().IsSame(myShape))
          {
            nbconnection++;
          }
        }
      }
      if (nbconnection < 2 && !TE->Degenerated())
      {
        BRepCheck::Add(lst, BRepCheck_FreeEdge);
      }
      else if (nbconnection > 2)
      {
        BRepCheck::Add(lst, BRepCheck_InvalidMultiConnexity);
      }
      else
      {
        BRepCheck::Add(lst, BRepCheck_NoError);
      }
    }
    break;
    default:
      break;
  }
  if (lst.IsEmpty())
  {
    lst.Append(BRepCheck_NoError);
  }
}

//=================================================================================================

void BRepCheck_Edge::Blind()
{
  //  Modified by skv - Tue Apr 27 11:36:01 2004 Begin
  // The body of this function is removed because of its useless.
  if (!myBlind)
  {
    myBlind = true;
  }
  //  Modified by skv - Tue Apr 27 11:36:02 2004 End
}

//=================================================================================================

void BRepCheck_Edge::GeometricControls(const bool B)
{
  myGctrl = B;
}

//=================================================================================================

bool BRepCheck_Edge::GeometricControls() const
{
  return myGctrl;
}

//=================================================================================================

void BRepCheck_Edge::SetStatus(const BRepCheck_Status theStatus)
{
  std::unique_lock<std::mutex> aLock =
    myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
  BRepCheck::Add(*myMap(myShape), theStatus);
}

//=================================================================================================

double BRepCheck_Edge::Tolerance()
{
  occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
  int    it, iRep = 1, nbRep = (TE->Curves()).Extent();
  if (nbRep <= 1)
  {
    return Precision::Confusion();
  }
  NCollection_Array1<occ::handle<Standard_Transient>> theRep(1, nbRep * 2);
  double             First, Last;
  if (!myHCurve.IsNull())
  {
    First = myHCurve->FirstParameter();
    Last  = myHCurve->LastParameter();
  }
  else
  {
    BRep_Tool::Range(TopoDS::Edge(myShape), First, Last);
  }

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
  for (; itcr.More(); itcr.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
    if (cr->IsCurve3D() && !TE->Degenerated())
    {
      //// modified by jgv, 20.03.03 ////
      TopLoc_Location    Loc = myShape.Location() * cr->Location();
      occ::handle<Geom_Curve> C3d =
        occ::down_cast<Geom_Curve>(cr->Curve3D()->Transformed(Loc.Transformation()));
      ///////////////////////////////////
      GeomAdaptor_Curve GAC3d(C3d, First, Last);
      it = iRep;
      if (iRep > 1)
      {
        theRep(iRep) = theRep(1);
        it           = 1;
      }
      theRep(it) = new GeomAdaptor_Curve(GAC3d);
      iRep++;
    }
    else if (cr->IsCurveOnSurface())
    {
      {
        occ::handle<Geom_Surface> Sref = cr->Surface();
        //// modified by jgv, 20.03.03 ////
        TopLoc_Location Loc = myShape.Location() * cr->Location();
        Sref = occ::down_cast<Geom_Surface>(Sref->Transformed(Loc.Transformation()));
        ///////////////////////////////////
        const occ::handle<Geom2d_Curve>& PCref   = cr->PCurve();
        occ::handle<GeomAdaptor_Surface> GAHSref = new GeomAdaptor_Surface(Sref);
        occ::handle<Geom2dAdaptor_Curve> GHPCref = new Geom2dAdaptor_Curve(PCref, First, Last);
        Adaptor3d_CurveOnSurface    ACSref(GHPCref, GAHSref);
        theRep(iRep) = new Adaptor3d_CurveOnSurface(ACSref);
        iRep++;
      }
      if (cr->IsCurveOnClosedSurface())
      {
        occ::handle<Geom_Surface> Sref = cr->Surface();
        Sref = occ::down_cast<Geom_Surface>(Sref->Transformed(cr->Location().Transformation()));
        const occ::handle<Geom2d_Curve>& PCref   = cr->PCurve2();
        occ::handle<GeomAdaptor_Surface> GAHSref = new GeomAdaptor_Surface(Sref);
        occ::handle<Geom2dAdaptor_Curve> GHPCref = new Geom2dAdaptor_Curve(PCref, First, Last);
        Adaptor3d_CurveOnSurface    ACSref(GHPCref, GAHSref);
        theRep(iRep) = new Adaptor3d_CurveOnSurface(ACSref);
        iRep++;
        nbRep++;
      }
    }
    else
    {
      nbRep--;
    }
  }

  double    dist2, tol2, tolCal = 0., prm;
  gp_Pnt           center, othP;
  int i;
  for (i = 0; i < NCONTROL; i++)
  {
    prm  = ((NCONTROL - 1 - i) * First + i * Last) / (NCONTROL - 1);
    tol2 = dist2 = 0.;
    center       = (*(occ::handle<Adaptor3d_Curve>*)&theRep(1))->Value(prm);
    if (Precision::IsInfinite(center.X()) || Precision::IsInfinite(center.Y())
        || Precision::IsInfinite(center.Z()))
    {
      return Precision::Infinite();
    }
    for (iRep = 2; iRep <= nbRep; iRep++)
    {
      othP = (*(occ::handle<Adaptor3d_Curve>*)&theRep(iRep))->Value(prm);
      if (Precision::IsInfinite(othP.X()) || Precision::IsInfinite(othP.Y())
          || Precision::IsInfinite(othP.Z()))
      {
        return Precision::Infinite();
      }
      dist2 = center.SquareDistance(othP);
      if (dist2 > tolCal)
        tolCal = dist2;
    }
    if (tol2 > tolCal)
    {
      tolCal = tol2;
    }
  }
  // On prend 5% de marge car au dessus on crontrole severement
  return sqrt(tolCal) * 1.05;
}

//=================================================================================================

BRepCheck_Status BRepCheck_Edge::CheckPolygonOnTriangulation(const TopoDS_Edge& theEdge)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>& aListOfCR =
    (*((occ::handle<BRep_TEdge>*)&theEdge.TShape()))->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator anITCR(aListOfCR);

  BRepAdaptor_Curve aBC;
  aBC.Initialize(theEdge);

  if (!aBC.Is3DCurve())
    return BRepCheck_NoError;

  while (anITCR.More())
  {
    if (!anITCR.Value()->IsPolygonOnTriangulation())
    {
      anITCR.Next();
      continue;
    }

    const occ::handle<BRep_CurveRepresentation> aCR = anITCR.Value();
    occ::handle<BRep_PolygonOnTriangulation>    aPT(occ::down_cast<BRep_PolygonOnTriangulation>(aCR));

    const TopLoc_Location aLL   = theEdge.Location() * aPT->Location();
    const gp_Trsf         aTrsf = aLL;

    const occ::handle<Poly_Triangulation>          aTriang   = aCR->Triangulation();
    const occ::handle<Poly_PolygonOnTriangulation> aPOnTriag = aCR->IsPolygonOnClosedTriangulation()
                                                            ? aCR->PolygonOnTriangulation2()
                                                            : aCR->PolygonOnTriangulation();
    const NCollection_Array1<int>&            anIndices = aPOnTriag->Nodes();
    const int                    aNbNodes  = anIndices.Length();

    const double aTol = aPOnTriag->Deflection() + BRep_Tool::Tolerance(theEdge);

    if (aPOnTriag->HasParameters())
    {
      for (int i = aPOnTriag->Parameters()->Lower();
           i <= aPOnTriag->Parameters()->Upper();
           i++)
      {
        const double aParam = aPOnTriag->Parameters()->Value(i);
        const gp_Pnt        aPE(aBC.Value(aParam));
        const gp_Pnt        aPnt(aTriang->Node(anIndices(i)).Transformed(aTrsf));

        const double aSQDist = aPE.SquareDistance(aPnt);
        if (aSQDist > aTol * aTol)
        {
          return BRepCheck_InvalidPolygonOnTriangulation;
        }
      }
    }
    else
    {
      // If aPOnTriag does not have any parameters we will check if it
      // inscribes into Bounding box, which is built on the edge triangulation.

      Bnd_Box aB;

      for (int i = 1; i <= aNbNodes; i++)
      {
        if (aTrsf.Form() == gp_Identity)
        {
          aB.Add(aTriang->Node(anIndices(i)));
        }
        else
        {
          aB.Add(aTriang->Node(anIndices(i)).Transformed(aTrsf));
        }
      }

      aB.Enlarge(aTol);

      double aFP = aBC.FirstParameter();
      double aLP = aBC.LastParameter();

      const double aStep = (aLP - aFP) / IntToReal(NCONTROL);
      gp_Pnt              aP;
      double       aPar = aFP;

      for (int i = 1; i < NCONTROL; i++)
      {
        aBC.D0(aPar, aP);
        if (aB.IsOut(aP))
        {
          return BRepCheck_InvalidPolygonOnTriangulation;
        }

        aPar += aStep;
      }

      aBC.D0(aLP, aP);
      if (aB.IsOut(aP))
      {
        return BRepCheck_InvalidPolygonOnTriangulation;
      }
    }

    anITCR.Next();
  }

  return BRepCheck_NoError;
}
