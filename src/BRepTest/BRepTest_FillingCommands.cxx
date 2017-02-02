// Created on: 1996-07-10
// Created by: Joelle CHAUVET
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

// Modified:	Wed Mar  5 09:45:42 1997
//    by:	Joelle CHAUVET
//              G1134 : new command "approxplate"
// Modified:	Thu Jun 12 16:51:36 1997
//    by:	Jerome LEMONIER
//              Mise a jour suite a la modification des methodes Curves2d 
//		et Sense GeomPlate_BuildPlateSurface.
// Modified:	Mon Nov  3 10:24:07 1997
//		utilisation de BRepFill_CurveConstraint



#include <GeometryTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Curve.hxx>  
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <BRep_Tool.hxx>

#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <GeomPlate_HArray1OfHCurve.hxx>

#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_PlateG1Criterion.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Geom_Surface.hxx>

#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_WireError.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>

#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApprox_PrefCutting.hxx>
#include <AdvApprox_Cutting.hxx>
#include <AdvApprox_DichoCutting.hxx>

#include <Geom_BSplineSurface.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

#include <BRepAdaptor_HCurve.hxx>
#include <Adaptor3d_HIsoCurve.hxx>

#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_Plane.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <TCollection_AsciiString.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>


#include <stdio.h>
#include <gp_Pnt.hxx>

// pour mes tests
#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
#include <Geom_Line.hxx>
#endif

const Standard_Integer defDegree = 3;
const Standard_Integer defNbPtsOnCur = 10;
const Standard_Integer defNbIter = 3;
const Standard_Boolean defAnisotropie = Standard_False;
const Standard_Real defTol2d = 0.00001;
const Standard_Real defTol3d = 0.0001;
const Standard_Real defTolAng = 0.01;
const Standard_Real defTolCurv = 0.1;
const Standard_Integer defMaxDeg = 8;
const Standard_Integer defMaxSegments = 9;

Standard_Integer Degree = defDegree;
Standard_Integer NbPtsOnCur = defNbPtsOnCur ;
Standard_Integer NbIter = defNbIter;
Standard_Boolean Anisotropie = defAnisotropie ;
Standard_Real Tol2d = defTol2d;
Standard_Real Tol3d = defTol3d;
Standard_Real TolAng = defTolAng;
Standard_Real TolCurv = defTolCurv;
Standard_Integer MaxDeg = defMaxDeg;
Standard_Integer MaxSegments = defMaxSegments;

//////////////////////////////////////////////////////////////////////////////// 
//  commande plate : resultat face sur surface plate
//////////////////////////////////////////////////////////////////////////////// 

