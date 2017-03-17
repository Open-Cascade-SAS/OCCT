// Created on: 1996-04-23
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepFeat.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_PntFace.hxx>
#include <Precision.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_MapOfShape.hxx>

#define NECHANTBARYC 11

//=======================================================================
//function : SampleEdges
//purpose  : 
//=======================================================================

void BRepFeat::SampleEdges(const TopoDS_Shape& theShape,
                           TColgp_SequenceOfPnt& theSeq)
{
  LocOpe::SampleEdges(theShape,theSeq);
}



//=======================================================================
//function : Barycenter
//purpose  : Calcul du barycentre des edges d'un shape
//=======================================================================

void BRepFeat::Barycenter(const TopoDS_Shape& S, 
                          gp_Pnt& B)
{
  TopTools_MapOfShape theMap;
  TopExp_Explorer exp(S,TopAbs_EDGE);
  TopLoc_Location Loc;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
  gp_XYZ Bar(0.,0.,0.);
  Standard_Integer i, nbp= 0;

  for (; exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg)) {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg)) {
      C = BRep_Tool::Curve(edg,Loc,f,l);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
      for (i=1;i<NECHANTBARYC; i++) {
        prm = ((NECHANTBARYC-i)*f + i*l)/NECHANTBARYC;
        Bar += C->Value(prm).XYZ();
        nbp++;
      }
    }
  }
  // Adds every vertex
  for (exp.Init(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (theMap.Add(exp.Current())) {
      Bar += (BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()))).XYZ();
      nbp++;
    }
  }

  Bar.Divide((Standard_Real)nbp);
  B.SetXYZ(Bar);
}


//=======================================================================
//function : ParametricBarycenter
//purpose  : Calcul du barycentre "parametrique" shape sur une courbe
//=======================================================================

Standard_Real BRepFeat::ParametricBarycenter(const TopoDS_Shape& S, 
                                             const Handle(Geom_Curve)& CC)
{
  TopTools_MapOfShape theMap;
  TopExp_Explorer exp(S,TopAbs_EDGE);
  TopLoc_Location Loc;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
  Standard_Integer i, nbp= 0;
  GeomAdaptor_Curve TheCurve(CC);
  Extrema_ExtPC extpc;
  extpc.Initialize(TheCurve,CC->FirstParameter(),CC->LastParameter());
  Standard_Real parbar = 0;

  for (; exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg)) {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg)) {
      C = BRep_Tool::Curve(edg,Loc,f,l);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
      for (i=1;i<NECHANTBARYC; i++) {
        prm = ((NECHANTBARYC-i)*f + i*l)/NECHANTBARYC;
        gp_Pnt pone = C->Value(prm);
        // On projette sur CC
        extpc.Perform(pone);
        if (extpc.IsDone() && extpc.NbExt() >= 1) {
          Standard_Real Dist2Min = extpc.SquareDistance(1);
          Standard_Integer kmin = 1;
          for (Standard_Integer k=2; k<=extpc.NbExt(); k++) {
            Standard_Real Dist2 = extpc.SquareDistance(k);
            if (Dist2 < Dist2Min) {
              Dist2Min = Dist2;
              kmin = k;
            }
          }
          nbp++;
          Standard_Real prmp = extpc.Point(kmin).Parameter();
          parbar += prmp;
        }
      }
    }
  }
  // Adds every vertex
  for (exp.Init(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (theMap.Add(exp.Current())) {
      gp_Pnt pone = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
      // On projette sur CC
      extpc.Perform(pone);
      if (extpc.IsDone() && extpc.NbExt() >= 1) {
        Standard_Real Dist2Min = extpc.SquareDistance(1);
        for (Standard_Integer k=2; k<=extpc.NbExt(); k++) {
          Standard_Real Dist2 = extpc.SquareDistance(k);
          if (Dist2 < Dist2Min) {
            Dist2Min = Dist2;
          }
        }
        nbp++;
      }          
    }
  }

  parbar /=((Standard_Real)nbp);
  return parbar;
}


