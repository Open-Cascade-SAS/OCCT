// Created on: 1996-06-03
// Created by: Laurent BUCHARD
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

#define OPTIMISATION 1 

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_SequenceOfPnt.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <IntCurveSurface_ThePolygonOfHInter.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <IntCurveSurface_ThePolyhedronToolOfHInter.hxx>
#include <Intf_Tool.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
//
static void ComputeSamplePars(const Handle(Adaptor3d_HSurface)& Hsurface, 
                              const Standard_Integer nbsu,
                              const Standard_Integer nbsv,
                              Handle(TColStd_HArray1OfReal)& UPars, 
                              Handle(TColStd_HArray1OfReal)& VPars)
{
  Standard_Integer NbUInts = Hsurface->NbUIntervals(GeomAbs_C2);
  Standard_Integer NbVInts = Hsurface->NbVIntervals(GeomAbs_C2);
  TColStd_Array1OfReal UInts(1, NbUInts + 1);
  TColStd_Array1OfReal VInts(1, NbVInts + 1);
  Hsurface->UIntervals(UInts, GeomAbs_C2);
  Hsurface->VIntervals(VInts, GeomAbs_C2);
  //
  TColStd_Array1OfInteger NbUSubInts(1, NbUInts);
  TColStd_Array1OfInteger NbVSubInts(1, NbVInts);
  //
  Standard_Integer i, j, ind, NbU, NbV;
  Standard_Real t, dt;
  t = UInts(NbUInts + 1) - UInts(1);
  t = 1. / t;
  NbU = 0;
  for(i = 1; i <= NbUInts; ++i)
  {
    dt = (UInts(i+1) - UInts(i));
    NbUSubInts(i) = RealToInt(nbsu * dt * t) + 1;
    NbU += NbUSubInts(i);
  }
  t = VInts(NbVInts + 1) - VInts(1);
  t = 1. / t;
  NbV = 0;
  for(i = 1; i <= NbVInts; ++i)
  {
    dt = (VInts(i+1) - VInts(i));
    NbVSubInts(i) = RealToInt(nbsv * dt * t) + 1;
    NbV += NbVSubInts(i);
  }
  UPars = new TColStd_HArray1OfReal(1, NbU + 1);
  VPars = new TColStd_HArray1OfReal(1, NbV + 1);
  //
  ind = 1;
  for(i = 1; i <= NbUInts; ++i)
  {
    UPars->SetValue(ind++, UInts(i));
    dt = (UInts(i+1) - UInts(i)) / NbUSubInts(i);
    t = UInts(i);
    for(j = 1; j < NbUSubInts(i); ++j)
    {
      t += dt;
      UPars->SetValue(ind++, t);
    }
  }
  UPars->SetValue(ind, UInts(NbUInts + 1));
  //
  ind = 1;
  for(i = 1; i <= NbVInts; ++i)
  {
    VPars->SetValue(ind++, VInts(i));
    dt = (VInts(i+1) - VInts(i)) / NbVSubInts(i);
    t = VInts(i);
    for(j = 1; j < NbVSubInts(i); ++j)
    {
      t += dt;
      VPars->SetValue(ind++, t);
    }
  }
  VPars->SetValue(ind, VInts(NbVInts + 1));
}
//
//=======================================================================
//function : SurfaceType
//purpose  : 
//=======================================================================
GeomAbs_SurfaceType IntCurvesFace_Intersector::SurfaceType() const 
{ 
  return(Adaptor3d_HSurfaceTool::GetType(Hsurface));
}
//=======================================================================
//function : IntCurvesFace_Intersector
//purpose  : 
//=======================================================================
IntCurvesFace_Intersector::IntCurvesFace_Intersector(const TopoDS_Face& Face,
                                                     const Standard_Real aTol,
                                                     const Standard_Boolean aRestr,
                                                     const Standard_Boolean UseBToler)
                                                    