static Standard_Integer plate (Draw_Interpretor & di,Standard_Integer n,const char** a)
{
  if (n < 8 ) return 1;
  Standard_Integer NbCurFront=Draw::Atoi(a[3]);
  Handle(GeomPlate_HArray1OfHCurve) Fronts = new GeomPlate_HArray1OfHCurve(1,NbCurFront);
  Handle(TColStd_HArray1OfInteger) Tang = new TColStd_HArray1OfInteger(1,NbCurFront);
  Handle(TColStd_HArray1OfInteger) NbPtsCur = new TColStd_HArray1OfInteger(1,NbCurFront);
  BRep_Builder B;
  
  GeomPlate_BuildPlateSurface Henri(3,15,2);

  Standard_Integer i;
  for (i=1; i<=NbCurFront ; i++) { 
    TopoDS_Shape aLocalEdge(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    TopoDS_Edge E = TopoDS::Edge(aLocalEdge);
//    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    if(E.IsNull()) return 1;
    TopoDS_Shape aLocalFace(DBRep::Get(a[3*i+2],TopAbs_FACE));
    TopoDS_Face F = TopoDS::Face(aLocalFace);
//    TopoDS_Face F = TopoDS::Face(DBRep::Get(a[3*i+2],TopAbs_FACE));
    if(F.IsNull()) return 1;
    Standard_Integer T = Draw::Atoi(a[3*i+3]);
    Tang->SetValue(i,T);
    NbPtsCur->SetValue(i,Draw::Atoi(a[2]));
    Handle(BRepAdaptor_HSurface) S = new BRepAdaptor_HSurface();
    S->ChangeSurface().Initialize(F);
    Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
    C->ChangeCurve2d().Initialize(E,F);
    Adaptor3d_CurveOnSurface ConS(C,S);
    Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
    Fronts->SetValue(i,HConS);
    Handle(GeomPlate_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(HConS,
				     Tang->Value(i),
				     NbPtsCur->Value(i));
    Henri.Add(Cont);
  }
  
  Henri.Perform();

  Standard_Real ErrG0 = 1.1*Henri.G0Error();
  //cout<<" dist. max = "<<Henri.G0Error()<<" ; angle max = "<<Henri.G1Error()<<endl;
  di<<" dist. max = "<<Henri.G0Error()<<" ; angle max = "<<Henri.G1Error()<<"\n";

  BRepBuilderAPI_MakeWire MW;
  for (i=1 ; i<=NbCurFront ; i++) {
    Standard_Integer iInOrder=Henri.Order()->Value(i);
    TopoDS_Edge E;
    if (Henri.Sense()->Value(iInOrder)==1) { 
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                          Henri.Surface(),
                          Fronts->Value(iInOrder)->LastParameter(),
                          Fronts->Value(iInOrder)->FirstParameter());
      E = ME.Edge();
      
    }
    else {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                          Henri.Surface(),
                          Fronts->Value(iInOrder)->FirstParameter(),
                          Fronts->Value(iInOrder)->LastParameter());
      E = ME.Edge();
    }
    B.UpdateVertex(TopExp::FirstVertex(E), ErrG0);
    B.UpdateVertex(TopExp::LastVertex(E), ErrG0);
    BRepLib::BuildCurve3d(E);
    char name[100];
    Sprintf(name,"Edge_%d", i);
    DBRep::Set(name, E);
    MW.Add(E);
    if (MW.IsDone()==Standard_False) {
      throw Standard_Failure("mkWire is over ");
    }
      
  }
  TopoDS_Wire W;
  W=MW.Wire();
  if (!(W.Closed())) throw Standard_Failure("Wire is not closed");
  BRepBuilderAPI_MakeFace MF(Henri.Surface(),W,Standard_True);
  DBRep::Set(a[1],MF.Face());
  return 0;
}

//////////////////////////////////////////////////////////////////////////////// 
//  commande gplate : resultat face egale a la surface approchee
//////////////////////////////////////////////////////////////////////////////// 
 
