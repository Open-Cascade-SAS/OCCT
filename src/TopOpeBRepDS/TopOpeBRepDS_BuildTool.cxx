// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

// includes especially needed by the static Project function
#ifdef DRAW
#include <TopOpeBRepDS_DRAW.hxx>
#include <Geom2d_Curve.hxx>
#endif

Standard_EXPORT Handle(Geom2d_Curve) BASISCURVE2D(const Handle(Geom2d_Curve)& C);

Standard_Boolean FUN_UisoLineOnSphe
(const TopoDS_Shape& F,
 const Handle(Geom2d_Curve)& PC)
{  
  if (PC.IsNull()) return Standard_False;

  Handle(Geom_Surface) SSS = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  Handle(Geom2d_Curve) LLL = ::BASISCURVE2D(PC);
  Handle(Standard_Type) TS = SSS->DynamicType();
  Handle(Standard_Type) T2 = LLL->DynamicType();
  Standard_Boolean issphere = (TS == STANDARD_TYPE(Geom_SphericalSurface)); 
  Standard_Boolean isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  Standard_Boolean isisoU = Standard_False;
  if (issphere && isline2d) {
    Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(LLL);
    const gp_Dir2d& d = L->Direction();
    isisoU = (Abs(d.X()) < Precision::Parametric(Precision::Confusion()));
  }
  return isisoU;
}

//=======================================================================
//function : TopOpeBRepDS_BuildTool
//purpose  : 
//=======================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool():
myCurveTool(TopOpeBRepTool_APPROX),
myOverWrite(Standard_True),
myTranslate(Standard_True)
{
}

//=======================================================================
//function : TopOpeBRepDS_BuildTool
//purpose  : 
//=======================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool
  (const TopOpeBRepTool_OutCurveType O) : 
myCurveTool(O),
myOverWrite(Standard_True),
myTranslate(Standard_True)
{
}

//=======================================================================
//function : TopOpeBRepDS_BuildTool
//purpose  : 
//=======================================================================

TopOpeBRepDS_BuildTool::TopOpeBRepDS_BuildTool
  (const TopOpeBRepTool_GeomTool& GT) : 
myCurveTool(GT),
myOverWrite(Standard_True),
myTranslate(Standard_True)
{
}

Standard_Boolean TopOpeBRepDS_BuildTool::OverWrite()const
{
  return myOverWrite;
}

void TopOpeBRepDS_BuildTool::OverWrite(const Standard_Boolean O)
{
  myOverWrite = O;
}

Standard_Boolean TopOpeBRepDS_BuildTool::Translate()const
{
  return myTranslate;
}

void TopOpeBRepDS_BuildTool::Translate(const Standard_Boolean T)
{
  myTranslate = T;
}

//=======================================================================
//function : GetGeomTool
//purpose  : 
//=======================================================================

const TopOpeBRepTool_GeomTool& TopOpeBRepDS_BuildTool::GetGeomTool() const 
{
  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  return GT;
}

//=======================================================================
//function : ChangeGeomTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_GeomTool& TopOpeBRepDS_BuildTool::ChangeGeomTool()
{
  TopOpeBRepTool_GeomTool& GT = myCurveTool.ChangeGeomTool();
  return GT;
}

//=======================================================================
//function : MakeVertex
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeVertex(TopoDS_Shape& V, 
					 const TopOpeBRepDS_Point& P)const 
{
  myBuilder.MakeVertex(TopoDS::Vertex(V),P.Point(),P.Tolerance());
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape& E, 
				       const TopOpeBRepDS_Curve& C)const 
{
  // Gestion des courbes nulles pour carreaux pointus
  // RLE 28-6-94

  if (C.Curve().IsNull()) {
    myBuilder.MakeEdge(TopoDS::Edge(E));
    myBuilder.Degenerated(TopoDS::Edge(E),Standard_True);
    return;
  }

  const Handle(Geom_Curve)& GC = C.Curve();
  myBuilder.MakeEdge(TopoDS::Edge(E),GC,C.Tolerance());

  Standard_Boolean addorigin = Standard_False;
  Standard_Boolean setrange  = Standard_False;

  if (addorigin) {
    if ( GC->IsClosed() ) {
      // in case of a closed curve, insert in E a vertex located at the origin
      // of the curve C.
      TopoDS_Vertex V;
      Standard_Real first = GC->FirstParameter();
      gp_Pnt P = GC->Value(first);
      myBuilder.MakeVertex(V,P,C.Tolerance());
      myBuilder.Add(E,V);
      V.Reverse();
      myBuilder.Add(E,V);
      
      // If the curve is a degree 1 bspline set the range to 1 .. NbPoles
      Handle(Geom_BSplineCurve) BSC = Handle(Geom_BSplineCurve)::DownCast(GC);
      if (!BSC.IsNull()) {
	if (BSC->Degree() == 1) {
	  myBuilder.Range(TopoDS::Edge(E),1,BSC->NbPoles());
	}
      }
    }
  }

  if (setrange) {
    Standard_Real first,last;
    Standard_Boolean rangedef = C.Range(first,last);
    if (rangedef) {
      Range(E,first,last);
    }
  }
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeEdge
(TopoDS_Shape& E, 
 const TopOpeBRepDS_Curve& C,
 const TopOpeBRepDS_DataStructure& BDS) const
{
  // Gestion des courbes nulles pour carreaux pointus
  // RLE 28-6-94

  TopoDS_Edge& EE = TopoDS::Edge(E);

  if (C.Curve().IsNull()) {
    myBuilder.MakeEdge(EE);
    myBuilder.Degenerated(EE,Standard_True);
    
    // Creation d'une arete avec PCurve connectee a la BDS Curve 
    // JYL 22-09-94
    Handle(TopOpeBRepDS_Interference) I = C.GetSCI1();
    Handle(TopOpeBRepDS_SurfaceCurveInterference) SCI;
    SCI=Handle(TopOpeBRepDS_SurfaceCurveInterference)::DownCast(I);
    Standard_Integer iS = SCI->Support();
    const TopOpeBRepDS_Surface& DSS = BDS.Surface(iS);
    const Handle(Geom_Surface)& GS  = DSS.Surface();
    const Handle(Geom2d_Curve)& PC  = SCI->PCurve();
    myBuilder.UpdateEdge(EE,PC,GS,TopLoc_Location(),DSS.Tolerance());
    return;
  }
  else {
    const Handle(Geom_Curve)& GC = C.Curve();
    myBuilder.MakeEdge(EE,GC,C.Tolerance());
  }

}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeEdge
(TopoDS_Shape& E, 
 const Handle(Geom_Curve)& C,
 const Standard_Real Tol)const 
{
  myBuilder.MakeEdge(TopoDS::Edge(E),C,Tol);
}


//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeEdge(TopoDS_Shape& E) const
{
  myBuilder.MakeEdge(TopoDS::Edge(E));
}


//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeWire(TopoDS_Shape& W)const 
{
  myBuilder.MakeWire(TopoDS::Wire(W));
}


//=======================================================================
//function : MakeFace
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeFace(TopoDS_Shape& F, 
				       const TopOpeBRepDS_Surface& S)const 
{
  myBuilder.MakeFace(TopoDS::Face(F),S.Surface(),S.Tolerance());
}


//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeShell(TopoDS_Shape& Sh)const 
{
  myBuilder.MakeShell(TopoDS::Shell(Sh));
}


//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::MakeSolid(TopoDS_Shape& S)const 
{
  myBuilder.MakeSolid(TopoDS::Solid(S));
}


//=======================================================================
//function : CopyEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::CopyEdge(const TopoDS_Shape& Ein, 
				       TopoDS_Shape& Eou)const 
{

  // Splendide evolution de BRep_Curve3D::BRep_Curve3D(Geom_Curve,Location)
  // apres modification de la primitive Sphere pour parametrisation de 
  // l'arete meridienne en -pi/2,+pi/2.
  // Ein est l'arete de couture complete d'une sphere complete
  // BRep_Tool::Range(Ein) --> -pi/2,+pi/2
  // BRep_Tool::Range(Ein.EmptyCopied()) --> 0,2pi
  // NYI reflexion sur la notion de Range d'une arete et de la geometrie
  // NYI sous jacente dans le cas ou, par construction, les vertex d'une
  // NYI arete on des valeurs de parametre HORS des bornes [first,last] de la
  // NYI courbe 3D support de l'arete (cas de l'arete de couture d'une sphere)
  // On redefinit desormais le range de l'arete Eou, a la place de se 
  // contenter du simplissime Eou = Ein.EmptyCopied();
  // merci les amis : correction bug PRO2586

  Standard_Real f,l;
  TopoDS_Edge E1 = TopoDS::Edge(Ein); 
  BRep_Tool::Range(E1,f,l);
  Eou = Ein.EmptyCopied();
  TopoDS_Edge E2 = TopoDS::Edge(Eou); 
  myBuilder.Range(E2,f,l);
}