//=======================================================================
//function : ParametricBarycenter
//purpose  : Calcul du barycentre "parametrique" shape sur une courbe
//=======================================================================

void BRepFeat::ParametricMinMax(const TopoDS_Shape& S, 
                                const Handle(Geom_Curve)& CC,
                                Standard_Real& prmin,
                                Standard_Real& prmax,
                                Standard_Real& prbmin,
                                Standard_Real& prbmax,
                                Standard_Boolean& flag,
                                const Standard_Boolean theOri)
{
  LocOpe_CSIntersector ASI(S);
  TColGeom_SequenceOfCurve scur;
  scur.Append(CC);
  ASI.Perform(scur);
  if(ASI.IsDone() && ASI.NbPoints(1) >=1) {
    if (!theOri) {
      prmin = Min(ASI.Point(1,1).Parameter(), 
                  ASI.Point(1, ASI.NbPoints(1)).Parameter());
      prmax = Max(ASI.Point(1,1).Parameter(), 
                  ASI.Point(1, ASI.NbPoints(1)).Parameter());
    }
    else {
      TopAbs_Orientation Ori = ASI.Point(1,1).Orientation();
      if (Ori ==  TopAbs_FORWARD) {
        prmin = ASI.Point(1,1).Parameter();
        prmax = ASI.Point(1, ASI.NbPoints(1)).Parameter();
      }
      else {
        prmax = ASI.Point(1,1).Parameter();
        prmin = ASI.Point(1, ASI.NbPoints(1)).Parameter();
      }
    }
    flag = Standard_True;
  }
  else {
    prmax = RealFirst();
    prmin = RealLast();
    flag = Standard_False; 
  }

  TopTools_MapOfShape theMap;
  TopExp_Explorer exp(S,TopAbs_EDGE);
  TopLoc_Location Loc;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
//  Standard_Integer i, nbp= 0;
  Standard_Integer i;
  GeomAdaptor_Curve TheCurve(CC);
  Extrema_ExtPC extpc;
  extpc.Initialize(TheCurve,CC->FirstParameter(),CC->LastParameter());
  prbmin = RealLast();
  prbmax = RealFirst();
  for (; exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg)) {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg)) {
      C = BRep_Tool::Curve(edg,Loc,f,l);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(Loc.Transformation()));
      for (i=1;i<NECHANTBARYC; i++) {
        prm = ((NECHANTBARYC-i)*f + i*l)/NECHANTBARYC;
        gp_Pnt pone = C->Value(prm);
        // On projette sur CC
        extpc.Perform(pone);
        if (extpc.IsDone() && extpc.NbExt() >= 1) {
          Standard_Real Dist2Min = extpc.SquareDistance(1);
          Standard_Integer kmin = 1;
          for (Standard_Integer k=2; k<=extpc.NbExt(); k++) {
            Standard_Real Dist2 = extpc.SquareDistance(k);
            if (Dist2 < Dist2Min) {
              Dist2Min = Dist2;
              kmin = k;
            }
          }
          Standard_Real prmp = extpc.Point(kmin).Parameter();
          if (prmp <= prbmin) {
            prbmin = prmp;
          }
          if (prmp >= prbmax) {
            prbmax = prmp;
          }
        }          
      }
    }
  }
  // Adds every vertex
  for (exp.Init(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (theMap.Add(exp.Current())) {
      gp_Pnt pone = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
      // On projette sur CC
      extpc.Perform(pone);
      if (extpc.IsDone() && extpc.NbExt() >= 1) {
        Standard_Real Dist2Min = extpc.SquareDistance(1);
        Standard_Integer kmin = 1;
        for (Standard_Integer k=2; k<=extpc.NbExt(); k++) {
          Standard_Real Dist2 = extpc.SquareDistance(k);
          if (Dist2 < Dist2Min) {
            Dist2Min = Dist2;
            kmin = k;
          }
        }
        Standard_Real prmp = extpc.Point(kmin).Parameter();
        if (prmp <= prbmin) {
          prbmin = prmp;
        }
        if (prmp >= prbmax) {
          prbmax = prmp;
        }
      }          
    }
  }
}




