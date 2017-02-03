// Created on: 1995-03-22
// Created by: Laurent BUCHARD
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomInt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_SequenceOfVec2d.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <stdio.h>
//=======================================================================
//function : IntTools_FClass2d:IntTools:_FClass2d
//purpose  : 
//=======================================================================
IntTools_FClass2d::IntTools_FClass2d()
{
}
//=======================================================================
//function : IntTools_FClass2d::IntTools_FClass2d
//purpose  : 
//=======================================================================
IntTools_FClass2d::IntTools_FClass2d(const TopoDS_Face& aFace,
				     const Standard_Real TolUV) 
: Toluv(TolUV), Face(aFace)  
{
  Init(Face, Toluv);
}
//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_FClass2d::IsHole() const
{
  return myIsHole;
} 
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void IntTools_FClass2d::Init(const TopoDS_Face& aFace,
			     const Standard_Real TolUV) 
{
  Standard_Boolean WireIsNotEmpty, Ancienpnt3dinitialise, degenerated;
  Standard_Integer nbpnts, firstpoint, NbEdges;
  Standard_Integer iX, aNbs1, nbs, Avant, BadWire;
  Standard_Real u, du, Tole, Tol, pfbid, plbid;
  Standard_Real FlecheU, FlecheV, TolVertex1, TolVertex;
  Standard_Real uFirst, uLast;
  Standard_Real aPrCf, aPrCf2;
  //
  TopoDS_Edge  edge;
  TopoDS_Vertex Va,Vb;
  TopAbs_Orientation Or;
  BRepTools_WireExplorer aWExp;
  TopExp_Explorer aExpF, aExp;
  Handle(Geom2d_Curve) aC2D;
  gp_Pnt Ancienpnt3d;
  TColgp_SequenceOfPnt2d SeqPnt2d;
  TColStd_DataMapOfIntegerInteger anIndexMap;
  TColgp_SequenceOfVec2d          aD1Prev;
  TColgp_SequenceOfVec2d          aD1Next;
  //
  aPrCf=Precision::Confusion();
  aPrCf2=aPrCf*aPrCf;
  myIsHole=Standard_True;
  //
  Toluv=TolUV;
  Face=aFace;
  Face.Orientation(TopAbs_FORWARD);
  Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
  surf->ChangeSurface().Initialize(aFace, Standard_False);
  //
  Tole = 0.;
  Tol=0.;
  Umin = Vmin = RealLast();
  Umax = Vmax = -Umin;
  BadWire=0;
  //
  //if face has several wires and one of them is bad,
  //it is necessary to process all of them for correct
  //calculation of Umin, Umax, Vmin, Vmax - ifv, 23.08.06 
  //
  aExpF.Init(Face,TopAbs_WIRE); 
  for(; aExpF.More();  aExpF.Next()) {
    const TopoDS_Wire& aW=*((TopoDS_Wire*)&aExpF.Current());
    //
    nbpnts = 0;
    firstpoint =1;
    FlecheU = 0.;
    FlecheV = 0.;
    TolVertex1=0.;
    TolVertex=0.;
    WireIsNotEmpty = Standard_False;
    Ancienpnt3dinitialise=Standard_False;
    Ancienpnt3d.SetCoord(0.,0.,0.);
    //
    SeqPnt2d.Clear();
    anIndexMap.Clear();
    aD1Prev.Clear();
    aD1Next.Clear();
    //
    // NbEdges
    NbEdges=0;
    aExp.Init(aW, TopAbs_EDGE);
    for(; aExp.More(); aExp.Next()) { 
      NbEdges++;
    }
    //
    aWExp.Init(aW, Face);
    for(;aWExp.More(); aWExp.Next()) {
      NbEdges--;
      edge = aWExp.Current();
      Or = edge.Orientation();
      if(!(Or==TopAbs_FORWARD || Or==TopAbs_REVERSED)) {
        continue;
      }
      //
      aC2D=BRep_Tool::CurveOnSurface(edge, Face, pfbid, plbid);
      if (aC2D.IsNull()) {
        return;
      }
      //
      BRepAdaptor_Curve2d C(edge,Face);
      BRepAdaptor_Curve C3d;
      //------------------------------------------
      degenerated=Standard_False;
      if(BRep_Tool::Degenerated(edge) ||
         BRep_Tool::IsClosed(edge, Face)) {
        degenerated=Standard_True;
      }
      //
      TopExp::Vertices(edge,Va,Vb);
      //
      TolVertex1=0.;
      TolVertex=0.;
      if (Va.IsNull()) {
        degenerated=Standard_True;
      }        
      else {
        TolVertex1=BRep_Tool::Tolerance(Va);
      }
      if (Vb.IsNull()){
        degenerated=Standard_True;
      }        
      else {
        TolVertex=BRep_Tool::Tolerance(Vb);
      }
        // 
      if(TolVertex<TolVertex1) {
        TolVertex=TolVertex1;
      }        
      //
      //-- Verification of cases when forgotten to code degenereted
      if(!degenerated) {
        // check that whole curve is located in vicinity of its middle point
        // (within sphere of Precision::Confusion() diameter)
        C3d.Initialize (edge, Face);
        gp_Pnt P3da = C3d.Value (0.5 * (pfbid + plbid));
        du = plbid - pfbid;
        const int NBSTEPS = 10;
        Standard_Real aPrec2 = 0.25 * Precision::Confusion() * Precision::Confusion();
        degenerated = Standard_True;
        for (Standard_Integer i=0; i <= NBSTEPS; i++)
        {
          Standard_Real U = pfbid + i * du / NBSTEPS;
          gp_Pnt P3db = C3d.Value (U);
          Standard_Real aR2 = P3da.SquareDistance (P3db);
          if (aR2 > aPrec2) {
            degenerated = Standard_False;
            break;
          }
        }
      }//if(!degenerated)
      //-- ----------------------------------------
      Tole = BRep_Tool::Tolerance(edge);
      if(Tole>Tol) {
        Tol=Tole;
      }
      //
      // NbSamples +> nbs
      nbs = Geom2dInt_Geom2dCurveTool::NbSamples(C);
      if (nbs > 2) {
        nbs*=4;
      }
      du = (plbid-pfbid)/(Standard_Real)(nbs-1);
      //
      if(Or==TopAbs_FORWARD) { 
        u = pfbid;
        uFirst=pfbid;
        uLast=plbid;
      }
      else { 
        u = plbid;
        uFirst=plbid;
        uLast=pfbid;
        du=-du;
      }
      //
      // aPrms
      aNbs1=nbs+1;
      TColStd_Array1OfReal aPrms(1, aNbs1);
      //
      if (nbs==2) {
        Standard_Real aCoef=0.0025;
        aPrms(1)=uFirst;
        aPrms(2)=uFirst+aCoef*(uLast-uFirst);
        aPrms(3)=uLast;
      }
      else if (nbs>2) {
        aNbs1=nbs;
        aPrms(1)=uFirst;
        for (iX=2; iX<aNbs1; ++iX) {
          aPrms(iX)=u+(iX-1)*du;
        }
        aPrms(aNbs1)=uLast;
      }
      //
      //-- ------------------------------------------------------------
      //-- Check distance uv between the start point of the edge
      //-- and the last point saved in SeqPnt2d
      //-- To to set the first point of the current 
      //-- afar from the last saved point
      Avant = nbpnts;
      for(iX=firstpoint; iX<=aNbs1; iX++) {
        Standard_Boolean IsRealCurve3d;
        Standard_Integer ii;
        Standard_Real aDstX;
        gp_Pnt2d P2d;
        gp_Pnt P3d;
        //
        u=aPrms(iX);
        P2d = C.Value(u);
        if(P2d.X()<Umin) Umin = P2d.X();
        if(P2d.X()>Umax) Umax = P2d.X();
        if(P2d.Y()<Vmin) Vmin = P2d.Y();
        if(P2d.Y()>Vmax) Vmax = P2d.Y();
        //
        aDstX=RealLast();        
        if(degenerated==Standard_False) { 
          P3d=C3d.Value(u);
          if(nbpnts>1) {
            if(Ancienpnt3dinitialise) { 
              aDstX=P3d.SquareDistance(Ancienpnt3d);
            }
          }
        }
        //
        IsRealCurve3d = Standard_True; 
        if (aDstX < aPrCf2)  {
          if(iX>1) {
            Standard_Real aDstX1;
            gp_Pnt MidP3d;
            //
            MidP3d = C3d.Value(0.5*(u+aPrms(iX-1)));
            aDstX1=P3d.SquareDistance( MidP3d );
            if (aDstX1 < aPrCf2){
              IsRealCurve3d = Standard_False;
            }
          }
        }
        //
        if (IsRealCurve3d) {
          if(degenerated==Standard_False) { 
            Ancienpnt3d=P3d;
            Ancienpnt3dinitialise=Standard_True;
          }
          nbpnts++;
          SeqPnt2d.Append(P2d);
        }
        //
        ii=nbpnts;
        if(ii>(Avant+4)) { 
          Standard_Real ul, dU, dV;
          gp_Pnt2d Pp;
          //
          gp_Lin2d Lin(SeqPnt2d(ii-2),gp_Dir2d(gp_Vec2d(SeqPnt2d(ii-2),SeqPnt2d(ii))));
          ul = ElCLib::Parameter(Lin,SeqPnt2d(ii-1));
          Pp = ElCLib::Value(ul,Lin);
          dU = Abs(Pp.X()-SeqPnt2d(ii-1).X());
          dV = Abs(Pp.Y()-SeqPnt2d(ii-1).Y());
          if(dU>FlecheU) {
            FlecheU = dU;
          }
          if(dV>FlecheV) {
            FlecheV = dV;
          }
        }
      }// for(iX=firstpoint; iX<=aNbs1; iX++) {
      //
      if(BadWire) {
        continue; //if face has several wires and one of them is bad,
                  //it is necessary to process all of them for correct
                  //calculation of Umin, Umax, Vmin, Vmax - ifv, 23.08.06 
      }
      //
      if(firstpoint==1) firstpoint=2;
      WireIsNotEmpty = Standard_True;
      // Append the derivative of the first parameter.
      Standard_Real aU = aPrms(1);
      gp_Pnt2d      aP;
      gp_Vec2d      aV;

      C.D1(aU, aP, aV);

      if(Or == TopAbs_REVERSED)
        aV.Reverse();

      aD1Next.Append(aV);

      // Append the derivative of the last parameter.
      aU = aPrms(aNbs1);
      C.D1(aU, aP, aV);

      if(Or == TopAbs_REVERSED)
        aV.Reverse();

      if (NbEdges > 0)
        aD1Prev.Append(aV);
      else
        aD1Prev.Prepend(aV);

      // Fill the map anIndexMap.
      if (Avant > 0)
        anIndexMap.Bind(Avant, aD1Next.Length());
      else
        anIndexMap.Bind(1, aD1Next.Length());
    } //for(;aWExp.More(); aWExp.Next()) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //
    if(NbEdges) { 
      //-- count ++ with normal explorer and -- with Wire Explorer
      TColgp_Array1OfPnt2d PClass(1,2);
      gp_Pnt2d anInitPnt(0., 0.);
      //
      PClass.Init(anInitPnt);
      TabClass.Append((void *)new CSLib_Class2d(PClass,
						FlecheU,
						FlecheV,
						Umin,Vmin,Umax,Vmax));
      BadWire=1;
      TabOrien.Append(-1);
    }
    //
    else if(WireIsNotEmpty) { 
      TColgp_Array1OfPnt2d PClass(1,nbpnts);
      gp_Pnt2d anInitPnt(0., 0.);
      //
      PClass.Init(anInitPnt);
      if(nbpnts>3) { 
        Standard_Integer im2=nbpnts-2;
        Standard_Integer im1=nbpnts-1;
        Standard_Integer im0=1;
        Standard_Integer ii;
        Standard_Real    angle = 0.0;
        Standard_Real aX0, aY0, aX1, aY1, aS;
        //
        aS=0.;
        //
        Standard_Integer iFlag=1;
	//
        PClass(im2)=SeqPnt2d.Value(im2);
        PClass(im1)=SeqPnt2d.Value(im1);
        PClass(nbpnts)=SeqPnt2d.Value(nbpnts);
        for(ii=1; ii<nbpnts; ii++,im0++,im1++,im2++) { 
          if(im2>=nbpnts) im2=1;
          if(im1>=nbpnts) im1=1;
          PClass(ii)=SeqPnt2d.Value(ii);
          //
          const gp_Pnt2d& aP2D1=PClass(im1);
          const gp_Pnt2d& aP2D0=PClass(im0);
          //aP2D0 is next to aP2D1
          aP2D0.Coord(aX0, aY0);
          aP2D1.Coord(aX1, aY1);
          aS=aS+(aY0+aY1)*(aX1-aX0); 

          gp_Vec2d A(PClass(im2),PClass(im1));
          gp_Vec2d B(PClass(im1),PClass(im0));

          Standard_Real N = A.Magnitude() * B.Magnitude();
          if(N>1e-16) { 
            Standard_Real a=A.Angle(B);
            //  
            if (anIndexMap.IsBound(im1)) {
              Standard_Integer  anInd  = anIndexMap.Find(im1);
              const gp_Vec2d   &aVPrev = aD1Prev.Value(anInd);
              const gp_Vec2d   &aVNext = aD1Next.Value(anInd);

              Standard_Real aN = aVPrev.Magnitude() * aVNext.Magnitude();
              if(aN > 1e-16) { 
                Standard_Real aDerivAngle, aAbsDA, aProduct, aPA;
                //ifv 23.08.06
		aPA=Precision::Angular();
		aDerivAngle = aVPrev.Angle(aVNext);
		aAbsDA=Abs(aDerivAngle);
                if(aAbsDA <= aPA) {
		  aDerivAngle = 0.; 
		}
		//
		aProduct=aDerivAngle * a;
		//
	        if(Abs(aAbsDA - M_PI) <= aPA) {
		  if (aProduct > 0.) {
		    aProduct=-aProduct; 
		  }
		}
                //ifv 23.08.06 : if edges continuity > G1, |aDerivAngle| ~0,
                //but can has wrong sign and causes condition aDerivAngle * a < 0.
                //that is wrong in such situation
                if (iFlag && aProduct < 0.) {
                  iFlag=0;
                  // Bad case.
                  angle = 0.;
                }
              }
            }
            angle+=a;
          }
        }//for(ii=1; ii<nbpnts; ii++,im0++,im1++,im2++) { 
        if (!iFlag) {
          angle = 0.; 
        }
        if(aS>0.){
          myIsHole=Standard_False;
        }
        //
        if(FlecheU<Toluv)
          FlecheU = Toluv;

        if(FlecheV<Toluv)
          FlecheV = Toluv;

        TabClass.Append((void *)new CSLib_Class2d(PClass,
						  FlecheU,
						  FlecheV,
						  Umin,Vmin,Umax,Vmax));
        //
        if((angle<2 && angle>-2)||(angle>10)||(angle<-10)) { 
          BadWire=1;
          TabOrien.Append(-1);
        } 
        else {         
          TabOrien.Append((angle>0.0)? 1 : 0);
        }
      } 
      else { 
        BadWire=1;
        TabOrien.Append(-1);
        TColgp_Array1OfPnt2d PPClass(1,2);
        PPClass.Init(anInitPnt);
        TabClass.Append((void *)new CSLib_Class2d(PPClass,
						  FlecheU,
						  FlecheV,
						  Umin,Vmin,Umax,Vmax));
      }
    }// else if(WireIsNotEmpty)
  } // for(; aExpF.More();  aExpF.Next()) {
  //
  Standard_Integer nbtabclass = TabClass.Length();
  //
  if(nbtabclass>0) { 
    //-- if an error on a wire was detected : all TabOrien set to -1
    if(BadWire) {
      TabOrien(1)=-1;
    }
    
    if(   surf->GetType()==GeomAbs_Cone
       || surf->GetType()==GeomAbs_Cylinder
       || surf->GetType()==GeomAbs_Torus
       || surf->GetType()==GeomAbs_Sphere
       || surf->GetType()==GeomAbs_SurfaceOfRevolution) { 
      Standard_Real uuu=M_PI+M_PI-(Umax-Umin);
      if(uuu<0) uuu=0;
      U1 = Umin-uuu*0.5;
      U2 = U1+M_PI+M_PI;
    }
    else { 
      U1=U2=0.0; 
    } 
    
    if(surf->GetType()==GeomAbs_Torus) { 
      Standard_Real uuu=M_PI+M_PI-(Vmax-Vmin);
      if(uuu<0) uuu=0;
      
      V1 = Vmin-uuu*0.5;
      V2 = V1+M_PI+M_PI;
    }
    else { 
      V1=V2=0.0; 
    }   
  }
}
//=======================================================================
//function : PerformInfinitePoint
//purpose  : 
//=======================================================================
TopAbs_State IntTools_FClass2d::PerformInfinitePoint() const 
{ 
  if(Umax==-RealLast() || Vmax==-RealLast() || 
     Umin==RealLast() || Vmin==RealLast()) { 
    return(TopAbs_IN);
  }
  gp_Pnt2d P(Umin-(Umax-Umin),Vmin-(Vmax-Vmin));
  return(Perform(P,Standard_False));
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
TopAbs_State IntTools_FClass2d::Perform
  (const gp_Pnt2d& _Puv,
   const Standard_Boolean RecadreOnPeriodic) const
{ 
  Standard_Integer nbtabclass = TabClass.Length();
  if (nbtabclass == 0)
  {
    return TopAbs_IN;
  }

  //-- U1 is the First Param and U2 is in this case U1+Period
  Standard_Real u = _Puv.X();
  Standard_Real v = _Puv.Y();
  Standard_Real uu = u;
  Standard_Real vv = v;
  TopAbs_State aStatus = TopAbs_UNKNOWN;

  Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
  surf->ChangeSurface().Initialize( Face, Standard_False );
  
  const Standard_Boolean IsUPer  = surf->IsUPeriodic();
  const Standard_Boolean IsVPer  = surf->IsVPeriodic();
  const Standard_Real    uperiod = IsUPer ? surf->UPeriod() : 0.0;
  const Standard_Real    vperiod = IsVPer ? surf->VPeriod() : 0.0;

  Standard_Boolean urecadre, vrecadre, bUseClassifier;
  Standard_Integer dedans = 1;
  //
  urecadre = Standard_False;
  vrecadre = Standard_False;
  //
  if (RecadreOnPeriodic) {
    Standard_Real du, dv;
    if (IsUPer) {
      GeomInt::AdjustPeriodic(uu, Umin, Umax, uperiod, uu, du);
    }// if (IsUPer) {
    //
    if (IsVPer) {
      GeomInt::AdjustPeriodic(vv, Vmin, Vmax, vperiod, vv, dv);
    }//if (IsVPer) {
  }
  //
  for(;;) {
    dedans = 1;
    gp_Pnt2d Puv(u,v);
    bUseClassifier = (TabOrien(1) == -1);
    if(!bUseClassifier) {
      Standard_Integer n, cur, TabOrien_n ;
      for(n=1; n<=nbtabclass; n++) { 
        cur = ((CSLib_Class2d *)TabClass(n))->SiDans(Puv);
        TabOrien_n=TabOrien(n);
        
        if(cur==1) { 
          if(TabOrien_n==0) { 
            dedans = -1; 
            break;
          }
        }
        else if(cur==-1) { 
          if(TabOrien_n==1) {  
            dedans = -1; 
            break;
          }
        }
        else { 
          dedans = 0;
          break;
        }
      } // for(n=1; n<=nbtabclass; n++)
      
      if(dedans==0) { 
        bUseClassifier = Standard_True;
      } 
      else {
        aStatus = (dedans == 1) ? TopAbs_IN : TopAbs_OUT;
      }
    } // if(TabOrien(1)!=-1) {
    //compute state of the point using face classifier
    if (bUseClassifier) {
      //compute tolerance to use in face classifier
      Standard_Real aURes, aVRes, aFCTol;
      Standard_Boolean bUIn, bVIn;
      //
      aURes = surf->UResolution(Toluv);
      aVRes = surf->VResolution(Toluv);
      //
      bUIn = (u >= Umin) && (u <= Umax);
      bVIn = (v >= Vmin) && (v <= Vmax);
      //
      if (bUIn==bVIn) {
	aFCTol = Min(aURes, aVRes);
      }
      else {
	aFCTol = (!bUIn) ? aURes : aVRes;
      }
      //
      BRepClass_FaceClassifier aClassifier;
      aClassifier.Perform(Face,Puv,aFCTol);
      aStatus = aClassifier.State();
    }
    
    if (!RecadreOnPeriodic || (!IsUPer && !IsVPer))
      return aStatus;
    
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;
    
    if (!urecadre){
      u = uu;
      urecadre = Standard_True;
    }
    else {
      if (IsUPer){
        u += uperiod;
      }
    }

    if (u > Umax || !IsUPer) {
      if (!vrecadre){
        v = vv;
        vrecadre = Standard_True;
      }
      else {
        if (IsVPer){
          v += vperiod;
        }
      }

      u = uu;
      
      if (v > Vmax || !IsVPer) {
        return aStatus;
      }
    }
  } //while (1)
}

//=======================================================================
//function : TestOnRestriction
//purpose  : 
//=======================================================================
TopAbs_State IntTools_FClass2d::TestOnRestriction
  (const gp_Pnt2d& _Puv,
   const Standard_Real Tol,
   const Standard_Boolean RecadreOnPeriodic) const
{ 
  Standard_Integer nbtabclass = TabClass.Length();
  if (nbtabclass == 0)
  {
    return TopAbs_IN;
  }

  //-- U1 is the First Param and U2 in this case is U1+Period
  Standard_Real u=_Puv.X();
  Standard_Real v=_Puv.Y();
  Standard_Real uu = u, vv = v;

  Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
  surf->ChangeSurface().Initialize( Face, Standard_False );
  const Standard_Boolean IsUPer  = surf->IsUPeriodic();
  const Standard_Boolean IsVPer  = surf->IsVPeriodic();
  const Standard_Real    uperiod = IsUPer ? surf->UPeriod() : 0.0;
  const Standard_Real    vperiod = IsVPer ? surf->VPeriod() : 0.0;
  TopAbs_State aStatus = TopAbs_UNKNOWN;
  Standard_Boolean urecadre = Standard_False, vrecadre = Standard_False;
  Standard_Integer dedans = 1;

  if (RecadreOnPeriodic) {
    Standard_Real du, dv;
    if (IsUPer) {
      GeomInt::AdjustPeriodic(uu, Umin, Umax, uperiod, uu, du);
    }// if (IsUPer) {
    //
    if (IsVPer) {
      GeomInt::AdjustPeriodic(vv, Vmin, Vmax, vperiod, vv, dv);
    }//if (IsVPer) {
  }
  //
  for (;;) {
    dedans = 1;
    gp_Pnt2d Puv(u,v);
    
    if(TabOrien(1)!=-1) { 
      for(Standard_Integer n=1; n<=nbtabclass; n++) { 
        Standard_Integer cur = ((CSLib_Class2d *)TabClass(n))->SiDans_OnMode(Puv,Tol);
        if(cur==1) { 
          if(TabOrien(n)==0) { 
            dedans = -1; 
            break;
          }
        }
        else if(cur==-1) { 
          if(TabOrien(n)==1) {  
            dedans = -1; 
            break;
          }
        }
        else { 
          dedans = 0;
          break;
        }
      }
      if(dedans==0) {
        aStatus = TopAbs_ON;
      }
      if(dedans == 1) {
        aStatus = TopAbs_IN;
      }
      if(dedans == -1) {
        aStatus = TopAbs_OUT;
      }
    }
    else {  //-- TabOrien(1)=-1  Wrong  Wire 
      BRepClass_FaceClassifier aClassifier;
      aClassifier.Perform(Face,Puv,Tol);
      aStatus = aClassifier.State();
    }
    
    if (!RecadreOnPeriodic || (!IsUPer && !IsVPer))
      return aStatus;
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;
    
    if (!urecadre)
      {
        u = uu;
        urecadre = Standard_True;
      }
    else
      if (IsUPer)
        u += uperiod;
    if (u > Umax || !IsUPer)
      {
        if (!vrecadre)
          {
            v = vv;
            vrecadre = Standard_True;
          }
        else
          if (IsVPer)
            v += vperiod;
        
        u = uu;
        
        if (v > Vmax || !IsVPer)
          return aStatus;
      }
  } //for (;;)
}
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void IntTools_FClass2d::Destroy() 
{ 
  Standard_Integer nbtabclass = TabClass.Length(); 
  for(Standard_Integer d=1; d<=nbtabclass;d++) {
    if(TabClass(d)) { 
      delete ((CSLib_Class2d *)TabClass(d));
      TabClass(d)=NULL;
    }
  }
}