static Standard_Integer gplate (Draw_Interpretor & ,Standard_Integer n,const char** a)
{
  if (n < 6 ) return 1; 
  Standard_Integer NbCurFront=Draw::Atoi(a[2]),
  NbPointConstraint=Draw::Atoi(a[3]);
  
  GeomPlate_BuildPlateSurface Henri(3,15,2);
  
  Standard_Integer i;
  Standard_Integer Conti;
  Standard_Integer Indice=4;
//Surface d'init  
  TopoDS_Shape aLocalFace(DBRep::Get(a[Indice++],TopAbs_FACE));
  TopoDS_Face SI = TopoDS::Face(aLocalFace);
//  TopoDS_Face SI = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
  if(SI.IsNull()) 
    Indice--;
  else
    { Handle(BRepAdaptor_HSurface) HSI = new BRepAdaptor_HSurface();
      HSI->ChangeSurface().Initialize(SI);
      Henri.LoadInitSurface( BRep_Tool::Surface(HSI->ChangeSurface().Face()));
    }
  for (i=1; i<=NbCurFront ; i++) { 
    TopoDS_Shape aLocalShape(DBRep::Get(a[Indice++],TopAbs_EDGE));
    TopoDS_Edge E = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[Indice++],TopAbs_EDGE));
    if(E.IsNull()) return 1;
    Conti=Draw::Atoi(a[Indice++]);
    if ((Conti==0)||(Conti==-1))
      { Handle(BRepAdaptor_HCurve) C = new BRepAdaptor_HCurve();
	C->ChangeCurve().Initialize(E);
        const Handle(Adaptor3d_HCurve)& aC = C; // to avoid ambiguity
	Handle(GeomPlate_CurveConstraint) Cont= new BRepFill_CurveConstraint(aC,Conti);
	Henri.Add(Cont);
     }
    else 
      { 
	aLocalFace = DBRep::Get(a[Indice++],TopAbs_FACE);
	TopoDS_Face F = TopoDS::Face(aLocalFace);
//	TopoDS_Face F = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
	if(F.IsNull()) 
	  return 1;
	Handle(BRepAdaptor_HSurface) S = new BRepAdaptor_HSurface();
	S->ChangeSurface().Initialize(F);
	Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
	C->ChangeCurve2d().Initialize(E,F);
	Adaptor3d_CurveOnSurface ConS(C,S);
	Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
	Handle(GeomPlate_CurveConstraint) Cont= new BRepFill_CurveConstraint(HConS,Conti);
	Henri.Add(Cont);
      }
  }
  
  for (i=1; i<=NbPointConstraint ; i++) 
    { 
//      gp_Pnt P1,P2,P3;
      gp_Pnt P1;
//      gp_Vec V1,V2,V3,V4,V5;

      if (DrawTrSurf::GetPoint(a[Indice], P1) ) 
	{ Conti=0;
	  Handle(GeomPlate_PointConstraint) PCont= new GeomPlate_PointConstraint(P1,0);
	  Henri.Add(PCont);
	  Indice++;
	}
      else
	{ Standard_Real u=Draw::Atof(a[Indice++]), 
	                v=Draw::Atof(a[Indice++]);

	  Conti=Draw::Atoi(a[Indice++]);
	  aLocalFace = DBRep::Get(a[Indice++],TopAbs_FACE);
	  TopoDS_Face F = TopoDS::Face(aLocalFace);
//	  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[Indice++],TopAbs_FACE));
	  if(F.IsNull()) 
	    return 1;	
	  Handle(BRepAdaptor_HSurface) HF = new BRepAdaptor_HSurface();
	  HF->ChangeSurface().Initialize(F);
	  Handle(GeomPlate_PointConstraint) PCont= new GeomPlate_PointConstraint(u,v,BRep_Tool::Surface(HF->ChangeSurface().Face()),Conti,0.001,0.001,0.001);
	  Henri.Add(PCont);
	}
    }    
  Henri.Perform();
  Standard_Integer nbcarreau=9;
  Standard_Integer degmax=8;
  Standard_Real seuil;

  Handle(GeomPlate_Surface) gpPlate = Henri.Surface();
  TColgp_SequenceOfXY S2d;
  TColgp_SequenceOfXYZ S3d;
  S2d.Clear();
  S3d.Clear();
  Henri.Disc2dContour(4,S2d);
  Henri.Disc3dContour(4,0,S3d);
  seuil = Max(0.0001,10*Henri.G0Error());
  GeomPlate_PlateG0Criterion critere (S2d,S3d,seuil);
  GeomPlate_MakeApprox Mapp(gpPlate,critere,0.0001,nbcarreau,degmax);
  Handle (Geom_Surface) Surf (Mapp.Surface());

  Standard_Real Umin, Umax, Vmin, Vmax;
  
  Henri.Surface()->Bounds( Umin, Umax, Vmin, Vmax);
  
  BRepBuilderAPI_MakeFace MF(Surf, Umin, Umax, Vmin, Vmax, Precision::Confusion());
  
  DBRep::Set(a[1],MF.Face());
  return 0;
}


//////////////////////////////////////////////////////////////////////////////// 
//  commande approxplate : resultat face sur surface approchee
//////////////////////////////////////////////////////////////////////////////// 