//=======================================================================
//function : IsIn
//purpose  : 
//=======================================================================

static Standard_Boolean IsIn (BRepTopAdaptor_FClass2d& FC,
                              Geom2dAdaptor_Curve      AC)
{
 Standard_Real Def = 100*Precision::Confusion();
 GCPnts_QuasiUniformDeflection QU(AC,Def);

 for (Standard_Integer i = 1; i <= QU.NbPoints(); i++) {
   gp_Pnt2d P = AC.Value(QU.Parameter(i));
   if (FC.Perform(P, Standard_False) == TopAbs_OUT) {
     return Standard_False;
   } 
 }
 return Standard_True;
}


//=======================================================================
//function : PutInBoundsU
//purpose  : Recadre la courbe 2d dans les bounds de la face
//=======================================================================
  //---------------
  // Recadre en U.
  //---------------

static void PutInBoundsU (Standard_Real      umin,
                          Standard_Real      umax,
                          Standard_Real      eps,
                          Standard_Real      period,
                          Standard_Real      f,
                          Standard_Real      l,
                          Handle(Geom2d_Curve)&       C2d)
{
  gp_Pnt2d      Pf      = C2d->Value(f);
  gp_Pnt2d      Pl      = C2d->Value(l);
  gp_Pnt2d      Pm      = C2d->Value(0.34*f + 0.66*l);
  Standard_Real minC    = Min(Pf.X(),Pl.X()); minC = Min(minC,Pm.X());
  Standard_Real maxC    = Max(Pf.X(),Pl.X()); maxC = Max(maxC,Pm.X());
  Standard_Real du = 0.;
  if (minC< umin - eps) {
    du = (int((umin - minC)/period) + 1)*period;
  }
  if (minC > umax + eps) {
    du = -(int((minC - umax)/period) + 1)*period;
  }
  if (du != 0) {
    gp_Vec2d T1(du,0.);
    C2d->Translate(T1);
    minC += du; maxC += du;
  }
    // Ajuste au mieux la courbe dans le domaine.
  if (maxC > umax +100*eps) {
    Standard_Real d1 = maxC - umax;
    Standard_Real d2 = umin - minC + period;
    if (d2 < d1) du =-period;
    if ( du != 0.) {
      gp_Vec2d T2(du,0.);
      C2d->Translate(T2);
    }
  }
} 


//=======================================================================
//function : PutInBoundsU
//purpose  : Recadre la courbe 2d dans les bounds de la face
//=======================================================================
  //---------------
  // Recadre en V.
  //---------------

static void PutInBoundsV (Standard_Real     vmin,
                          Standard_Real      vmax,
                          Standard_Real      eps,
                          Standard_Real      period,
                          Standard_Real      f,
                          Standard_Real      l,
                          Handle(Geom2d_Curve)&       C2d)
{
  gp_Pnt2d      Pf      = C2d->Value(f);
  gp_Pnt2d      Pl      = C2d->Value(l);
  gp_Pnt2d      Pm      = C2d->Value(0.34*f + 0.66*l);
  Standard_Real minC    = Min(Pf.Y(),Pl.Y()); minC = Min(minC,Pm.Y());
  Standard_Real maxC    = Max(Pf.Y(),Pl.Y()); maxC = Max(maxC,Pm.Y());
  Standard_Real dv = 0.;
  if (minC< vmin - eps) {
    dv = (int((vmin - minC)/period) + 1)*period;
  }
  if (minC > vmax + eps) {
    dv = -(int((minC - vmax)/period) + 1)*period;
  }
  if (dv != 0) {
    gp_Vec2d T1(0.,dv);
    C2d->Translate(T1);
    minC += dv; maxC += dv;
  }
  // Ajuste au mieux la courbe dans le domaine.
  if (maxC > vmax +100*eps) {
    Standard_Real d1 = maxC - vmax;
    Standard_Real d2 = vmin - minC + period;
    if (d2 < d1) dv =-period;
    if ( dv != 0.) {
      gp_Vec2d T2(0.,dv);
      C2d->Translate(T2);
    }
  }
}


