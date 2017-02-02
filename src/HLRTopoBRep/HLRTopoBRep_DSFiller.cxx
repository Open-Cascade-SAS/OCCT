// Created on: 1993-08-10
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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


#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepApprox_Approx.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Contap_Contour.hxx>
#include <Contap_Line.hxx>
#include <Contap_Point.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomProjLib.hxx>
#include <HLRTopoBRep_Data.hxx>
#include <HLRTopoBRep_DSFiller.hxx>
#include <HLRTopoBRep_FaceIsoLiner.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : Insert
//purpose  : explore the faces and insert them
//=======================================================================

void  HLRTopoBRep_DSFiller::Insert (const TopoDS_Shape& S,
				    Contap_Contour& FO,
				    HLRTopoBRep_Data& DS,
				    BRepTopAdaptor_MapOfShapeTool& MST,
				    const Standard_Integer nbIso)
{
  TopTools_MapOfShape ShapeMap;
  TopExp_Explorer ex(S,TopAbs_FACE);
  DS.Clear();
  Standard_Boolean withPCurve = Standard_True; // instead of nbIso != 0;
  Standard_Integer f = 0;
  
  while (ex.More()) {
    if (ShapeMap.Add(ex.Current())) {
      f++;
      TopoDS_Face S1 = TopoDS::Face(ex.Current());
      S1.Orientation(TopAbs_FORWARD);
      Handle(BRepTopAdaptor_TopolTool) Domain; 
      Handle(Adaptor3d_HSurface)         Surface;
      if(MST.IsBound(S1)) {  
	BRepTopAdaptor_Tool& BRT = MST.ChangeFind(S1);
	Domain  = BRT.GetTopolTool();
	Surface = BRT.GetSurface();
      }
      else { 
	BRepTopAdaptor_Tool BRT(S1,Precision::PConfusion());
	MST.Bind(S1,BRT);
	Domain  = BRT.GetTopolTool();
	Surface = BRT.GetSurface();
      }
      FO.Perform(Surface, Domain);
      if (FO.IsDone()) {
	if (!FO.IsEmpty())
	  InsertFace(f,S1,FO,DS,withPCurve);
      }
      if (nbIso != 0) HLRTopoBRep_FaceIsoLiner::Perform(f,S1,DS,nbIso);
    }
    ex.Next();
  }
  ProcessEdges(DS);
}

//=======================================================================
//function : InsertFace
//purpose  : private, insert the outlines of a face
//=======================================================================