static Standard_Integer approxplate (Draw_Interpretor & di,Standard_Integer n,const char** a)
{
  if (n < 9 ) return 1;
  Standard_Integer NbMedium=Draw::Atoi(a[2]);
  Standard_Integer NbCurFront=Draw::Atoi(a[3]);
  Handle(GeomPlate_HArray1OfHCurve) Fronts = new GeomPlate_HArray1OfHCurve(1,NbCurFront);
  Handle(TColStd_HArray1OfInteger) Tang = new TColStd_HArray1OfInteger(1,NbCurFront);
  Handle(TColStd_HArray1OfInteger) NbPtsCur = new TColStd_HArray1OfInteger(1,NbCurFront);
  
  GeomPlate_BuildPlateSurface Henri(3,15,2);

  Standard_Integer i;
  for (i=1; i<=NbCurFront ; i++) { 
    TopoDS_Shape aLocalShape(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    TopoDS_Edge E = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[3*i+1],TopAbs_EDGE));
    if(E.IsNull()) return 1;
    TopoDS_Shape aLocalFace(DBRep::Get(a[3*i+2],TopAbs_FACE));
    TopoDS_Face F = TopoDS::Face(aLocalFace);
//    TopoDS_Face F = TopoDS::Face(DBRep::Get(a[3*i+2],TopAbs_FACE));
    if(F.IsNull()) return 1;
    Standard_Integer T = Draw::Atoi(a[3*i+3]);
    Tang->SetValue(i,T);
    NbPtsCur->SetValue(i,NbMedium);
    Handle(BRepAdaptor_HSurface) S = new BRepAdaptor_HSurface();
    S->ChangeSurface().Initialize(F);
    Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
    C->ChangeCurve2d().Initialize(E,F);
    Adaptor3d_CurveOnSurface ConS(C,S);
    Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
    Fronts->SetValue(i,HConS);
    Handle(GeomPlate_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(HConS,
				     Tang->Value(i),
				     NbPtsCur->Value(i));
    Henri.Add(Cont);
  }
  
  Henri.Perform();

  Standard_Real dmax = Henri.G0Error(),
                anmax = Henri.G1Error();
  //cout<<" dist. max = "<<dmax<<" ; angle max = "<<anmax<<endl;
  di<<" dist. max = "<<dmax<<" ; angle max = "<<anmax<<"\n";

  Tol3d = Draw::Atof(a[3*NbCurFront+4]);
  Standard_Integer Nbmax = Draw::Atoi(a[3*NbCurFront+5]);
  Standard_Integer degmax = Draw::Atoi(a[3*NbCurFront+6]);
  Standard_Integer CritOrder = Draw::Atoi(a[3*NbCurFront+7]);
  Handle(GeomPlate_Surface) surf = Henri.Surface();
  Handle(Geom_BSplineSurface) support;

  if (CritOrder==-1) {
    GeomPlate_MakeApprox MApp(surf,Tol3d,Nbmax,degmax,dmax,-1);
    support = MApp.Surface();
  }
  else if (CritOrder>=0) {
    TColgp_SequenceOfXY S2d;
    TColgp_SequenceOfXYZ S3d;
    S2d.Clear();
    S3d.Clear();
    Standard_Real seuil;
    if (CritOrder==0) {
      Henri.Disc2dContour(4,S2d);
      Henri.Disc3dContour(4,0,S3d);
      seuil = Max(Tol3d,dmax*10);
      GeomPlate_PlateG0Criterion Crit0(S2d,S3d,seuil);
      GeomPlate_MakeApprox MApp(surf,Crit0,Tol3d,Nbmax,degmax);
      support = MApp.Surface();
    }
    else if (CritOrder==1) {
      Henri.Disc2dContour(4,S2d);
      Henri.Disc3dContour(4,1,S3d);
      seuil = Max(Tol3d,anmax*10);
      GeomPlate_PlateG1Criterion Crit1(S2d,S3d,seuil);
      GeomPlate_MakeApprox MApp(surf,Crit1,Tol3d,Nbmax,degmax);
      support = MApp.Surface();
    }
  }

  BRepBuilderAPI_MakeWire MW;
  BRep_Builder B;
  for (i=1 ; i<=NbCurFront ; i++) {
    Standard_Integer iInOrder=Henri.Order()->Value(i);
    TopoDS_Edge E;
    if (Henri.Sense()->Value(iInOrder)==1) { 
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                          support,
                          Fronts->Value(iInOrder)->LastParameter(),
                          Fronts->Value(iInOrder)->FirstParameter());
      E = ME.Edge();
    }
    else {
      BRepBuilderAPI_MakeEdge ME(Henri.Curves2d()->Value(iInOrder),
                          support,
                          Fronts->Value(iInOrder)->FirstParameter(),
                          Fronts->Value(iInOrder)->LastParameter());
      E = ME.Edge();
    }
    B.UpdateVertex(TopExp::FirstVertex(E), dmax);
    B.UpdateVertex(TopExp::LastVertex(E), dmax);
    BRepLib::BuildCurve3d(E);
    MW.Add(E);
    if (MW.IsDone()==Standard_False) {
      throw Standard_Failure("mkWire is over ");
    }
  }
  TopoDS_Wire W;
  W=MW.Wire();
  if (!(W.Closed())) throw Standard_Failure("Wire is not closed");
  BRepBuilderAPI_MakeFace MF(support,W,Standard_True);
  DBRep::Set(a[1],MF.Face());

  return 0;
}