//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================


Standard_Boolean BRepFeat::IsInside(const TopoDS_Face& F1,
                                    const TopoDS_Face& F2)
{
  TopExp_Explorer exp;
  exp.Init(F1, TopAbs_EDGE);

  Standard_Real   umin,umax,vmin,vmax, uperiod=0, vperiod=0;
  Standard_Integer flagu = 0, flagv = 0; 
  TopLoc_Location L; // Recup S avec la location pour eviter la copie.
  Handle (Geom_Surface) S   = BRep_Tool::Surface(F2);
//  Standard_Real periodu, periodv; 
  BRepTools::UVBounds(F2,umin,umax,vmin,vmax);

  if (S->IsUPeriodic()) {
    flagu = 1;
    uperiod = S->UPeriod();
  }

  if (S->IsVPeriodic()) {
    flagv = 1;
    vperiod = S->VPeriod();
  }
  TopoDS_Shape aLocalShape = F2.Oriented(TopAbs_FORWARD);
  BRepTopAdaptor_FClass2d FC (TopoDS::Face(aLocalShape),Precision::Confusion());
//  BRepTopAdaptor_FClass2d FC (TopoDS::Face(F2.Oriented(TopAbs_FORWARD)),
//                                Precision::Confusion());
  for(; exp.More(); exp.Next()) {
    Standard_Real   f1,l1;
    Handle(Geom_Curve) C0 = BRep_Tool::Curve(TopoDS::Edge(exp.Current()),f1,l1);
    Handle(Geom2d_Curve) C = GeomProjLib::Curve2d(C0,f1,l1,S);        
    TopoDS_Edge E = TopoDS::Edge(exp.Current());
    if(flagu == 1 || flagv == 1) {
      Standard_Real   eps = BRep_Tool::Tolerance(E);
      BRep_Tool::Range(E,f1,l1);  
      if(flagu == 1) PutInBoundsU(umin, umax, eps, uperiod, f1, l1, C); 
      if(flagv == 1) PutInBoundsV(vmin, vmax, eps, vperiod, f1, l1, C); 
    }
    Geom2dAdaptor_Curve  AC(C,f1,l1);
    if (!IsIn(FC,AC)) {
      return Standard_False;
    }
  }
  return Standard_True;
}



//=======================================================================
//function : FaceUntil
//purpose  : 
//=======================================================================