//=======================================================================
//function : GetOrientedEdgeVertices
//purpose  : 
//=======================================================================

void TopOpeBRepDS_BuildTool::GetOrientedEdgeVertices
(TopoDS_Edge& E,
 TopoDS_Vertex& Vmin, TopoDS_Vertex& Vmax,
 Standard_Real& Parmin, Standard_Real& Parmax) const
{
  if ( E.Orientation() == TopAbs_FORWARD) 
    TopExp::Vertices(E,Vmin,Vmax);
  else
    TopExp::Vertices(E,Vmax,Vmin);
  if ( !Vmin.IsNull() && !Vmax.IsNull()) {  
    Parmin = BRep_Tool::Parameter(Vmin,E);
    Parmax = BRep_Tool::Parameter(Vmax,E);
  }
}

//=======================================================================
//function : UpdateEdgeCurveTol
//purpose  : 
//=======================================================================

void TopOpeBRepDS_BuildTool::UpdateEdgeCurveTol
//(const TopoDS_Face& F1,const TopoDS_Face& F2,
(const TopoDS_Face& ,const TopoDS_Face& ,
 TopoDS_Edge& E, const Handle(Geom_Curve)& C3Dnew,
// const Standard_Real tol3d,
 const Standard_Real ,
// const Standard_Real tol2d1,
 const Standard_Real ,
// const Standard_Real tol2d2,
 const Standard_Real ,
 Standard_Real& newtol,
 Standard_Real& newparmin,
 Standard_Real& newparmax) const

{
  if (C3Dnew.IsNull()) return;
  BRep_Builder BB;

  // newtol = max des tolerances atteintes en 3d
// JMB le 06 Juillet 1999
// les valeurs tol3d et tol2d1,tol2d2 proviennent des approx. Dans la version 2.0 de CasCade, 
// elles n'etaient pas calculees et on renvoyait systematiquement les valeurs initiales (a savoir)
// 1.E-7. Dans la version 2.1 de CasCade, ces valeurs sont desormais calculees selon un calcul
// d'erreur dans les Approx. Malheureusement, il apparait que ce calcul d'erreur renvoit dans la
// plupart des cas de tres grosses valeurs (parfois de l'ordre de 1.E-1). Ce qui amenait la topologie
// a coder des tolerances enormes dans les pieces resultats rendant celles-ci inexpoitables.
// De plus on essayait de rafiner la tolerance en appelant les UResolution sur les surfaces support.
// sur des surfaces tres particulieres, ce UREsolution n'a plus aucun sens et peut amener a des valeurs
// abberantes.
// On decide donc de laisser la tolerance de l'edge telle qu'elle est afin d'avoir un comportement similaire
// a 2.0. Jusqu'a present on a constate que des problemes avec la methode de calcul d'erreur des approx.


  newtol = 1.E-7;
//  Standard_Real r1,r2;
//  r1 = TopOpeBRepTool_ShapeTool::Resolution3d(F1,tol2d1);
//  r2 = TopOpeBRepTool_ShapeTool::Resolution3d(F2,tol2d2);
//  newtol=tol3d;
//  if (r1>newtol) newtol=r1;
//  if (r2>newtol) newtol=r2;

//  newtol *= 1.5;

  TopoDS_Vertex Vmin,Vmax; Standard_Real parmin,parmax;
  GetOrientedEdgeVertices(E,Vmin,Vmax,parmin,parmax);
  
  Standard_Real tolmin=BRep_Tool::Tolerance(Vmin);
  if(newtol>tolmin) tolmin=newtol;
  Standard_Real tolmax=BRep_Tool::Tolerance(Vmax);
  if(newtol>tolmax) tolmax=newtol;


//  newparmin=C3Dnew->FirstParameter(); // -merge 04-07-97
//  newparmax=C3Dnew->LastParameter(); // -merge 04-07-97

 // +merge 04-07-97
  Handle(Geom_TrimmedCurve) GTC = Handle(Geom_TrimmedCurve)::DownCast(C3Dnew);
  if(GTC.IsNull()) {
    Handle(Geom_BSplineCurve) GBSC = Handle(Geom_BSplineCurve)::DownCast(C3Dnew);
    if(GBSC.IsNull()) {
      newparmin = parmin;
      newparmax = parmax;
    } else {
      newparmin=C3Dnew->FirstParameter();
      newparmax=C3Dnew->LastParameter();
    } 
  } else {
    newparmin=C3Dnew->FirstParameter();
    newparmax=C3Dnew->LastParameter();
  } // +merge 04-07-97

  if (Vmin.Orientation() == TopAbs_FORWARD) { 
    BB.UpdateVertex(Vmin,newparmin,E,tolmin);
    BB.UpdateVertex(Vmax,newparmax,E,tolmax);
  }
  else {
    BB.UpdateVertex(Vmin,newparmax,E,tolmin);
    BB.UpdateVertex(Vmax,newparmin,E,tolmax);
  }
  
//  DSBT.Curve3D(E,C3Dnew,newtol);  // -merge 04-07-97
  Curve3D(E,C3Dnew,newtol);
  
  // projection des vertex INTERNAL de E pour parametrage
  // sur la nouvelle courbe C3Dnew de l'arete E
  TopExp_Explorer exi(E,TopAbs_VERTEX);
  for (;exi.More(); exi.Next() ) {
    const TopoDS_Vertex& vi = TopoDS::Vertex(exi.Current());
    if ( vi.Orientation() != TopAbs_INTERNAL ) continue;
    gp_Pnt P = BRep_Tool::Pnt(vi);
    Standard_Real tolvi=TopOpeBRepTool_ShapeTool::Tolerance(vi);
    GeomAPI_ProjectPointOnCurve dm(P,C3Dnew,newparmin,newparmax);
    Standard_Boolean dmdone = dm.Extrema().IsDone();
    if ( dmdone ) {
      if ( dm.NbPoints() ) {
	Standard_Real newpar = dm.LowerDistanceParameter();
	BB.UpdateVertex(vi,newpar,E,tolvi);
      }
    }
  } // INTERNAL vertex
}

