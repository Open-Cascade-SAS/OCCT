// Created on: 1992-05-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <IntPatch_ImpPrmIntersection.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <ElCLib.hxx>
#include <IntPatch_ArcFunction.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_RstInt.hxx>
#include <IntPatch_SpecialPoints.hxx>
#include <IntPatch_TheIWLineOfTheIWalking.hxx>
#include <IntPatch_TheIWalking.hxx>
#include <IntPatch_TheSurfFunction.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <IntSurf_SequenceOfPathPoint.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTrans_CurveTransition.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif

static Standard_Boolean DecomposeResult(const Handle(IntPatch_PointLine)& theLine,
                                        const Standard_Boolean       IsReversed,
                                        const IntSurf_Quadric&       theQuad,
                                        const Handle(Adaptor3d_TopolTool)& thePDomain,
                                        const Handle(Adaptor3d_HSurface)&  theQSurf,
                                        const Handle(Adaptor3d_HSurface)&  theOtherSurf,
                                        const Standard_Real                theArcTol,
                                        const Standard_Real                theTolTang,
                                        IntPatch_SequenceOfLine&           theLines);
static 
  void ComputeTangency (const IntPatch_TheSOnBounds& solrst,
  IntSurf_SequenceOfPathPoint& seqpdep,
  const Handle(Adaptor3d_TopolTool)& Domain,
  IntPatch_TheSurfFunction& Func,
  const Handle(Adaptor3d_HSurface)& PSurf,
  TColStd_Array1OfInteger& Destination);
static 
  void Recadre(const Standard_Boolean ,
  GeomAbs_SurfaceType typeS1,
  GeomAbs_SurfaceType typeS2,
  IntPatch_Point&  pt,
  const Handle(IntPatch_TheIWLineOfTheIWalking)& iwline,
  Standard_Integer Param,
  Standard_Real U1,
  Standard_Real V1,
  Standard_Real U2,
  Standard_Real V2);

static 
  Standard_Boolean IsCoincide(IntPatch_TheSurfFunction& theFunc,
                              const Handle(IntPatch_PointLine)& theLine,
                              const Handle(Adaptor2d_HCurve2d)& theArc,
                              const Standard_Boolean isTheSurface1Using,
                              const Standard_Real theToler3D,
                              const Standard_Real theToler2D,
                              const Standard_Real thePeriod);

//=======================================================================
//function : IsSeamOrPole
//purpose  : 
//=======================================================================
static IntPatch_SpecPntType IsSeamOrPole(const Handle(Adaptor3d_HSurface)& theQSurf,
                                         const Handle(IntSurf_LineOn2S)& theLine,
                                         const Standard_Boolean IsReversed,
                                         const Standard_Integer theRefIndex,
                                         const Standard_Real theDeltaMax)
{
  if((theRefIndex < 1) || (theRefIndex >= theLine->NbPoints()))
    return IntPatch_SPntNone;

  //Parameters on Quadric and on parametric for reference point
  Standard_Real aUQRef, aVQRef, aUPRef, aVPRef;
  Standard_Real aUQNext, aVQNext, aUPNext, aVPNext;

  if(IsReversed)
  {
    theLine->Value(theRefIndex).Parameters  (aUPRef, aVPRef, aUQRef, aVQRef);
    theLine->Value(theRefIndex+1).Parameters(aUPNext, aVPNext, aUQNext, aVQNext);
  }
  else
  {
    theLine->Value(theRefIndex).Parameters  (aUQRef, aVQRef, aUPRef, aVPRef);
    theLine->Value(theRefIndex+1).Parameters(aUQNext, aVQNext, aUPNext, aVPNext);
  }

  const GeomAbs_SurfaceType aType = theQSurf->GetType();

  const Standard_Real aDeltaU = Abs(aUQRef - aUQNext);

  if((aType != GeomAbs_Torus) && (aDeltaU < theDeltaMax))
    return IntPatch_SPntNone;

  switch(aType)
  {
  case GeomAbs_Cylinder:
    return IntPatch_SPntSeamU;

  case GeomAbs_Torus:
    {
      const Standard_Real aDeltaV = Abs(aVQRef - aVQNext);

      if((aDeltaU >= theDeltaMax) && (aDeltaV >= theDeltaMax))
        return IntPatch_SPntSeamUV;

      if(aDeltaU >= theDeltaMax)
        return IntPatch_SPntSeamU;

      if(aDeltaV >= theDeltaMax)
        return IntPatch_SPntSeamV;
    }

    break;
  case GeomAbs_Sphere:
  case GeomAbs_Cone:
    return IntPatch_SPntPoleSeamU;
  default:
    break;
  }

  return IntPatch_SPntNone;
}

//=======================================================================
//function : IntPatch_ImpPrmIntersection
//purpose  : 
//=======================================================================
IntPatch_ImpPrmIntersection::IntPatch_ImpPrmIntersection ()
  : done(Standard_False),
  empt(Standard_False),
  myIsStartPnt(Standard_False),
  myUStart(0.0),
  myVStart(0.0)
{ }


//=======================================================================
//function : IntPatch_ImpPrmIntersection
//purpose  : 
//=======================================================================

IntPatch_ImpPrmIntersection::IntPatch_ImpPrmIntersection
  (const Handle(Adaptor3d_HSurface)&    Surf1,
  const Handle(Adaptor3d_TopolTool)&   D1,
  const Handle(Adaptor3d_HSurface)&    Surf2,
  const Handle(Adaptor3d_TopolTool)&   D2,
  const Standard_Real    TolArc,
  const Standard_Real    TolTang,
  const Standard_Real    Fleche,
  const Standard_Real    Pas)
  : done(Standard_False),
  empt(Standard_False),
  myIsStartPnt(Standard_False),
  myUStart(0.0),
  myVStart(0.0)
{
  Perform(Surf1,D1,Surf2,D2,TolArc,TolTang,Fleche,Pas);
}


//=======================================================================
//function : SetStartPoint
//purpose  : 
//=======================================================================

void IntPatch_ImpPrmIntersection::SetStartPoint(const Standard_Real U,
  const Standard_Real V)
{
  myIsStartPnt = Standard_True;
  myUStart = U; myVStart = V;
}