void  HLRTopoBRep_DSFiller::InsertFace (const Standard_Integer /*FI*/,
					const TopoDS_Face& F,
					Contap_Contour& FO,
					HLRTopoBRep_Data& DS,
					const Standard_Boolean withPCurve)
{
  // Insert the intersections of FO in DS

  const Standard_Real tol = BRep_Tool::Tolerance(F);
  TopTools_ListOfShape& IntL = DS.AddIntL(F);
  TopTools_ListOfShape& OutL = DS.AddOutL(F);

  TopoDS_Vertex VF,VL;
  /*
  TopTools_MapOfShape VM;
  TopExp_Explorer ex(F,TopAbs_EDGE);
  while (ex.More()) {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    if (BRep_Tool::IsClosed(E,F)) {
      TopExp::Vertices(E,VF,VL);
      VM.Add(VF);
      VM.Add(VL);
    }
    ex.Next();
  }
  */

  const Standard_Integer NbLines = FO.NbLines();
  Standard_Integer CurLine;
  for (CurLine = 1; CurLine <= NbLines; CurLine++)
  {
    const Contap_Line& Line = FO.Line(CurLine);
    const Standard_Integer NbPoints = Line.NbVertex();
    Standard_Integer CurPoint;
    if (Line.TypeContour() == Contap_Restriction)
    {
      // OutLine on restriction
      TopoDS_Edge E = (*(BRepAdaptor_Curve2d*)&(Line.Arc()->Curve2d())).Edge();
      OutL.Append(E);
      TopExp::Vertices(E,VF,VL);
      // insert the Internal points.

      for (CurPoint = 1; CurPoint <= NbPoints; CurPoint++) {
        Contap_Point P = Line.Vertex(CurPoint);
        if (P.IsInternal()) {
          if (P.Value().IsEqual(BRep_Tool::Pnt(VF),BRep_Tool::Tolerance(VF))) {
            if (P.Value().IsEqual(BRep_Tool::Pnt(VL),BRep_Tool::Tolerance(VL))) {
              InsertVertex(P,tol,E,DS);
            }
          }
        }
      }
    }
    else
    {
      for (CurPoint = 1; CurPoint <= NbPoints; CurPoint++) {

        const Contap_Point PF = Line.Vertex(CurPoint);
        if (PF.IsInternal() && CurPoint != 1)
          VF = VL;
        else
          VF = MakeVertex(PF,tol,DS);
        const Standard_Real parF = PF.ParameterOnLine();

        if (CurPoint < NbPoints) {
          const Contap_Point PL = Line.Vertex(CurPoint+1);
          VL = MakeVertex(PL,tol,DS);
          const Standard_Real parL = PL.ParameterOnLine();

          if( (parL-parF) > Precision::PConfusion() ) {

            Handle(Geom_Curve) C;
            Handle(Geom2d_Curve) C2d;
            Standard_Real first = parF;
            Standard_Real last  = parL;
            Standard_Boolean InsuffisantNumberOfPoints=Standard_False;

            switch (Line.TypeContour()) {
	      
              case Contap_Lin :
              {
                C = new Geom_Line(Line.Line());
                if (withPCurve) {
                  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
                  Standard_Real Tol = 1e-7;
                  C2d = GeomProjLib::Curve2d(C,first,last,S,Tol); 
                }
              }
              break;
	      
              case Contap_Circle :
              {
                C = new Geom_Circle(Line.Circle());
                if (withPCurve) {
                  TopLoc_Location Loc;
                  Handle(Geom_Surface) S = BRep_Tool::Surface(F,Loc);
                  if (!Loc.IsIdentity()) {
                    S = Handle(Geom_Surface)::DownCast(S->Transformed(Loc.Transformation()));
                  }
                  Standard_Real Tol = 1e-7;
                  C2d = GeomProjLib::Curve2d(C,first,last,S,Tol); 
                }
              }
              break;
	      
              case Contap_Walking :
              {
                // copy the points
                Standard_Integer ipF = Standard_Integer(parF);
                Standard_Integer ipL = Standard_Integer(parL);
		
                if(ipL-ipF < 1) { 
                  InsuffisantNumberOfPoints=Standard_True;
                  //cout<<"\n !! Pb ds HLRTopoBRep_DSFiller.cxx (Contour App Nbp <3)"<<endl;
                }
/*
		else if(ipL-ipF < 6) { 
		  // compute the tangents
		  Contap_SurfFunction& SFunc =
		    FO.SurfaceFunction();
		  
		  Standard_Boolean isTg1,isTg2;
		  gp_Vec tg1,tg2;
		  gp_Vec2d uv1,uv2;
		  math_Vector UV(1,2),F(1,1);
		  
		  Line.Point(ipF).ParametersOnS2(UV(1),UV(2));
		  SFunc.Value(UV,F);
		  isTg1 = SFunc.IsTangent();
		  if (!isTg1) {
		    tg1 = SFunc.Direction3d();
		    if (withPCurve) uv1 = SFunc.Direction2d();
		  }
		  
		  Line.Point(ipL).ParametersOnS2(UV(1),UV(2));
		  SFunc.Value(UV,F);
		  isTg2 = SFunc.IsTangent();
		  if (!isTg2) {
		    tg2 = SFunc.Direction3d();
		    if (withPCurve) uv2 = SFunc.Direction2d();
		  }
		  // interpolate
		  Standard_Integer nbp = ipL - ipF + 1;
		  AppDef_MultiLine MLine(nbp);
		  Standard_Integer nb2d = 0;
		  if (withPCurve)  nb2d = 1;
		  
		  for (Standard_Integer i = 1; i <= nbp; i++) {
		    AppDef_MultiPointConstraint MP(1, nb2d);
		    MP.SetPoint(1,Line.Point(i + ipF - 1).Value());
		    if (withPCurve) {
		      Line.Point(i + ipF - 1).ParametersOnS2(UV(1),UV(2));
		      MP.SetPoint2d(2,gp_Pnt2d(UV(1),UV(2)));
		    }
		    
		    if (i == 1   && !isTg1) {
		      MP.SetTang  (1,tg1);
		      if (withPCurve) MP.SetTang2d(2,uv1);
		    }
		    if (i == nbp && !isTg2) {
		      MP.SetTang  (1,tg2);
		      if (withPCurve) MP.SetTang2d(2,uv2);
		    }
		    MLine.SetValue(i,MP);
		  }
		  AppDef_BSplineCompute interp;
		  interp.Interpol(MLine);
		  AppParCurves_MultiBSpCurve TheCurve = interp.Value();
		  Standard_Integer Degree = TheCurve.Degree();
		  TColgp_Array1OfPnt   Poles(1,TheCurve.NbPoles());
		  TheCurve.Curve(1,Poles);
		  C   = new Geom_BSplineCurve(Poles, 
					      TheCurve.Knots(),
					      TheCurve.Multiplicities(),
					      Degree);
		  if (withPCurve) {
		    TColgp_Array1OfPnt2d Pol2d(1,TheCurve.NbPoles());
		    TheCurve.Curve(2,Pol2d);
		    C2d = new Geom2d_BSplineCurve(Pol2d, 
						  TheCurve.Knots(),
						  TheCurve.Multiplicities(),
						  Degree);
		  }
		  first = 0;
		  last = 1;
		}
*/
                else if(ipL-ipF < 5) { 
                  const Standard_Integer nbp = ipL - ipF + 1;
                  TColStd_Array1OfReal knots(1,nbp);
                  TColStd_Array1OfInteger mults(1,nbp);
                  TColgp_Array1OfPnt Points(1,nbp);

                  for(Standard_Integer i=1;i<=nbp;i++) {
                    knots.SetValue(i,(Standard_Real)i);
                    mults.SetValue(i,1);
                    Points.SetValue(i,Line.Point(i+ipF-1).Value());
                  }
                  mults(1)=mults(nbp)=2;
                  C = new Geom_BSplineCurve(Points,knots,mults,1);
		  
                  if(withPCurve) { 
                    TColgp_Array1OfPnt2d Points2d(1,nbp);
                    for(Standard_Integer i=1;i<=nbp;i++) {
                      Standard_Real u,v;
                      Line.Point(i+ipF-1).ParametersOnS2(u,v);
                      Points2d.SetValue(i,gp_Pnt2d(u,v));
                    }
                    C2d = new Geom2d_BSplineCurve(Points2d,knots,mults,1);
                  }
                  first = 1;
                  last = nbp;
                }
                else { 
                  const Standard_Integer nbp = ipL - ipF + 1;
                  TColStd_Array1OfReal knots(1,nbp);
                  TColStd_Array1OfInteger mults(1,nbp);
                  TColgp_Array1OfPnt Points(1,nbp);

                  Standard_Real Maxx,Maxy,Maxz,Maxu,Maxv;
                  Standard_Real Minx,Miny,Minz,Minu,Minv;
                  Maxx=Maxy=Maxz=Maxu=Maxv=-RealLast();
                  Minx=Miny=Minz=Minu=Minv=RealLast();
		  
                  for(Standard_Integer i=1;i<=nbp;i++) {
                    knots.SetValue(i,(Standard_Real)i);
                    mults.SetValue(i,1);
                    const gp_Pnt& P= Line.Point(i+ipF-1).Value();
                    if(P.X()<Minx) Minx=P.X();
                    if(P.Y()<Miny) Miny=P.Y();
                    if(P.Z()<Minz) Minz=P.Z();
                    if(P.X()>Maxx) Maxx=P.X();
                    if(P.Y()>Maxy) Maxy=P.Y();
                    if(P.Z()>Maxz) Maxz=P.Z();		    
                    Points.SetValue(i,P);
                  }
                  mults(1)=mults(nbp)=2;
                  Handle(Geom_BSplineCurve)   AppC;
                  Handle(Geom2d_BSplineCurve) AppC2d;
                  AppC = new Geom_BSplineCurve(Points,knots,mults,1);

                  if(withPCurve) {
                    TColgp_Array1OfPnt2d Points2d(1,nbp);
                    for(Standard_Integer i=1;i<=nbp;i++) {
                      Standard_Real u,v;
                      Line.Point(i+ipF-1).ParametersOnS2(u,v);
                      if(u<Minu) Minu=u;
                      if(v<Minv) Minv=v;
                      if(u>Maxu) Maxu=u;
                      if(v>Maxv) Maxv=v;
                      Points2d.SetValue(i,gp_Pnt2d(u,v));
                    }
                    AppC2d = new Geom2d_BSplineCurve(Points2d,knots,mults,1);
                  }
                  first = 1;
                  last = nbp;
		  
                  Handle(BRepApprox_ApproxLine) AppLine;
                  Handle(Geom2d_BSplineCurve) CNull;
                  AppLine = new BRepApprox_ApproxLine(AppC,AppC2d,CNull);
		  
                  Standard_Integer dmin=4,dmax=8,niter=0;
                  Standard_Boolean tg= Standard_False;
                  BRepApprox_Approx Approx;
                  Standard_Real TOL3d,TOL2d,TOL=0.0001;

                  Maxx-=Minx; Maxy-=Miny; Maxz-=Minz;
                  Maxu-=Minu; Maxv-=Minv;
                  if(Maxy>Maxx) Maxx=Maxy;
                  if(Maxz>Maxx) Maxx=Maxy;
                  if(Maxv>Maxu) Maxu=Maxv;

                  TOL3d=TOL*Maxx; if(TOL3d<1e-12) TOL3d=1e-12; else if(TOL3d>0.1) TOL3d=0.1;
                  TOL2d=TOL*Maxu; if(TOL2d<1e-12) TOL2d=1e-12; else if(TOL2d>0.1) TOL2d=0.1;
		  
                  //-- cout<<"\nHLRTopoBRep_DSFiller : nbp="<<nbp<<"  Tol3d="<<TOL3d<<"   Tol2d="<<TOL2d<<endl;

                  Approx.SetParameters(TOL3d, TOL2d, dmin, dmax, niter, 30, tg);
                  Approx.Perform(AppLine,Standard_True,Standard_True,Standard_False,1,nbp);
                  if (!Approx.IsDone()) {
                    C = AppC;
                    C2d=AppC2d; 
                    first = 1;
                    last = nbp;
                  }
                  else { 
                    const AppParCurves_MultiBSpCurve& AppVal = Approx.Value(1); 
                    TColgp_Array1OfPnt poles3d(1,AppVal.NbPoles());
                    AppVal.Curve(1,poles3d);
                    C = new Geom_BSplineCurve(poles3d,AppVal.Knots(),AppVal.Multiplicities(),AppVal.Degree());
		    
                    const AppParCurves_MultiBSpCurve& AppVal2 = Approx.Value(2);
                    TColgp_Array1OfPnt2d poles2d(1,AppVal2.NbPoles());
                    AppVal2.Curve(2,poles2d);
                    C2d = new Geom2d_BSplineCurve(poles2d,AppVal2.Knots(),AppVal2.Multiplicities(),AppVal2.Degree());
                    first = C2d->FirstParameter();
                    last  = C2d->LastParameter();
                  }
                }
              }
              break;
	      
              case Contap_Restriction :
              {
                throw Standard_ProgramError("HLRTopoBRep_DSFiller::InsertFace : Restriction");
              }
              break;
            }
	    
            // compute the PCurve
            // make the edge
            if (!InsuffisantNumberOfPoints) {
              TopoDS_Edge E;
              BRep_Builder B;
              B.MakeEdge(E,C,tol);
              VF.Orientation(TopAbs_FORWARD);
              VL.Orientation(TopAbs_REVERSED);
              B.Add(E,VF);
              B.Add(E,VL);
              B.Range(E,first,last);

              if (!C2d.IsNull()) {
                B.UpdateEdge(E,C2d,F,BRep_Tool::Tolerance(F));
              }
	      
              // add the edge in the DS
              if (!E.IsNull())
                IntL.Append(E);
            }
          }
        }
      }
    }
  }

  //Correction of internal outlines: unite coinciding vertices
  const Standard_Real SqTol = tol*tol;
  TopTools_ListIteratorOfListOfShape itl1(IntL);
  for (; itl1.More(); itl1.Next())
  {
    TopoDS_Edge anIntLine = TopoDS::Edge(itl1.Value());
    anIntLine.Orientation(TopAbs_FORWARD);
    TopoDS_Vertex aVer [2];
    TopExp::Vertices(anIntLine, aVer[0], aVer[1]);
    TopTools_ListIteratorOfListOfShape itl2 = itl1;
    for (; itl2.More(); itl2.Next())
    {
      TopoDS_Edge anIntLine2 = TopoDS::Edge(itl2.Value());
      anIntLine2.Orientation(TopAbs_FORWARD);
      if (anIntLine2.IsSame(anIntLine))
        continue;
      TopoDS_Vertex aVer2 [2];
      TopExp::Vertices(anIntLine2, aVer2[0], aVer2[1]);
      for (Standard_Integer i = 0; i < 2; i++)
      {
        if (i == 1 && aVer[0].IsSame(aVer[1]))
          continue;
        gp_Pnt Pnt1 = BRep_Tool::Pnt(aVer[i]);
        for (Standard_Integer j = 0; j < 2; j++)
        {
          if (aVer[i].IsSame(aVer2[j]))
            continue;
          gp_Pnt Pnt2 = BRep_Tool::Pnt(aVer2[j]);
          if (Pnt1.SquareDistance(Pnt2) <= SqTol)
          {
            BRep_Builder aBB;
            aBB.Remove(anIntLine2, aVer2[j]);
            aVer[i].Orientation((j==0)? TopAbs_FORWARD : TopAbs_REVERSED);
            aBB.Add(anIntLine2, aVer[i]);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : MakeVertex
//purpose  : private, make a vertex from an intersection point
//=======================================================================

TopoDS_Vertex
HLRTopoBRep_DSFiller::MakeVertex (const Contap_Point& P,
				  const Standard_Real tol,
				  HLRTopoBRep_Data& DS)
{
  BRep_Builder B;
  TopoDS_Vertex V;
  if (P.IsVertex()) {
    V = Handle(BRepTopAdaptor_HVertex)::DownCast(P.Vertex())->Vertex();
    DS.AddOutV(V);
  }
  else {
    // if on arc, insert in the DS
    if (P.IsOnArc()) {
      const TopoDS_Edge& E = 
	(*(BRepAdaptor_Curve2d*)&((P.Arc())->Curve2d())).Edge();
      Standard_Real Par = P.ParameterOnArc();
      const gp_Pnt& P3d = P.Value();

      for (DS.InitVertex(E); DS.MoreVertex(); DS.NextVertex()) {
	TopoDS_Vertex curV = DS.Vertex();
	Standard_Real curP = DS.Parameter();
	const gp_Pnt& PPP=BRep_Tool::Pnt(curV);
	Standard_Real TTT=BRep_Tool::Tolerance(curV);
	if (P3d.IsEqual(PPP,TTT)) { 
	  V = curV;
	  break;
	}
	else if (Par < curP) {
	  B.MakeVertex(V,P.Value(),tol);
	  DS.InsertBefore(V,Par);
	  break;
	}
      }
      if (!DS.MoreVertex()) {
	B.MakeVertex(V,P.Value(),tol);
	DS.Append(V,Par);
      }	
      DS.AddOutV(V);
    }
    // if internal create a vertex and insert in the DS
    else {
      B.MakeVertex(V,P.Value(),tol);
      if (P.IsInternal())
	DS.AddIntV(V);
      else
	DS.AddOutV(V);
    }
  }
  return V;
}

//=======================================================================
//function : InsertVertex
//purpose  : private, insert a vertex from an internal intersection point
//           on resctriction
//=======================================================================

void
HLRTopoBRep_DSFiller::InsertVertex (const Contap_Point& P,
				    const Standard_Real tol,
				    const TopoDS_Edge& E,
				    HLRTopoBRep_Data& DS)
{
  BRep_Builder B;
  TopoDS_Vertex V;

  if (P.IsVertex()) {
    V = Handle(BRepTopAdaptor_HVertex)::DownCast(P.Vertex())->Vertex();
  }
  else {
    Standard_Real Par = P.ParameterOnLine();
    
    for (DS.InitVertex(E); DS.MoreVertex(); DS.NextVertex()) {
      TopoDS_Vertex curV = DS.Vertex();
      Standard_Real curP = DS.Parameter();
      if (P.Value().IsEqual(BRep_Tool::Pnt(curV),
			    BRep_Tool::Tolerance(curV))) {
	V = curV;
	break;
      }
      else if (Par < curP) {
	B.MakeVertex(V,P.Value(),tol);
	DS.InsertBefore(V,Par);
	break;
      }
    }
    if (!DS.MoreVertex()) {
      B.MakeVertex(V,P.Value(),tol);
      DS.Append(V,Par);
    }	
  }
  DS.AddIntV(V);
}

//=======================================================================
//function : ProcessEdges
//purpose  : private, split edges with outline vertices
//=======================================================================

void  HLRTopoBRep_DSFiller::ProcessEdges (HLRTopoBRep_Data& DS)
{
  BRep_Builder B;
  TopoDS_Edge newE;
  TopoDS_Vertex VF,VL,VI;
  Standard_Real PF,PL,PI;

  for (DS.InitEdge(); DS.MoreEdge(); DS.NextEdge()) {
    TopoDS_Edge E = DS.Edge();
    TopTools_ListOfShape& SplE = DS.AddSplE(E);
    VF = TopExp::FirstVertex(E);
    VL = TopExp::LastVertex(E);
    BRep_Tool::Range(E,PF,PL);
    VF.Orientation(TopAbs_FORWARD);
    VL.Orientation(TopAbs_REVERSED);

    for (DS.InitVertex(E); DS.MoreVertex(); DS.NextVertex()) {
      VI = DS.Vertex();
      PI = DS.Parameter();
      VI.Orientation(TopAbs_REVERSED);
      newE = E;
      newE.EmptyCopy();
      newE.Orientation(TopAbs_FORWARD);
      B.Add(newE,VF);
      B.UpdateVertex(VF,PF,newE,BRep_Tool::Tolerance(VF));
      B.Add(newE,VI);
      B.UpdateVertex(VI,PI,newE,BRep_Tool::Tolerance(VI));
      newE.Orientation(E.Orientation());
      SplE.Append(newE);
      VF = VI;
      PF = PI;
      VF.Orientation(TopAbs_FORWARD);
    }
    newE = E;
    newE.EmptyCopy();
    newE.Orientation(TopAbs_FORWARD);
    B.Add(newE,VF);
    B.UpdateVertex(VF,PF,newE,BRep_Tool::Tolerance(VF));
    B.Add(newE,VL);
    B.UpdateVertex(VL,PL,newE,BRep_Tool::Tolerance(VL));
    newE.Orientation(E.Orientation());
    SplE.Append(newE);
  }
}

