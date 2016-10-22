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


#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BRepTools.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Geom_Plane.hxx>
#include <Extrema_ExtSS.hxx>
#include <GeomAdaptor_Surface.hxx>
//
static Standard_Boolean CanUseEdges(const Adaptor3d_Surface& BS);
//
static void FindExactUVBounds(const TopoDS_Face F, 
                              Standard_Real& umin, Standard_Real& umax, 
                              Standard_Real& vmin, Standard_Real& vmax,
                              const Standard_Real Tol, 
                              Standard_Boolean& isNaturalRestriction);
//
static void AdjustFaceBox(const BRepAdaptor_Surface& BS, 
                          const Standard_Real umin, const Standard_Real umax, 
                          const Standard_Real vmin, const Standard_Real vmax,
                          Bnd_Box& FaceBox,
                          const Bnd_Box& EdgeBox, const Standard_Real Tol);
//
static Standard_Boolean IsModifySize(const BRepAdaptor_Surface& theBS, 
                                     const gp_Pln& thePln, const gp_Pnt& theP,
                                     const Standard_Real umin, const Standard_Real umax,
                                     const Standard_Real vmin, const Standard_Real vmax,
                                     const BRepTopAdaptor_FClass2d& theFClass,
                                     const Standard_Real theTolU, const Standard_Real theTolV);

//
//=======================================================================
//function : Add
//purpose  : Add a shape bounding to a box
//=======================================================================
void BRepBndLib::Add(const TopoDS_Shape& S, Bnd_Box& B, Standard_Boolean useTriangulation)
{
  TopExp_Explorer ex;

  // Add the faces
  BRepAdaptor_Surface BS;
  TopLoc_Location l, aDummyLoc;
  Standard_Integer i, nbNodes;
  BRepAdaptor_Curve BC;

  for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation(F, l);
    const Handle(Geom_Surface)& GS = BRep_Tool::Surface (F, aDummyLoc);
    if ((useTriangulation || GS.IsNull()) && !T.IsNull())
    {
      nbNodes = T->NbNodes();
      const TColgp_Array1OfPnt& Nodes = T->Nodes();
      for (i = 1; i <= nbNodes; i++) {
        if (l.IsIdentity()) B.Add(Nodes(i));
        else B.Add(Nodes(i).Transformed(l));
      }
      //       B.Enlarge(T->Deflection());
      B.Enlarge(T->Deflection() + BRep_Tool::Tolerance(F));
    } else
    {
      if (!GS.IsNull()) {
        BS.Initialize(F, Standard_False);
        if (BS.GetType() != GeomAbs_Plane) {
          BS.Initialize(F);
          BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
        }
        else {
          // on travaille directement sur les courbes 3d.
          TopExp_Explorer ex2(F, TopAbs_EDGE);
          if (!ex2.More()) {
            BS.Initialize(F);
            BndLib_AddSurface::Add(BS, BRep_Tool::Tolerance(F), B);
          }
          else {
            for (;ex2.More();ex2.Next()) {
              BC.Initialize(TopoDS::Edge(ex2.Current()));
              BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(F), B);
            }
            B.Enlarge(BRep_Tool::Tolerance(F));
          }
        }
      }
    }
  }

  // Add the edges not in faces
  Handle(TColStd_HArray1OfInteger) HIndices;
  Handle(Poly_PolygonOnTriangulation) Poly;
  Handle(Poly_Triangulation) T;
  for (ex.Init(S,TopAbs_EDGE,TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Handle(Poly_Polygon3D) P3d = BRep_Tool::Polygon3D(E, l);
    if (!P3d.IsNull())
    {
      const TColgp_Array1OfPnt& Nodes = P3d->Nodes();
      nbNodes = P3d->NbNodes();
      for (i = 1; i <= nbNodes; i++)
      {
        if (l.IsIdentity()) B.Add(Nodes(i));
        else B.Add(Nodes(i).Transformed(l));
      }
      //       B.Enlarge(P3d->Deflection());
      B.Enlarge(P3d->Deflection() + BRep_Tool::Tolerance(E));
    }
    else
    {
      BRep_Tool::PolygonOnTriangulation(E, Poly, T, l);
      if (useTriangulation && !Poly.IsNull())
      {
        const TColStd_Array1OfInteger& Indices = Poly->Nodes();
        const TColgp_Array1OfPnt& Nodes = T->Nodes();
        nbNodes = Indices.Length();
        for (i = 1; i <= nbNodes; i++)
        {
          if (l.IsIdentity()) B.Add(Nodes(Indices(i)));
          else B.Add(Nodes(Indices(i)).Transformed(l));
        }
        // 	B.Enlarge(T->Deflection());
        B.Enlarge(Poly->Deflection() + BRep_Tool::Tolerance(E));
      }
      else {
        if (BRep_Tool::IsGeometric(E))
        {
          BC.Initialize(E);
          BndLib_Add3dCurve::Add(BC, BRep_Tool::Tolerance(E), B);
        }
      }
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ex.More(); ex.Next()) {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
    B.Enlarge(BRep_Tool::Tolerance(TopoDS::Vertex(ex.Current())));
  }
}



//=======================================================================
//function : AddClose
//purpose  : Add a precise shape bounding to a box
//=======================================================================

void BRepBndLib::AddClose(const TopoDS_Shape& S, Bnd_Box& B)
{
  TopExp_Explorer ex;

  // No faces

  // Add the edges

  BRepAdaptor_Curve BC;

  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    BC.Initialize(TopoDS::Edge(ex.Current()));
    BndLib_Add3dCurve::Add(BC,0.,B);
  }

  // Add the vertices not in edges

  for (ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ex.More(); ex.Next()) {
    B.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
  }
}