//=======================================================================
//function : ApproxCurves
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::ApproxCurves
(const TopOpeBRepDS_Curve& C,
 TopoDS_Edge& E,
 Standard_Integer& inewC,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  TopOpeBRepDS_Curve newC1;
  inewC = HDS->MakeCurve(C,newC1);
  TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);

  // C1 curves have been approximated by BSplines of degree 1 :
  // compute new geometry on curves.

  const TopoDS_Face& F1 = TopoDS::Face(newC.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(newC.Shape2());
  
  const Handle(Geom_Curve)& C3D = C.Curve();
  const Handle(Geom2d_Curve)& PC1 = C.Curve1();
  const Handle(Geom2d_Curve)& PC2 = C.Curve2();

  // Vmin,Vmax = bounding vertices of edge <E>
  // and their parameters parmin,parmax .

  TopoDS_Vertex Vmin,Vmax;Standard_Real parmin,parmax;
  GetOrientedEdgeVertices(E,Vmin,Vmax,parmin,parmax);

  Handle(Geom_Curve)   C3Dnew;
  Handle(Geom2d_Curve) PC1new;
  Handle(Geom2d_Curve) PC2new;
  Standard_Real tolreached3d,tolreached2d;
  
  Standard_Boolean approxMade = myCurveTool.MakeCurves(parmin,parmax,
			 C3D,PC1,PC2,F1,F2,
			 C3Dnew,PC1new,PC2new,
			 tolreached3d,tolreached2d);

  Standard_Real newtol,newparmin,newparmax;
  // MSV Nov 12, 2001: if approx failed than leave old curves of degree 1
  if (!approxMade) {
    newtol = BRep_Tool::Tolerance(E);
    newparmin = parmin;
    newparmax = parmax;
    C3Dnew = C3D;
    PC1new = PC1;
    PC2new = PC2;
  }
  else {
    UpdateEdgeCurveTol
      (F1,F2,E,C3Dnew,tolreached3d,tolreached2d,tolreached2d,
       newtol,newparmin,newparmax);
  }

  if (!C3Dnew.IsNull()) {
    newC.DefineCurve(C3Dnew,newtol,Standard_False);
    newC.SetRange(newparmin,newparmax);
  }

  if (!PC1new.IsNull()) newC.Curve1(PC1new);
  if (!PC2new.IsNull()) newC.Curve2(PC2new);
}


//=======================================================================
//function : ComputePCurves
//purpose  : 
//=======================================================================
Standard_Boolean FUN_getUV
(const Handle(Geom_Surface) surf,
 const Handle(Geom_Curve) C3D,
 const Standard_Real par3d,
 Standard_Real& u0,
 Standard_Real& v0)
{
  gp_Pnt P3d; C3D->D0(par3d,P3d);
  GeomAPI_ProjectPointOnSurf pons(P3d,surf);
  if (pons.NbPoints() < 1) return Standard_False;
  pons.LowerDistanceParameters(u0,v0);
  return Standard_True;
}

Standard_Boolean FUN_reversePC
(Handle(Geom2d_Curve) PCnew,
 const TopoDS_Face& F,
 const gp_Pnt& P3DC3D,
 const Standard_Real par2d,
 const Standard_Real tol)
{
  gp_Pnt2d P2D; PCnew->D0(par2d,P2D);
  BRepAdaptor_Surface BAS(F,Standard_False);
  gp_Pnt P3D = BAS.Value(P2D.X(),P2D.Y());
  Standard_Boolean PCreversed = Standard_False;
  Standard_Boolean sam = P3D.IsEqual(P3DC3D,tol);
  PCreversed = !sam;

  if ( PCreversed ) {
    Handle(Geom2d_Curve) PC = ::BASISCURVE2D(PCnew);
    if (!PC.IsNull()) {
      Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(PC);
      gp_Dir2d d = L->Direction();
      d.Reverse();
      L->SetDirection(d);
    }
  }
  return PCreversed;
}
Standard_Boolean FUN_makeUisoLineOnSphe
(const TopoDS_Face& F, // with geometry the spherical surface
 const Handle(Geom_Curve) C3D, 
 Handle(Geom2d_Curve) PCnew,
 const Standard_Real tol3d)
{
  // p3df,p3dl : C3d first and last parameters
  Standard_Real p3df = C3D->FirstParameter();
  Standard_Real p3dl = C3D->LastParameter();

  // u0,v0 : C3d(par3d) UV parameters
  Standard_Real deltainf = 0.243234, deltasup = 0.543345;
  Standard_Real par3dinf = (1-deltainf)*p3df + deltainf*p3dl;
  Standard_Real par3dsup = (1-deltasup)*p3df + deltasup*p3dl;
  Standard_Real uinf,vinf,usup,vsup;
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
  if (!FUN_getUV(surf,C3D,par3dinf,uinf,vinf)) return Standard_False;
  if (!FUN_getUV(surf,C3D,par3dsup,usup,vsup)) return Standard_False;
  Standard_Real tol = Precision::Parametric(tol3d);
  if (Abs(uinf-usup) > tol) return Standard_False;

  Standard_Boolean isvgrowing = (vsup - vinf > -tol);
  gp_Dir2d vdir;
  if (isvgrowing) vdir = gp_Dir2d(0,1);
  else            vdir = gp_Dir2d(0,-1);
  
  gp_Pnt2d origin(uinf,vinf);
  origin.Translate(gp_Vec2d(vdir).Scaled(p3df-par3dinf));
  Handle(Geom2d_Curve) PC = ::BASISCURVE2D(PCnew);
  if (!PC.IsNull()) {  
    Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(PC); 
    L->SetLin2d(gp_Lin2d(origin,vdir));
  } // (!PC.IsNull())
  
  return Standard_True;
}