void BRepFeat::FaceUntil(const TopoDS_Shape& Sbase,
                         TopoDS_Face& FUntil)
{
  Bnd_Box B;
  BRepBndLib::Add(Sbase,B);
  Standard_Real x[2], y[2], z[2];
  B.Get(x[0],y[0],z[0],x[1],y[1],z[1]);
  Standard_Real diam = 10.*Sqrt(B.SquareExtent());
  
  Handle(Geom_Surface) s = BRep_Tool::Surface(FUntil);
  Handle(Standard_Type) styp = s->DynamicType();
  if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    s = Handle(Geom_RectangularTrimmedSurface)::DownCast(s)->BasisSurface();
    styp = s->DynamicType();
  }
  Handle(Geom_RectangularTrimmedSurface) str;
  if (styp == STANDARD_TYPE(Geom_Plane)) {
    gp_Pln aPln = Handle(Geom_Plane)::DownCast(s)->Pln();
    Standard_Real u, v, umin = RealLast(), umax = -umin,
                        vmin = RealLast(), vmax = -vmin;
    for(Standard_Integer i = 0 ; i < 2; i++) 
    {
      for(Standard_Integer j = 0; j < 2; j++)
      {
        for(Standard_Integer k = 0; k < 2; k++)
        {
          gp_Pnt aP(x[i], y[j], z[k]);
          ElSLib::Parameters(aPln, aP, u, v);
          if(u < umin) 
            umin = u;
          if(u > umax) 
            umax = u;
          if(v < vmin)
            vmin = v;
          if(v > vmax)
            vmax = v;
        }
      }
    }
    umin -= diam;
    umax += diam;
    vmin -= diam;
    vmax += diam;
    str = new Geom_RectangularTrimmedSurface
      (s, umin, umax, vmin, vmax, Standard_True, Standard_True);
  }
  else if (styp == STANDARD_TYPE(Geom_CylindricalSurface)) {
    gp_Cylinder aCyl = Handle(Geom_CylindricalSurface)::DownCast(s)->Cylinder();
    Standard_Real u, v, vmin = RealLast(), vmax = -vmin;
    for(Standard_Integer i = 0 ; i < 2; i++) 
    {
      for(Standard_Integer j = 0; j < 2; j++)
      {
        for(Standard_Integer k = 0; k < 2; k++)
        {
          gp_Pnt aP(x[i], y[j], z[k]);
          ElSLib::Parameters(aCyl, aP, u, v);
          if(v < vmin)
            vmin = v;
          if(v > vmax)
            vmax = v;
        }
      }
    }
    vmin -= diam;
    vmax += diam;
    str = new Geom_RectangularTrimmedSurface
      (s, vmin, vmax, Standard_False, Standard_True);
  }
  else if (styp == STANDARD_TYPE(Geom_ConicalSurface)) {
    gp_Cone aCon = Handle(Geom_ConicalSurface)::DownCast(s)->Cone();
    Standard_Real u, v, vmin = RealLast(), vmax = -vmin;
    for(Standard_Integer i = 0 ; i < 2; i++) 
    {
      for(Standard_Integer j = 0; j < 2; j++)
      {
        for(Standard_Integer k = 0; k < 2; k++)
        {
          gp_Pnt aP(x[i], y[j], z[k]);
          ElSLib::Parameters(aCon, aP, u, v);
          if(v < vmin)
            vmin = v;
          if(v > vmax)
            vmax = v;
        }
      }
    }
    vmin -= diam;
    vmax += diam;
    str = new Geom_RectangularTrimmedSurface
      (s, vmin, vmax, Standard_False, Standard_True);
  }
  else {
    FUntil.Nullify();
    return;
  }

  FUntil = BRepLib_MakeFace(str, Precision::Confusion());
}



//=======================================================================
//function : Tool
//purpose  : 
//=======================================================================

TopoDS_Solid BRepFeat::Tool(const TopoDS_Shape& SRef,
                            const TopoDS_Face& Fac,
                            const TopAbs_Orientation Orf)
{
  TopTools_ListOfShape lfaces;
//  for (TopExp_Explorer exp(SRef,TopAbs_FACE); exp.More(); exp.Next()) {
  TopExp_Explorer exp(SRef,TopAbs_FACE) ;
  for ( ; exp.More(); exp.Next()) {
    if (exp.Current().ShapeType() == TopAbs_FACE) {
      lfaces.Append(exp.Current());
    }
  }
   
  LocOpe_BuildShape bs(lfaces);
  const TopoDS_Shape& Res = bs.Shape();
  TopoDS_Shell Sh;
  if (Res.ShapeType() == TopAbs_SHELL) {
    // faire un solide
    Sh = TopoDS::Shell(Res);
  }
  else if (Res.ShapeType() == TopAbs_SOLID) {
    exp.Init(Res,TopAbs_SHELL);
    Sh = TopoDS::Shell(exp.Current());
    exp.Next();
    if (exp.More()) {
      Sh.Nullify();
    }
  }

  if (Sh.IsNull()) {
    TopoDS_Solid So;
    return So;
  }


  Sh.Orientation(TopAbs_FORWARD);

  TopAbs_Orientation orient = TopAbs_FORWARD;

  for (exp.Init(Sh,TopAbs_FACE); exp.More(); exp.Next()) {
    if (exp.Current().IsSame(Fac)) {
      orient = exp.Current().Orientation();
      break;
    }
  }
  
  Standard_Boolean reverse = Standard_False;
  if ((orient == Fac.Orientation() && Orf == TopAbs_REVERSED) ||
      (orient != Fac.Orientation() && Orf == TopAbs_FORWARD)) {
    reverse = Standard_True;
  }

  if (reverse) {
    Sh.Reverse();
  }

  BRep_Builder B;
  TopoDS_Solid Soc;
  B.MakeSolid(Soc);
  B.Add(Soc,Sh);
  return Soc;
}