static Standard_Integer filling( Draw_Interpretor & di, Standard_Integer n, const char** a )
{
#ifdef OCCT_DEBUG
  // Chronmetrage
  OSD_Chronometer Chrono;
  Chrono.Reset();
  Chrono.Start();
#endif

  if (n < 7) return 1;
  Standard_Integer NbBounds = Draw::Atoi( a[2] );
  Standard_Integer NbConstraints = Draw::Atoi( a[3] );
  Standard_Integer NbPoints = Draw::Atoi( a[4] );

  BRepOffsetAPI_MakeFilling MakeFilling( Degree,
				   NbPtsOnCur,
				   NbIter,
				   Anisotropie,
				   Tol2d,
				   Tol3d,
				   TolAng,
				   TolCurv,
				   MaxDeg,
				   MaxSegments );
  //TopoDS_Shape aLocalFace(DBRep::Get( a[5], TopAbs_FACE ) );
  //TopoDS_Face InitFace = TopoDS::Face( aLocalFace);
  TopoDS_Face InitFace = TopoDS::Face( DBRep::Get(a[5], TopAbs_FACE) );
  if (! InitFace.IsNull())
    MakeFilling.LoadInitSurface( InitFace );
  
  Standard_Integer i = (InitFace.IsNull())? 5 : 6, k;
  TopoDS_Edge E;
  TopoDS_Face F;
  gp_Pnt Point;
  Standard_Integer Order;
  for (k = 1; k <= NbBounds; k++)
    { 
      E.Nullify();
      F.Nullify();
      //TopoDS_Shape aLocalEdge(DBRep::Get( a[i], TopAbs_EDGE ));
      //E = TopoDS::Edge(aLocalEdge);
      E = TopoDS::Edge( DBRep::Get(a[i], TopAbs_EDGE) );
      if (! E.IsNull())
	i++;
      //aLocalFace =  DBRep::Get( a[i], TopAbs_FACE ) ;
      //F = TopoDS::Face(aLocalFace);
      F = TopoDS::Face( DBRep::Get(a[i], TopAbs_FACE) );
      if (! F.IsNull())
	i++;

      Order = Draw::Atoi( a[i++] );
      
      if (! E.IsNull() && ! F.IsNull())
	MakeFilling.Add( E, F, (GeomAbs_Shape)Order );
      else if (E.IsNull())
	{
	  if (F.IsNull())
	    {
	      //cout<<endl<<"Wrong parameters"<<endl<<endl;
	      di<<"\nWrong parameters\n\n";
	      return 1;
	    }
	  else
	    MakeFilling.Add( F, (GeomAbs_Shape)Order );
	}
      else
	MakeFilling.Add( E, (GeomAbs_Shape)Order );
    }
  for (k = 1; k <= NbConstraints; k++)
    { 
      E.Nullify();
      F.Nullify();
      //TopoDS_Shape aLocalEdge(DBRep::Get( a[i++], TopAbs_EDGE ));
      //E = TopoDS::Edge( aLocalEdge);
      E = TopoDS::Edge( DBRep::Get(a[i++], TopAbs_EDGE) );
      if (E.IsNull())
	{
	  //cout<<"Wrong parameters"<<endl;
	  di<<"Wrong parameters\n";
	  return 1;
	}
      //TopoDS_Shape alocalFace(DBRep::Get( a[i], TopAbs_FACE ) );
      //F = TopoDS::Face( alocalFace);
      F = TopoDS::Face( DBRep::Get(a[i], TopAbs_FACE) );
      if (! F.IsNull())
	i++;
      
      Order = Draw::Atoi( a[i++] );
      
      if (F.IsNull())
	MakeFilling.Add( E, (GeomAbs_Shape)Order, Standard_False );
      else
	MakeFilling.Add( E, F, (GeomAbs_Shape)Order, Standard_False );
    }
  for (k = 1; k <= NbPoints; k++)
    {
      if (DrawTrSurf::GetPoint( a[i], Point )) 
	{
	  MakeFilling.Add( Point );
	  i++;
	}
      else
	{
	  Standard_Real U = Draw::Atof( a[i++] ), V = Draw::Atof( a[i++] );
	  //aLocalFace = DBRep::Get( a[i++], TopAbs_FACE );
	  //F = TopoDS::Face( aLocalFace);
	  F = TopoDS::Face( DBRep::Get(a[i++], TopAbs_FACE));
	  if (F.IsNull()) 
	    {
	      //cout<<"Wrong parameters"<<endl;
	      di<<"Wrong parameters\n";
	      return 1;
	    }
	  Order = Draw::Atoi( a[i++] );

	  MakeFilling.Add( U, V, F, (GeomAbs_Shape)Order );
	}
    }

  MakeFilling.Build();
  if (! MakeFilling.IsDone())
    {
      //cout<<"filling failed"<<endl;
      di<<"filling failed\n";
      return 0;
    }

  Standard_Real dmax = MakeFilling.G0Error(),
                angmax = MakeFilling.G1Error(),
                curvmax = MakeFilling.G2Error();
  //cout<<" dist. max = "<<dmax<<" ; angle max = "<<angmax<<" ; diffcurv max = "<<curvmax<<endl;
  di<<" dist. max = "<<dmax<<" ; angle max = "<<angmax<<" ; diffcurv max = "<<curvmax<<"\n";

  TopoDS_Face ResFace= TopoDS::Face( MakeFilling.Shape() );
  DBRep::Set( a[1], ResFace );

#ifdef OCCT_DEBUG
  Chrono.Stop();
  Standard_Real Tps;
  Chrono.Show(Tps);
  //cout<<"*** FIN DE FILLING ***"<<endl;
  //cout<<"Temps de calcul  : "<<Tps<<endl;
  di<<"*** FIN DE FILLING ***\n";
  di<<"Temps de calcul  : "<<Tps<<"\n";
#endif

  return 0;
}

