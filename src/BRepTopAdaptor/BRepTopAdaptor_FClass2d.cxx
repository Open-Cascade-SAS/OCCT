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

#define AFFICHAGE 0

#define No_Standard_OutOfRange


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#ifdef _MSC_VER
#include <stdio.h>
#endif


#ifdef OCCT_DEBUG
#define LBRCOMPT 0
#else
#define LBRCOMPT 0
#endif


#if LBRCOMPT  
class StatistiquesFClass2d { 
public:
  long unsigned NbConstrShape;
  long unsigned NbPerformInfinitePoint;
  long unsigned NbPerform;
  long unsigned NbTestOnRestriction;
  long unsigned NbDestroy;
public:
  StatistiquesFClass2d() { 
    NbConstrShape=NbPerform=NbPerformInfinitePoint=NbDestroy=0;
  }
  ~StatistiquesFClass2d() { 
    printf("\n--- Statistiques BRepTopAdaptor:\n");
    printf("\nConstructeur(Shape) : %10lu",NbConstrShape);
    printf("\nPerformInfinitePoint: %10lu",NbPerformInfinitePoint);
    printf("\nTestOnRestriction   : %10lu",NbTestOnRestriction);
    printf("\nPerform(pnt2d)      : %10lu",NbPerform);
    printf("\nDestroy             : %10lu",NbDestroy);
  }
};

static StatistiquesFClass2d STAT;
#endif