: 
  Tol(aTol),
  done(Standard_False),
  nbpnt(0),
  PtrOnPolyhedron(NULL),
  PtrOnBndBounding(NULL),
  myUseBoundTol (UseBToler)
{ 
  BRepAdaptor_Surface surface;
  face = Face;
  surface.Initialize(Face, aRestr);
  Hsurface = new BRepAdaptor_HSurface(surface);
  myTopolTool = new BRepTopAdaptor_TopolTool(Hsurface);
  
  GeomAbs_SurfaceType SurfaceType = Adaptor3d_HSurfaceTool::GetType(Hsurface);
  if(   (SurfaceType != GeomAbs_Plane) 
     && (SurfaceType != GeomAbs_Cylinder)
     && (SurfaceType != GeomAbs_Cone) 
     && (SurfaceType != GeomAbs_Sphere)
     && (SurfaceType != GeomAbs_Torus)) {
    Standard_Integer nbsu,nbsv;
    Standard_Real U0,V0,U1,V1;
    U0 = Hsurface->FirstUParameter();
    U1 = Hsurface->LastUParameter();
    V0 = Hsurface->FirstVParameter();
    V1 = Hsurface->LastVParameter();
    //
    nbsu = myTopolTool->NbSamplesU();
    nbsv = myTopolTool->NbSamplesV();
    //
    Standard_Real aURes = Hsurface->UResolution(1.0);
    Standard_Real aVRes = Hsurface->VResolution(1.0);

    // Checking correlation between number of samples and length of the face along each axis
    const Standard_Real aTresh = 100.0;
    Standard_Integer aMinSamples = 20;
    const Standard_Integer aMaxSamples = 40;
    const Standard_Integer aMaxSamples2 = aMaxSamples * aMaxSamples;
    Standard_Real dU = (U1 - U0) / aURes;
    Standard_Real dV = (V1 - V0) / aVRes;
    if (nbsu < aMinSamples) nbsu = aMinSamples;
    if (nbsv < aMinSamples) nbsv = aMinSamples;
    if (nbsu > aMaxSamples) nbsu = aMaxSamples;
    if (nbsv > aMaxSamples) nbsv = aMaxSamples;

    if (dU > gp::Resolution() && dV > gp::Resolution()) {
      if (Max(dU, dV) > Min(dU, dV) * aTresh)
      {
        aMinSamples = 10;
        nbsu = (Standard_Integer)(Sqrt(dU / dV) * aMaxSamples);
        if (nbsu < aMinSamples) nbsu = aMinSamples;
        nbsv = aMaxSamples2 / nbsu;
        if (nbsv < aMinSamples)
        {
          nbsv = aMinSamples;
          nbsu = aMaxSamples2 / aMinSamples;
        }
      }
    }
    else {
      if (dU < gp::Resolution()) {
        nbsu = 1;
      }
      if (dV < gp::Resolution()) {
        nbsv = 1;
      }
    }

    Standard_Integer NbUOnS = Hsurface->NbUIntervals(GeomAbs_C2);
    Standard_Integer NbVOnS = Hsurface->NbVIntervals(GeomAbs_C2);

    if(NbUOnS > 1 || NbVOnS > 1)
    {
      Handle(TColStd_HArray1OfReal) UPars, VPars;
      ComputeSamplePars(Hsurface, nbsu, nbsv, UPars, VPars);
      PtrOnPolyhedron = (IntCurveSurface_ThePolyhedronOfHInter *)
        new IntCurveSurface_ThePolyhedronOfHInter(Hsurface, UPars->ChangeArray1(), 
                                                            VPars->ChangeArray1());
    }
    else 
    {
      PtrOnPolyhedron = (IntCurveSurface_ThePolyhedronOfHInter *)
        new IntCurveSurface_ThePolyhedronOfHInter(Hsurface,nbsu,nbsv,U0,V0,U1,V1);
    }
  }
}
//=======================================================================
//function : InternalCall
//purpose  : 
//=======================================================================
void IntCurvesFace_Intersector::InternalCall(const IntCurveSurface_HInter &HICS,
					     const Standard_Real parinf,
					     const Standard_Real parsup) 
{
  if(HICS.IsDone() && HICS.NbPoints() > 0) {
    //Calculate tolerance for 2d classifier
    Standard_Real mintol3d = BRep_Tool::Tolerance(face);
    Standard_Real maxtol3d = mintol3d;
    Standard_Real mintol2d = Tol, maxtol2d = Tol;
    TopExp_Explorer anExp(face, TopAbs_EDGE);
    for(; anExp.More(); anExp.Next())
    {
      Standard_Real curtol = BRep_Tool::Tolerance(TopoDS::Edge(anExp.Current()));
      mintol3d = Min(mintol3d, curtol);
      maxtol3d = Max(maxtol3d, curtol);
    }
    Standard_Real minres = Max(Hsurface->UResolution(mintol3d), Hsurface->VResolution(mintol3d));
    Standard_Real maxres = Max(Hsurface->UResolution(maxtol3d), Hsurface->VResolution(maxtol3d));
    mintol2d = Max(minres, Tol); 
    maxtol2d = Max(maxres, Tol);
    //
    Handle(BRepTopAdaptor_TopolTool) anAdditionalTool;
    for(Standard_Integer index=HICS.NbPoints(); index>=1; index--) {  
      const IntCurveSurface_IntersectionPoint& HICSPointindex = HICS.Point(index);
      gp_Pnt2d Puv(HICSPointindex.U(),HICSPointindex.V());

      //TopAbs_State currentstate = myTopolTool->Classify(Puv,Tol);
      TopAbs_State currentstate = myTopolTool->Classify(Puv, !myUseBoundTol ? 0 : mintol2d);
      if(myUseBoundTol && currentstate == TopAbs_OUT && maxtol2d > mintol2d) {
        if(anAdditionalTool.IsNull())
        {
          anAdditionalTool = new BRepTopAdaptor_TopolTool(Hsurface);
        }
        currentstate = anAdditionalTool->Classify(Puv,maxtol2d);
        if(currentstate == TopAbs_ON)
        {
          currentstate = TopAbs_OUT;
          //Find out nearest edge and it's tolerance
          anExp.Init(face, TopAbs_EDGE);
          for(; anExp.More(); anExp.Next())
          {
            TopoDS_Edge anE = TopoDS::Edge(anExp.Current());
            Standard_Real curtol = BRep_Tool::Tolerance(anE);
            Standard_Real tol2d = Max(Hsurface->UResolution(curtol), Hsurface->VResolution(curtol));
            tol2d = Max(tol2d, Tol);
            Standard_Real f, l;
            Handle(Geom2d_Curve) aPC = BRep_Tool::CurveOnSurface(anE, face, f, l);
            Geom2dAPI_ProjectPointOnCurve aProj(Puv, aPC, f, l);
            if(aProj.NbPoints() > 0)
            {
              Standard_Real d = aProj.LowerDistance();
              if(d <= tol2d)
              {
                //Nearest edge is found, state is really ON
                currentstate = TopAbs_ON;
                break;
              }
            }
          }
        }
      }
      if(currentstate==TopAbs_IN || currentstate==TopAbs_ON) { 
        Standard_Real HICSW = HICSPointindex.W();
        if(HICSW >= parinf && HICSW <= parsup ) { 
          Standard_Real U          = HICSPointindex.U();
          Standard_Real V          = HICSPointindex.V();
          Standard_Real W          = HICSW; 
          IntCurveSurface_TransitionOnCurve transition = HICSPointindex.Transition();
          gp_Pnt pnt        = HICSPointindex.Pnt();
          //	  state      = currentstate;
          //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
          Standard_Integer anIntState = (currentstate == TopAbs_IN) ? 0 : 1;
          //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End

          if(transition != IntCurveSurface_Tangent && face.Orientation()==TopAbs_REVERSED) { 
            if(transition == IntCurveSurface_In) 
              transition = IntCurveSurface_Out;
            else 
              transition = IntCurveSurface_In;
          }
          //----- Insertion du point 
          if(nbpnt==0) { 
            IntCurveSurface_IntersectionPoint PPP(pnt,U,V,W,transition);
            SeqPnt.Append(PPP);
            //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
            mySeqState.Append(anIntState);
            //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
          }
          else { 
            Standard_Integer i = 1;
            Standard_Integer b = nbpnt+1;                    
            while(i<=nbpnt) {
              const IntCurveSurface_IntersectionPoint& Pnti=SeqPnt.Value(i);
              Standard_Real wi = Pnti.W();
              if(wi >= W) { b=i; i=nbpnt; }
              i++;
            }
            IntCurveSurface_IntersectionPoint PPP(pnt,U,V,W,transition);
            //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
            // 	    if(b>nbpnt)          { SeqPnt.Append(PPP); } 
            // 	    else if(b>0)             { SeqPnt.InsertBefore(b,PPP); } 
            if(b>nbpnt) {
              SeqPnt.Append(PPP);
              mySeqState.Append(anIntState);
            } else if(b>0) {
              SeqPnt.InsertBefore(b,PPP);
              mySeqState.InsertBefore(b, anIntState);
            }
            //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
          }


          nbpnt++;
        } 
      } //-- classifier state is IN or ON
    } //-- Loop on Intersection points.
  } //-- HICS.IsDone()
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntCurvesFace_Intersector::Perform(const gp_Lin& L,
					const Standard_Real ParMin,
					const Standard_Real ParMax)
{ 
  done = Standard_True;
  SeqPnt.Clear();
  mySeqState.Clear();
  nbpnt = 0;
  
  IntCurveSurface_HInter HICS; 
  Handle(Geom_Line) geomline = new Geom_Line(L);
  GeomAdaptor_Curve LL(geomline);
  Handle(GeomAdaptor_HCurve) HLL  = new GeomAdaptor_HCurve(LL);
  Standard_Real parinf=ParMin;
  Standard_Real parsup=ParMax;
  //
  if(PtrOnPolyhedron == NULL) { 
    HICS.Perform(HLL,Hsurface);
  }
  else { 
    Intf_Tool bndTool;
    Bnd_Box   boxLine;
    bndTool.LinBox
      (L,
       ((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron)->Bounding(),
       boxLine);
    if(bndTool.NbSegments() == 0) 
      return;
    for(Standard_Integer nbseg=1; nbseg<= bndTool.NbSegments(); nbseg++) { 
      Standard_Real pinf = bndTool.BeginParam(nbseg);
      Standard_Real psup = bndTool.EndParam(nbseg);
      Standard_Real pppp = 0.05*(psup-pinf);
      pinf-=pppp;
      psup+=pppp;
      if((psup - pinf)<1e-10) { pinf-=1e-10; psup+=1e-10; } 
      if(nbseg==1) { parinf=pinf; parsup=psup; }
      else { 
	if(parinf>pinf) parinf = pinf;
	if(parsup<psup) parsup = psup;
      }
    }
    if(parinf>ParMax) { return; } 
    if(parsup<ParMin) { return; }
    if(parinf<ParMin) parinf=ParMin;
    if(parsup>ParMax) parsup=ParMax;
    if(parinf>(parsup-1e-9)) return; 
    IntCurveSurface_ThePolygonOfHInter polygon(HLL,
					       parinf,
					       parsup,
					       2);
#if OPTIMISATION
    if(PtrOnBndBounding==NULL) { 
      PtrOnBndBounding = (Bnd_BoundSortBox *) new Bnd_BoundSortBox();
      IntCurveSurface_ThePolyhedronOfHInter *thePolyh=
	(IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron;
      ((Bnd_BoundSortBox *)(PtrOnBndBounding))->
	Initialize(IntCurveSurface_ThePolyhedronToolOfHInter::Bounding(*thePolyh),
		   IntCurveSurface_ThePolyhedronToolOfHInter::ComponentsBounding(*thePolyh));
    }
    HICS.Perform(HLL,
		 polygon,
		 Hsurface,
		 *((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron),
		 *((Bnd_BoundSortBox *)PtrOnBndBounding));
#else
    HICS.Perform(HLL,
		 polygon,
		 Hsurface,
		 *((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron));
#endif
  }
  
  InternalCall(HICS,parinf,parsup);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntCurvesFace_Intersector::Perform(const Handle(Adaptor3d_HCurve)& HCu,
					const Standard_Real ParMin,
					const Standard_Real ParMax) 
{ 
  done = Standard_True;
  SeqPnt.Clear();
  //  Modified by skv - Wed Sep  3 16:14:10 2003 OCC578 Begin
  mySeqState.Clear();
  //  Modified by skv - Wed Sep  3 16:14:11 2003 OCC578 End
  nbpnt = 0;
  IntCurveSurface_HInter            HICS; 
  
  //-- 
  Standard_Real parinf=ParMin;
  Standard_Real parsup=ParMax;

  if(PtrOnPolyhedron == NULL) { 
    HICS.Perform(HCu,Hsurface);
  }
  else { 
    parinf = IntCurveSurface_TheHCurveTool::FirstParameter(HCu);
    parsup = IntCurveSurface_TheHCurveTool::LastParameter(HCu);
    if(parinf<ParMin) parinf = ParMin;
    if(parsup>ParMax) parsup = ParMax;
    if(parinf>(parsup-1e-9)) return; 
    Standard_Integer nbs;
    nbs = IntCurveSurface_TheHCurveTool::NbSamples(HCu,parinf,parsup);
    
    IntCurveSurface_ThePolygonOfHInter polygon(HCu,
					       parinf,
					       parsup,
					       nbs);
#if OPTIMISATION
    if(PtrOnBndBounding==NULL) { 
      PtrOnBndBounding = (Bnd_BoundSortBox *) new Bnd_BoundSortBox();
      IntCurveSurface_ThePolyhedronOfHInter *thePolyh=(IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron;
      ((Bnd_BoundSortBox *)(PtrOnBndBounding))->Initialize(IntCurveSurface_ThePolyhedronToolOfHInter::Bounding(*thePolyh),
							   IntCurveSurface_ThePolyhedronToolOfHInter::ComponentsBounding(*thePolyh));
    }
    HICS.Perform(HCu,
		 polygon,
		 Hsurface,
		 *((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron),
		 *((Bnd_BoundSortBox *)PtrOnBndBounding));
#else
    HICS.Perform(HCu,
		 polygon,
		 Hsurface,
		 *((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron));
#endif
  }
  InternalCall(HICS,parinf,parsup);
}

//============================================================================
Bnd_Box IntCurvesFace_Intersector::Bounding() const {
  if(PtrOnPolyhedron !=NULL) {
    return(((IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron)->Bounding());
  }
  else { 
    Bnd_Box B;
    return(B);
  }
}
TopAbs_State IntCurvesFace_Intersector::ClassifyUVPoint(const gp_Pnt2d& Puv) const { 
  TopAbs_State state = myTopolTool->Classify(Puv,1e-7);
  return(state);
}
//============================================================================
void IntCurvesFace_Intersector::Destroy() { 
  if(PtrOnPolyhedron !=NULL) { 
    delete (IntCurveSurface_ThePolyhedronOfHInter *)PtrOnPolyhedron;
    PtrOnPolyhedron = NULL;
  }
  if(PtrOnBndBounding !=NULL) { 
    delete (Bnd_BoundSortBox *)PtrOnBndBounding;
    PtrOnBndBounding=NULL;
  }
}

 void IntCurvesFace_Intersector::SetUseBoundToler(Standard_Boolean UseBToler)
 {
   myUseBoundTol = UseBToler;
 }

 Standard_Boolean IntCurvesFace_Intersector::GetUseBoundToler() const
 {
   return myUseBoundTol;
 }