//=======================================================================
//function : AddOptimal
//purpose  : Add a shape bounding to a box
//=======================================================================
void BRepBndLib::AddOptimal(const TopoDS_Shape& S, Bnd_Box& B, 
                            const Standard_Boolean useTriangulation, 
                            const Standard_Boolean useShapeTolerance)
{
  TopExp_Explorer ex;

  // Add the faces
  BRepAdaptor_Surface BS;
  Handle(Poly_Triangulation) T;
  TopLoc_Location l;
  Standard_Integer i, nbNodes;
  BRepAdaptor_Curve BC;

  for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    T = BRep_Tool::Triangulation(F, l);
    Bnd_Box aLocBox;
    if (useTriangulation && !T.IsNull())
    {
      nbNodes = T->NbNodes();
      const TColgp_Array1OfPnt& Nodes = T->Nodes();
      for (i = 1; i <= nbNodes; i++) {
        if (l.IsIdentity()) aLocBox.Add(Nodes(i));
        else aLocBox.Add(Nodes(i).Transformed(l));
      }
      //       B.Enlarge(T->Deflection());
      aLocBox.Enlarge(T->Deflection() + BRep_Tool::Tolerance(F));
      Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
    } 
    else
    {
      const Handle(Geom_Surface)& GS = BRep_Tool::Surface(F, l);
      if (!GS.IsNull()) {
        BS.Initialize(F, Standard_False);
        if (CanUseEdges(BS)) {
          TopExp_Explorer ex2(F, TopAbs_EDGE);
          if (!ex2.More()) {
            BS.Initialize(F);
            Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(F) : 0.;
            BndLib_AddSurface::AddOptimal(BS, Tol, aLocBox);
          }
          else
          {
            Standard_Real Tol;
            for (;ex2.More();ex2.Next()) {
              Bnd_Box anEBox;
              const TopoDS_Edge& anE = TopoDS::Edge(ex2.Current());
              if(BRep_Tool::Degenerated(anE))
              {
                continue;
              }
              BC.Initialize(anE);
              Tol = useShapeTolerance?  BRep_Tool::Tolerance(anE) : 0.;
              BndLib_Add3dCurve::AddOptimal(BC, Tol, anEBox);
              aLocBox.Add(anEBox);
            }
          }
        }
        else
        {
          Standard_Real umin, umax, vmin, vmax;
          Standard_Boolean isNaturalRestriction = Standard_False;
          Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(F) : 0.;
          FindExactUVBounds(F, umin, umax, vmin, vmax, Tol, isNaturalRestriction);
          BndLib_AddSurface::AddOptimal(BS, umin, umax, vmin, vmax, 
                                        Tol, aLocBox);
          //
          if(!isNaturalRestriction)
          {
            TopExp_Explorer ex2(F, TopAbs_EDGE);
            Bnd_Box EBox;
            for (;ex2.More();ex2.Next()) {
              Bnd_Box anEBox;
              const TopoDS_Edge& anE = TopoDS::Edge(ex2.Current());
              if(BRep_Tool::Degenerated(anE))
              {
                continue;
              }
              BC.Initialize(anE);
              Tol = useShapeTolerance?  BRep_Tool::Tolerance(anE) : 0.;
              BndLib_Add3dCurve::AddOptimal(BC, Tol, anEBox);
              EBox.Add(anEBox);
            }
            Tol = useShapeTolerance?  BRep_Tool::Tolerance(F) : 0.;
            AdjustFaceBox(BS, umin, umax, vmin, vmax, aLocBox, EBox, 
                          Tol);
          }
        }
        Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
        aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
      }
    }
  }

  // Add the edges not in faces
  Handle(TColStd_HArray1OfInteger) HIndices;
  Handle(Poly_PolygonOnTriangulation) Poly;

  for (ex.Init(S,TopAbs_EDGE,TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Bnd_Box aLocBox;
    Handle(Poly_Polygon3D) P3d = BRep_Tool::Polygon3D(E, l);
    if (useTriangulation && (!P3d.IsNull()))
    {
      const TColgp_Array1OfPnt& Nodes = P3d->Nodes();
      nbNodes = P3d->NbNodes();
      for (i = 1; i <= nbNodes; i++)
      {
        if (l.IsIdentity()) aLocBox.Add(Nodes(i));
        else aLocBox.Add(Nodes(i).Transformed(l));
      }
      Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(E) : 0.;
      aLocBox.Enlarge(P3d->Deflection() + Tol);
    }
    else
    {
      BRep_Tool::PolygonOnTriangulation(E, Poly, T, l);
      if (useTriangulation && !Poly.IsNull())
      {
        const TColStd_Array1OfInteger& Indices = Poly->Nodes();
        const TColgp_Array1OfPnt& Nodes = T->Nodes();
        nbNodes = Indices.Length();
        for (i = 1; i <= nbNodes; i++)
        {
          if (l.IsIdentity()) aLocBox.Add(Nodes(Indices(i)));
          else aLocBox.Add(Nodes(Indices(i)).Transformed(l));
        }
        Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(E) : 0.;
        aLocBox.Enlarge(Poly->Deflection() + Tol);
      }
      else {
        if (BRep_Tool::IsGeometric(E))
        {
          BC.Initialize(E);
          Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(E) : 0.;
          BndLib_Add3dCurve::AddOptimal(BC, Tol, aLocBox);
        }
      }
    }
    if (!aLocBox.IsVoid())
    {
      Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
      aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
      B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
    }
  }

  // Add the vertices not in edges

  for (ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ex.More(); ex.Next()) {
    Bnd_Box aLocBox;
    const TopoDS_Vertex& aV = TopoDS::Vertex(ex.Current());
    aLocBox.Add(BRep_Tool::Pnt(aV));
    Standard_Real Tol = useShapeTolerance?  BRep_Tool::Tolerance(aV) : 0.;
    aLocBox.Enlarge(Tol);
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    aLocBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
}

//=======================================================================
//function : CanUseEdges
//purpose  : Define is it possible using only edges bnd boxes 
//           to get face bnd box
//=======================================================================
Standard_Boolean CanUseEdges(const Adaptor3d_Surface& BS)
{
  GeomAbs_SurfaceType aST = BS.GetType();
  if(aST == GeomAbs_Plane ||
     aST == GeomAbs_Cylinder ||
     aST == GeomAbs_Cone ||
     aST == GeomAbs_SurfaceOfExtrusion)
  {
    return Standard_True;
  }
  else if(aST == GeomAbs_SurfaceOfRevolution)
  {
    const Handle(Adaptor3d_HCurve)& aBC = BS.BasisCurve();
    if(aBC->GetType() == GeomAbs_Line)
    {
      return Standard_True;
    }
    else
    {
      return Standard_False;
    }
  }
  else if(aST == GeomAbs_OffsetSurface)
  {
    const Handle(Adaptor3d_HSurface)& aS = BS.BasisSurface();
    return CanUseEdges(aS->Surface());
  }
  else if(aST == GeomAbs_BSplineSurface)
  {
    Handle(Geom_BSplineSurface) aBSpl = BS.BSpline();
    if((aBSpl->UDegree() == 1 && aBSpl->NbUKnots() == 2) ||
       (aBSpl->VDegree() == 1 && aBSpl->NbVKnots() == 2))
    {
      return Standard_True;
    }
    else
    {
      return Standard_False;
    }
  }
  else if(aST == GeomAbs_BezierSurface)
  {
    Handle(Geom_BezierSurface) aBz = BS.Bezier();
    if((aBz->UDegree() == 1 ) ||
       (aBz->VDegree() == 1 ))
    {
      return Standard_True;
    }
    else
    {
      return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : FindExactUVBounds
//purpose  : 
//=======================================================================
void FindExactUVBounds(const TopoDS_Face FF, 
                       Standard_Real& umin, Standard_Real& umax, 
                       Standard_Real& vmin, Standard_Real& vmax,
                       const Standard_Real Tol, 
                       Standard_Boolean& isNaturalRestriction)
{
  TopoDS_Face F = FF;
  F.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex(F,TopAbs_EDGE);
  //
  //Check Natural restriction
  isNaturalRestriction = BRep_Tool::NaturalRestriction(F); //Can we trust this flag?
  BRepAdaptor_Surface aBAS(F, Standard_False);
  if(!isNaturalRestriction)
  {
    //Check by comparing pcurves and surface boundaries
    umin = aBAS.FirstUParameter();
    umax = aBAS.LastUParameter();
    vmin = aBAS.FirstVParameter();
    vmax = aBAS.LastVParameter();
    Standard_Boolean isUperiodic = aBAS.IsUPeriodic(), isVperiodic = aBAS.IsVPeriodic();
    Standard_Real aT1, aT2;
    Standard_Real TolU = Max(aBAS.UResolution(Tol), Precision::PConfusion());
    Standard_Real TolV = Max(aBAS.VResolution(Tol), Precision::PConfusion());
    Standard_Integer Nu = 0, Nv = 0, NbEdges = 0;
    gp_Vec2d Du(1, 0), Dv(0, 1);
    gp_Pnt2d aP;
    gp_Vec2d aV;
    for (;ex.More();ex.Next()) {
      NbEdges++;
      if(NbEdges > 4)
      {
        break;
      }
      const TopoDS_Edge& aE = TopoDS::Edge(ex.Current());
      const Handle(Geom2d_Curve) aC2D = BRep_Tool::CurveOnSurface(aE, F, aT1, aT2);
      if (aC2D.IsNull()) 
      {
        break;
      }
      //
      aC2D->D1((aT1 + aT2)/2., aP, aV);
      Standard_Real magn = aV.SquareMagnitude();
      if(magn < gp::Resolution())
      {
        break;
      }
      else
      {
        aV /= Sqrt(magn);
      }
      Standard_Real u = aP.X(), v = aP.Y();
      if(isUperiodic)
      {
        ElCLib::InPeriod(u, umin, umax);
      }
      if(isVperiodic)
      {
        ElCLib::InPeriod(v, vmin, vmax);
      }
      //
      if(Abs(u - umin) <= TolU || Abs(u - umax) <= TolU)
      {
        Standard_Real d = Dv * aV;
        if(1. - Abs(d) <= Precision::PConfusion())
        {
          Nu++;
          if(Nu > 2)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      else if(Abs(v - vmin) <= TolV || Abs(v - vmax) <= TolV)
      {
        Standard_Real d = Du * aV;
        if(1. - Abs(d) <= Precision::PConfusion())
        {
          Nv++;
          if(Nv > 2)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    }
    if(Nu == 2 && Nv == 2)
    {
      isNaturalRestriction = Standard_True;
    }
  }
  //
  if(isNaturalRestriction)
  {
    umin = aBAS.FirstUParameter();
    umax = aBAS.LastUParameter();
    vmin = aBAS.FirstVParameter();
    vmax = aBAS.LastVParameter();
    return;
  }

  // fill box for the given face
  Standard_Real aT1, aT2;
  Standard_Real TolU = Max(aBAS.UResolution(Tol), Precision::PConfusion());
  Standard_Real TolV = Max(aBAS.VResolution(Tol), Precision::PConfusion());
  Standard_Real TolUV = Max(TolU, TolV);
  Bnd_Box2d aBox;
  ex.Init(F,TopAbs_EDGE);
  for (;ex.More();ex.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(ex.Current());
    const Handle(Geom2d_Curve) aC2D = BRep_Tool::CurveOnSurface(aE, F, aT1, aT2);
    if (aC2D.IsNull()) 
    {
      continue;
    }
    //
    BndLib_Add2dCurve::AddOptimal(aC2D, aT1, aT2, TolUV, aBox);
    //
  }
  //
  aBox.Get(umin, vmin, umax, vmax);
  //
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aS = BRep_Tool::Surface(FF, aLoc);
  Standard_Real aUmin, aUmax, aVmin, aVmax;
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);
  if(!aS->IsUPeriodic())
  {
    umin = Max(aUmin, umin);
    umax = Min(aUmax, umax);
  }
  else
  {
    if(umax - umin > aS->UPeriod())
    {
      Standard_Real delta = umax - umin - aS->UPeriod();
      umin += delta/2.;
      umax -= delta/2;
    }
  }
  //
  if(!aS->IsVPeriodic())
  {
    vmin = Max(aVmin, vmin);
    vmax = Min(aVmax, vmax);
  }
  else
  {
    if(vmax - vmin > aS->VPeriod())
    {
      Standard_Real delta = vmax - vmin - aS->VPeriod();
      vmin += delta/2.;
      vmax -= delta/2;
    }
  }
}
//=======================================================================
//function : Reorder
//purpose  : 
//=======================================================================
inline void Reorder(Standard_Real& a, Standard_Real& b)
{
  if(a > b)
  {
    Standard_Real t = a;
    a = b;
    b = t;
  }
}
//=======================================================================
//function : IsModifySize
//purpose  : 
//=======================================================================
Standard_Boolean IsModifySize(const BRepAdaptor_Surface& theBS, 
                              const gp_Pln& thePln, const gp_Pnt& theP,
                              const Standard_Real umin, const Standard_Real umax,
                              const Standard_Real vmin, const Standard_Real vmax,
                              const BRepTopAdaptor_FClass2d& theFClass,
                              const Standard_Real theTolU, const Standard_Real theTolV)
{
  Standard_Real pu1 = 0, pu2, pv1 = 0, pv2;
  ElSLib::PlaneParameters(thePln.Position(), theP, pu2, pv2);
  Reorder(pu1, pu2);
  Reorder(pv1, pv2);
  Handle(Geom_Plane) aPlane = new Geom_Plane(thePln);
  GeomAdaptor_Surface aGAPln(aPlane, pu1, pu2, pv1, pv2);
  Extrema_ExtSS anExtr(aGAPln, theBS, pu1, pu2, pv1, pv2, umin, umax, vmin, vmax, theTolU, theTolV);
  if(anExtr.IsDone())
  {
    if(anExtr.NbExt() > 0)
    {
      Standard_Integer i, imin = 0;;
      Standard_Real dmin = RealLast();
      Standard_Real uextr = 0., vextr = 0.;
      Extrema_POnSurf P1, P2;
      for(i = 1; i <= anExtr.NbExt(); ++i)
      {
        Standard_Real d = anExtr.SquareDistance(i);
        if(d < dmin)
        {
          imin = i;
          dmin = d;
        }
      }
      if(imin > 0)
      {
        anExtr.Points(imin, P1, P2);
        P2.Parameter(uextr, vextr);
      }
      else
      {
        return Standard_False;
      }
      //
      gp_Pnt2d aP2d(uextr, vextr);
      TopAbs_State aSt = theFClass.Perform(aP2d);
      if(aSt == TopAbs_OUT)
      {
        return Standard_True;
      }
    }
    else
    {
      return Standard_True; //extrema point seems to be out of face UV bounds
    }
  }
  //
  return Standard_False;
}
//
//=======================================================================
//function : AdjustFaceBox
//purpose  : 
//=======================================================================
void AdjustFaceBox(const BRepAdaptor_Surface& BS, 
                   const Standard_Real umin, const Standard_Real umax, 
                   const Standard_Real vmin, const Standard_Real vmax,
                   Bnd_Box& FaceBox,
                   const Bnd_Box& EdgeBox, const Standard_Real Tol)
{
  Standard_Real fxmin, fymin, fzmin, fxmax, fymax, fzmax;
  Standard_Real exmin, eymin, ezmin, exmax, eymax, ezmax;
  //
  FaceBox.Get(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  EdgeBox.Get(exmin, eymin, ezmin, exmax, eymax, ezmax);
  //
  Standard_Real TolU = Max(BS.UResolution(Tol), Precision::PConfusion());
  Standard_Real TolV = Max(BS.VResolution(Tol), Precision::PConfusion());
  BRepTopAdaptor_FClass2d FClass(BS.Face(), Max(TolU, TolV));
  //
  Standard_Boolean isModified = Standard_False;
  if(exmin > fxmin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DX()));
    gp_Pnt aP(fxmin, fymax, fzmax);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fxmin = exmin;
      isModified = Standard_True;
    }
  }
  if(exmax < fxmax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DX()));
    gp_Pnt aP(fxmax, fymin, fzmin);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fxmax = exmax;
      isModified = Standard_True;
    }
  }
  //
  if(eymin > fymin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DY()));
    gp_Pnt aP(fxmax, fymin, fzmax);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fymin = eymin;
      isModified = Standard_True;
    }
  }
  if(eymax < fymax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DY()));
    gp_Pnt aP(fxmin, fymax, fzmin);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fymax = eymax;
      isModified = Standard_True;
    }
  }
  //
  if(ezmin > fzmin)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmin, fymin, fzmin), gp::DZ()));
    gp_Pnt aP(fxmax, fymax, fzmin);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fzmin = ezmin;
      isModified = Standard_True;
    }
  }
  if(ezmax < fzmax)
  {
    //
    gp_Pln pl(gp_Ax3(gp_Pnt(fxmax, fymax, fzmax), gp::DZ()));
    gp_Pnt aP(fxmin, fymin, fzmax);
    if(IsModifySize(BS, pl, aP,
                    umin, umax, vmin, vmax, FClass, TolU, TolV))
    {
      fzmax = ezmax;
      isModified = Standard_True;
    }
  }
  //
  if(isModified)
  {
    FaceBox.SetVoid();
    FaceBox.Update(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
  }
}