BRepTopAdaptor_FClass2d::BRepTopAdaptor_FClass2d(const TopoDS_Face& aFace,const Standard_Real TolUV) 
: Toluv(TolUV), Face(aFace)  { 
  
#if LBRCOMPT 
  STAT.NbConstrShape++;
#endif

  //-- dead end on surfaces defined on more than one period

  Face.Orientation(TopAbs_FORWARD);
  Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
  surf->ChangeSurface().Initialize(aFace,Standard_False);
  
  TopoDS_Edge  edge;
  TopAbs_Orientation Or;
  Standard_Real u,du,Tole = 0.0,Tol=0.0;
  BRepTools_WireExplorer WireExplorer;
  TopExp_Explorer FaceExplorer;

  Umin = Vmin = 0.0; //RealLast();
  Umax = Vmax = -Umin;

  Standard_Integer BadWire=0;
  for( FaceExplorer.Init(Face,TopAbs_WIRE); (FaceExplorer.More() && BadWire==0); FaceExplorer.Next() )
    {
      Standard_Integer nbpnts = 0;
      TColgp_SequenceOfPnt2d SeqPnt2d;
      Standard_Integer firstpoint = 1;
      Standard_Real FlecheU = 0.0;
      Standard_Real FlecheV = 0.0;
      Standard_Boolean WireIsNotEmpty = Standard_False;
      Standard_Integer NbEdges = 0;

      TopExp_Explorer Explorer;
      for( Explorer.Init(FaceExplorer.Current(),TopAbs_EDGE); Explorer.More(); Explorer.Next() ) NbEdges++;
        
      gp_Pnt Ancienpnt3d(0,0,0);
      Standard_Boolean Ancienpnt3dinitialise = Standard_False;

      for( WireExplorer.Init(TopoDS::Wire(FaceExplorer.Current()),Face); WireExplorer.More(); WireExplorer.Next() )
	{
	  
	  NbEdges--;
	  edge = WireExplorer.Current();
	  Or = edge.Orientation();
	  if(Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)
	    {
	      Standard_Real pfbid,plbid;
	      if(BRep_Tool::CurveOnSurface(edge,Face,pfbid,plbid).IsNull()) return;
	      BRepAdaptor_Curve2d C(edge,Face);
	
	      //-- ----------------------------------------
	      Standard_Boolean degenerated=Standard_False;
	      if(BRep_Tool::Degenerated(edge))   degenerated=Standard_True;
	      if(BRep_Tool::IsClosed(edge,Face)) degenerated=Standard_True;
	      TopoDS_Vertex Va,Vb;
	      TopExp::Vertices(edge,Va,Vb);
	      Standard_Real TolVertex1=0.,TolVertex=0.;
	      if (Va.IsNull()) degenerated=Standard_True;
	      else TolVertex1=BRep_Tool::Tolerance(Va);
	      if (Vb.IsNull()) degenerated=Standard_True;
	      else TolVertex=BRep_Tool::Tolerance(Vb);
	      if(TolVertex<TolVertex1) TolVertex=TolVertex1;
	      BRepAdaptor_Curve C3d;
	
	      if(Abs(plbid-pfbid) < 1.e-9) continue;

	      //if(degenerated==Standard_False)
	      //  C3d.Initialize(edge,Face);

	      //-- Check cases when it was forgotten to code degenerated :  PRO17410 (janv 99)
	      if(degenerated == Standard_False)
		{
		  C3d.Initialize(edge,Face);
		  du=(plbid-pfbid)*0.1;
		  u=pfbid+du;
		  gp_Pnt P3da=C3d.Value(u);
		  degenerated=Standard_True;
		  u+=du;
		  do
		    {
		      
		      gp_Pnt P3db=C3d.Value(u);
		      // 		      if(P3da.SquareDistance(P3db)) { degenerated=Standard_False; break; }
		      if(P3da.SquareDistance(P3db) > Precision::Confusion()) { degenerated=Standard_False; break; }
		      u+=du;
		    }
		  while(u<plbid);
		}
	      
	      //-- ----------------------------------------

	      Tole = BRep_Tool::Tolerance(edge);
	      if(Tole>Tol) Tol=Tole;
	      
	      //Standard_Integer nbs = 1 + Geom2dInt_Geom2dCurveTool::NbSamples(C);
	      Standard_Integer nbs = Geom2dInt_Geom2dCurveTool::NbSamples(C);
	      //-- Attention to rational bsplines of degree 3. (ends of circles among others)
	      if (nbs > 2) nbs*=4;
	      du = (plbid-pfbid)/(Standard_Real)(nbs-1);

	      if(Or==TopAbs_FORWARD) u = pfbid;
	      else { u = plbid; du=-du;	}
	
	      //-- ------------------------------------------------------------
	      //-- Check distance uv between the start point of the edge
	      //-- and the last point registered in SeqPnt2d
	      //-- Try to remote the first point of the current edge 
	      //-- from the last saved point
#ifdef OCCT_DEBUG
	      gp_Pnt2d Pnt2dDebutEdgeCourant = C.Value(u); (void)Pnt2dDebutEdgeCourant;
#endif

	      //Standard_Real Baillement2dU=0;
	      //Standard_Real Baillement2dV=0;
#if AFFICHAGE
	      if(nbpnts>1) printf("\nTolVertex %g ",TolVertex);
#endif

	      if(firstpoint==2) u+=du;
	      Standard_Integer Avant = nbpnts;
	      for(Standard_Integer e = firstpoint; e<=nbs; e++)
		{
		  gp_Pnt2d P2d = C.Value(u);
		  if(P2d.X()<Umin) Umin = P2d.X();
		  if(P2d.X()>Umax) Umax = P2d.X();
		  if(P2d.Y()<Vmin) Vmin = P2d.Y();
		  if(P2d.Y()>Vmax) Vmax = P2d.Y();
	  
		  Standard_Real dist3dptcourant_ancienpnt=1e+20;//RealLast();
		  gp_Pnt P3d;
		  if(degenerated==Standard_False)
		    {
		      P3d=C3d.Value(u);
		      if(nbpnts>1 && Ancienpnt3dinitialise) dist3dptcourant_ancienpnt = P3d.Distance(Ancienpnt3d);
		    }
		  Standard_Boolean IsRealCurve3d = Standard_True; //patch
		  if(dist3dptcourant_ancienpnt < Precision::Confusion())
		    {
		      gp_Pnt MidP3d = C3d.Value( u-du/2. );
		      if (P3d.Distance( MidP3d ) < Precision::Confusion()) IsRealCurve3d = Standard_False;
		    }
		  if(IsRealCurve3d)
		    {
		      if(degenerated==Standard_False) { Ancienpnt3d=P3d;  Ancienpnt3dinitialise=Standard_True; }
		      nbpnts++;
		      SeqPnt2d.Append(P2d);
		    }
#if AFFICHAGE
                  else { static int mm=0; printf("\npoint p%d  %g %g %g",++mm,P3d.X(),P3d.Y(),P3d.Z());	}
#endif
		  u+=du;
		  Standard_Integer ii = nbpnts;
		  //-- printf("\n nbpnts:%4d  u=%7.5g   FlecheU=%7.5g  FlecheV=%7.5g  ii=%3d  Avant=%3d ",nbpnts,u,FlecheU,FlecheV,ii,Avant);
// 		  if(ii>(Avant+4))
//  Modified by Sergey KHROMOV - Fri Apr 19 09:46:12 2002 Begin
		  if(ii>(Avant+4) && SeqPnt2d(ii-2).SquareDistance(SeqPnt2d(ii)))
//  Modified by Sergey KHROMOV - Fri Apr 19 09:46:13 2002 End
		    {
		      gp_Lin2d Lin(SeqPnt2d(ii-2),gp_Dir2d(gp_Vec2d(SeqPnt2d(ii-2),SeqPnt2d(ii))));
		      Standard_Real ul = ElCLib::Parameter(Lin,SeqPnt2d(ii-1));
		      gp_Pnt2d Pp = ElCLib::Value(ul,Lin);
		      Standard_Real dU = Abs(Pp.X()-SeqPnt2d(ii-1).X());
		      Standard_Real dV = Abs(Pp.Y()-SeqPnt2d(ii-1).Y());
		      //-- printf(" (du=%7.5g   dv=%7.5g)",dU,dV);
		      if(dU>FlecheU) FlecheU = dU;
		      if(dV>FlecheV) FlecheV = dV;
		    }
		}//for(e=firstpoint
	      if(firstpoint==1) firstpoint=2;
	      WireIsNotEmpty = Standard_True;
	    }//if(Or==FORWARD,REVERSED
	} //-- Edges -> for(Ware.Explorer

      if(NbEdges)
	{ //-- on compte ++ with a normal explorer and with the Wire Explorer
/*
#ifdef OCCT_DEBUG

	  cout << endl;
	  cout << "*** BRepTopAdaptor_Fclass2d  ** Wire Probablement FAUX **" << endl;
	  cout << "*** WireExplorer does not find all edges " << endl;
	  cout << "*** Connect old classifier" << endl;
#endif
*/
	  TColgp_Array1OfPnt2d PClass(1,2);
	  //// modified by jgv, 28.04.2009 ////
	  PClass.Init(gp_Pnt2d(0.,0.));
	  /////////////////////////////////////
	  TabClass.Append((void *)new CSLib_Class2d(PClass,FlecheU,FlecheV,Umin,Vmin,Umax,Vmax));
	  BadWire=1;
	  TabOrien.Append(-1);
	}
      else if(WireIsNotEmpty)
	{
	  //Standard_Real anglep=0,anglem=0;
	  TColgp_Array1OfPnt2d PClass(1,nbpnts);
	  Standard_Real square = 0.0;

	  //-------------------------------------------------------------------
	  //-- ** The mode of calculation was somewhat changed 
	  //-- Before Oct 31 97 , the total angle of  
	  //-- rotation of the wire was evaluated on all angles except for the last 
	  //-- ** Now, exactly the angle of rotation is evaluated
	  //-- If a value remote from 2PI or -2PI is found, it means that there is 
	  //-- an uneven number of loops

	  if(nbpnts>3)
	    {
//	      Standard_Integer im2=nbpnts-2;
	      Standard_Integer im1=nbpnts-1;
	      Standard_Integer im0=1;
//	      PClass(im2)=SeqPnt2d.Value(im2);
	      PClass(im1)=SeqPnt2d.Value(im1);
	      PClass(nbpnts)=SeqPnt2d.Value(nbpnts);

              Standard_Real aPer = 0.;
//	      for(Standard_Integer ii=1; ii<nbpnts; ii++,im0++,im1++,im2++)
	      for(Standard_Integer ii=1; ii<nbpnts; ii++,im0++,im1++)
		{ 
//		  if(im2>=nbpnts) im2=1;
		  if(im1>=nbpnts) im1=1;
		  PClass(ii)=SeqPnt2d.Value(ii);
//		  gp_Vec2d A(PClass(im2),PClass(im1));
//		  gp_Vec2d B(PClass(im1),PClass(im0));
//		  Standard_Real N = A.Magnitude() * B.Magnitude();

		  square += (PClass(im0).X()-PClass(im1).X())*(PClass(im0).Y()+PClass(im1).Y())*.5; 
                  aPer += (PClass(im0).XY() - PClass(im1).XY()).Modulus();

//		  if(N>1e-16){ Standard_Real a=A.Angle(B); angle+=a; }
		}

              Standard_Real anExpThick = Max(2. * Abs(square) / aPer, 1e-7);
              Standard_Real aDefl = Max(FlecheU, FlecheV);
              Standard_Real aDiscrDefl = Min(aDefl*0.1, anExpThick * 10.);
              while (aDefl > anExpThick && aDiscrDefl > 1e-7)
              {
                // Deflection of the polygon is too much for this ratio of area and perimeter,
                // and this might lead to self-intersections.
                // Discretize the wire more tightly to eliminate the error.
                firstpoint = 1;
                SeqPnt2d.Clear();
                FlecheU = 0.0;
                FlecheV = 0.0;
                for (WireExplorer.Init(TopoDS::Wire(FaceExplorer.Current()), Face);
                  WireExplorer.More(); WireExplorer.Next())
                {
                  edge = WireExplorer.Current();
                  Or = edge.Orientation();
                  if (Or == TopAbs_FORWARD || Or == TopAbs_REVERSED)
                  {
                    Standard_Real pfbid, plbid;
                    BRep_Tool::Range(edge, Face, pfbid, plbid);
                    if (Abs(plbid - pfbid) < 1.e-9) continue;
                    BRepAdaptor_Curve2d C(edge, Face);
                    GCPnts_QuasiUniformDeflection aDiscr(C, aDiscrDefl);
                    if (!aDiscr.IsDone())
                      break;
                    Standard_Integer nbp = aDiscr.NbPoints();
                    Standard_Integer iStep = 1, i = 1, iEnd = nbp + 1;
                    if (Or == TopAbs_REVERSED)
                    {
                      iStep = -1;
                      i = nbp;
                      iEnd = 0;
                    }
                    if (firstpoint == 2)
                      i += iStep;
                    for (; i != iEnd; i += iStep)
                    {
                      gp_Pnt2d aP2d = C.Value(aDiscr.Parameter(i));
                      SeqPnt2d.Append(aP2d);
                    }
                    if (nbp > 2)
                    {
                      Standard_Integer ii = SeqPnt2d.Length();
                      gp_Lin2d Lin(SeqPnt2d(ii - 2), gp_Dir2d(gp_Vec2d(SeqPnt2d(ii - 2), SeqPnt2d(ii))));
                      Standard_Real ul = ElCLib::Parameter(Lin, SeqPnt2d(ii - 1));
                      gp_Pnt2d Pp = ElCLib::Value(ul, Lin);
                      Standard_Real dU = Abs(Pp.X() - SeqPnt2d(ii - 1).X());
                      Standard_Real dV = Abs(Pp.Y() - SeqPnt2d(ii - 1).Y());
                      if (dU > FlecheU) FlecheU = dU;
                      if (dV > FlecheV) FlecheV = dV;
                    }
                    firstpoint = 2;
                  }
                }
                nbpnts = SeqPnt2d.Length();
                PClass.Resize(1, nbpnts, Standard_False);
                im1 = nbpnts - 1;
                im0 = 1;
                PClass(im1) = SeqPnt2d.Value(im1);
                PClass(nbpnts) = SeqPnt2d.Value(nbpnts);
                square = 0.;
                aPer = 0.;
                for (Standard_Integer ii = 1; ii<nbpnts; ii++, im0++, im1++)
                {
                  if (im1 >= nbpnts) im1 = 1;
                  PClass(ii) = SeqPnt2d.Value(ii);
                  square += (PClass(im0).X() - PClass(im1).X())*(PClass(im0).Y() + PClass(im1).Y())*.5;
                  aPer += (PClass(im0).XY() - PClass(im1).XY()).Modulus();
                }

                anExpThick = Max(2. * Abs(square) / aPer, 1e-7);
                aDefl = Max(FlecheU, FlecheV);
                aDiscrDefl = Min(aDiscrDefl * 0.1, anExpThick * 10.);
              }
      
	      //-- FlecheU*=10.0;
	      //-- FlecheV*=10.0;
	      if(FlecheU<Toluv) FlecheU = Toluv;
	      if(FlecheV<Toluv) FlecheV = Toluv;
	      //-- cout<<" U:"<<FlecheU<<" V:"<<FlecheV<<endl;
	      TabClass.Append((void *)new CSLib_Class2d(PClass,FlecheU,FlecheV,Umin,Vmin,Umax,Vmax));

//	      if((angle<2 && angle>-2)||(angle>10)||(angle<-10))
//		{
//		  BadWire=1;
//		  TabOrien.Append(-1);
//#ifdef OCCT_DEBUG
//		  cout << endl;
//		  cout << "*** BRepTopAdaptor_Fclass2d  ** Wire Probably FALSE **" << endl;
//		  cout << "*** Total rotation angle of the wire : " << angle << endl;
//		  cout << "*** Connect the old classifier" << endl;
//#endif
//		} 
//	      else TabOrien.Append(((angle>0.0)? 1 : 0));
	      TabOrien.Append(((square < 0.0)? 1 : 0));
	    }//if(nbpoints>3
	  else
	    { 
#ifdef OCCT_DEBUG
	      cout << endl;
	      cout << "*** BRepTopAdaptor_Fclass2d  ** Wire Probably FALSE **" << endl;
	      cout << "*** The sample wire contains less than 3 points" << endl;
	      cout << "*** Connect the old classifier" << endl;
#endif       
	      BadWire=1;
	      TabOrien.Append(-1);
	      TColgp_Array1OfPnt2d xPClass(1,2);
	      xPClass(1) = SeqPnt2d(1); 
	      xPClass(2) = SeqPnt2d(2);
	      TabClass.Append((void *)new CSLib_Class2d(xPClass,FlecheU,FlecheV,Umin,Vmin,Umax,Vmax));
	    }
	}//else if(WareIsNotEmpty
  }//for(FaceExplorer

  Standard_Integer nbtabclass = TabClass.Length();

  if(nbtabclass>0)
    {
      //-- If an error was detected on a wire: set all TabOrien to -1
      if(BadWire) TabOrien(1)=-1;

      if(   surf->GetType()==GeomAbs_Cone
	 || surf->GetType()==GeomAbs_Cylinder
	 || surf->GetType()==GeomAbs_Torus
	 || surf->GetType()==GeomAbs_Sphere
	 || surf->GetType()==GeomAbs_SurfaceOfRevolution)
	
	{
	  Standard_Real uuu=M_PI+M_PI-(Umax-Umin);
	  if(uuu<0) uuu=0;
	  U1 = 0.0;  // modified by NIZHNY-OFV  Thu May 31 14:24:10 2001 ---> //Umin-uuu*0.5;
	  U2 = 2*M_PI; // modified by NIZHNY-OFV  Thu May 31 14:24:35 2001 ---> //U1+M_PI+M_PI;
	}
      else { U1=U2=0.0; } 
    
      if(surf->GetType()==GeomAbs_Torus)
	{ 
	  Standard_Real uuu=M_PI+M_PI-(Vmax-Vmin);
	  if(uuu<0) uuu=0;
	  V1 = 0.0;  // modified by NIZHNY-OFV  Thu May 31 14:24:55 2001 ---> //Vmin-uuu*0.5;
	  V2 = 2*M_PI; // modified by NIZHNY-OFV  Thu May 31 14:24:59 2001 ---> //V1+M_PI+M_PI;
	}
      else { V1=V2=0.0; }   
    }
}