//=======================================================================
//function : Print
//purpose  : Print the error Description of a StatusError on a stream.
//=======================================================================
            
Standard_OStream& BRepFeat::Print(const BRepFeat_StatusError se, 
                                  Standard_OStream& s)
{
  switch(se) {
  case BRepFeat_OK :
    s << "No error";
    break;
  case BRepFeat_BadDirect :
    s << "Directions must be opposite";
    break;
  case BRepFeat_BadIntersect :
    s << "Intersection failure";
    break;
  case BRepFeat_EmptyBaryCurve :
    s << "Empty BaryCurve";
    break;
  case BRepFeat_EmptyCutResult :
    s << "Failure in Cut : Empty resulting shape";
    break;;
  case BRepFeat_FalseSide :
    s << "Verify plane and wire orientation";
    break;
  case BRepFeat_IncDirection : 
    s << "Incoherent Direction for shapes From and Until";
    break;
  case BRepFeat_IncSlidFace : 
    s << "Sliding face not in Base shape";
    break;
  case BRepFeat_IncParameter :
    s << "Incoherent Parameter : shape Until before shape From";
    break;
  case BRepFeat_IncTypes : 
    s << "Invalid option for faces From and Until : 1 Support and 1 not";
    break;
  case BRepFeat_IntervalOverlap :
    s << "Shapes From and Until overlap";
    break;
  case BRepFeat_InvFirstShape :
    s << "Invalid First shape : more than 1 face";
    break;
  case BRepFeat_InvOption :
    s << "Invalid option";
    break;
  case BRepFeat_InvShape :
    s << "Invalid shape";
    break;
  case BRepFeat_LocOpeNotDone :
    s << "Local Operation not done";
    break;
  case BRepFeat_LocOpeInvNotDone :
    s << "Local Operation : intersection line conflict";
    break;
  case BRepFeat_NoExtFace :
    s << "No Extreme faces";
    break;
  case BRepFeat_NoFaceProf :
    s << "No Face Profile";
    break;
  case BRepFeat_NoGluer :
    s << "Gluer Failure";
    break;
  case BRepFeat_NoIntersectF :
    s << "No intersection between Feature and shape From";
    break;
  case BRepFeat_NoIntersectU :
    s << "No intersection between Feature and shape Until";
    break;
  case BRepFeat_NoParts :
    s << "No parts of tool kept";
    break;
  case BRepFeat_NoProjPt :
    s << "No projection points";
    break;
  case BRepFeat_NotInitialized :
    s << "Fields not initialized";
    break;
  case BRepFeat_NotYetImplemented : 
    s << "Not yet implemented";
    break;
  case BRepFeat_NullRealTool :
    s << "Real Tool : Null DPrism";
    break;
  case BRepFeat_NullToolF :
    s << "Null Tool : Invalid type for shape Form";
    break;
  case BRepFeat_NullToolU :
    s << "Null Tool : Invalid type for shape Until";
    break;
  }
  return s;
}