void TopOpeBRepDS_BuildTool::ComputePCurves
(const TopOpeBRepDS_Curve& C,
 TopoDS_Edge& E,
 TopOpeBRepDS_Curve& newC,
 const Standard_Boolean comppc1,
 const Standard_Boolean comppc2,
 const Standard_Boolean compc3d) const
{
  const TopoDS_Face& F1 = TopoDS::Face(newC.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(newC.Shape2());
  
  const Handle(Geom_Curve)& C3D = C.Curve();

  // get bounding vertices Vmin,Vmax supported by the new edge <E>
  // and their corresponding parameters parmin,parmax .

  TopoDS_Vertex Vmin,Vmax;Standard_Real parmin,parmax;
  GetOrientedEdgeVertices(E,Vmin,Vmax,parmin,parmax);

  Handle(Geom2d_Curve) PC1new;
  Handle(Geom2d_Curve) PC2new;
  
  if(C3D.IsNull()) {
    Standard_Real tolreached2d1 = Precision::Confusion(), tolreached2d2 = Precision::Confusion(), r1, r2, tol=Precision::Confusion();
    if (comppc1) PC1new = myCurveTool.MakePCurveOnFace(F1,C3D,tolreached2d1);
    if (comppc2) PC2new = myCurveTool.MakePCurveOnFace(F2,C3D,tolreached2d2);
    
    r1 = TopOpeBRepTool_ShapeTool::Resolution3d(F1,tolreached2d1);
    r2 = TopOpeBRepTool_ShapeTool::Resolution3d(F2,tolreached2d2);
    tol = Max(tol,r1);
    tol = Max(tol,r2);
    newC.Tolerance(tol);
    
    if (!PC1new.IsNull()) newC.Curve1(PC1new);
    if (!PC2new.IsNull()) newC.Curve2(PC2new);

    return;
  }
  
  Handle(Geom_Curve) C3Dnew = C3D;
  
  if ( C3D->IsPeriodic() ) {
    // ellipse on cone : periodize parmin,parmax
    Standard_Real period = C3D->LastParameter() - C3D->FirstParameter();
    Standard_Real f,l;
    if (Vmin.Orientation() == TopAbs_FORWARD) { f = parmin; l = parmax; }
    else {                                      f = parmax; l = parmin; }
    parmin = f; parmax = l;
    ElCLib::AdjustPeriodic(f,f+period,Precision::PConfusion(),parmin,parmax);
    if (compc3d) C3Dnew = new Geom_TrimmedCurve(C3D,parmin,parmax);
    
  }
  
  Standard_Real tolreached3d = C.Tolerance();
  Standard_Real tolreached2d1 = C.Tolerance();
  Standard_Real tolreached2d2 = C.Tolerance();
  
  if (comppc1) PC1new = myCurveTool.MakePCurveOnFace(F1,C3Dnew,tolreached2d1);
  if (comppc2) PC2new = myCurveTool.MakePCurveOnFace(F2,C3Dnew,tolreached2d2);
  
  Standard_Real newtol,newparmin,newparmax;
  UpdateEdgeCurveTol(F1,F2,E,C3Dnew,tolreached3d,tolreached2d1,tolreached2d2,
		     newtol,newparmin,newparmax);
  
    // xpu : suite merge : 07-07-97
    // xpu : 17-06-97
    // Rmq : C1.Curve<i>() ne sert plus qu'a determiner si la courbe
    //       est une isos de la sphere
    // NYI : enlever FUN_reversePC
    Standard_Boolean UisoLineOnSphe1 = Standard_False;
    UisoLineOnSphe1 = ::FUN_UisoLineOnSphe(F1,PC1new);
    if (UisoLineOnSphe1) ::FUN_makeUisoLineOnSphe(F1,C3Dnew,PC1new,newtol);

    Standard_Boolean UisoLineOnSphe2 = Standard_False;
    UisoLineOnSphe2 = ::FUN_UisoLineOnSphe(F2,PC2new);
    if (UisoLineOnSphe2) ::FUN_makeUisoLineOnSphe(F2,C3Dnew,PC2new,newtol);
    // xpu : 17-06-97
    // xpu : suite merge : 07-07-97

  if (!C3Dnew.IsNull()) {
    newC.Curve(C3Dnew,newtol);
    newC.SetRange(newparmin, newparmax);
  }
  if (!PC1new.IsNull()) newC.Curve1(PC1new);
  if (!PC2new.IsNull()) newC.Curve2(PC2new);
}

//=======================================================================
//function : PutPCurves
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::PutPCurves
(const TopOpeBRepDS_Curve& newC,
 TopoDS_Edge& E,
 const Standard_Boolean comppc1,
 const Standard_Boolean comppc2) const
{

  TopoDS_Face& F1 = *((TopoDS_Face*)(void*)&(TopoDS::Face(newC.Shape1())));
  Handle(Geom2d_Curve) PC1 = newC.Curve1();
  if (!PC1.IsNull() && comppc1) {
    PCurve(F1,E,PC1);
  }
  
  TopoDS_Face& F2 = *((TopoDS_Face*)(void*)&(TopoDS::Face(newC.Shape2())));
  Handle(Geom2d_Curve) PC2 = newC.Curve2();
  if (!PC2.IsNull() && comppc2) {
    PCurve(F2,E,PC2);
  }
  
}

//=======================================================================
//function : RecomputeCurves
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::RecomputeCurves
(const TopOpeBRepDS_Curve& C,
// const TopoDS_Edge& oldE,
 const TopoDS_Edge& ,
 TopoDS_Edge& E,
 Standard_Integer& inewC,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  const Standard_Boolean compc3d = GT.CompC3D();
  const Standard_Boolean comppc1 = GT.CompPC1();
  const Standard_Boolean comppc2 = GT.CompPC2();
  const Standard_Boolean comppc = comppc1 || comppc2;
  const Standard_Boolean iswalk = C.IsWalk();
  const Standard_Boolean approx = Approximation();
  
  const Handle(Geom_Curve)& C3D = C.Curve();
  if (comppc1 && C.Shape1().IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 2");
  if (comppc2 && C.Shape2().IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 3");
  TopoDS_Vertex Vmin,Vmax; TopExp::Vertices(E,Vmin,Vmax);
  if ( Vmin.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 4");
  if ( Vmax.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 5");
  
  if (iswalk && approx) {
    if (compc3d && C3D.IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 1");
    ApproxCurves(C, E, inewC, HDS);
    TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
    PutPCurves(newC, E, comppc1, comppc2);
  }
//  else if (iswalk && interpol) {
//    InterpolCurves(C, E, inewC, comppc1, comppc2, HDS);
//    TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
//    PutPCurves(newC, E, comppc1, comppc2);
//  }

  else {
    if (comppc) {
      TopOpeBRepDS_Curve newC1;
      inewC = HDS->MakeCurve(C,newC1);
      TopOpeBRepDS_Curve& newC = HDS->ChangeCurve(inewC);
      if(iswalk && !approx) {
	if (compc3d && C3D.IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 1");
	newC.Curve1(C.Curve1());
	newC.Curve2(C.Curve2());
      }
      else
	ComputePCurves(C, E, newC, comppc1, comppc2, compc3d);	
      PutPCurves(newC, E, comppc1, comppc2);
    }
  }
}

//=======================================================================
//function : CopyFace
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::CopyFace(const TopoDS_Shape& Fin, 
				       TopoDS_Shape& Fou)const 
{
  Fou = Fin.EmptyCopied();
}


//=======================================================================
//function : AddEdgeVertex
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddEdgeVertex(const TopoDS_Shape& Ein, 
					    TopoDS_Shape& Eou, 
					    const TopoDS_Shape& V)const 
{
  myBuilder.Add(Eou,V);
  TopoDS_Edge e1 = TopoDS::Edge(Ein);
  TopoDS_Edge e2 = TopoDS::Edge(Eou);
  TopoDS_Vertex v1 = TopoDS::Vertex(V);
  myBuilder.Transfert(e1,e2,v1,v1);
}


//=======================================================================
//function : AddEdgeVertex
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddEdgeVertex(TopoDS_Shape& E, 
					    const TopoDS_Shape& V)const 
{
  myBuilder.Add(E,V);
}


//=======================================================================
//function : AddWireEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddWireEdge(TopoDS_Shape& W, 
					  const TopoDS_Shape& E)const 
{
  myBuilder.Add(W,E);
}


//=======================================================================
//function : AddFaceWire
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddFaceWire(TopoDS_Shape& F, 
					  const TopoDS_Shape& W)const 
{
  myBuilder.Add(F,W);
}


//=======================================================================
//function : AddShellFace
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddShellFace(TopoDS_Shape& Sh, 
					   const TopoDS_Shape& F)const 
{
  myBuilder.Add(Sh,F);
}


//=======================================================================
//function : AddSolidShell
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::AddSolidShell(TopoDS_Shape& S, 
					    const TopoDS_Shape& Sh)const 
{
  myBuilder.Add(S,Sh);
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Parameter(const TopoDS_Shape& E, 
					const TopoDS_Shape& V, 
					const Standard_Real P)const 
{
  const TopoDS_Edge&   e = TopoDS::Edge(E);
  const TopoDS_Vertex& v = TopoDS::Vertex(V);
  Standard_Real p = P;

  // 13/07/95 : 
  TopLoc_Location loc; Standard_Real f,l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(e,loc,f,l);
  if ( !C.IsNull() && C->IsPeriodic()) {
    Standard_Real per = C->Period();

    TopAbs_Orientation oV=TopAbs_FORWARD;

    TopExp_Explorer exV(e,TopAbs_VERTEX);
    for (; exV.More(); exV.Next()) {
      const TopoDS_Vertex& vofe = TopoDS::Vertex(exV.Current());
      if ( vofe.IsSame(v) ) {
	oV = vofe.Orientation();
	break;
      }
    }
    if ( exV.More() ) {
      if ( oV == TopAbs_REVERSED ) {
	if ( p < f ) {
	  Standard_Real pp = ElCLib::InPeriod(p,f,f+per);
	  p = pp;
	}
      }
    }
  }

  myBuilder.UpdateVertex(v,p,e,
			 0);   // NYI : Tol on new vertex ?? 
}

//=======================================================================
//function : Range
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Range(const TopoDS_Shape& E, 
				    const Standard_Real first,
				    const Standard_Real last)const 
{
  myBuilder.Range(TopoDS::Edge(E),first,last);
}


//=======================================================================
//function : UpdateEdge
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::UpdateEdge(const TopoDS_Shape& Ein,
					 TopoDS_Shape& Eou)const
{
  TopLoc_Location loc;
  Standard_Real f1,l1;
  Standard_Real f2,l2;
  Handle(Geom_Curve) Cin = BRep_Tool::Curve(TopoDS::Edge(Ein),loc,f1,l1);
  Handle(Geom_Curve) Cou = BRep_Tool::Curve(TopoDS::Edge(Eou),loc,f2,l2);
  if (Cin.IsNull() || Cou.IsNull()) return;
  
  if ( Cou->IsPeriodic() ) {
    Standard_Real f2n = f2, l2n = l2;
    if ( l2n <= f2n ) {
      ElCLib::AdjustPeriodic(f1,l1,Precision::PConfusion(),f2n,l2n);
      Range(Eou,f2n,l2n);
    }
  }
}

//=======================================================================
//function : Project
//purpose  : project a vertex on a curve
//=======================================================================

static Standard_Boolean Project(const Handle(Geom_Curve)& C,
				const TopoDS_Vertex& V,
				Standard_Real& p)
{
  gp_Pnt P = BRep_Tool::Pnt(V);
  Standard_Real tol = BRep_Tool::Tolerance(V);
  GeomAdaptor_Curve GAC(C);
  Extrema_ExtPC extrema(P,GAC);
  if (extrema.IsDone()) {
    Standard_Integer i,n = extrema.NbExt();
    for (i = 1; i <= n; i++) {
      if (extrema.IsMin(i)) {
	Extrema_POnCurv EPOC = extrema.Point(i);
	if (P.Distance(EPOC.Value()) <= tol) {
	  p = EPOC.Parameter();
	  return Standard_True;
	}
      }
    }
  }
  return Standard_False;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Parameter(const TopOpeBRepDS_Curve& C,
					TopoDS_Shape& E,
					TopoDS_Shape& V)const 
{
  Standard_Real newparam;
  Project(C.Curve(),TopoDS::Vertex(V),newparam);
  Parameter(E,V,newparam);
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Curve3D
(TopoDS_Shape& E, 
 const Handle(Geom_Curve)& C,
 const Standard_Real Tol)const 
{
  myBuilder.UpdateEdge(TopoDS::Edge(E),
		       C,
		       Tol);
}


//=======================================================================
//function : TranslateOnPeriodic
//purpose  : 
//=======================================================================

void TopOpeBRepDS_BuildTool::TranslateOnPeriodic
  (TopoDS_Shape& F,
   TopoDS_Shape& E,
   Handle(Geom2d_Curve)& PC) const
{
  // get range C3Df,C3Dl of 3d curve C3D of E
  TopLoc_Location L; 
  Standard_Real C3Df,C3Dl;
//  Handle(Geom_Curve) C3D = BRep_Tool::Curve(TopoDS::Edge(E),L,C3Df,C3Dl);
  Handle(Geom_Curve) C3D = BRep_Tool::Curve(TopoDS::Edge(E),C3Df,C3Dl); // 13-07-97: xpu
 
  Standard_Real first = C3Df, last = C3Dl;
  if (C3D->IsPeriodic()) {
    if ( last < first ) last += Abs(first - last);
  }

  // jyl-xpu : 13-06-97 : 
  // if <PC> is U isoline on sphere, a special parametrization
  // is to provide, we compute <PC> (which is a line) bounds 
  // with C3D bounds.
  Standard_Boolean UisoLineOnSphe = FUN_UisoLineOnSphe(F,PC);
  Standard_Boolean newv = Standard_True;

  Standard_Real du,dv;

  gp_Pnt2d ptest;
  Standard_Real t =(first+last)*.5;
  PC->D0(t,ptest);
  Standard_Real u1 = ptest.X(), u2 = u1;
  Standard_Real v1 = ptest.Y(), v2 = v1;

  if (newv) {
    if (UisoLineOnSphe) {
      Handle(Geom_Curve) c3d = BRep_Tool::Curve(TopoDS::Edge(E),C3Df,C3Dl);
      GeomAdaptor_Curve GC(c3d); gp_Pnt p3dtest = GC.Value(t);
      Handle(Geom_Surface) surf = BRep_Tool::Surface(TopoDS::Face(F));
      GeomAPI_ProjectPointOnSurf pons(p3dtest,surf);
      if (!(pons.NbPoints() < 1))
	pons.LowerDistanceParameters(u2,v2);
    } else TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(F,u2,v2);
  }
  if (!newv) TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(F,u2,v2);
  du = u2 - u1, dv = v2 - v1;    

  if ( du != 0. || dv != 0.) {
    // translate curve PC of du,dv
    Handle(Geom2d_Curve) PCT = Handle(Geom2d_Curve)::DownCast(PC->Copy());
    PCT->Translate(gp_Vec2d(du,dv));
    PC = PCT;
  }
}


// RLE - IAB 16 june 94
// should be provided by the BRep_Builder

Standard_EXPORT void TopOpeBRepDS_SetThePCurve(const BRep_Builder& B,
					       TopoDS_Edge& E,
					       const TopoDS_Face& F,
					       const TopAbs_Orientation O,
					       const Handle(Geom2d_Curve)& C)
{
  // check if there is already a pcurve on non planar faces
  Standard_Real f,l;
  Handle(Geom2d_Curve) OC;
  TopLoc_Location SL;
  Handle(Geom_Plane) GP = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F,SL));
  if (GP.IsNull())
    OC = BRep_Tool::CurveOnSurface(E,F,f,l);

  if (OC.IsNull()) 
    B.UpdateEdge(E,C,F,Precision::Confusion());
  else {
    Standard_Boolean degen = BRep_Tool::Degenerated(E);
    if(!degen){
      if (O == TopAbs_REVERSED) 
	B.UpdateEdge(E,OC,C,F,Precision::Confusion());
      else 
	B.UpdateEdge(E,C,OC,F,Precision::Confusion());
    }
  }
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::PCurve(TopoDS_Shape& F, 
				     TopoDS_Shape& E, 
				     const Handle(Geom2d_Curve)& PC)const 
{
  if ( ! PC.IsNull() ) {
    TopoDS_Face FF = TopoDS::Face(F);
    TopoDS_Edge EE = TopoDS::Edge(E);
    Handle(Geom2d_Curve) PCT = PC;

    // pour iab, ajout de Translate
    Standard_Boolean tran = myTranslate;

    // xpu : 13-06-97 : 
    // recompute twice the pcurve boundaries if OverWrite
    // if the pcurve <PC> is U isoline on sphere -> to avoid.
    Standard_Boolean UisoLineOnSphe = FUN_UisoLineOnSphe(F,PC);
    Standard_Boolean overwrite = UisoLineOnSphe? Standard_False:myOverWrite;
    // xpu : 13-06-97

    if (tran)      
      TranslateOnPeriodic(F,E,PCT);  

    if (overwrite) 
      myBuilder.UpdateEdge(EE,PCT,FF,0);
    else           
      TopOpeBRepDS_SetThePCurve(myBuilder,EE,FF,E.Orientation(),PCT);

    // parametrage sur la nouvelle courbe 2d
    TopExp_Explorer exi(E,TopAbs_VERTEX);
    for (;exi.More(); exi.Next() ) {
      const TopoDS_Vertex& vi = TopoDS::Vertex(exi.Current());
      if ( vi.Orientation() != TopAbs_INTERNAL ) continue;
      Standard_Real tolvi = TopOpeBRepTool_ShapeTool::Tolerance(vi);
      // NYI tester l'existence d'au moins
      // NYI un parametrage de vi sur EE (en 3d ou en 2d) 
      // NYI --> a faire dans BRep_Tool
      Standard_Real newpar = BRep_Tool::Parameter(vi,EE);
      myBuilder.UpdateVertex(vi,newpar,EE,FF,tolvi);
    } // INTERNAL vertex
  }
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::PCurve(TopoDS_Shape& F, 
				     TopoDS_Shape& E, 
				     const TopOpeBRepDS_Curve& CDS,
				     const Handle(Geom2d_Curve)& PC)const
{
  if ( ! PC.IsNull() ) {
    TopoDS_Face FF = TopoDS::Face(F);
    TopoDS_Edge EE = TopoDS::Edge(E);

    Handle(Geom2d_Curve) PCT = PC;
    Standard_Real    CDSmin,CDSmax;
    Standard_Boolean rangedef = CDS.Range(CDSmin,CDSmax);

    
    TopLoc_Location L; Standard_Real Cf,Cl;
    Handle(Geom_Curve) C = BRep_Tool::Curve(EE,L,Cf,Cl);
      
    if (!C.IsNull()){
      Standard_Boolean deca = (Abs(Cf - CDSmin) > Precision::PConfusion());
      Handle(Geom2d_Line) line2d = Handle(Geom2d_Line)::DownCast(PCT);
      Standard_Boolean isline2d = !line2d.IsNull();
      Standard_Boolean tran=(rangedef && deca && C->IsPeriodic() && isline2d);
      if (tran) {
	TopLoc_Location Loc;
	const Handle(Geom_Surface) Surf = BRep_Tool::Surface(FF,Loc);
	Standard_Boolean isUperio = Surf->IsUPeriodic();
	Standard_Boolean isVperio = Surf->IsVPeriodic();
	gp_Dir2d dir2d = line2d->Direction();
	Standard_Real delta;
	if (isUperio && dir2d.IsParallel(gp::DX2d(),Precision::Angular())) {
	  delta = (CDSmin - Cf) * dir2d.X();
	  PCT->Translate(gp_Vec2d(delta,0.));
	}
	else if(isVperio && dir2d.IsParallel(gp::DY2d(),Precision::Angular())){
	  delta = (CDSmin - Cf) * dir2d.Y();
	  PCT->Translate(gp_Vec2d(0.,delta));
	}
      }
    }
    
    TopOpeBRepDS_SetThePCurve(myBuilder,EE,FF,E.Orientation(),PCT);
  }
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Orientation(TopoDS_Shape& S, 
					  const TopAbs_Orientation O)const 
{
  S.Orientation(O);
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  TopOpeBRepDS_BuildTool::Orientation
  (const TopoDS_Shape& S) const
{
  return S.Orientation();
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::Closed(TopoDS_Shape& S, 
				     const Standard_Boolean B)const 
{
  S.Closed(B);
}


//=======================================================================
//function : Approximation
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_BuildTool::Approximation() const
{
  return myCurveTool.GetGeomTool().TypeC3D() != TopOpeBRepTool_BSPLINE1;
}

void TopOpeBRepDS_BuildTool::UpdateSurface(const TopoDS_Shape& F,const Handle(Geom_Surface)& SU) const
{
  BRep_Builder BB;
  TopLoc_Location L;
  Standard_Real tol = BRep_Tool::Tolerance(TopoDS::Face(F));
  BB.UpdateFace(TopoDS::Face(F),SU,L,tol);
}

void TopOpeBRepDS_BuildTool::UpdateSurface(const TopoDS_Shape& E,const TopoDS_Shape& oldF,
					   const TopoDS_Shape& newF) const
{
  BRep_Builder BB;
  Standard_Real f,l;
  const Handle(Geom2d_Curve)& PC = BRep_Tool::CurveOnSurface(TopoDS::Edge(E),TopoDS::Face(oldF),f,l);
  Standard_Real tol = BRep_Tool::Tolerance(TopoDS::Face(oldF));
  BB.UpdateEdge(TopoDS::Edge(E),PC,TopoDS::Face(newF),tol);
}


/* // - merge 04-07-97
//=======================================================================
//function : RecomputeCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::RecomputeCurve
(const TopOpeBRepDS_Curve& C1,
 TopoDS_Shape& E,
 TopOpeBRepDS_Curve& C2 ) const
{
  // - C1 curves have been approximated by BSplines of degree 1 :
  // or
  // - C1.Curve() is non projectable on at least one of the original
  // intersecting faces.

  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  Standard_Boolean compc3d = GT.CompC3D();
  Standard_Boolean comppc1 = GT.CompPC1();
  Standard_Boolean comppc2 = GT.CompPC2();
  
  const Handle(Geom_Curve)& C3D = C1.Curve();
  if (compc3d && C3D.IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 1");
  if (comppc1 && C2.Shape1().IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 2");
  if (comppc2 && C2.Shape2().IsNull()) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 3");
  TopoDS_Vertex Vmin,Vmax; TopExp::Vertices(TopoDS::Edge(E),Vmin,Vmax);
  if ( Vmin.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 4");
  if ( Vmax.IsNull() ) throw Standard_ProgramError("TopOpeBRepDS_BuildTool::RecomputeCurve 5");

  Standard_Boolean kbspl1 = Standard_False;
  Handle(Geom_BSplineCurve) BS = Handle(Geom_BSplineCurve)::DownCast(C3D);
  if (!BS.IsNull()) kbspl1 = (BS->Degree() == 1);
  if (kbspl1) RecomputeBSpline1Curve(C1,E,C2);
  else        RecomputeCurveOnCone(C1,E,C2);
}

//=======================================================================
//function : RecomputeBSpline1Curve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::RecomputeBSpline1Curve
(const TopOpeBRepDS_Curve& C1,
 TopoDS_Shape& EE,
 TopOpeBRepDS_Curve& C2) const
{
  // C1 curves have been approximated by BSplines of degree 1 :
  // compute new geometry on curves.

  TopoDS_Edge& E = TopoDS::Edge(EE);

  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  TopOpeBRepTool_OutCurveType typec3d = GT.TypeC3D();
  Standard_Boolean compc3d =            GT.CompC3D();
  Standard_Boolean comppc1 =            GT.CompPC1();
  Standard_Boolean comppc2 =            GT.CompPC2();

  const TopoDS_Face& F1 = TopoDS::Face(C2.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(C2.Shape2());
  
  const Handle(Geom_Curve)&   C3D = C1.Curve();
  const Handle(Geom2d_Curve)& PC1 = C1.Curve1();
  const Handle(Geom2d_Curve)& PC2 = C1.Curve2();

  // Vmin,Vmax = bounding vertices of edge <E>
  // and their parameters parmin,parmax .

  TopoDS_Vertex Vmin,Vmax; Standard_Real parmin,parmax;
  ::GetOrientedEdgeVertices(E,Vmin,Vmax,parmin,parmax);

 
  Handle(Geom_Curve)   C3Dnew;
  Handle(Geom2d_Curve) PC1new;
  Handle(Geom2d_Curve) PC2new;
  Standard_Real tolreached3d,tolreached2d;

  if ( typec3d == TopOpeBRepTool_BSPLINE1 ) {
    if ( compc3d ) {
      C3Dnew = Handle(Geom_BSplineCurve)::DownCast(C3D->Copy());
      (Handle(Geom_BSplineCurve)::DownCast(C3Dnew))->Segment(parmin,parmax);
    }
    if ( comppc1 && (!PC1.IsNull()) ) {
      PC1new = Handle(Geom2d_BSplineCurve)::DownCast(PC1->Copy());
      (Handle(Geom2d_BSplineCurve)::DownCast(PC1new))->Segment(parmin,parmax);
    }
    if ( comppc2 && (!PC2.IsNull()) ) {
      PC2new = Handle(Geom2d_BSplineCurve)::DownCast(PC2->Copy());
      (Handle(Geom2d_BSplineCurve)::DownCast(PC2new))->Segment(parmin,parmax);
    }
  }

  else if ( typec3d == TopOpeBRepTool_APPROX ) {
    if (!comppc1 || !comppc2) throw Standard_NotImplemented("DSBuildToolAPPROX");
    myCurveTool.MakeCurves(parmin,parmax,
			   C3D,PC1,PC2,F1,F2,
			   C3Dnew,PC1new,PC2new,
			   tolreached3d,tolreached2d);
  }

  else if ( typec3d == TopOpeBRepTool_INTERPOL ) {
    throw Standard_NotImplemented("DSBuildToolINTERPOL");
  }

  Standard_Real newtol,newparmin,newparmax;
  ::FUN_updateEDGECURVETOL
    (*this,F1,F2,E,C3Dnew,tolreached3d,tolreached2d,tolreached2d,
     newtol,newparmin,newparmax);
  
  if (!C3Dnew.IsNull()) {
    C2.DefineCurve(C3Dnew,newtol,Standard_False);
    C2.SetRange(newparmin,newparmax);
  }
  if (!PC1new.IsNull()) C2.Curve1(PC1new);
  if (!PC2new.IsNull()) C2.Curve2(PC2new);
}


//=======================================================================
//function : RecomputeCurveOnCone
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_BuildTool::RecomputeCurveOnCone
  (const TopOpeBRepDS_Curve& C1,
   TopoDS_Shape&             EE,
   TopOpeBRepDS_Curve&       C2 ) const
{
  // C1 Pcurves have not been computed because C1 Curve is not projectable
  // on one at least of the intersecting faces giving C1 Curve.
  // (see TopOpeBRepTool_CurveTool::IsProjectable())

  TopoDS_Edge& E = TopoDS::Edge(EE);
  
  const TopOpeBRepTool_GeomTool& GT = myCurveTool.GetGeomTool();
  TopOpeBRepTool_OutCurveType typec3d = GT.TypeC3D();
  Standard_Boolean compc3d =            GT.CompC3D();
  Standard_Boolean comppc1 =            GT.CompPC1();
  Standard_Boolean comppc2 =            GT.CompPC2();
  
  const TopoDS_Face& F1 = TopoDS::Face(C2.Shape1());
  const TopoDS_Face& F2 = TopoDS::Face(C2.Shape2());
  
  const Handle(Geom_Curve)&   C3D = C1.Curve();
  const Handle(Geom2d_Curve)& PC1 = C1.Curve1();
  const Handle(Geom2d_Curve)& PC2 = C1.Curve2();

  // get bounding vertices Vmin,Vmax supported by the new edge <E>
  // and their corresponding parameters parmin,parmax .

  TopoDS_Vertex Vmin,Vmax; Standard_Real parmin,parmax;
  ::GetOrientedEdgeVertices(E,Vmin,Vmax,parmin,parmax);

  if ( C3D->IsPeriodic() ) {
    // ellipse on cone : periodize parmin,parmax
    Standard_Real period = C3D->LastParameter() - C3D->FirstParameter();
    Standard_Real f,l;
    if (Vmin.Orientation() == TopAbs_FORWARD) { f = parmin; l = parmax; }
    else {                                      f = parmax; l = parmin; }
    parmin = f; parmax = l;
    ElCLib::AdjustPeriodic(f,f+period,Precision::PConfusion(),parmin,parmax);
  }
  
  Handle(Geom_TrimmedCurve) C3Dnew;
  Handle(Geom2d_Curve) PC1new;
  Handle(Geom2d_Curve) PC2new;
  Standard_Real tolreached3d = C1.Tolerance();
  Standard_Real tolreached2d1 = C1.Tolerance();
  Standard_Real tolreached2d2 = C1.Tolerance();
  if (compc3d) C3Dnew = new Geom_TrimmedCurve(C3D,parmin,parmax);
  if (comppc1) PC1new = myCurveTool.MakePCurveOnFace(F1,C3Dnew,tolreached2d1);
  if (comppc2) PC2new = myCurveTool.MakePCurveOnFace(F2,C3Dnew,tolreached2d2);

#ifdef DRAW
  if (tBUTO) {FUN_draw(F1); FUN_draw(F2); FUN_draw(E);}
#endif

  Standard_Real newtol,newparmin,newparmax;
  FUN_updateEDGECURVETOL
  (*this,F1,F2,E,C3Dnew,tolreached3d,tolreached2d1,tolreached2d2,
   newtol,newparmin,newparmax);

//   jyl : 16-06-97
//  Standard_Real fac = 0.3798123578771;
//  Standard_Real tol = newtol;
//  Standard_Real par3d = (1-fac)*newparmin + (fac)*newparmax;
//  Standard_Real par2d = par3d - newparmin;
//
//  gp_Pnt P3DC3D;       C3D->D0(par3d,P3DC3D);
//
//  Standard_Boolean UisoLineOnSphe1 = Standard_False;
//  UisoLineOnSphe1 = ::FUN_UisoLineOnSphe(F1,PC1new);
//  if (UisoLineOnSphe1) {
//    Standard_Real isrev1 = 
//      ::FUN_reversePC(PC1new,F1,P3DC3D,par2d,tol);
//    
//
//  }
//
//  Standard_Boolean UisoLineOnSphe2 = Standard_False;
//  UisoLineOnSphe2 = ::FUN_UisoLineOnSphe(F2,PC2new);
//  if (UisoLineOnSphe2) {
//    Standard_Real isrev2 = 
//     ::FUN_reversePC(PC2new,F2,P3DC3D,par2d,tol);
//
//  }

  // xpu : 17-06-97
  // Rmq : C1.Curve<i>() ne sert plus qu'a determiner si la courbe
  //       est une isos de la sphere
  // NYI : enlever FUN_reversePC
  Standard_Boolean UisoLineOnSphe1 = Standard_False;
  UisoLineOnSphe1 = ::FUN_UisoLineOnSphe(F1,PC1new);
  if (UisoLineOnSphe1) {
    ::FUN_makeUisoLineOnSphe(F1,C3Dnew,PC1new,newtol);
  }
  Standard_Boolean UisoLineOnSphe2 = Standard_False;
  UisoLineOnSphe2 = ::FUN_UisoLineOnSphe(F2,PC2new);
  if (UisoLineOnSphe2) {
    ::FUN_makeUisoLineOnSphe(F2,C3Dnew,PC2new,newtol);
  } // xpu : 17-06-97

  if (!C3Dnew.IsNull()) C2.Curve(C3Dnew,newtol);
  if (!PC1new.IsNull()) C2.Curve1(PC1new);
  if (!PC2new.IsNull()) C2.Curve2(PC2new);
}*/ // - merge 04-07-97