//=======================================================================
//function : ComputeTangency
//purpose  : 
//=======================================================================
void ComputeTangency (const IntPatch_TheSOnBounds& solrst,
  IntSurf_SequenceOfPathPoint& seqpdep,
  const Handle(Adaptor3d_TopolTool)& Domain,
  IntPatch_TheSurfFunction& Func,
  const Handle(Adaptor3d_HSurface)& PSurf,
  TColStd_Array1OfInteger& Destination)
{
  Standard_Integer i,k, NbPoints, seqlength;
  Standard_Real theparam,test;
  Standard_Boolean fairpt, ispassing;
  TopAbs_Orientation arcorien,vtxorien;
  Handle(Adaptor2d_HCurve2d) thearc;
  Handle(Adaptor3d_HVertex) vtx,vtxbis;
  //Standard_Boolean ispassing;
  IntPatch_ThePathPointOfTheSOnBounds PStart;
  IntSurf_PathPoint PPoint;
  gp_Vec vectg;
  gp_Dir2d dirtg;
  gp_Pnt ptbid;
  gp_Vec d1u,d1v,v1,v2;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  //
  double aX[2], aF[1], aD[1][2];
  math_Vector X(aX, 1, 2);
  math_Vector F(aF, 1, 1);
  math_Matrix D(aD, 1, 1, 1, 2); 
  //
  seqlength = 0;
  NbPoints = solrst.NbPoints();
  for (i=1; i<= NbPoints; i++) {
    if (Destination(i) == 0) {
      PStart = solrst.Point(i);
      thearc   = PStart.Arc();
      theparam = PStart.Parameter();
      arcorien = Domain->Orientation(thearc);
      ispassing = (arcorien == TopAbs_INTERNAL || 
        arcorien == TopAbs_EXTERNAL);

      thearc->D0(theparam,p2d);
      X(1) = p2d.X(); 
      X(2) = p2d.Y();
      PPoint.SetValue(PStart.Value(),X(1),X(2));

      Func.Values(X,F,D);
      if (Func.IsTangent()) {
        PPoint.SetTangency(Standard_True);
        Destination(i) = seqlength+1;
        if (!PStart.IsNew()) {
          vtx = PStart.Vertex();
          for (k=i+1; k<=NbPoints; k++) {
            if (Destination(k) ==0) {
              PStart = solrst.Point(k);
              if (!PStart.IsNew()) {
                vtxbis = PStart.Vertex();
                if (Domain->Identical(vtx,vtxbis)) {
                  thearc   = PStart.Arc();
                  theparam = PStart.Parameter();
                  arcorien = Domain->Orientation(thearc);
                  ispassing = ispassing && (arcorien == TopAbs_INTERNAL ||
                    arcorien == TopAbs_EXTERNAL);

                  thearc->D0(theparam,p2d);
                  PPoint.AddUV(p2d.X(),p2d.Y());
                  Destination(k) = seqlength+1;
                }
              }
            }
          }
        }
        PPoint.SetPassing(ispassing);
        seqpdep.Append(PPoint);
        seqlength++;
      }
      else { // on a un point de depart potentiel

        vectg = Func.Direction3d();
        dirtg = Func.Direction2d();

        PSurf->D1(X(1),X(2),ptbid,d1u,d1v);
        thearc->D1(theparam,p2d,d2d);
        v2.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);
        v1 = d1u.Crossed(d1v);

        test = vectg.Dot(v1.Crossed(v2));
        if (PStart.IsNew()) {
          if ((test < 0. && arcorien == TopAbs_FORWARD) ||
            (test > 0. && arcorien == TopAbs_REVERSED)) {
              vectg.Reverse();
              dirtg.Reverse();
          }
          PPoint.SetDirections(vectg,dirtg);
          PPoint.SetPassing(ispassing);
          Destination(i) = seqlength+1;
          seqpdep.Append(PPoint);
          seqlength++;
        }
        else { // traiter la transition complexe
          gp_Dir bidnorm(1.,1.,1.);
          Standard_Real tole = 1.e-8;
          TopAbs_Orientation LocTrans;
          TopTrans_CurveTransition comptrans;
          comptrans.Reset(vectg,bidnorm,0.);
          if (arcorien == TopAbs_FORWARD || 
            arcorien == TopAbs_REVERSED) {
              // pour essai

              vtx = PStart.Vertex();
              vtxorien = Domain->Orientation(vtx);
              if (Abs(test) <= tole) {
                LocTrans = TopAbs_EXTERNAL; // et pourquoi pas INTERNAL
              }
              else {
                if (((test > 0.)&& arcorien == TopAbs_FORWARD) ||
                  ((test < 0.)&& arcorien == TopAbs_REVERSED)){
                    LocTrans = TopAbs_FORWARD;
                }
                else {
                  LocTrans = TopAbs_REVERSED;
                }
                if (arcorien == TopAbs_REVERSED) {v2.Reverse();}
              }

              comptrans.Compare(tole,v2,bidnorm,0.,LocTrans,vtxorien);
          }
          Destination(i) = seqlength+1;
          for (k= i+1; k<=NbPoints; k++) {
            if (Destination(k) == 0) {
              PStart = solrst.Point(k);
              if (!PStart.IsNew()) {
                vtxbis = PStart.Vertex();
                if (Domain->Identical(vtx,vtxbis)) {
                  thearc   = PStart.Arc();
                  theparam = PStart.Parameter();
                  arcorien = Domain->Orientation(thearc);

                  PPoint.AddUV(X(1),X(2));

                  thearc->D1(theparam,p2d,d2d);
                  PPoint.AddUV(p2d.X(),p2d.Y());

                  if (arcorien == TopAbs_FORWARD || 
                    arcorien == TopAbs_REVERSED) {
                      ispassing = Standard_False;
                      v2.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);

                      test = vectg.Dot(v1.Crossed(v2));
                      vtxorien = Domain->Orientation(PStart.Vertex());
                      if (Abs(test) <= tole) {
                        LocTrans = TopAbs_EXTERNAL; // et pourquoi pas INTERNAL
                      }
                      else {
                        if (((test > 0.)&& arcorien == TopAbs_FORWARD) ||
                          ((test < 0.)&& arcorien == TopAbs_REVERSED)){
                            LocTrans = TopAbs_FORWARD;
                        }
                        else {
                          LocTrans = TopAbs_REVERSED;
                        }
                        if (arcorien == TopAbs_REVERSED) {v2.Reverse();}
                      }

                      comptrans.Compare(tole,v2,bidnorm,0.,LocTrans,vtxorien);
                  }
                  Destination(k) = seqlength+1;
                }
              }
            }
          }
          fairpt = Standard_True;
          if (!ispassing) {
            TopAbs_State Before = comptrans.StateBefore();
            TopAbs_State After  = comptrans.StateAfter();
            if ((Before == TopAbs_UNKNOWN)||(After == TopAbs_UNKNOWN)) {
              fairpt = Standard_False;
            }
            else if (Before == TopAbs_IN) {
              if (After == TopAbs_IN) {
                ispassing = Standard_True;
              }
              else {
                vectg.Reverse();
                dirtg.Reverse();
              }
            }
            else {
              if (After !=TopAbs_IN) {
                fairpt = Standard_False;
              }
            }
          }
          if (fairpt) {
            PPoint.SetDirections(vectg,dirtg);
            PPoint.SetPassing(ispassing);
            seqpdep.Append(PPoint);
            seqlength++;
          }
          else { // il faut remettre en "ordre" si on ne garde pas le point.
            for (k=i; k <=NbPoints ; k++) {
              if (Destination(k)==seqlength + 1) {
                Destination(k) = -Destination(k);
              }
            }
          }
        }
      }
    }
  }
}
//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================
void Recadre(const Standard_Boolean ,
  GeomAbs_SurfaceType typeS1,
  GeomAbs_SurfaceType typeS2,
  IntPatch_Point&  pt,
  const Handle(IntPatch_TheIWLineOfTheIWalking)& iwline,
  Standard_Integer Param,
  Standard_Real U1,
  Standard_Real V1,
  Standard_Real U2,
  Standard_Real V2)
{
  Standard_Real U1p,V1p,U2p,V2p;
  iwline->Line()->Value(Param).Parameters(U1p,V1p,U2p,V2p);
  switch(typeS1)
  {
  case GeomAbs_Torus:
    while(V1<(V1p-1.5*M_PI)) V1+=M_PI+M_PI;
    while(V1>(V1p+1.5*M_PI)) V1-=M_PI+M_PI;
    Standard_FALLTHROUGH
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
    while(U1<(U1p-1.5*M_PI)) U1+=M_PI+M_PI;
    while(U1>(U1p+1.5*M_PI)) U1-=M_PI+M_PI;
  default:
    break;
  }
  switch(typeS2)
  { 
  case GeomAbs_Torus:
    while(V2<(V2p-1.5*M_PI)) V2+=M_PI+M_PI;
    while(V2>(V2p+1.5*M_PI)) V2-=M_PI+M_PI;
    Standard_FALLTHROUGH
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
    while(U2<(U2p-1.5*M_PI)) U2+=M_PI+M_PI;
    while(U2>(U2p+1.5*M_PI)) U2-=M_PI+M_PI;
  default:
    break;
  }
  pt.SetParameters(U1,V1,U2,V2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntPatch_ImpPrmIntersection::Perform (const Handle(Adaptor3d_HSurface)& Surf1,
  const Handle(Adaptor3d_TopolTool)& D1,
  const Handle(Adaptor3d_HSurface)& Surf2,
  const Handle(Adaptor3d_TopolTool)& D2,
  const Standard_Real TolArc,
  const Standard_Real TolTang,
  const Standard_Real Fleche,
  const Standard_Real Pas)
{
  Standard_Boolean reversed, procf, procl, dofirst, dolast;
  Standard_Integer indfirst = 0, indlast = 0, ind2, NbSegm;
  Standard_Integer NbPointIns, NbPointRst, Nblines, Nbpts, NbPointDep;
  Standard_Real U1,V1,U2,V2,paramf,paraml,currentparam;

  IntPatch_TheSegmentOfTheSOnBounds thesegm;
  IntSurf_PathPoint PPoint;

  Handle(IntPatch_RLine) rline;
  Handle(IntPatch_WLine) wline;
  IntPatch_ThePathPointOfTheSOnBounds PStart,PStartf,PStartl;
  IntPatch_Point ptdeb,ptfin,ptbis;

  IntPatch_IType typ;
  IntSurf_Transition TLine,TArc;
  IntSurf_TypeTrans trans1,trans2;
  gp_Pnt valpt,ptbid;
  gp_Vec tgline,tgrst,norm1,norm2,d1u,d1v;
  gp_Dir DirNormale;
  gp_Vec VecNormale;

  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  Handle(Adaptor2d_HCurve2d) currentarc;
  GeomAbs_SurfaceType typeS1, typeS2;
  IntSurf_Quadric Quad;
  IntPatch_TheSurfFunction Func;
  IntPatch_ArcFunction AFunc;
  //
  typeS1 = Surf1->GetType();
  typeS2 = Surf2->GetType();

  paramf =0.;
  paraml =0.;
  trans1 = IntSurf_Undecided;
  trans2 = IntSurf_Undecided;
  //
  done = Standard_False;
  empt = Standard_True;
  slin.Clear();
  spnt.Clear();
  //
  reversed = Standard_False;
  switch (typeS1)
  {
  case GeomAbs_Plane:
    Quad.SetValue(Surf1->Plane());
    break;

  case GeomAbs_Cylinder:
    Quad.SetValue(Surf1->Cylinder());
    break;

  case GeomAbs_Sphere:
    Quad.SetValue(Surf1->Sphere());
    break;

  case GeomAbs_Cone:
    Quad.SetValue(Surf1->Cone());
    break;

  default:
    {
      reversed = Standard_True;
      switch (typeS2)
      {
      case GeomAbs_Plane:
        Quad.SetValue(Surf2->Plane());
        break;

      case GeomAbs_Cylinder:
        Quad.SetValue(Surf2->Cylinder());
        break;

      case GeomAbs_Sphere:
        Quad.SetValue(Surf2->Sphere());
        break;

      case GeomAbs_Cone:
        Quad.SetValue(Surf2->Cone());
        break;
      default:
        {
          throw Standard_ConstructionError();
          break;
        }
      } 
    }
    break;
  }
  //
  Func.SetImplicitSurface(Quad);
  Func.Set(IntSurf_QuadricTool::Tolerance(Quad));
  AFunc.SetQuadric(Quad);
  //
  if (!reversed) {
    Func.Set(Surf2);
    AFunc.Set(Surf2);
  }
  else {
    Func.Set(Surf1);
    AFunc.Set(Surf1);
  }
  //
  if (!reversed) {
    solrst.Perform(AFunc,D2,TolArc,TolTang);
  }
  else {
    solrst.Perform(AFunc,D1,TolArc,TolTang);
  }
  if (!solrst.IsDone()) {
    return;
  }
  //
  IntSurf_SequenceOfPathPoint seqpdep;
  IntSurf_SequenceOfInteriorPoint seqpins;
  //
  NbPointRst = solrst.NbPoints();
  TColStd_Array1OfInteger Destination(1,NbPointRst+1); Destination.Init(0);
  if (NbPointRst) {
    if (!reversed) {
      ComputeTangency(solrst,seqpdep,D2,Func,Surf2,Destination);
    }
    else {
      ComputeTangency(solrst,seqpdep,D1,Func,Surf1,Destination);
    }
  }
  //
  Standard_Boolean SearchIns = Standard_True;
  if(Quad.TypeQuadric() == GeomAbs_Plane && solrst.NbSegments() > 0)
  {
    //For such kind of cases it is possible that whole surface is on one side of plane,
    //plane only touches surface and does not cross it,
    //so no inner points exist.
    SearchIns = Standard_False;
    Handle(Adaptor3d_TopolTool) T;
    if(reversed)
    {
      T = D1;
    }
    else
    {
      T = D2;
    }
    Standard_Integer aNbSamples = 0;
    aNbSamples = T->NbSamples();
    gp_Pnt2d s2d;
    gp_Pnt s3d;
    Standard_Real aValf[1], aUVap[2];
    math_Vector Valf(aValf,1,1), UVap(aUVap,1,2);
    T->SamplePoint(1,s2d, s3d);
    UVap(1)=s2d.X(); 
    UVap(2)=s2d.Y();
    Func.Value(UVap,Valf);
    Standard_Real rvalf = Sign(1.,Valf(1));
    for(Standard_Integer i = 2; i <= aNbSamples; ++i)
    {
      T->SamplePoint(i,s2d, s3d);
      UVap(1)=s2d.X(); 
      UVap(2)=s2d.Y();
      Func.Value(UVap,Valf);
      if(rvalf * Valf(1) < 0.)
      {
        SearchIns = Standard_True;
        break;
      }   
    }
  }
  // Recherche des points interieurs
  NbPointIns = 0;
  if(SearchIns) {
    if (!reversed) {
      if (myIsStartPnt)
        solins.Perform(Func,Surf2,myUStart,myVStart);
      else
        solins.Perform(Func,Surf2,D2,TolTang);
    }
    else {
      if (myIsStartPnt)
        solins.Perform(Func,Surf1,myUStart,myVStart);
      else
        solins.Perform(Func,Surf1,D1,TolTang);
    }
    NbPointIns = solins.NbPoints();
    for (Standard_Integer i=1; i <= NbPointIns; i++) {
      seqpins.Append(solins.Value(i));
    }
  }
  //
  NbPointDep=seqpdep.Length();
  //
  if (NbPointDep || NbPointIns) {
    IntPatch_TheIWalking iwalk(TolTang,Fleche,Pas);
    if (!reversed) {
      iwalk.Perform(seqpdep,seqpins,Func,Surf2);
    }
    else {
      iwalk.Perform(seqpdep,seqpins,Func,Surf1,Standard_True);
    }
    if(!iwalk.IsDone()) {
      return;
    }

    Standard_Real Vmin, Vmax, TolV = 1.e-14;
    if (!reversed) { //Surf1 is quadric
      Vmin = Surf1->FirstVParameter();
      Vmax = Surf1->LastVParameter();
    }
    else { //Surf2 is quadric
      Vmin = Surf2->FirstVParameter();
      Vmax = Surf2->LastVParameter();
    }
    //
    Nblines = iwalk.NbLines();
    for (Standard_Integer j=1; j<=Nblines; j++) {
      const Handle(IntPatch_TheIWLineOfTheIWalking)&  iwline  = iwalk.Value(j);
      const Handle(IntSurf_LineOn2S)&                 thelin  = iwline->Line();

      Nbpts = thelin->NbPoints();
      if(Nbpts>=2) { 
        Standard_Integer k = 0;
        tgline = iwline->TangentVector(k);	
        if(k>=1 && k<=Nbpts) { } else { k=Nbpts>>1; } 
        valpt = thelin->Value(k).Value();	

        if (!reversed) {
          thelin->Value(k).ParametersOnS2(U2,V2);
          norm1 = Quad.Normale(valpt);
          Surf2->D1(U2,V2,ptbid,d1u,d1v);
          norm2 = d1u.Crossed(d1v);
        }
        else {
          thelin->Value(k).ParametersOnS1(U2,V2);
          norm2 = Quad.Normale(valpt);
          Surf1->D1(U2,V2,ptbid,d1u,d1v);
          norm1 = d1u.Crossed(d1v);
        }
        if (tgline.DotCross(norm2,norm1) > 0.) {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }

        //
        Standard_Real AnU1,AnU2,AnV2;

        GeomAbs_SurfaceType typQuad = Quad.TypeQuadric();
        Standard_Boolean arecadr=Standard_False;
        valpt = thelin->Value(1).Value();
        Quad.Parameters(valpt,AnU1,V1);

        if((V1 < Vmin) && (Vmin-V1 < TolV)) V1 = Vmin;
        if((V1 > Vmax) && (V1-Vmax < TolV)) V1 = Vmax;

        if(reversed) { 
          thelin->SetUV(1,Standard_False,AnU1,V1); //-- on va lire u2,v2
          thelin->Value(1).ParametersOnS1(AnU2,AnV2);
        }
        else { 
          thelin->SetUV(1,Standard_True,AnU1,V1);  //-- on va lire u1,v1 
          thelin->Value(1).ParametersOnS2(AnU2,AnV2);
        }

        if(typQuad==GeomAbs_Cylinder || 
          typQuad==GeomAbs_Cone || 
          typQuad==GeomAbs_Sphere) { 
            arecadr=Standard_True; 
        } 
        //
        for (k=2; k<=Nbpts; ++k) {
          valpt = thelin->Value(k).Value();
          Quad.Parameters(valpt,U1,V1);
          //
          if((V1 < Vmin) && (Vmin-V1 < TolV)) {
            V1 = Vmin;
          }
          if((V1 > Vmax) && (V1-Vmax < TolV)) {
            V1 = Vmax;
          }
          //
          if(arecadr) {
            //modified by NIZNHY-PKV Fri Mar 28 15:06:01 2008f
            Standard_Real aCf, aTwoPI;
            //
            aCf=0.;
            aTwoPI=M_PI+M_PI;
            if ((U1-AnU1) >  1.5*M_PI) { 
              while ((U1-AnU1) > (1.5*M_PI+aCf*aTwoPI)) {
                aCf=aCf+1.;
              }
              U1=U1-aCf*aTwoPI;
            } 
            //
            else {
              while ((U1-AnU1) < (-1.5*M_PI-aCf*aTwoPI)) {
                aCf=aCf+1.;
              }
              U1=U1+aCf*aTwoPI;
            }
            // was:
            //if ((U1-AnU1) >  1.5*M_PI) { 
            //  U1-=M_PI+M_PI;
            //}
            //else if ((U1-AnU1) < -1.5*M_PI) { 
            //  U1+=M_PI+M_PI; 
            //}
            //modified by NIZNHY-PKV Fri Mar 28 15:06:11 2008t
          }
          //
          if(reversed) { 
            thelin->SetUV(k,Standard_False,U1,V1);

            thelin->Value(k).ParametersOnS1(U2,V2);
            switch(typeS1) { 
            case GeomAbs_Cylinder:
            case GeomAbs_Cone:
            case GeomAbs_Sphere:
            case GeomAbs_Torus:
              while(U2<(AnU2-1.5*M_PI)) U2+=M_PI+M_PI;
              while(U2>(AnU2+1.5*M_PI)) U2-=M_PI+M_PI;
              break;
            default: 
              break;
            }
            if(typeS2==GeomAbs_Torus) { 
              while(V2<(AnV2-1.5*M_PI)) V2+=M_PI+M_PI;
              while(V2>(AnV2+1.5*M_PI)) V2-=M_PI+M_PI;
            }
            thelin->SetUV(k,Standard_True,U2,V2);
          }
          else { 
            thelin->SetUV(k,Standard_True,U1,V1);

            thelin->Value(k).ParametersOnS2(U2,V2);
            switch(typeS2) { 
            case GeomAbs_Cylinder:
            case GeomAbs_Cone:
            case GeomAbs_Sphere:
            case GeomAbs_Torus:
              while(U2<(AnU2-1.5*M_PI)) U2+=M_PI+M_PI;
              while(U2>(AnU2+1.5*M_PI)) U2-=M_PI+M_PI;
              break;
            default: 
              break;
            }
            if(typeS2==GeomAbs_Torus) { 
              while(V2<(AnV2-1.5*M_PI)) V2+=M_PI+M_PI;
              while(V2>(AnV2+1.5*M_PI)) V2-=M_PI+M_PI;
            }
            thelin->SetUV(k,Standard_False,U2,V2);

          }

          AnU1=U1;
          AnU2=U2;
          AnV2=V2;
        }
        // <-A
        wline = new IntPatch_WLine(thelin,Standard_False,trans1,trans2);

#ifdef INTPATCH_IMPPRMINTERSECTION_DEBUG
        wline->Dump(0);
#endif

        if (   iwline->HasFirstPoint() 
          && iwline->IsTangentAtBegining() == Standard_False) 
        {
          indfirst = iwline->FirstPointIndex();
          PPoint = seqpdep(indfirst);
          tgline = PPoint.Direction3d();
          Standard_Integer themult = PPoint.Multiplicity();
          for (Standard_Integer i=NbPointRst; i>=1; i--) {
            if (Destination(i) == indfirst) {
              if (!reversed) { //-- typeS1 = Pln || Cyl || Sph || Cone
                Quad.Parameters(PPoint.Value(),U1,V1);

                if((V1 < Vmin) && (Vmin-V1 < TolV)) V1 = Vmin;
                if((V1 > Vmax) && (V1-Vmax < TolV)) V1 = Vmax;

                PPoint.Parameters(themult,U2,V2);
                Surf2->D1(U2,V2,ptbid,d1u,d1v); //-- @@@@
              }
              else {  //-- typeS1 != Pln && Cyl && Sph && Cone
                Quad.Parameters(PPoint.Value(),U2,V2);

                if((V2 < Vmin) && (Vmin-V2 < TolV)) V2 = Vmin;
                if((V2 > Vmax) && (V2-Vmax < TolV)) V2 = Vmax;

                PPoint.Parameters(themult,U1,V1);
                Surf1->D1(U1,V1,ptbid,d1u,d1v); //-- @@@@
              }

              VecNormale = d1u.Crossed(d1v);                      
              //-- Modif du 27 Septembre 94 (Recadrage des pts U,V) 
              ptdeb.SetValue(PPoint.Value(),TolArc,Standard_False);
              ptdeb.SetParameters(U1,V1,U2,V2);
              ptdeb.SetParameter(1.);

              Recadre(reversed,typeS1,typeS2,ptdeb,iwline,1,U1,V1,U2,V2);

              currentarc = solrst.Point(i).Arc();
              currentparam = solrst.Point(i).Parameter();
              currentarc->D1(currentparam,p2d,d2d);
              tgrst.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);

              Standard_Real squaremagnitudeVecNormale = VecNormale.SquareMagnitude();
              if(squaremagnitudeVecNormale > 1e-13) { 
                DirNormale=VecNormale;
                IntSurf::MakeTransition(tgline,tgrst,DirNormale,TLine,TArc);
              }
              else { 
                TLine.SetValue(Standard_True,IntSurf_Undecided);
                TArc.SetValue(Standard_True,IntSurf_Undecided);
              }

              ptdeb.SetArc(reversed,currentarc,currentparam,TLine,TArc);
              if (!solrst.Point(i).IsNew()) {
                ptdeb.SetVertex(reversed,solrst.Point(i).Vertex());
              }
              wline->AddVertex(ptdeb);
              if (themult == 0) {
                wline->SetFirstPoint(wline->NbVertex());
              }

              themult--;
            }
          }
        }
        else if (iwline->IsTangentAtBegining()) 
        {
          gp_Pnt psol = thelin->Value(1).Value();
          thelin->Value(1).ParametersOnS1(U1,V1);
          thelin->Value(1).ParametersOnS2(U2,V2);
          ptdeb.SetValue(psol,TolArc,Standard_True);
          ptdeb.SetParameters(U1,V1,U2,V2);
          ptdeb.SetParameter(1.);
          wline->AddVertex(ptdeb);
          wline->SetFirstPoint(wline->NbVertex());
        }
        else 
        { 
          gp_Pnt psol = thelin->Value(1).Value();
          thelin->Value(1).ParametersOnS1(U1,V1);
          thelin->Value(1).ParametersOnS2(U2,V2);
          ptdeb.SetValue(psol,TolArc,Standard_False);
          ptdeb.SetParameters(U1,V1,U2,V2);
          ptdeb.SetParameter(1.);
          wline->AddVertex(ptdeb);
          wline->SetFirstPoint(wline->NbVertex());
        }


        if (   iwline->HasLastPoint() 
          && iwline->IsTangentAtEnd() == Standard_False) 
        {
          indlast = iwline->LastPointIndex();
          PPoint = seqpdep(indlast);
          tgline = PPoint.Direction3d().Reversed();
          Standard_Integer themult = PPoint.Multiplicity();
          for (Standard_Integer i=NbPointRst; i >=1; i--) {
            if (Destination(i) == indlast) {
              if (!reversed) {
                Quad.Parameters(PPoint.Value(),U1,V1);

                if((V1 < Vmin) && (Vmin-V1 < TolV)) V1 = Vmin;
                if((V1 > Vmax) && (V1-Vmax < TolV)) V1 = Vmax;

                PPoint.Parameters(themult,U2,V2);
                Surf2->D1(U2,V2,ptbid,d1u,d1v); //-- @@@@
                VecNormale = d1u.Crossed(d1v);                    //-- @@@@
              }
              else {
                Quad.Parameters(PPoint.Value(),U2,V2);

                if((V2 < Vmin) && (Vmin-V2 < TolV)) V2 = Vmin;
                if((V2 > Vmax) && (V2-Vmax < TolV)) V2 = Vmax;

                PPoint.Parameters(themult,U1,V1);
                Surf1->D1(U1,V1,ptbid,d1u,d1v); //-- @@@@
                VecNormale = d1u.Crossed(d1v);                    //-- @@@@
              }

              ptfin.SetValue(PPoint.Value(),TolArc,Standard_False);
              ptfin.SetParameters(U1,V1,U2,V2);
              ptfin.SetParameter(Nbpts);

              Recadre(reversed,typeS1,typeS2,ptfin,iwline,Nbpts-1,U1,V1,U2,V2);

              currentarc = solrst.Point(i).Arc();
              currentparam = solrst.Point(i).Parameter();
              currentarc->D1(currentparam,p2d,d2d);
              tgrst.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);


              Standard_Real squaremagnitudeVecNormale = VecNormale.SquareMagnitude();
              if(squaremagnitudeVecNormale > 1e-13) { 
                DirNormale=VecNormale;
                IntSurf::MakeTransition(tgline,tgrst,DirNormale,TLine,TArc);
              }
              else { 
                TLine.SetValue(Standard_True,IntSurf_Undecided);
                TArc.SetValue(Standard_True,IntSurf_Undecided);
              }


              ptfin.SetArc(reversed,currentarc,currentparam,TLine,TArc);
              if (!solrst.Point(i).IsNew()) {
                ptfin.SetVertex(reversed,solrst.Point(i).Vertex());
              }
              wline->AddVertex(ptfin);
              if (themult == 0) {
                wline->SetLastPoint(wline->NbVertex());
              }

              themult--;
            }
          }
        }
        else if (iwline->IsTangentAtEnd()) 
        {
          gp_Pnt psol = thelin->Value(Nbpts).Value();
          thelin->Value(Nbpts).ParametersOnS1(U1,V1);
          thelin->Value(Nbpts).ParametersOnS2(U2,V2);
          ptfin.SetValue(psol,TolArc,Standard_True);
          ptfin.SetParameters(U1,V1,U2,V2);
          ptfin.SetParameter(Nbpts);
          wline->AddVertex(ptfin);
          wline->SetLastPoint(wline->NbVertex());
        }
        else 
        { 
          gp_Pnt psol = thelin->Value(Nbpts).Value();
          thelin->Value(Nbpts).ParametersOnS1(U1,V1);
          thelin->Value(Nbpts).ParametersOnS2(U2,V2);
          ptfin.SetValue(psol,TolArc,Standard_False);
          ptfin.SetParameters(U1,V1,U2,V2);
          ptfin.SetParameter(Nbpts);
          wline->AddVertex(ptfin);
          wline->SetLastPoint(wline->NbVertex());
        }
        //
        // Il faut traiter les points de passage.
        slin.Append(wline);
      }// if(Nbpts>=2) {
    }// for (j=1; j<=Nblines; j++) {

    // ON GERE LES RACCORDS ENTRE LIGNES. ELLE NE PEUVENT SE RACCORDER
    // QUE SUR DES POINTS DE TANGENCE


    Nblines = slin.Length();
    for (Standard_Integer j=1; j<=Nblines-1; j++) {
      dofirst = dolast = Standard_False;
      const  Handle(IntPatch_Line)& slinj = slin(j);
      Handle(IntPatch_WLine) wlin1 (Handle(IntPatch_WLine)::DownCast (slinj));
      if (wlin1->HasFirstPoint()) {
        ptdeb = wlin1->FirstPoint(indfirst);
        if (ptdeb.IsTangencyPoint()) {
          dofirst = Standard_True;
        }
      }
      if (wlin1->HasLastPoint()) {
        ptfin = wlin1->LastPoint(indlast);
        if (ptfin.IsTangencyPoint()) {
          dolast = Standard_True;
        }
      }

      if (dofirst || dolast) {
        for (Standard_Integer k=j+1; k<=Nblines;k++) {
          const  Handle(IntPatch_Line)& slink = slin(k);
          Handle(IntPatch_WLine) wlin2 (Handle(IntPatch_WLine)::DownCast (slink));
          if (wlin2->HasFirstPoint()) {
            ptbis = wlin2->FirstPoint(ind2);
            if (ptbis.IsTangencyPoint()) {
              if (dofirst ) {
                if (ptdeb.Value().Distance(ptbis.Value()) <= TolArc) {
                  ptdeb.SetMultiple(Standard_True);
                  if (!ptbis.IsMultiple()) {
                    ptbis.SetMultiple(Standard_True);
                    wlin2->Replace(ind2,ptbis);
                  }
                }
              }
              if (dolast ) {
                if (ptfin.Value().Distance(ptbis.Value()) <= TolArc) {
                  ptfin.SetMultiple(Standard_True);
                  if (!ptbis.IsMultiple()) {
                    ptbis.SetMultiple(Standard_True);
                    wlin2->Replace(ind2,ptbis);
                  }
                }
              }
            }
          }
          if (wlin2->HasLastPoint()) {
            ptbis = wlin2->LastPoint(ind2);
            if (ptbis.IsTangencyPoint()) {
              if (dofirst ) {
                if (ptdeb.Value().Distance(ptbis.Value()) <= TolArc) {
                  ptdeb.SetMultiple(Standard_True);
                  if (!ptbis.IsMultiple()) {
                    ptbis.SetMultiple(Standard_True);
                    wlin2->Replace(ind2,ptbis);
                  }
                }
              }
              if (dolast ) {
                if (ptfin.Value().Distance(ptbis.Value()) <= TolArc) {
                  ptfin.SetMultiple(Standard_True);
                  if (!ptbis.IsMultiple()) {
                    ptbis.SetMultiple(Standard_True);
                    wlin2->Replace(ind2,ptbis);
                  }
                }
              }
            }
          }
        }
        if(dofirst) 
          wlin1->Replace(indfirst,ptdeb);
        if(dolast) 
          wlin1->Replace(indlast,ptfin);
      }
    }
  }// if (seqpdep.Length() != 0 || seqpins.Length() != 0) {
  //
  // Treatment the segments
  NbSegm = solrst.NbSegments();
  if (NbSegm) {
    for(Standard_Integer i=1; i<=NbSegm; i++) {
      thesegm = solrst.Segment(i);  
      //Check if segment is degenerated
      if(thesegm.HasFirstPoint() && thesegm.HasLastPoint())
      {
        Standard_Real tol2 = Precision::Confusion();
        tol2 *= tol2;
        const gp_Pnt& aPf = thesegm.FirstPoint().Value();
        const gp_Pnt& aPl = thesegm.LastPoint().Value();
        if(aPf.SquareDistance(aPl) <= tol2)
        {
          //segment can be degenerated - check inner point
          paramf = thesegm.FirstPoint().Parameter();
          paraml = thesegm.LastPoint().Parameter();
          gp_Pnt2d _p2d = 
            thesegm.Curve()->Value(.57735 * paramf + 0.42265 * paraml);
          gp_Pnt aPm;
          if(reversed)
          {
            Surf1->D0(_p2d.X(), _p2d.Y(), aPm);
          }
          else
          {
            Surf2->D0(_p2d.X(), _p2d.Y(), aPm);
          }
          if(aPm.SquareDistance(aPf) <= tol2)
          {
            //Degenerated
            continue;
          }
        }
      }


      //----------------------------------------------------------------------      
      // on cree une ligne d intersection contenant uniquement le segment.
      // VOIR POUR LA TRANSITION DE LA LIGNE
      // On ajoute aussi un polygone pour le traitement des intersections
      // entre ligne et restrictions de la surface implicite (PutVertexOnLine)
      //----------------------------------------------------------------------
      //-- Calcul de la transition sur la rline (12 fev 97)
      //-- reversed a le sens de OnFirst
      //--
      dofirst = dolast  = Standard_False;
      procf = Standard_False;
      procl = Standard_False;
      IntSurf_Transition TLineUnk,TArcUnk;

      IntPatch_Point _thepointAtBeg;
      IntPatch_Point _thepointAtEnd;

      Standard_Boolean TransitionOK=Standard_False;

      if(thesegm.HasFirstPoint()) { 
        Standard_Real _u1,_v1,_u2,_v2;

        dofirst = Standard_True;
        PStartf = thesegm.FirstPoint();
        paramf = PStartf.Parameter();

        gp_Pnt2d _p2d     = thesegm.Curve()->Value(paramf);
        Handle(Adaptor3d_HVertex) _vtx;
        if(PStartf.IsNew()==Standard_False) 
          _vtx= PStartf.Vertex();
        const gp_Pnt& _Pp = PStartf.Value();
        _thepointAtBeg.SetValue(_Pp,PStartf.Tolerance(),Standard_False);
        if (!reversed) { //-- typeS1 = Pln || Cyl || Sph || Cone
          Quad.Parameters(_Pp,_u1,_v1);
          _u2=_p2d.X(); _v2=_p2d.Y();
        }
        else {  //-- typeS1 != Pln && Cyl && Sph && Cone
          Quad.Parameters(_Pp,_u2,_v2);
          _u1=_p2d.X(); _v1=_p2d.Y();
        }
        _thepointAtBeg.SetParameters(_u1,_v1,_u2,_v2);
        _thepointAtBeg.SetParameter(paramf);
        if(PStartf.IsNew()==Standard_False) 
          _thepointAtBeg.SetVertex(reversed,_vtx);
        _thepointAtBeg.SetArc(reversed,thesegm.Curve(),paramf,TLineUnk,TArcUnk);


        gp_Vec d1u1,d1v1,d1u2,d1v2; gp_Vec2d _d2d;
        Surf1->D1(_u1,_v1,ptbid,d1u1,d1v1);
        norm1 = d1u1.Crossed(d1v1);
        Surf2->D1(_u2,_v2,ptbid,d1u2,d1v2);
        norm2 = d1u2.Crossed(d1v2);

        thesegm.Curve()->D1(paramf,_p2d,_d2d);
        if(reversed) { 
          tgline.SetLinearForm(_d2d.X(),d1u1,_d2d.Y(),d1v1);
        }
        else { 
          tgline.SetLinearForm(_d2d.X(),d1u2,_d2d.Y(),d1v2);
        }
        _u1=tgline.DotCross(norm2,norm1);
        TransitionOK=Standard_True;
        if (_u1 > 0.00000001) {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if(_u1 < -0.00000001) { 
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else { 
          TransitionOK=Standard_False;
        }
      }
      if(thesegm.HasLastPoint()) {  
        Standard_Real _u1,_v1,_u2,_v2;

        dolast = Standard_True;
        PStartl = thesegm.LastPoint();
        paraml = PStartl.Parameter();

        gp_Pnt2d _p2d = thesegm.Curve()->Value(paraml);
        Handle(Adaptor3d_HVertex) _vtx;
        if(PStartl.IsNew()==Standard_False) 
          _vtx = PStartl.Vertex();
        const gp_Pnt& _Pp = PStartl.Value();
        IntPatch_Point _thepoint;
        _thepointAtEnd.SetValue(_Pp,PStartl.Tolerance(),Standard_False);
        if (!reversed) { //-- typeS1 = Pln || Cyl || Sph || Cone
          Quad.Parameters(_Pp,_u1,_v1);
          _u2=_p2d.X(); _v2=_p2d.Y();
        }
        else {  //-- typeS1 != Pln && Cyl && Sph && Cone
          Quad.Parameters(_Pp,_u2,_v2);
          _u1=_p2d.X(); _v1=_p2d.Y();
        }
        _thepointAtEnd.SetParameters(_u1,_v1,_u2,_v2);
        _thepointAtEnd.SetParameter(paraml);
        if(PStartl.IsNew()==Standard_False)
          _thepointAtEnd.SetVertex(reversed,_vtx);
        _thepointAtEnd.SetArc(reversed,thesegm.Curve(),paraml,TLineUnk,TArcUnk);



        gp_Vec d1u1,d1v1,d1u2,d1v2; gp_Vec2d _d2d;
        Surf1->D1(_u1,_v1,ptbid,d1u1,d1v1);
        norm1 = d1u1.Crossed(d1v1);
        Surf2->D1(_u2,_v2,ptbid,d1u2,d1v2);
        norm2 = d1u2.Crossed(d1v2);

        thesegm.Curve()->D1(paraml,_p2d,_d2d);
        if(reversed) { 
          tgline.SetLinearForm(_d2d.X(),d1u1,_d2d.Y(),d1v1);
        }
        else { 
          tgline.SetLinearForm(_d2d.X(),d1u2,_d2d.Y(),d1v2);
        }
        _u1=tgline.DotCross(norm2,norm1);
        TransitionOK=Standard_True;
        if (_u1 > 0.00000001) {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if(_u1 < -0.00000001) { 
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else { 
          TransitionOK=Standard_False;
        }	
      }
      if(TransitionOK==Standard_False) { 
        //-- rline = new IntPatch_RLine (thesegm.Curve(),reversed,Standard_False);
        rline =  new IntPatch_RLine (Standard_False);
        if(reversed) { 
          rline->SetArcOnS1(thesegm.Curve());
        }
        else { 
          rline->SetArcOnS2(thesegm.Curve());
        }
      }
      else { 
        //-- rline = new IntPatch_RLine (thesegm.Curve(),reversed,Standard_False,trans1,trans2);
        rline =  new IntPatch_RLine (Standard_False,trans1,trans2);
        if(reversed) { 
          rline->SetArcOnS1(thesegm.Curve());
        }
        else { 
          rline->SetArcOnS2(thesegm.Curve());
        }
      }

      //------------------------------
      //-- Ajout des points 
      //--
      if (thesegm.HasFirstPoint()) {
        rline->AddVertex(_thepointAtBeg);
        rline->SetFirstPoint(rline->NbVertex());
      }

      if (thesegm.HasLastPoint()) {
        rline->AddVertex(_thepointAtEnd);
        rline->SetLastPoint(rline->NbVertex());
      }

      // Polygone sur restriction solution
      if (dofirst && dolast) {
        Standard_Real prm;
        gp_Pnt ptpoly;
        IntSurf_PntOn2S p2s;
        Handle(IntSurf_LineOn2S) Thelin = new IntSurf_LineOn2S ();
        Handle(Adaptor2d_HCurve2d) arcsegm = thesegm.Curve();
        Standard_Integer nbsample = 100;

        if (!reversed) {
          for (Standard_Integer j=1; j<=nbsample; j++) {
            prm = paramf + (j-1)*(paraml-paramf)/(nbsample-1);
            arcsegm->D0(prm,p2d);
            Surf2->D0(p2d.X(),p2d.Y(),ptpoly);

            Quad.Parameters(ptpoly,U1,V1);
            p2s.SetValue(ptpoly,U1,V1,p2d.X(),p2d.Y());
            Thelin->Add(p2s);
          }
        }
        else {
          for (Standard_Integer j=1; j<=nbsample; j++) {
            prm = paramf + (j-1)*(paraml-paramf)/(nbsample-1);
            arcsegm->D0(prm,p2d);
            Surf1->D0(p2d.X(),p2d.Y(),ptpoly);

            Quad.Parameters(ptpoly,U2,V2);
            p2s.SetValue(ptpoly,p2d.X(),p2d.Y(),U2,V2);
            Thelin->Add(p2s);
          }
        }
        rline->Add(Thelin);
      }

      if (dofirst || dolast) {
        Nblines = slin.Length();
        for (Standard_Integer j=1; j<=Nblines; j++) {
          const Handle(IntPatch_Line)& slinj = slin(j);
          typ = slinj->ArcType();
          if (typ == IntPatch_Walking) {
            Nbpts = Handle(IntPatch_WLine)::DownCast (slinj)->NbVertex();
          }
          else {
            Nbpts = Handle(IntPatch_RLine)::DownCast (slinj)->NbVertex();
          }
          for (Standard_Integer k=1; k<=Nbpts;k++) {
            if (typ == IntPatch_Walking) {
              ptdeb = Handle(IntPatch_WLine)::DownCast (slinj)->Vertex(k);
            }
            else {
              ptdeb = Handle(IntPatch_RLine)::DownCast (slinj)->Vertex(k);
            }
            if (dofirst) {

              if (ptdeb.Value().Distance(PStartf.Value()) <=TolArc) {
                ptdeb.SetMultiple(Standard_True);
                if (typ == IntPatch_Walking) {
                  Handle(IntPatch_WLine)::DownCast (slinj)->Replace(k,ptdeb);
                }
                else {
                  Handle(IntPatch_RLine)::DownCast (slinj)->Replace(k,ptdeb);
                }
                ptdeb.SetParameter(paramf);
                rline->AddVertex(ptdeb);
                if (!procf){
                  procf=Standard_True;
                  rline->SetFirstPoint(rline->NbVertex());
                }
              }
            }
            if (dolast) {
              if(dofirst) { //-- on recharge le ptdeb
                if (typ == IntPatch_Walking) {
                  ptdeb = Handle(IntPatch_WLine)::DownCast (slinj)->Vertex(k);
                }
                else {
                  ptdeb = Handle(IntPatch_RLine)::DownCast (slinj)->Vertex(k);
                }
              }
              if (ptdeb.Value().Distance(PStartl.Value()) <=TolArc) {
                ptdeb.SetMultiple(Standard_True);
                if (typ == IntPatch_Walking) {
                  Handle(IntPatch_WLine)::DownCast (slinj)->Replace(k,ptdeb);
                }
                else {
                  Handle(IntPatch_RLine)::DownCast (slinj)->Replace(k,ptdeb);
                }
                ptdeb.SetParameter(paraml);
                rline->AddVertex(ptdeb);
                if (!procl){
                  procl=Standard_True;
                  rline->SetLastPoint(rline->NbVertex());
                }
              }
            }
          }
        }
      }
      slin.Append(rline);
    }
  }// if (NbSegm) 
  //
  // on traite les restrictions de la surface implicite

  for (Standard_Integer i=1, aNbLin = slin.Length(); i<=aNbLin; i++)
  {
    Handle(IntPatch_PointLine) aL = Handle(IntPatch_PointLine)::DownCast(slin(i));
    
    if (!reversed)
      IntPatch_RstInt::PutVertexOnLine(aL,Surf1,D1,Surf2,Standard_True,TolTang);
    else
      IntPatch_RstInt::PutVertexOnLine(aL,Surf2,D2,Surf1,Standard_False,TolTang);

    if (aL->NbPnts() <= 2)
    {
      Standard_Boolean aCond = aL->NbPnts() < 2;
      if (!aCond)
        aCond = (aL->Point(1).IsSame(aL->Point(2), Precision::Confusion()));

      if (aCond)
      {
        slin.Remove(i);
        i--;
        aNbLin--;
        continue;
      }
    }

    if(aL->ArcType() == IntPatch_Walking)
    {
      const Handle(IntPatch_WLine) aWL = Handle(IntPatch_WLine)::DownCast(aL);
      slin.Append(aWL);
      slin.Remove(i);
      i--;
      aNbLin--;
    }
  }

  // Now slin is filled as follows: lower indices correspond to Restriction line,
  // after (higher indices) - only Walking-line.

  const Standard_Real aTol3d = Max(Func.Tolerance(), TolTang); 
  const Handle(Adaptor3d_HSurface)& aQSurf = (reversed) ? Surf2 : Surf1;
  const Handle(Adaptor3d_HSurface)& anOtherSurf = (reversed) ? Surf1 : Surf2;

  for (Standard_Integer i = 1; i <= slin.Length(); i++)
  {
    const Handle(IntPatch_PointLine)& aL1 = Handle(IntPatch_PointLine)::DownCast(slin(i));
    const Handle(IntPatch_RLine)& aRL1 = Handle(IntPatch_RLine)::DownCast(aL1);

    if(aRL1.IsNull())
    {
      //Walking-Walking cases are not supported
      break;
    }

    const Handle(Adaptor2d_HCurve2d)& anArc = aRL1->IsArcOnS1() ? 
                                              aRL1->ArcOnS1() :
                                              aRL1->ArcOnS2();
    if(anArc->Curve2d().GetType() != GeomAbs_Line)
    {
      //Restriction line must be isoline.
      //Other cases are not supported by
      //existing algorithms.

      break;
    }

    Standard_Boolean isFirstDeleted = Standard_False;

    for(Standard_Integer j = i + 1; j <= slin.Length(); j++)
    {
      Handle(IntPatch_PointLine) aL2 = Handle(IntPatch_PointLine)::DownCast(slin(j));
      Handle(IntPatch_RLine) aRL2 = Handle(IntPatch_RLine)::DownCast(aL2);

      //Here aL1 (i-th line) is Restriction-line and aL2 (j-th line) is
      //Restriction or Walking

      if(!aRL2.IsNull())
      {
        const Handle(Adaptor2d_HCurve2d)& anArc2 = aRL2->IsArcOnS1() ?
                                                   aRL2->ArcOnS1() :
                                                   aRL2->ArcOnS2();
        if(anArc2->Curve2d().GetType() != GeomAbs_Line)
        {
          //Restriction line must be isoline.
          //Other cases are not supported by
          //existing algorithms.

          continue;
        }
      }

      //aDir can be equal to one of following four values only
      //(because Reastriction line is boundary of rectangular surface):
      //either {0, 1} or {0, -1} or {1, 0} or {-1, 0}.
      const gp_Dir2d aDir = anArc->Curve2d().Line().Direction();

      Standard_Real aTol2d = anOtherSurf->UResolution(aTol3d),
                    aPeriod = anOtherSurf->IsVPeriodic() ? anOtherSurf->VPeriod() : 0.0;

      if(Abs(aDir.X()) < 0.5)
      {//Restriction directs along V-direction
        aTol2d = anOtherSurf->VResolution(aTol3d);
        aPeriod = anOtherSurf->IsUPeriodic() ? anOtherSurf->UPeriod() : 0.0;
      }

      const Standard_Boolean isCoincide = IsCoincide(Func, aL2, anArc, aRL1->IsArcOnS1(),
                                                      aTol3d, aTol2d, aPeriod);

      if(isCoincide)
      {
        if(aRL2.IsNull())
        {//Delete Walking-line
          slin.Remove(j);
          j--;
        }
        else
        {//Restriction-Restriction
          const Handle(Adaptor2d_HCurve2d)& anArc2 = aRL2->IsArcOnS1() ?
                                                     aRL2->ArcOnS1() :
                                                     aRL2->ArcOnS2();

          const Standard_Real aRange2 = anArc2->LastParameter() - 
                                        anArc2->FirstParameter();
          const Standard_Real aRange1 = anArc->LastParameter() -
                                        anArc->FirstParameter();

          if(aRange2 > aRange1)
          {
            isFirstDeleted = Standard_True;
            break;
          }
          else
          {//Delete j-th line
            slin.Remove(j);
            j--;
          }
        }
      }
    } //for(Standard_Integer j = i + 1; j <= slin.Length(); j++)

    if(isFirstDeleted)
    {//Delete i-th line
      slin.Remove(i--);
    }
  }//for (Standard_Integer i = 1; i <= slin.Length(); i++)

  empt = (slin.Length() == 0 && spnt.Length() == 0);
  done = Standard_True;


  if(slin.Length() == 0)
    return;

  Standard_Boolean isDecomposeRequired =  (Quad.TypeQuadric() == GeomAbs_Cone) || 
                                          (Quad.TypeQuadric() == GeomAbs_Sphere) ||
                                          (Quad.TypeQuadric() == GeomAbs_Cylinder) ||
                                          (Quad.TypeQuadric() == GeomAbs_Torus);

  if(!isDecomposeRequired)
    return;

  // post processing for cones and spheres

  const Handle(Adaptor3d_TopolTool)& PDomain = (reversed) ? D1 : D2;

  IntPatch_SequenceOfLine dslin;
  Standard_Boolean isDecompose = Standard_False;
  for(Standard_Integer i = 1; i <= slin.Length(); i++ )
  {
    if(DecomposeResult( Handle(IntPatch_PointLine)::DownCast(slin(i)),
                                        reversed, Quad, PDomain, aQSurf,
                                        anOtherSurf, TolArc, aTol3d, dslin))
    {
      isDecompose = Standard_True;
    }
  }

  if(!isDecompose)
    return;

  slin.Clear();
  for(Standard_Integer i = 1; i <= dslin.Length(); i++ )
    slin.Append(dslin(i));
}

// correct U parameter of the start point of line on Quadric
// (change 0->2PI or vs, if necessary)
static Standard_Real AdjustUFirst(Standard_Real U1,Standard_Real U2)
{
  Standard_Real u = U1;

  // case: no adjustment
  if( U1 > 0. && U1 < (2.*M_PI) )
    return u;

  // case: near '0'
  if( U1 == 0. || fabs(U1) <= 1.e-9 ) {
    if( U2 > 0. && U2 < (2.*M_PI) )
      u = ( U2 < ((2.*M_PI)-U2) ) ? 0. : (2.*M_PI);
    else {
      Standard_Real uu = U2;
      if( U2 > (2.*M_PI) )
        while( uu > (2.*M_PI) )
          uu -= (2.*M_PI);
      else 
        while( uu < 0.)
          uu += (2.*M_PI);

      u = ( uu < ((2.*M_PI)-uu) ) ? 0. : (2.*M_PI);
    }
  }
  // case: near '2PI'
  else if( U1 == (2.*M_PI) || fabs((2.*M_PI)-fabs(U1)) <= 1.e-9 ) {
    if( U2 > 0. && U2 < (2.*M_PI) )
      u = ( U2 < ((2.*M_PI)-U2) ) ? 0. : (2.*M_PI);
    else {
      Standard_Real uu = U2;
      if( U2 > (2.*M_PI) )
        while( uu > (2.*M_PI) )
          uu -= (2.*M_PI);
      else 
        while( uu < 0.)
          uu += (2.*M_PI);

      u = ( uu < ((2.*M_PI)-uu) ) ? 0. : (2.*M_PI);
    }
  }
  // case: '<0. || >2PI'
  else {
    if(U1 < 0.)
      while(u < 0.)
        u += 2.*M_PI;
    if(U1 > (2.*M_PI))
      while(u > (2.*M_PI))
        u -= (2.*M_PI);
  }

  return u;
}

// collect vertices, reject equals
static Handle(IntSurf_LineOn2S) GetVertices(const Handle(IntPatch_PointLine)& thePLine,
                                            const Standard_Real           TOL3D,
                                            const Standard_Real           TOL2D)
{
  //  Standard_Real TOL3D = 1.e-12, TOL2D = 1.e-8;

  Handle(IntSurf_LineOn2S) vertices = new IntSurf_LineOn2S();

  Standard_Real U1 = 0., U2 = 0., V1 = 0., V2 = 0.;
  Standard_Integer i = 0, k = 0;
  Standard_Integer NbVrt = thePLine->NbVertex();

  TColStd_Array1OfInteger anVrts(1,NbVrt);
  anVrts.Init(0);

  // check equal vertices
  for(i = 1; i <= NbVrt; i++) {

    if( anVrts(i) == -1 ) continue;

    const IntPatch_Point& Pi = thePLine->Vertex(i);

    for(k = (i+1); k <= NbVrt; k++) {

      if( anVrts(k) == -1 ) continue;

      const IntPatch_Point& Pk = thePLine->Vertex(k);

      if(Pi.Value().Distance(Pk.Value()) <= TOL3D) {
        // suggest the points are equal;
        // test 2d parameters on surface
        Standard_Boolean sameU1 = Standard_False;
        Standard_Boolean sameV1 = Standard_False;
        Standard_Boolean sameU2 = Standard_False;
        Standard_Boolean sameV2 = Standard_False;

        Pi.ParametersOnS1(U1,V1);
        Pk.ParametersOnS1(U2,V2);
        if(fabs(U1-U2) <= TOL2D) sameU1 = Standard_True;
        if(fabs(V1-V2) <= TOL2D) sameV1 = Standard_True;

        Pi.ParametersOnS2(U1,V1);
        Pk.ParametersOnS2(U2,V2);
        if(fabs(U1-U2) <= TOL2D) sameU2 = Standard_True;
        if(fabs(V1-V2) <= TOL2D) sameV2 = Standard_True;

        if((sameU1 && sameV1) && (sameU2 && sameV2))
          anVrts(k) = -1;
      }
    }
  }

  // copy further processed vertices
  for(i = 1; i <= NbVrt; i++) {
    if( anVrts(i) == -1 ) continue;
    vertices->Add(thePLine->Vertex(i).PntOn2S());
  }
  return vertices;
}

static void SearchVertices(const Handle(IntSurf_LineOn2S)& Line,
  const Handle(IntSurf_LineOn2S)& Vertices,
  TColStd_Array1OfInteger&        PTypes)
{
  Standard_Integer nbp = Line->NbPoints(), nbv = Vertices->NbPoints();
  Standard_Integer ip = 0, iv = 0;
  for(ip = 1; ip <= nbp; ip++) {
    const IntSurf_PntOn2S& aP = Line->Value(ip);
    Standard_Integer type = 0;
    for(iv = 1; iv <= nbv; iv++) {
      const IntSurf_PntOn2S& aV = Vertices->Value(iv);
      if(aP.IsSame(aV, Precision::Confusion(), Precision::PConfusion())) {
        type = iv; 
        break;
      }
    }
    PTypes(ip) = type;
  }
}

static inline Standard_Boolean IsSeamParameter(const Standard_Real U,
  const Standard_Real TOL2D)
{
  return (fabs(U) <= TOL2D || fabs(2.*M_PI - U) <= TOL2D);
}

static inline Standard_Real AdjustU(const Standard_Real U)
{
  Standard_Real u = U, DBLPI = 2.*M_PI;
  if(u < 0. || u > DBLPI) {
    if(u < 0.)
      while(u < 0.)
        u += DBLPI;
    else
      while(u > DBLPI)
        u -= DBLPI;
  }
  return u;
}

static inline void Correct2DBounds(const Standard_Real UF,
  const Standard_Real UL,
  const Standard_Real VF,
  const Standard_Real VL,
  const Standard_Real TOL2D,
  Standard_Real&      U,
  Standard_Real&      V)
{
  Standard_Real Eps = 1.e-16;
  Standard_Real dUF = fabs(U - UF);
  Standard_Real dUL = fabs(U - UL);
  Standard_Real dVF = fabs(V - VF);
  Standard_Real dVL = fabs(V - VL);
  if(dUF <= TOL2D && dUF > Eps) U = UF;
  if(dUL <= TOL2D && dUL > Eps) U = UL;
  if(dVF <= TOL2D && dVF > Eps) V = VF;
  if(dVL <= TOL2D && dVL > Eps) V = VL;
}

static void AdjustLine(Handle(IntSurf_LineOn2S)& Line,
  const Standard_Boolean    IsReversed,
  const Handle(Adaptor3d_HSurface)&         QSurf,
  const Standard_Real       TOL2D)
{
  Standard_Real VF = QSurf->FirstVParameter();
  Standard_Real VL = QSurf->LastVParameter();
  Standard_Real UF = QSurf->FirstUParameter();
  Standard_Real UL = QSurf->LastUParameter();

  Standard_Integer nbp = Line->NbPoints(), ip = 0;
  Standard_Real U = 0., V = 0.;
  for(ip = 1; ip <= nbp; ip++) {
    if(IsReversed) {
      Line->Value(ip).ParametersOnS2(U,V);
      U = AdjustU(U);
      Correct2DBounds(UF,UL,VF,VL,TOL2D,U,V);
      Line->SetUV(ip,Standard_False,U,V);
    }
    else {
      Line->Value(ip).ParametersOnS1(U,V);
      U = AdjustU(U);
      Correct2DBounds(UF,UL,VF,VL,TOL2D,U,V);
      Line->SetUV(ip,Standard_True,U,V);
    }
  }
}

static Standard_Boolean InsertSeamVertices(Handle(IntSurf_LineOn2S)&       Line,
  const Standard_Boolean          IsReversed,
  Handle(IntSurf_LineOn2S)&       Vertices,
  const TColStd_Array1OfInteger&  PTypes,
  const Standard_Real             TOL2D)
{
  Standard_Boolean result = Standard_False;
  Standard_Integer ip = 0, nbp = Line->NbPoints();
  Standard_Real U = 0., V = 0.;
  for(ip = 1; ip <= nbp; ip++) {
    Standard_Integer ipt = PTypes(ip);
    if(ipt != 0) {
      const IntSurf_PntOn2S& aP = Line->Value(ip);
      if(IsReversed)
        aP.ParametersOnS2(U,V); // S2 - quadric
      else
        aP.ParametersOnS1(U,V); // S1 - quadric
      U = AdjustU(U);
      if(IsSeamParameter(U,TOL2D)) {
        if(ip == 1 || ip == nbp) {
          Standard_Real U1 = 0., V1 = 0.;
          Standard_Integer ipp = (ip == 1) ? (ip+1) : (ip-1);
          if(IsReversed)
            Line->Value(ipp).ParametersOnS2(U1,V1); // S2 - quadric
          else
            Line->Value(ipp).ParametersOnS1(U1,V1); // S1 - quadric
          Standard_Real u = AdjustUFirst(U,U1);
          if(fabs(u-U) >= 1.5*M_PI) {
            Standard_Real U2 = 0., V2 = 0.;
            if(IsReversed) {
              Line->Value(ip).ParametersOnS1(U2,V2); // prm
              Line->SetUV(ip,Standard_False,u,V);
              Line->SetUV(ip,Standard_True,U2,V2);
            }
            else {
              Line->Value(ip).ParametersOnS2(U2,V2); // prm
              Line->SetUV(ip,Standard_True,u,V);
              Line->SetUV(ip,Standard_False,U2,V2);
            }
          }
        }
        else {
          Standard_Integer ipp = ip - 1;
          Standard_Integer ipn = ip + 1;
          Standard_Real U1 = 0., V1 = 0., U2 = 0., V2 = 0.;
          if(IsReversed) {
            Line->Value(ipp).ParametersOnS2(U1,V1); // quad
            Line->Value(ipn).ParametersOnS2(U2,V2); // quad
          }
          else {
            Line->Value(ipp).ParametersOnS1(U1,V1); // quad
            Line->Value(ipn).ParametersOnS1(U2,V2); // quad
          }
          U1 = AdjustU(U1);
          U2 = AdjustU(U2);
          Standard_Boolean pnearZero = (fabs(U1) < fabs(2.*M_PI-U1)) ? Standard_True : Standard_False;
          Standard_Boolean cnearZero = (fabs(U) < fabs(2.*M_PI-U)) ? Standard_True : Standard_False;
          if(pnearZero == cnearZero) {
            if(!IsSeamParameter(U2,TOL2D) && !IsSeamParameter(U1,TOL2D)) {
              Standard_Real nU = (cnearZero) ? (2.*M_PI) : 0.;
              IntSurf_PntOn2S nP;
              nP.SetValue(aP.Value());
              Standard_Real U3 = 0., V3 = 0.;
              if(IsReversed) {
                Line->Value(ip).ParametersOnS1(U3,V3); // prm
                nP.SetValue(Standard_False,nU,V);
                nP.SetValue(Standard_True,U3,V3);
              }
              else {
                Line->Value(ip).ParametersOnS2(U3,V3); // prm
                nP.SetValue(Standard_True,nU,V);
                nP.SetValue(Standard_False,U3,V3);
              }
              Line->InsertBefore(ipn,nP);
              Vertices->Add(nP);
              result = Standard_True;
              break;
            }
          }
          else {
            if(!IsSeamParameter(U2,TOL2D) && !IsSeamParameter(U1,TOL2D)) {
              Standard_Real nU = (cnearZero) ? (2.*M_PI) : 0.;
              IntSurf_PntOn2S nP;
              nP.SetValue(aP.Value());
              Standard_Real U3 = 0., V3 = 0.;
              if(IsReversed) {
                Line->Value(ip).ParametersOnS1(U3,V3); // prm
                nP.SetValue(Standard_False,nU,V);
                nP.SetValue(Standard_True,U3,V3);
              }
              else {
                Line->Value(ip).ParametersOnS2(U3,V3); // prm
                nP.SetValue(Standard_True,nU,V);
                nP.SetValue(Standard_False,U3,V3);
              }
              Line->InsertBefore(ip,nP);
              Vertices->Add(nP);
              result = Standard_True;
              break;
            }
            else {
              // Line->InsertBefore(ip,Line->Value(ipn));
              // Line->RemovePoint(ip+2);
              // result = Standard_True;
              // cout << "swap vertex " << endl;
              // break;
            }
          }
        }
      }
    }
  }
  return result;
}

static void ToSmooth( const Handle(IntSurf_LineOn2S)& Line,
                      const Standard_Boolean          IsReversed,
                      const IntSurf_Quadric&          Quad,
                      const Standard_Boolean          IsFirst,
                      Standard_Real&                  D3D)
{
  if(Line->NbPoints() <= 10)
    return;

  D3D = 0.;
  Standard_Integer NbTestPnts = Line->NbPoints() / 5;
  if(NbTestPnts < 5) NbTestPnts = 5;

  Standard_Integer startp = (IsFirst) ? 2 : (Line->NbPoints() - NbTestPnts - 2);
  Standard_Integer ip = 0;
  Standard_Real Uc = 0., Vc = 0., Un = 0., Vn = 0., DDU = 0., DDV = 0.;

  for(ip = startp; ip <= NbTestPnts; ip++) {
    if(IsReversed) {
      Line->Value(ip).ParametersOnS2(Uc,Vc); // S2 - quadric
      Line->Value(ip+1).ParametersOnS2(Un,Vn);
    }
    else {
      Line->Value(ip).ParametersOnS1(Uc,Vc); // S1 - quadric
      Line->Value(ip+1).ParametersOnS1(Un,Vn);
    }
    DDU += fabs(fabs(Uc)-fabs(Un));
    DDV += fabs(fabs(Vc)-fabs(Vn));

    if(ip > startp) {
      Standard_Real DP = Line->Value(ip).Value().Distance(Line->Value(ip-1).Value());
      D3D += DP;
    }
  }

  DDU /= (Standard_Real) NbTestPnts + 1;
  DDV /= (Standard_Real) NbTestPnts + 1;

  D3D /= (Standard_Real) NbTestPnts + 1;


  Standard_Integer Index1 = (IsFirst) ? 1 : (Line->NbPoints());
  Standard_Integer Index2 = (IsFirst) ? 2 : (Line->NbPoints()-1);
  Standard_Integer Index3 = (IsFirst) ? 3 : (Line->NbPoints()-2);

  Standard_Boolean doU = Standard_False;

  Standard_Real U1 = 0., U2 = 0., V1 = 0., V2 = 0., U3 = 0., V3 = 0.;

  if(IsReversed) {
    Line->Value(Index1).ParametersOnS2(U1,V1); // S2 - quadric
    Line->Value(Index2).ParametersOnS2(U2,V2);
    Line->Value(Index3).ParametersOnS2(U3,V3);
  }
  else {
    Line->Value(Index1).ParametersOnS1(U1,V1); // S1 - quadric
    Line->Value(Index2).ParametersOnS1(U2,V2);
    Line->Value(Index3).ParametersOnS1(U3,V3);
  }

  if(!doU && Quad.TypeQuadric() == GeomAbs_Sphere) {
    if(fabs(fabs(U1)-fabs(U2)) > (M_PI/16.)) doU = Standard_True;

    if(doU && (fabs(U1) <= 1.e-9 || fabs(U1-2.*M_PI) <= 1.e-9)) {
      if(fabs(V1-M_PI/2.) <= 1.e-9 || fabs(V1+M_PI/2.) <= 1.e-9) {}
      else {
        doU = Standard_False;
      }
    }
  }

  if(Quad.TypeQuadric() == GeomAbs_Cone) {
    Standard_Real Uapx = 0., Vapx = 0.;
    Quad.Parameters(Quad.Cone().Apex(),Uapx,Vapx);

    if(fabs(fabs(U1)-fabs(U2)) > M_PI/32.) doU = Standard_True;

    if(doU && (fabs(U1) <= 1.e-9 || fabs(U1-2.*M_PI) <= 1.e-9)) {
      if(fabs(V1-Vapx) <= 1.e-9) {}
      else {
        doU = Standard_False;
      }
    }
  }

  if(doU) {
    Standard_Real dU = Min((DDU/10.),5.e-8);
    Standard_Real U = (U2 > U3) ? (U2 + dU) : (U2 - dU);
    if(IsReversed)
      Line->SetUV(Index1,Standard_False,U,V1);
    else
      Line->SetUV(Index1,Standard_True,U,V1);
    U1 = U;
  }
} 

static Standard_Boolean TestMiddleOnPrm(const IntSurf_PntOn2S& aP,
                                        const IntSurf_PntOn2S& aV,
                                        const Standard_Boolean IsReversed,
                                        const Standard_Real    ArcTol,
                                        const Handle(Adaptor3d_TopolTool)&  PDomain)

{
  Standard_Boolean result = Standard_False;
  Standard_Real Up = 0., Vp = 0., Uv = 0., Vv = 0.;
  if(IsReversed) {
    aP.ParametersOnS1(Up,Vp); //S1 - parametric
    aV.ParametersOnS1(Uv,Vv);
  }
  else {
    aP.ParametersOnS2(Up,Vp); // S2 - parametric
    aV.ParametersOnS2(Uv,Vv);
  }
  Standard_Real Um = (Up + Uv)*0.5, Vm = (Vp + Vv)*0.5;
  gp_Pnt2d a2DPntM(Um,Vm);
  TopAbs_State PosM = PDomain->Classify(a2DPntM,ArcTol);
  if(PosM == TopAbs_ON || PosM == TopAbs_IN )
    result = Standard_True;
  return result;
}

static void VerifyVertices( const Handle(IntSurf_LineOn2S)&    Line,
                            const Standard_Boolean             IsReversed,
                            const Handle(IntSurf_LineOn2S)&    Vertices,
                            const Standard_Real                TOL2D,
                            const Standard_Real       ArcTol,
                            const Handle(Adaptor3d_TopolTool)& PDomain,
                            IntSurf_PntOn2S&          VrtF,
                            Standard_Boolean&         AddFirst,
                            IntSurf_PntOn2S&          VrtL,
                            Standard_Boolean&         AddLast)
{
  Standard_Integer nbp = Line->NbPoints(), nbv = Vertices->NbPoints();
  Standard_Integer FIndexSame = 0, FIndexNear = 0, LIndexSame = 0, LIndexNear = 0;
  const IntSurf_PntOn2S& aPF = Line->Value(1);
  const IntSurf_PntOn2S& aPL = Line->Value(nbp);
  Standard_Real UF = 0., VF = 0., UL = 0., VL = 0.;
  if(IsReversed) {
    aPF.ParametersOnS2(UF,VF);
    aPL.ParametersOnS2(UL,VL);
  }
  else {
    aPF.ParametersOnS1(UF,VF);
    aPL.ParametersOnS1(UL,VL);
  }
  gp_Pnt2d a2DPF(UF,VF);
  gp_Pnt2d a2DPL(UL,VL);
  Standard_Real DistMinF = 1.e+100, DistMinL = 1.e+100;
  Standard_Integer FConjugated = 0, LConjugated = 0;

  Standard_Integer iv = 0;

  for(iv = 1; iv <= nbv; iv++) {
    Standard_Real Uv = 0., Vv = 0.;
    if(IsReversed) {
      Vertices->Value(iv).ParametersOnS2(Uv,Vv);
      Uv = AdjustU(Uv);
      Vertices->SetUV(iv,Standard_False,Uv,Vv);
    }
    else {
      Vertices->Value(iv).ParametersOnS1(Uv,Vv);
      Uv = AdjustU(Uv);
      Vertices->SetUV(iv,Standard_True,Uv,Vv);
    }
  }

  for(iv = 1; iv <= nbv; iv++) {
    const IntSurf_PntOn2S& aV = Vertices->Value(iv);
    if(aPF.IsSame(aV, Precision::Confusion(), Precision::PConfusion())) {
      FIndexSame = iv;
      break;
    }
    else {
      Standard_Real Uv = 0., Vv = 0.;
      if(IsReversed)
        aV.ParametersOnS2(Uv,Vv);
      else
        aV.ParametersOnS1(Uv,Vv);
      gp_Pnt2d a2DV(Uv,Vv);
      Standard_Real Dist = a2DV.Distance(a2DPF);
      if(Dist < DistMinF) {
        DistMinF = Dist;
        FIndexNear = iv;
        if(FConjugated != 0)
          FConjugated = 0;
      }
      if(IsSeamParameter(Uv,TOL2D)) {
        Standard_Real Ucv = (fabs(Uv) < fabs(2.*M_PI-Uv)) ? (2.*M_PI) : 0.;
        gp_Pnt2d a2DCV(Ucv,Vv);
        Standard_Real CDist = a2DCV.Distance(a2DPF);
        if(CDist < DistMinF) {
          DistMinF = CDist;
          FConjugated = iv;
          FIndexNear = iv;
        }
      }
    }
  }

  for(iv = 1; iv <= nbv; iv++) {
    const IntSurf_PntOn2S& aV = Vertices->Value(iv);
    if(aPL.IsSame(aV, Precision::Confusion(), Precision::PConfusion())) {
      LIndexSame = iv;
      break;
    }
    else {
      Standard_Real Uv = 0., Vv = 0.;
      if(IsReversed)
        aV.ParametersOnS2(Uv,Vv);
      else
        aV.ParametersOnS1(Uv,Vv);
      gp_Pnt2d a2DV(Uv,Vv);
      Standard_Real Dist = a2DV.Distance(a2DPL);
      if(Dist < DistMinL) {
        DistMinL = Dist;
        LIndexNear = iv;
        if(LConjugated != 0)
          LConjugated = 0;
      }
      if(IsSeamParameter(Uv,TOL2D)) {
        Standard_Real Ucv = (fabs(Uv) < fabs(2.*M_PI-Uv)) ? (2.*M_PI) : 0.;
        gp_Pnt2d a2DCV(Ucv,Vv);
        Standard_Real CDist = a2DCV.Distance(a2DPL);
        if(CDist < DistMinL) {
          DistMinL = CDist;
          LConjugated = iv;
          LIndexNear = iv;
        }
      }
    }
  }

  AddFirst = Standard_False;
  AddLast  = Standard_False;

  if(FIndexSame == 0) {
    if(FIndexNear != 0) {
      const IntSurf_PntOn2S& aV = Vertices->Value(FIndexNear);
      Standard_Real Uv = 0., Vv = 0.;
      if(IsReversed)
        aV.ParametersOnS2(Uv,Vv);
      else
        aV.ParametersOnS1(Uv,Vv);
      if(IsSeamParameter(Uv,TOL2D)) {
        Standard_Real Ucv = (fabs(Uv) < fabs(2.*M_PI-Uv)) ? (2.*M_PI) : 0.;
        Standard_Boolean test = TestMiddleOnPrm(aPF,aV,IsReversed,ArcTol,PDomain);
        if(test) {
          VrtF.SetValue(aV.Value());
          if(IsReversed) {
            Standard_Real U2 = 0., V2 = 0.;
            aV.ParametersOnS1(U2,V2); // S1 - prm
            VrtF.SetValue(Standard_True,U2,V2);
            if(FConjugated == 0)
              VrtF.SetValue(Standard_False,Uv,Vv);
            else
              VrtF.SetValue(Standard_False,Ucv,Vv);
          }
          else {
            Standard_Real U2 = 0., V2 = 0.;
            aV.ParametersOnS2(U2,V2); // S2 - prm
            VrtF.SetValue(Standard_False,U2,V2);
            if(FConjugated == 0)
              VrtF.SetValue(Standard_True,Uv,Vv);
            else
              VrtF.SetValue(Standard_True,Ucv,Vv);
          }
          Standard_Real Dist3D = VrtF.Value().Distance(aPF.Value());
          if(Dist3D > 1.5e-7 && DistMinF > TOL2D) {
            AddFirst = Standard_True;
          }
        }
      }
      else {
        // to do: analyze internal vertex
      }
    }
  }

  if(LIndexSame == 0) {
    if(LIndexNear != 0) {
      const IntSurf_PntOn2S& aV = Vertices->Value(LIndexNear);
      Standard_Real Uv = 0., Vv = 0.;
      if(IsReversed)
        aV.ParametersOnS2(Uv,Vv);
      else
        aV.ParametersOnS1(Uv,Vv);
      if(IsSeamParameter(Uv,TOL2D)) {
        Standard_Real Ucv = (fabs(Uv) < fabs(2.*M_PI-Uv)) ? (2.*M_PI) : 0.;
        Standard_Boolean test = TestMiddleOnPrm(aPL,aV,IsReversed,ArcTol,PDomain);
        if(test) {
          VrtL.SetValue(aV.Value());
          if(IsReversed) {
            Standard_Real U2 = 0., V2 = 0.;
            aV.ParametersOnS1(U2,V2); // S1 - prm
            VrtL.SetValue(Standard_True,U2,V2);
            if(LConjugated == 0)
              VrtL.SetValue(Standard_False,Uv,Vv);
            else
              VrtL.SetValue(Standard_False,Ucv,Vv);
          }
          else {
            Standard_Real U2 = 0., V2 = 0.;
            aV.ParametersOnS2(U2,V2); // S2 - prm
            VrtL.SetValue(Standard_False,U2,V2);
            if(LConjugated == 0)
              VrtL.SetValue(Standard_True,Uv,Vv);
            else
              VrtL.SetValue(Standard_True,Ucv,Vv);
          }
          Standard_Real Dist3D = VrtL.Value().Distance(aPL.Value());
          if(Dist3D > 1.5e-7 && DistMinL > TOL2D) {
            AddLast = Standard_True;
          }
        }
      }
      else {
        // to do: analyze internal vertex
      }
    }
  }
}

static Standard_Boolean AddVertices(Handle(IntSurf_LineOn2S)& Line,
  const IntSurf_PntOn2S&    VrtF,
  const Standard_Boolean    AddFirst,
  const IntSurf_PntOn2S&    VrtL,
  const Standard_Boolean    AddLast,
  const Standard_Real       D3DF,
  const Standard_Real       D3DL)
{
  Standard_Boolean result = Standard_False;
  if(AddFirst) {
    Standard_Real DF = Line->Value(1).Value().Distance(VrtF.Value());
    if((D3DF*2.) > DF && DF > 1.5e-7) {
      Line->InsertBefore(1,VrtF);
      result = Standard_True;
    }
  }
  if(AddLast) {
    Standard_Real DL = Line->Value(Line->NbPoints()).Value().Distance(VrtL.Value());
    if((D3DL*2.) > DL && DL > 1.5e-7) {
      Line->Add(VrtL);
      result = Standard_True;
    }
  }
  return result;
}


static void PutIntVertices(const Handle(IntPatch_PointLine)&    Line,
  Handle(IntSurf_LineOn2S)& Result,
  Standard_Boolean          theIsReversed,
  Handle(IntSurf_LineOn2S)& Vertices,
  const Standard_Real       ArcTol)
{
  Standard_Integer nbp = Result->NbPoints(), nbv = Vertices->NbPoints();

  if(nbp < 3)
    return;

  const Handle(IntPatch_RLine) aRLine = Handle(IntPatch_RLine)::DownCast(Line);

  Standard_Integer ip = 0, iv = 0;
  gp_Pnt aPnt;
  IntPatch_Point thePnt;
  Standard_Real U1 = 0., V1 = 0., U2 = 0., V2 = 0.;

  for(ip = 2; ip <= (nbp-1); ip++) {
    const IntSurf_PntOn2S& aP = Result->Value(ip);
    for(iv = 1; iv <= nbv; iv++) {
      const IntSurf_PntOn2S& aV = Vertices->Value(iv);
      if(aP.IsSame(aV, Precision::Confusion(), Precision::PConfusion())) {
        aPnt = Result->Value(ip).Value();
        Result->Value(ip).ParametersOnS1(U1,V1);
        Result->Value(ip).ParametersOnS2(U2,V2);
        thePnt.SetValue(aPnt,ArcTol,Standard_False);
        thePnt.SetParameters(U1,V1,U2,V2);
        
        Standard_Real aParam = (Standard_Real)ip;

        if(!aRLine.IsNull())
        {
          //In fact, aRLine is always on the parametric surface.
          //If (theIsReversed == TRUE) then (U1, V1) - point on
          //parametric surface, otherwise - point on quadric.
          const Handle(Adaptor2d_HCurve2d)& anArc = aRLine->IsArcOnS1() ?
                                                    aRLine->ArcOnS1() :
                                                    aRLine->ArcOnS2();

          const gp_Lin2d aLin(anArc->Curve2d().Line());
          gp_Pnt2d aPSurf;

          if(theIsReversed)
          {
            aPSurf.SetCoord(U1, V1);
          }
          else
          {
            aPSurf.SetCoord(U2, V2);
          }

          aParam = ElCLib::Parameter(aLin, aPSurf);
        }
        
        thePnt.SetParameter(aParam);
        Line->AddVertex(thePnt);
      }
    }
  }
}

static Standard_Boolean HasInternals(Handle(IntSurf_LineOn2S)& Line,
  Handle(IntSurf_LineOn2S)& Vertices)
{
  Standard_Integer nbp = Line->NbPoints(), nbv = Vertices->NbPoints();
  Standard_Integer ip = 0, iv = 0;
  Standard_Boolean result = Standard_False;

  if(nbp < 3)
    return result;

  for(ip = 2; ip <= (nbp-1); ip++) {
    const IntSurf_PntOn2S& aP = Line->Value(ip);
    for(iv = 1; iv <= nbv; iv++) {
      const IntSurf_PntOn2S& aV = Vertices->Value(iv);
      if(aP.IsSame(aV, Precision::Confusion(), Precision::PConfusion())) {
        result = Standard_True;
        break;
      }
    }
    if(result)
      break;
  }

  return result;
}
static Handle(IntPatch_WLine) MakeSplitWLine (Handle(IntPatch_WLine)&        WLine,
  Standard_Boolean         Tang,
  IntSurf_TypeTrans        Trans1,
  IntSurf_TypeTrans        Trans2,
  Standard_Real            ArcTol,
  Standard_Integer         ParFirst,
  Standard_Integer         ParLast)
{
  Handle(IntSurf_LineOn2S) SLine = WLine->Curve();
  Handle(IntSurf_LineOn2S) sline = new IntSurf_LineOn2S();

  Standard_Integer ip = 0;
  for(ip = ParFirst; ip <= ParLast; ip++)
    sline->Add(SLine->Value(ip));

  Handle(IntPatch_WLine) wline = new IntPatch_WLine(sline,Tang,Trans1,Trans2);

  gp_Pnt aSPnt;
  IntPatch_Point TPntF,TPntL;
  Standard_Real uu1 = 0., vv1 = 0., uu2 = 0., vv2 = 0.;

  aSPnt = sline->Value(1).Value();
  sline->Value(1).ParametersOnS1(uu1,vv1);
  sline->Value(1).ParametersOnS2(uu2,vv2);
  TPntF.SetValue(aSPnt,ArcTol,Standard_False);
  TPntF.SetParameters(uu1,vv1,uu2,vv2);
  TPntF.SetParameter(1.);
  wline->AddVertex(TPntF);
  wline->SetFirstPoint(1);

  aSPnt =  sline->Value(sline->NbPoints()).Value();
  sline->Value(sline->NbPoints()).ParametersOnS1(uu1,vv1);
  sline->Value(sline->NbPoints()).ParametersOnS2(uu2,vv2);
  TPntL.SetValue(aSPnt,ArcTol,Standard_False);
  TPntL.SetParameters(uu1,vv1,uu2,vv2);
  TPntL.SetParameter((Standard_Real)sline->NbPoints());
  wline->AddVertex(TPntL);
  wline->SetLastPoint(wline->NbVertex());

  return wline;
}

static Standard_Boolean SplitOnSegments(Handle(IntPatch_WLine)&        WLine,
  Standard_Boolean         Tang,
  IntSurf_TypeTrans        Trans1,
  IntSurf_TypeTrans        Trans2,
  Standard_Real            ArcTol,
  IntPatch_SequenceOfLine& Segments)
{
  Standard_Boolean result = Standard_False;
  Segments.Clear();

  Standard_Integer nbv = WLine->NbVertex();
  if(nbv > 3) {
    Standard_Integer iv = 0;
    for(iv = 1; iv < nbv; iv++) {
      Standard_Integer firstPar = 
                    (Standard_Integer) WLine->Vertex(iv).ParameterOnLine();
      Standard_Integer lastPar  = 
                    (Standard_Integer) WLine->Vertex(iv+1).ParameterOnLine();
      if((lastPar - firstPar) <= 1)
        continue;
      else {
        Handle(IntPatch_WLine) splitwline = MakeSplitWLine(WLine,Tang,Trans1,Trans2,
                                                                ArcTol,firstPar,lastPar);
        Segments.Append(splitwline);
        if(!result)
          result = Standard_True;
      }
    }
  }
  return result;
}

//=======================================================================
//function : IsPointOnBoundary
//purpose  : Returns TRUE if point <theParam> matches <theBoundary +/- thePeriod>
//            with given tolerance criterion.
//            For not-periodic case, thePeriod must be equal to 0.0.
//=======================================================================
static Standard_Boolean IsPointOnBoundary(const Standard_Real theToler2D,
                                          const Standard_Real theBoundary,
                                          const Standard_Real thePeriod,
                                          const Standard_Real theParam)
{
  Standard_Real aDelta = Abs(theParam - theBoundary);
  if (thePeriod != 0.0)
  {
    aDelta = fmod(aDelta, thePeriod);
    
    // 0 <= aDelta < thePeriod
    return ((aDelta < theToler2D) || ((thePeriod - aDelta) < theToler2D));
  }

  // Here, thePeriod == 0.0, aDelta > 0.0

  return (aDelta < theToler2D);
}

//=======================================================================
//function : DetectOfBoundaryAchievement
//purpose  : Can change values of theNewLine (by adding the computed point on boundary,
//            which parameter will be adjusted) and theIsOnBoundary variables.
//=======================================================================
static void DetectOfBoundaryAchievement(const Handle(Adaptor3d_HSurface)& theQSurf, // quadric
                                        const Standard_Boolean theIsReversed,
                                        const Handle(IntSurf_LineOn2S)& theSourceLine,
                                        const Standard_Integer thePointIndex,
                                        const Standard_Real theToler2D,
                                        Handle(IntSurf_LineOn2S)& theNewLine,
                                        Standard_Boolean& theIsOnBoundary)
{
  const Standard_Real aUPeriod = theQSurf->IsUPeriodic() ? theQSurf->UPeriod() : 0.0,
                      aVPeriod = theQSurf->IsVPeriodic() ? theQSurf->VPeriod() : 0.0;
  const Standard_Real aUf = theQSurf->FirstUParameter(),
                      aUl = theQSurf->LastUParameter(),
                      aVf = theQSurf->FirstVParameter(),
                      aVl = theQSurf->LastVParameter();

  const IntSurf_PntOn2S &aPPrev = theSourceLine->Value(thePointIndex - 1),
                        &aPCurr = theSourceLine->Value(thePointIndex);
  Standard_Real aUPrev, aVPrev, aUCurr, aVCurr;
  if (theIsReversed)
  {
    aPPrev.ParametersOnS2(aUPrev, aVPrev);   // S2 - quadric, set U,V by Pnt3D
    aPCurr.ParametersOnS2(aUCurr, aVCurr);   // S2 - quadric, set U,V by Pnt3D
  }
  else
  {
    aPPrev.ParametersOnS1(aUPrev, aVPrev);    // S1 - quadric, set U,V by Pnt3D
    aPCurr.ParametersOnS1(aUCurr, aVCurr);    // S1 - quadric, set U,V by Pnt3D
  }

  // Ignore cases when the WLine goes along the surface boundary completely.

  if (IsPointOnBoundary(theToler2D, aUf, aUPeriod, aUCurr) &&
      !IsPointOnBoundary(theToler2D, aUf, aUPeriod, aUPrev))
  {
    theIsOnBoundary = Standard_True;
  }
  else if (IsPointOnBoundary(theToler2D, aUl, aUPeriod, aUCurr) &&
           !IsPointOnBoundary(theToler2D, aUl, aUPeriod, aUPrev))
  {
    theIsOnBoundary = Standard_True;
  }
  else if (IsPointOnBoundary(theToler2D, aVf, aVPeriod, aVCurr) &&
           !IsPointOnBoundary(theToler2D, aVf, aVPeriod, aVPrev))
  {
    theIsOnBoundary = Standard_True;
  }
  else if (IsPointOnBoundary(theToler2D, aVl, aVPeriod, aVCurr) &&
           !IsPointOnBoundary(theToler2D, aVl, aVPeriod, aVPrev))
  {
    theIsOnBoundary = Standard_True;
  }

  if (theIsOnBoundary)
  {
    // Adjust, to avoid bad jumping of the WLine.

    const Standard_Real aDu = (aUPrev - aUCurr);
    const Standard_Real aDv = (aVPrev - aVCurr);
    if (aUPeriod > 0.0 && (2.0*Abs(aDu) > aUPeriod))
    {
      aUCurr += Sign(aUPeriod, aDu);
    }

    if (aVPeriod > 0.0 && (2.0*Abs(aDv) > aVPeriod))
    {
      aVCurr += Sign(aVPeriod, aDv);
    }

    IntSurf_PntOn2S aPoint = aPCurr;
    aPoint.SetValue(!theIsReversed, aUCurr, aVCurr);
    theNewLine->Add(aPoint);
  }
}
//=======================================================================
//function : DecomposeResult
//purpose  : Split <theLine> in the places where it passes through seam edge
//            or singularity (apex of cone or pole of sphere).
//            This passage is detected by jump of U-parameter
//            from point to point.
//=======================================================================
static Standard_Boolean DecomposeResult(const Handle(IntPatch_PointLine)& theLine,
                                        const Standard_Boolean       IsReversed,
                                        const IntSurf_Quadric&       theQuad,
                                        const Handle(Adaptor3d_TopolTool)& thePDomain,
                                        const Handle(Adaptor3d_HSurface)&  theQSurf, //quadric
                                        const Handle(Adaptor3d_HSurface)&  thePSurf, //parametric
                                        const Standard_Real                theArcTol,
                                        const Standard_Real                theTolTang,
                                        IntPatch_SequenceOfLine&           theLines)
{
  if(theLine->ArcType() == IntPatch_Restriction)
  {
    const Handle(IntPatch_RLine)& aRL = Handle(IntPatch_RLine)::DownCast(theLine);
    if(!aRL.IsNull())
    {
      const Handle(Adaptor2d_HCurve2d)& anArc = aRL->IsArcOnS1() ?
                                        aRL->ArcOnS1() :
                                        aRL->ArcOnS2();
      if(anArc->Curve2d().GetType() != GeomAbs_Line)
      {
        //Restriction line must be isoline.
        //Other cases are not supported by
        //existing algorithms.

        return Standard_False;
      }
    }
  }
  
  const Standard_Real aDeltaUmax = M_PI_2;
  const Standard_Real aTOL3D = 1.e-10, 
                      aTOL2D = Precision::PConfusion(),
                      aTOL2DS = Precision::PConfusion();

  const Handle(IntSurf_LineOn2S)& aSLine = theLine->Curve();

  if(aSLine->NbPoints() <= 2)
  {
    return Standard_False;
  }
  
  //Deletes repeated vertices
  Handle(IntSurf_LineOn2S) aVLine = GetVertices(theLine,aTOL3D,aTOL2D);
  
  Handle(IntSurf_LineOn2S) aSSLine(aSLine);

  if(aSSLine->NbPoints() <= 1)
    return Standard_False;

  AdjustLine(aSSLine,IsReversed,theQSurf,aTOL2D);

  if(theLine->ArcType() == IntPatch_Walking)
  {
    Standard_Boolean isInserted = Standard_True;
    while(isInserted)
    {
      const Standard_Integer aNbPnts = aSSLine->NbPoints();
      TColStd_Array1OfInteger aPTypes(1,aNbPnts);
      SearchVertices(aSSLine,aVLine,aPTypes);
      isInserted = InsertSeamVertices(aSSLine,IsReversed,aVLine,aPTypes,aTOL2D);
    }
  }

  const Standard_Integer aLindex = aSSLine->NbPoints();
  Standard_Integer aFindex = 1, aBindex = 0;

  // build WLine parts (if any)
  Standard_Boolean flNextLine = Standard_True;
  Standard_Boolean hasBeenDecomposed = Standard_False;
  IntPatch_SpecPntType aPrePointExist = IntPatch_SPntNone;

  IntSurf_PntOn2S PrePoint;
  while(flNextLine)
  {
    // reset variables
    flNextLine = Standard_False;
    Standard_Boolean isDecomposited = Standard_False;

    Handle(IntSurf_LineOn2S) sline = new IntSurf_LineOn2S();

    //if((Lindex-Findex+1) <= 2 )
    if((aLindex <= aFindex) && !aPrePointExist)
    {
      //break of "while(flNextLine)" cycle
      break;
    }

    if(aPrePointExist)
    {
      const IntSurf_PntOn2S& aRefPt = aSSLine->Value(aFindex);

      const Standard_Real aURes = theQSurf->UResolution(theArcTol),
                          aVRes = theQSurf->VResolution(theArcTol);

      const Standard_Real aTol2d = (aPrePointExist == IntPatch_SPntPole) ? -1.0 : 
              (aPrePointExist == IntPatch_SPntSeamV)? aVRes :
              (aPrePointExist == IntPatch_SPntSeamUV)? Max(aURes, aVRes) : aURes;

      if(IntPatch_SpecialPoints::ContinueAfterSpecialPoint(theQSurf, thePSurf, aRefPt,
                                                              aPrePointExist, aTol2d,
                                                              PrePoint, IsReversed))
      {
        sline->Add(PrePoint);
      }
      else
      {
        //break of "while(flNextLine)" cycle
        break;
      }
    }

    aPrePointExist = IntPatch_SPntNone;

    // analyze other points
    for(Standard_Integer k = aFindex; k <= aLindex; k++)
    {
      if( k == aFindex )
      {
        PrePoint = aSSLine->Value(k);
        sline->Add(PrePoint);
        continue;
      }

      //Check whether the current point is on the boundary of theQSurf.
      //If that is TRUE then the Walking-line will be decomposed in this point.
      //However, this boundary is not singular-point (like seam or pole of sphere).
      //Therefore, its processing will be simplified.
      Standard_Boolean isOnBoundary = Standard_False;

      // Values of sline and isOnBoundary can be changed by this function
      DetectOfBoundaryAchievement(theQSurf, IsReversed, aSSLine,
                                  k, aTOL2D, sline, isOnBoundary);

      aPrePointExist = IsSeamOrPole(theQSurf, aSSLine, IsReversed, k - 1, aDeltaUmax);

      if (isOnBoundary && (aPrePointExist != IntPatch_SPntPoleSeamU))
      {
        // If the considered point is on seam then its UV-parameters 
        // are defined to within the surface period. Therefore, we can
        // trust already computed parameters of this point.
        // However, if this point (which is on the surface boundary) is
        // a sphere pole or cone apex then its (point's) parameters
        // have to be recomputed in the code below 
        // (see IntPatch_SpecialPoints::AddSingularPole() method).
        // E.g. see "bugs modalg_6 bug26684_2" test case.

        aPrePointExist = IntPatch_SPntNone;
      }

      if (aPrePointExist != IntPatch_SPntNone)
      {
        aBindex = k;
        isDecomposited = Standard_True;
        ////
        const IntSurf_PntOn2S& aRefPt = aSSLine->Value(aBindex-1);

        Standard_Real aCompareTol3D = Precision::Confusion();
        Standard_Real aCompareTol2D = Precision::PConfusion();

        IntSurf_PntOn2S aNewPoint = aRefPt;
        IntPatch_SpecPntType aLastType = IntPatch_SPntNone;

        if(aPrePointExist == IntPatch_SPntSeamUV)
        {
          aPrePointExist = IntPatch_SPntNone;
          aLastType = IntPatch_SPntSeamUV;
          IntPatch_SpecialPoints::AddCrossUVIsoPoint(theQSurf, thePSurf, 
                                                        aRefPt, theTolTang,
                                                        aNewPoint, IsReversed);
        }
        else if(aPrePointExist == IntPatch_SPntSeamV)
        {//WLine goes through seam
          aPrePointExist = IntPatch_SPntNone;
          aLastType = IntPatch_SPntSeamV;

          //Not quadric point
          Standard_Real aU0 = 0.0, aV0 = 0.0;
          //Quadric point
          Standard_Real aUQuadRef = 0.0, aVQuadRef = 0.0;

          if(IsReversed)
          {
            aRefPt.Parameters(aU0, aV0, aUQuadRef, aVQuadRef);
          }
          else
          {
            aRefPt.Parameters(aUQuadRef, aVQuadRef, aU0, aV0);
          }

          math_Vector aTol(1, 3), aStartPoint(1,3),
            anInfBound(1, 3), aSupBound(1, 3);

          //Parameters on parametric surface
          Standard_Real aUp = 0.0, aVp = 0.0, aUq = 0.0, aVq = 0.0;
          if(IsReversed)
          {
            aSSLine->Value(k).Parameters(aUp, aVp, aUq, aVq);
          }
          else
          {
            aSSLine->Value(k).Parameters(aUq, aVq, aUp, aVp);
          }

          aTol(1) = thePSurf->UResolution(theArcTol);
          aTol(2) = thePSurf->VResolution(theArcTol);
          aTol(3) = theQSurf->UResolution(theArcTol);
          aStartPoint(1) = 0.5*(aU0 + aUp);
          aStartPoint(2) = 0.5*(aV0 + aVp);
          aStartPoint(3) = 0.5*(aUQuadRef + aUq);
          anInfBound(1) = thePSurf->FirstUParameter();
          anInfBound(2) = thePSurf->FirstVParameter();
          anInfBound(3) = theQSurf->FirstUParameter();
          aSupBound(1) = thePSurf->LastUParameter();
          aSupBound(2) = thePSurf->LastVParameter();
          aSupBound(3) = theQSurf->LastUParameter();

          IntPatch_SpecialPoints::
                      AddPointOnUorVIso(theQSurf, thePSurf, aRefPt, Standard_False,
                                        aTol, aStartPoint, anInfBound, aSupBound,
                                        aNewPoint, IsReversed);
        }
        else if(aPrePointExist == IntPatch_SPntPoleSeamU)
        {
          aPrePointExist = IntPatch_SPntNone;

          IntPatch_Point aVert;
          aVert.SetValue(aRefPt);

          if(IntPatch_SpecialPoints::
                      AddSingularPole(theQSurf, thePSurf, aRefPt, theTolTang,
                                      aVert, aNewPoint, IsReversed))
          {
            aPrePointExist = IntPatch_SPntPole;
            aLastType = IntPatch_SPntPole;
            if (isOnBoundary)
            {
              // It is necessary to replace earlier added point on 
              // the surface boundary with the pole. For that,
              // here we delete excess point. New point will be added later.
              isOnBoundary = Standard_False;
              sline->RemovePoint(sline->NbPoints());
            }

            aCompareTol2D = -1.0;
          } //if(IntPatch_AddSpecialPoints::AddSingularPole(...))
          else
          {//Pole is not an intersection point
            aPrePointExist = IntPatch_SPntSeamU;
          }
        }

        if(aPrePointExist == IntPatch_SPntSeamU)
        {//WLine goes through seam
          aPrePointExist = IntPatch_SPntNone;
          aLastType = IntPatch_SPntSeamU;

          //Not quadric point
          Standard_Real aU0 = 0.0, aV0 = 0.0;
          //Quadric point
          Standard_Real aUQuadRef = 0.0, aVQuadRef = 0.0;

          if(IsReversed)
          {
            aRefPt.Parameters(aU0, aV0, aUQuadRef, aVQuadRef);
          }
          else
          {
            aRefPt.Parameters(aUQuadRef, aVQuadRef, aU0, aV0);
          }

          math_Vector aTol(1, 3), aStartPoint(1,3),
                      anInfBound(1, 3), aSupBound(1, 3);
          
          //Parameters on parametric surface
          Standard_Real aUp = 0.0, aVp = 0.0, aUq = 0.0, aVq = 0.0;
          if (IsReversed)
          {
            aSSLine->Value(k).Parameters(aUp, aVp, aUq, aVq);
          }
          else
          {
            aSSLine->Value(k).Parameters(aUq, aVq, aUp, aVp);
          }

          aTol(1) = thePSurf->UResolution(theArcTol);
          aTol(2) = thePSurf->VResolution(theArcTol);
          aTol(3) = theQSurf->VResolution(theArcTol);
          aStartPoint(1) = 0.5*(aU0 + aUp);
          aStartPoint(2) = 0.5*(aV0 + aVp);
          aStartPoint(3) = 0.5*(aVQuadRef + aVq);
          anInfBound(1) = thePSurf->FirstUParameter();
          anInfBound(2) = thePSurf->FirstVParameter();
          anInfBound(3) = theQSurf->FirstVParameter();
          aSupBound(1) = thePSurf->LastUParameter();
          aSupBound(2) = thePSurf->LastVParameter();
          aSupBound(3) = theQSurf->LastVParameter();

          IntPatch_SpecialPoints::
                AddPointOnUorVIso(theQSurf, thePSurf, aRefPt, Standard_True, aTol,
                                  aStartPoint, anInfBound, aSupBound, aNewPoint,
                                  IsReversed);
        }

        if(!aNewPoint.IsSame(aRefPt, aCompareTol3D, aCompareTol2D))
        {
          if (isOnBoundary)
            break;

          sline->Add(aNewPoint);
          aPrePointExist = aLastType;
          PrePoint = aNewPoint;
        }
        else
        {
          if (isOnBoundary || (sline->NbPoints() == 1))
          {
            //FIRST point of the sline is the pole of the quadric.
            //Therefore, there is no point in decomposition.

            // If the considered point is on surface boundary then
            // it is already marked as vertex. So, decomposition is 
            // not required, too.

            PrePoint = aRefPt;
            aPrePointExist = isOnBoundary ? IntPatch_SPntNone : aLastType;
          }
        }

        ////
        break;
      } //if (aPrePointExist != IntPatch_SPntNone) cond.

      PrePoint = aSSLine->Value(k);

      if (isOnBoundary)
      {
        aBindex = k;
        isDecomposited = Standard_True;
        aPrePointExist = IntPatch_SPntNone;
        break;
      }
      else
      {
        sline->Add(aSSLine->Value(k));
      }
    } //for(Standard_Integer k = aFindex; k <= aLindex; k++)

    //Creation of new line as part of existing theLine.
    //This part is defined by sline.

    if(sline->NbPoints() == 1)
    {
      flNextLine = Standard_True;
      aFindex = aBindex;

      //Go to the next part of aSSLine
      //because we cannot create the line
      //with single point.

      continue;
    }

    IntSurf_PntOn2S aVF, aVL;
    Standard_Boolean addVF = Standard_False, addVL = Standard_False;
    VerifyVertices(sline,IsReversed,aVLine,aTOL2DS,theArcTol,
                                    thePDomain,aVF,addVF,aVL,addVL);

    Standard_Boolean hasInternals = HasInternals(sline,aVLine);

    Standard_Real D3F = 0., D3L = 0.;
    ToSmooth(sline,IsReversed,theQuad,Standard_True,D3F);
    ToSmooth(sline,IsReversed,theQuad,Standard_False,D3L);

    //if(D3F <= 1.5e-7 && sline->NbPoints() >=3) {
    //  D3F = sline->Value(2).Value().Distance(sline->Value(3).Value());
    //}
    //if(D3L <= 1.5e-7 && sline->NbPoints() >=3) {
    //  D3L = sline->Value(sline->NbPoints()-1).Value().Distance(sline->
    //                                  Value(sline->NbPoints()-2).Value());
    //}

    if(addVF || addVL)
    {
      Standard_Boolean isAdded = AddVertices(sline,aVF,addVF,aVL,addVL,D3F,D3L);
      if(isAdded)
      {
        ToSmooth(sline,IsReversed,theQuad,Standard_True,D3F);
        ToSmooth(sline,IsReversed,theQuad,Standard_False,D3L);
      }
    }

    if(theLine->ArcType() == IntPatch_Walking)
    {
      IntPatch_Point aTPntF, aTPntL;

      Handle(IntPatch_WLine) wline = 
                          new IntPatch_WLine(sline,Standard_False,
                          theLine->TransitionOnS1(),theLine->TransitionOnS2());

      Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
      gp_Pnt aSPnt(sline->Value(1).Value());
      sline->Value(1).Parameters(aU1, aV1, aU2, aV2);
      aTPntF.SetValue(aSPnt,theArcTol,Standard_False);
      aTPntF.SetParameters(aU1, aV1, aU2, aV2);
      aTPntF.SetParameter(1.0);
      wline->AddVertex(aTPntF);
      wline->SetFirstPoint(1);

      if(hasInternals)
      {
        PutIntVertices(wline,sline,IsReversed,aVLine,theArcTol);
      }

      aSPnt =  sline->Value(sline->NbPoints()).Value();
      sline->Value(sline->NbPoints()).Parameters(aU1, aV1, aU2, aV2);
      aTPntL.SetValue(aSPnt,theArcTol,Standard_False);
      aTPntL.SetParameters(aU1, aV1, aU2, aV2);
      aTPntL.SetParameter(sline->NbPoints());
      wline->AddVertex(aTPntL);
      wline->SetLastPoint(wline->NbVertex());

      IntPatch_SequenceOfLine segm;
      Standard_Boolean isSplited = SplitOnSegments(wline,Standard_False,
                      theLine->TransitionOnS1(),theLine->TransitionOnS2(),theArcTol,segm);

      if(!isSplited)
      {
        theLines.Append(wline);
      }
      else
      {
        Standard_Integer nbsegms = segm.Length();
        Standard_Integer iseg = 0;
        for(iseg = 1; iseg <= nbsegms; iseg++)
          theLines.Append(segm(iseg));
      }
    }
    else
    {//theLine->ArcType() == IntPatch_Restriction
      if(!isDecomposited && !hasBeenDecomposed)
      {
        //The line has not been changed
        theLines.Append(Handle(IntPatch_RLine)::DownCast(theLine));
        return hasBeenDecomposed;
      }

      IntPatch_Point aTPnt;
      gp_Pnt2d aPSurf;
      gp_Pnt aSPnt;

      Handle(IntPatch_RLine) aRLine = new IntPatch_RLine(*Handle(IntPatch_RLine)::DownCast(theLine));
      
      aRLine->ClearVertexes();
      aRLine->SetCurve(sline);

      if(hasInternals)
      {
        PutIntVertices(aRLine,sline,IsReversed,aVLine,theArcTol);
      }

      const Handle(Adaptor2d_HCurve2d)& anArc = aRLine->IsArcOnS1() ?
                                                aRLine->ArcOnS1() :
                                                aRLine->ArcOnS2();

      Standard_Real aFPar = anArc->FirstParameter(),
                    aLPar = anArc->LastParameter();

      const IntSurf_PntOn2S &aRFirst = sline->Value(1),
                            &aRLast = sline->Value(sline->NbPoints());

      const gp_Lin2d aLin(anArc->Curve2d().Line());
      
      for(Standard_Integer aFLIndex = 0; aFLIndex < 2; aFLIndex++)
      {
        Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
        if(aFLIndex == 0)
        {
          aRFirst.Parameters(aU1, aV1, aU2, aV2);
          aSPnt.SetXYZ(aRFirst.Value().XYZ());
        }
        else
        {
          aRLast.Parameters(aU1, aV1, aU2, aV2);
          aSPnt.SetXYZ(aRLast.Value().XYZ());
        }

        if(IsReversed)
        {
          aPSurf.SetCoord(aU1, aV1);
        }
        else
        {
          aPSurf.SetCoord(aU2, aV2);
        }

        Standard_Real aPar = ElCLib::Parameter(aLin, aPSurf);

        if(aFLIndex == 0)
        {
          aFPar = Max(aFPar, aPar);
          aPar = aFPar;
        }
        else
        {
          aLPar = Min(aLPar, aPar);
          aPar = aLPar;
        }

        aTPnt.SetParameter(aPar);
        aTPnt.SetValue(aSPnt,theArcTol,Standard_False);
        aTPnt.SetParameters(aU1, aV1, aU2, aV2);

        aRLine->AddVertex(aTPnt);
      }

      if(aLPar - aFPar > Precision::PConfusion())
      {
        aRLine->SetFirstPoint(1);
        aRLine->SetLastPoint(aRLine->NbVertex());

        anArc->Trim(aFPar, aLPar, theArcTol);

        theLines.Append(aRLine);
      }
    }

    if(isDecomposited)
    {
      aFindex = aBindex;
      flNextLine = hasBeenDecomposed = Standard_True;
    }
  }

  return hasBeenDecomposed;
}

//=======================================================================
//function : CheckSegmSegm
//purpose  : Returns TRUE if the segment [theParF, theParL] is included
//            in the segment [theRefParF, theRefParL] segment.
//=======================================================================
static Standard_Boolean CheckSegmSegm(const Standard_Real theRefParF,
                                      const Standard_Real theRefParL,
                                      const Standard_Real theParF,
                                      const Standard_Real theParL)
{
  if((theParF < theRefParF) || (theParF > theRefParL))
  {
    return Standard_False;
  }

  if((theParL < theRefParF) || (theParL > theRefParL))
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : IsCoincide
//purpose  : Check, if theLine is coincided with theArc (in 2d-space).
//
// Attention!!!
//            Cases when theArc is not 2d-line adaptor are suppored by
//          TopOpeBRep classes only (i.e. are archaic).
//=======================================================================
Standard_Boolean IsCoincide(IntPatch_TheSurfFunction& theFunc,
                            const Handle(IntPatch_PointLine)& theLine,
                            const Handle(Adaptor2d_HCurve2d)& theArc,
                            const Standard_Boolean isTheSurface1Using, //Surf1 is parametric?
                            const Standard_Real theToler3D,
                            const Standard_Real theToler2D,
                            const Standard_Real thePeriod) // Period of parametric surface in direction which is perpendicular to theArc direction.
{
  if(theLine->ArcType() == IntPatch_Restriction)
  {//Restriction-restriction processing
    const Handle(IntPatch_RLine)& aRL2 = Handle(IntPatch_RLine)::DownCast(theLine);
    const Handle(Adaptor2d_HCurve2d)& anArc = aRL2->IsArcOnS1() ? aRL2->ArcOnS1() : aRL2->ArcOnS2();
    
    if(anArc->Curve2d().GetType() != GeomAbs_Line)
    {
      //Restriction line must be isoline.
      //Other cases are not supported by
      //existing algorithms.

      return Standard_False;
    }

    const gp_Lin2d aLin1(theArc->Curve2d().Line()),
                   aLin2(anArc->Curve2d().Line());

    if(!aLin1.Direction().IsParallel(aLin2.Direction(), Precision::Angular()))
    {
      return Standard_False;
    }

    const Standard_Real aDist = 
            theArc->Curve2d().Line().Distance(anArc->Curve2d().Line());
    if((aDist < theToler2D) || (Abs(aDist - thePeriod) < theToler2D))
    {
      const Standard_Real aRf = theArc->FirstParameter(),
                          aRl = theArc->LastParameter();
      const Standard_Real aParf = anArc->FirstParameter(),
                          aParl = anArc->LastParameter();
      const gp_Pnt2d aP1(ElCLib::Value(aParf, aLin2)),
                     aP2(ElCLib::Value(aParl, aLin2));

      Standard_Real aParam1 = ElCLib::Parameter(aLin1, aP1),
                    aParam2 = ElCLib::Parameter(aLin1, aP2);

      if(CheckSegmSegm(aRf, aRl, aParam1, aParam2))
        return Standard_True;

      //Lines are parallel. Therefore, there is no point in
      //projecting points to another line in order to check
      //if segment second line is included in segment of first one.

      return CheckSegmSegm(aParam1, aParam2, aRf, aRl);
    }

    return Standard_False;
  }

  const Standard_Integer aNbPnts = theLine->NbPnts();
  const Standard_Real aUAf = theArc->FirstParameter(),
                      aUAl = theArc->LastParameter();
  const gp_Lin2d anArcLin(theArc->Curve2d().Line());

  math_Vector aX(1, 2), aVal(1, 1);

  for(Standard_Integer aPtID = 1; aPtID <= aNbPnts; aPtID++)
  {
    Standard_Real aUf = 0.0, aVf = 0.0;
    if(isTheSurface1Using)
      theLine->Point(aPtID).ParametersOnS1(aUf, aVf);
    else
      theLine->Point(aPtID).ParametersOnS2(aUf, aVf);

    //Take 2d-point in parametric surface (because theArc is
    //2d-line in parametric surface).
    const gp_Pnt2d aPloc(aUf, aVf);

    const Standard_Real aRParam = ElCLib::Parameter(anArcLin, aPloc);

    if((aRParam < aUAf) || (aRParam > aUAl))
      return Standard_False;

    const gp_Pnt2d aPmin(ElCLib::Value(aRParam, anArcLin));
    
    const Standard_Real aDist = aPloc.Distance(aPmin);
    if((aDist < theToler2D) || (Abs(aDist - thePeriod) < theToler2D))
    {//Considered point is in Restriction line.
     //Go to the next point.
      continue;
    }

    //Check if intermediate points between aPloc and theArc are
    //intersection point (i.e. if aPloc is in tangent zone between
    //two intersected surfaces).

    const Standard_Real aUl = aPmin.X(), aVl = aPmin.Y();

    const Standard_Integer aNbPoints = 4;
    const Standard_Real aStepU = (aUl - aUf)/aNbPoints,
                        aStepV = (aVl - aVf)/aNbPoints;

    Standard_Real aU = aUf+aStepU, aV = aVf+aStepV;
    for(Standard_Integer i = 1; i < aNbPoints; i++)
    {
      aX.Value(1) = aU;
      aX.Value(2) = aV;

      if(!theFunc.Value(aX, aVal))
      {
        return Standard_False;
      }

      if(Abs(aVal(1)) > theToler3D)
      {
        return Standard_False;
      }
      
      aU += aStepU;
      aV += aStepV;
    }
  }

  return Standard_True;
}