TopAbs_State BRepTopAdaptor_FClass2d::PerformInfinitePoint() const { 
#if LBRCOMPT 
  STAT.NbPerformInfinitePoint++;
#endif
  
  if(Umax==-RealLast() || Vmax==-RealLast() || Umin==RealLast() || Vmin==RealLast()) { 
    return(TopAbs_IN);
  }
  gp_Pnt2d P(Umin-(Umax-Umin),Vmin-(Vmax-Vmin));
  return(Perform(P,Standard_False));
}

TopAbs_State BRepTopAdaptor_FClass2d::Perform(const gp_Pnt2d& _Puv,
					      const Standard_Boolean RecadreOnPeriodic) const
{ 
#if LBRCOMPT 
  STAT.NbPerform++;
#endif
  
  Standard_Integer dedans;
  Standard_Integer nbtabclass = TabClass.Length();
  
  if(nbtabclass==0) { 
    return(TopAbs_IN);
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

  if (RecadreOnPeriodic)
    {
      if (IsUPer)
	{
	  if (uu < Umin)
	    while (uu < Umin)
	      uu += uperiod;
	  else
	    {
	      while (uu >= Umin)
		uu -= uperiod;
	      uu += uperiod;
	    }
	}
      if (IsVPer)
	{
	  if (vv < Vmin)
	    while (vv < Vmin)
	      vv += vperiod;
	  else
	    {
	      while (vv >= Vmin)
		vv -= vperiod;
	      vv += vperiod;
	    }
	}
    }

  for (;;)
    {
      dedans = 1;
      gp_Pnt2d Puv(u,v);
      
      if(TabOrien(1)!=-1) { 
	for(Standard_Integer n=1; n<=nbtabclass; n++) { 
	  Standard_Integer cur = ((CSLib_Class2d *)TabClass(n))->SiDans(Puv);
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
	  BRepClass_FaceClassifier aClassifier;
	  Standard_Real m_Toluv = (Toluv > 4.0) ? 4.0 : Toluv;
	  //aClassifier.Perform(Face,Puv,Toluv);
	  aClassifier.Perform(Face,Puv,m_Toluv);
	  aStatus = aClassifier.State();
	}
	if(dedans == 1) { 
	  aStatus = TopAbs_IN;
	}
	if(dedans == -1) {
	  aStatus = TopAbs_OUT;
	}
      }
      else {  //-- TabOrien(1)=-1    False Wire
	BRepClass_FaceClassifier aClassifier;
	aClassifier.Perform(Face,Puv,Toluv);
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

TopAbs_State BRepTopAdaptor_FClass2d::TestOnRestriction(const gp_Pnt2d& _Puv,
							const Standard_Real Tol,
							const Standard_Boolean RecadreOnPeriodic) const
{ 
#if LBRCOMPT 
  STAT.NbConstrShape++;
#endif
  
  Standard_Integer dedans;
  Standard_Integer nbtabclass = TabClass.Length();
  
  if(nbtabclass==0) { 
    return(TopAbs_IN);
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
  
  if (RecadreOnPeriodic)
    {
      if (IsUPer)
	{
	  if (uu < Umin)
	    while (uu < Umin)
	      uu += uperiod;
	  else
	    {
	      while (uu >= Umin)
		uu -= uperiod;
	      uu += uperiod;
	    }
	}
      if (IsVPer)
	{
	  if (vv < Vmin)
	    while (vv < Vmin)
	      vv += vperiod;
	  else
	    {
	      while (vv >= Vmin)
		vv -= vperiod;
	      vv += vperiod;
	    }
	}
    }
  
  for (;;)
    {
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
      else {  //-- TabOrien(1)=-1    False Wire
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


void BRepTopAdaptor_FClass2d::Destroy() { 
#if LBRCOMPT
  STAT.NbDestroy++;
#endif
  
  Standard_Integer nbtabclass = TabClass.Length(); 
  for(Standard_Integer d=1; d<=nbtabclass;d++) {
    if(TabClass(d)) { 
      delete ((CSLib_Class2d *)TabClass(d));
      TabClass(d)=NULL;
    }
  }
}



#include <Standard_ConstructionError.hxx>


//const BRepTopAdaptor_FClass2d &  BRepTopAdaptor_FClass2d::Copy(const BRepTopAdaptor_FClass2d& Other) const { 
const BRepTopAdaptor_FClass2d &  BRepTopAdaptor_FClass2d::Copy(const BRepTopAdaptor_FClass2d& ) const { 
#ifdef OCCT_DEBUG
  cerr<<"Copy not allowed in BRepTopAdaptor_FClass2d"<<endl;
#endif
  throw Standard_ConstructionError();
}