static Standard_Integer fillingparam( Draw_Interpretor & di, Standard_Integer n, const char** a )
{
  if ( n == 1) {

    //cout << "fillingparam : options are"  <<endl;
    //cout << "-l : to list current values" << endl;
    //cout << "-i : to set default values"   << endl;
    //cout << "-r deg nbPonC nbIt anis : to set filling options" <<endl;
    //cout << "-c t2d t3d tang tcur : to set tolerances" << endl;
    //cout << "-a maxdeg maxseg : Approximation option" << endl;
    di << "fillingparam : options are"  <<"\n";
    di << "-l : to list current values\n";
    di << "-i : to set default values"   << "\n";
    di << "-r deg nbPonC nbIt anis : to set filling options\n";
    di << "-c t2d t3d tang tcur : to set tolerances\n";
    di << "-a maxdeg maxseg : Approximation option\n";
  }
  else if (n > 1)
    {
      TCollection_AsciiString AS( a[1] );
      AS.LowerCase();
      const char* flag = AS.ToCString();
      if (strcmp( flag, "-l" ) == 0 && n == 2)
	{
	  //cout<<endl;
	  //cout<<"Degree = "<<Degree<<endl;
	  //cout<<"NbPtsOnCur = "<<NbPtsOnCur<<endl;
	  //cout<<"NbIter = "<<NbIter<<endl;
	  //cout<<"Anisotropie = "<<Anisotropie<<endl<<endl;
	  //
	  //cout<<"Tol2d = "<<Tol2d<<endl;
	  //cout<<"Tol3d = "<<Tol3d<<endl;
	  //cout<<"TolAng = "<<TolAng<<endl;
	  //cout<<"TolCurv = "<<TolCurv<<endl<<endl;
	  //
	  //cout<<"MaxDeg = "<<MaxDeg<<endl;
	  //cout<<"MaxSegments = "<<MaxSegments<<endl<<endl;
	  di<<"\n";
	  di<<"Degree = "<<Degree<<"\n";
	  di<<"NbPtsOnCur = "<<NbPtsOnCur<<"\n";
	  di<<"NbIter = "<<NbIter<<"\n";
	  di<<"Anisotropie = "<< (Standard_Integer) Anisotropie<<"\n\n";
	  
	  di<<"Tol2d = "<<Tol2d<<"\n";
	  di<<"Tol3d = "<<Tol3d<<"\n";
	  di<<"TolAng = "<<TolAng<<"\n";
	  di<<"TolCurv = "<<TolCurv<<"\n\n";
	  
	  di<<"MaxDeg = "<<MaxDeg<<"\n";
	  di<<"MaxSegments = "<<MaxSegments<<"\n\n";
	}
      else if (strcmp( flag, "-i" ) == 0 && n == 2)
	{
	  Degree = defDegree;
	  NbPtsOnCur = defNbPtsOnCur;
	  NbIter = defNbIter;
	  Anisotropie = defAnisotropie;
	  
	  Tol2d = defTol2d;
	  Tol3d = defTol3d;
	  TolAng = defTolAng;
	  TolCurv = defTolCurv;
	  
	  MaxDeg = defMaxDeg;
	  MaxSegments = defMaxSegments;
	}
      else if (strcmp( flag, "-r" ) == 0 && n == 6)
	{
	  Degree      = Draw::Atoi( a[2] );
	  NbPtsOnCur  = Draw::Atoi( a[3] );
	  NbIter      = Draw::Atoi( a[4] );
	  Anisotropie = Draw::Atoi( a[5] ) != 0;
	}
      else if (strcmp( flag, "-c" ) == 0 && n == 6)
	{
	  Tol2d   = Draw::Atof( a[2] ); 
	  Tol3d   = Draw::Atof( a[3] );
	  TolAng  = Draw::Atof( a[4] );
	  TolCurv = Draw::Atof( a[5] );
	}
      else if (strcmp( flag, "-a" ) == 0 && n == 4)
	{
	  MaxDeg      = Draw::Atoi( a[2] );
	  MaxSegments = Draw::Atoi( a[3] );
	}
      else
	{
	  //cout<<"Wrong parameters"<<endl;
	  di<<"Wrong parameters\n";
	  return 1;
	}
    }
  return 0;
}




void  BRepTest::FillingCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);
  GeometryTest::SurfaceCommands(theCommands);

  const char* g = "Surface filling topology commands";
  theCommands.Add("plate",
		  "plate result nbrpntoncurve nbrcurfront edge face tang (0:vif;1:tang) ...",
		  __FILE__,
		  plate,
		  g) ;

  theCommands.Add("gplate",
		  "gplate result nbrcurfront nbrpntconst [SurfInit] [edge 0] [edge tang (1:G1;2:G2) surf]... [point] [u v tang (1:G1;2:G2) surf] ...",
		  __FILE__,
		  gplate,
		  g) ;

  theCommands.Add("approxplate",
		  "approxplate result nbrpntoncurve nbrcurfront edge face tang (0:vif;1:tang) ... tol nmax degmax crit",
		  __FILE__,
		  approxplate,
		  g) ;



  theCommands.Add("filling",
		  "filling result nbB nbC nbP [SurfInit] [edge][face]order... edge[face]order... point/u v face order...",
		  __FILE__,
		  filling,
		  g) ;

  theCommands.Add("fillingparam",
		  "fillingparam : no arg give help",
		  __FILE__,
		  fillingparam,
		  g) ;

}
