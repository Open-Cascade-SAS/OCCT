// Created on: 1995-01-04
// Created by: Bruno DUMORTIER
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


#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeShell.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TColGeom2d_Array1OfCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>

//=======================================================================
//function : BRepLib_MakeShell
//purpose  : 
//=======================================================================
BRepLib_MakeShell::BRepLib_MakeShell() :
       myError(BRepLib_EmptyShell)
{
}


//=======================================================================
//function : BRepLib_MakeShell
//purpose  : 
//=======================================================================

BRepLib_MakeShell::BRepLib_MakeShell(const Handle(Geom_Surface)& S,
				     const Standard_Boolean Segment)
{
  Standard_Real UMin,UMax,VMin,VMax;
  S->Bounds(UMin,UMax,VMin,VMax);
  Init(S,UMin,UMax,VMin,VMax,Segment);
}


//=======================================================================
//function : BRepLib_MakeShell
//purpose  : 
//=======================================================================

BRepLib_MakeShell::BRepLib_MakeShell(const Handle(Geom_Surface)& S, 
				     const Standard_Real UMin,
				     const Standard_Real UMax, 
				     const Standard_Real VMin, 
				     const Standard_Real VMax,
				     const Standard_Boolean Segment)
{
  Init(S,UMin,UMax,VMin,VMax,Segment);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepLib_MakeShell::Init(const Handle(Geom_Surface)& S, 
			     const Standard_Real UMin, 
			     const Standard_Real UMax, 
			     const Standard_Real VMin, 
			     const Standard_Real VMax,
			     const Standard_Boolean Segment)
{
  Handle(Geom_Surface) BS = S;
  if ( S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) RTS = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
    BS = RTS->BasisSurface();
  }
  myError = BRepLib_EmptyShell;
  Standard_Real tol = Precision::Confusion();

  // Make a shell from a surface
  GeomAdaptor_Surface GS(BS,UMin,UMax,VMin,VMax);

  Standard_Integer nu = GS.NbUIntervals(GeomAbs_C2);
  Standard_Integer nv = GS.NbVIntervals(GeomAbs_C2);

  Standard_Boolean uperiodic = GS.IsUPeriodic();
  Standard_Boolean vperiodic = GS.IsVPeriodic();

  if (nu == 0 || nv == 0) return;

  // arrays of parameters and pcurves
  TColStd_Array1OfReal upars(1,nu+1);
  TColStd_Array1OfReal vpars(1,nv+1);
  TColGeom2d_Array1OfCurve uisos(1,nu+1);
  TColGeom2d_Array1OfCurve visos(1,nv+1);

  Standard_Integer iu,iv;

  GS.UIntervals(upars,GeomAbs_C2);
  gp_Dir2d dv(0,1);
  for (iu = 1; iu <= nu+1; iu++) {
    Standard_Real u = upars(iu);
    if (!Precision::IsInfinite(u))
      uisos(iu)  = new Geom2d_Line(gp_Pnt2d(u,0.),dv);
  }
  
  GS.VIntervals(vpars,GeomAbs_C2);
  gp_Dir2d du(1,0);
  for (iv = 1; iv <= nv+1 ; iv++) {
    Standard_Real v = vpars(iv);
    if (!Precision::IsInfinite(v))
      visos(iv)  = new Geom2d_Line(gp_Pnt2d(0.,v),du);
  }
    
  // create row by row

  // create the shell
  BRep_Builder B;
  B.MakeShell(TopoDS::Shell(myShape));

  // arrays of edges and vertices for each row
  TopTools_Array1OfShape botedges(1,nu);
  TopTools_Array1OfShape botvertices(1,nu+1);

  // copies of the first ones for periodic case
  TopTools_Array1OfShape fbotedges(1,nu);
  TopTools_Array1OfShape fbotvertices(1,nu+1);

  TopoDS_Face F;
  TopoDS_Wire W;
  TopoDS_Edge eleft,eright,etop,ebot,feleft;
  TopoDS_Vertex vlb,vlt,vrb,vrt,fvlt;


  // init the botedges and botvertices
  if (!Precision::IsInfinite(vpars(1))) {
    if (!Precision::IsInfinite(upars(1)))
      B.MakeVertex(vrt,S->Value(upars(1),vpars(1)),tol);
    fbotvertices(1) = botvertices(1) = vrt;

    for (iu = 1; iu <= nu; iu++) {
      vlt = vrt;

      if (uperiodic && iu == nu)
	vrt = TopoDS::Vertex(botvertices(1));
      else if (!Precision::IsInfinite(upars(iu+1)))
	B.MakeVertex(vrt,S->Value(upars(iu+1),vpars(1)),tol);

      fbotvertices(iu+1) = botvertices(iu+1) = vrt;
      B.MakeEdge(etop);
      if (!vlt.IsNull()) {
	vlt.Orientation(TopAbs_FORWARD);
	B.Add(etop,vlt);
      }
      if (!vrt.IsNull()) {
	vrt.Orientation(TopAbs_REVERSED);
	B.Add(etop,vrt);
      }
      fbotedges(iu) = botedges(iu) = etop;
    }
  }

  for (iv = 1; iv <= nv; iv++) {

    // compute the first edge and vertices of the line
    vrb = TopoDS::Vertex(botvertices(1));

    if (vperiodic && iv == nv) {
      vrt = TopoDS::Vertex(fbotvertices(1));
    }
    else {
      vrt.Nullify();
      if (!Precision::IsInfinite(vpars(iv+1))) {
	if (!Precision::IsInfinite(upars(1)))
	  B.MakeVertex(vrt,S->Value(upars(1),vpars(iv+1)),tol);
      }
    }

    eright.Nullify();
    if (!Precision::IsInfinite(upars(1))) {
      B.MakeEdge(eright);
      if (!vrb.IsNull()) {
	vrb.Orientation(TopAbs_FORWARD);
	B.Add(eright,vrb);
      }
      if (!vrt.IsNull()) {
	vrt.Orientation(TopAbs_REVERSED);
	B.Add(eright,vrt);
      }
    }

    fvlt  = vrt;
    feleft = eright;


    // make the row of faces
    
    for (iu = 1; iu <= nu; iu++) {
      
      // create the face at iu, iv

      // the surface
      Handle(Geom_Surface) SS = Handle(Geom_Surface)::DownCast(BS->Copy());
      if (GS.GetType() == GeomAbs_BSplineSurface && Segment) {
	Handle(Geom_BSplineSurface)::DownCast(SS)
	  ->Segment(upars(iu),upars(iu+1),
		    vpars(iv),vpars(iv+1) );
      }
      B.MakeFace(F,SS,tol);

      // the wire

      B.MakeWire(W);

      // the vertices

      vlb = vrb;
      vrb = TopoDS::Vertex(botvertices(iu+1));
      vlt = vrt;

      if (uperiodic && iu == nu)
	vrt = fvlt;
      else {
	vrt.Nullify();
	if (!Precision::IsInfinite(vpars(iv+1))) {
	  if (!Precision::IsInfinite(upars(iu+1)))
	    B.MakeVertex(vrt,S->Value(upars(iu+1),vpars(iv+1)),tol);
	}
      }

      botvertices(iu)   = vlt;
      botvertices(iu+1) = vrt;

      // the edges

      eleft = eright;

      if  (uperiodic && iu == nu)
	eright = feleft;
      else {
	eright.Nullify();
	if (!Precision::IsInfinite(upars(iu+1))) {
	  B.MakeEdge(eright);
	  if (!vrb.IsNull()) {
	    vrb.Orientation(TopAbs_FORWARD);
	    B.Add(eright,vrb);
	  }
	  if (!vrt.IsNull()) {
	    vrt.Orientation(TopAbs_REVERSED);
	    B.Add(eright,vrt);
	  }
	}
      }

      if ( uperiodic && nu == 1) {
	if (!eleft.IsNull() && !eright.IsNull()) {
	  B.UpdateEdge(eleft,uisos(2),uisos(1),F,tol);
	  B.Range(eleft,F,vpars(iv),vpars(iv+1));
	}
      }
      else {
	if (!eleft.IsNull()) {
	  B.UpdateEdge(eleft,uisos(iu),F,tol);
	  B.Range(eleft,F,vpars(iv),vpars(iv+1));
	}
	if (!eright.IsNull()) {
	  B.UpdateEdge(eright,uisos(iu+1),F,tol);
	  B.Range(eright,F,vpars(iv),vpars(iv+1));
	}
      }
      
      ebot = TopoDS::Edge(botedges(iu));

      if (vperiodic && iv == nv) 
	etop = TopoDS::Edge(fbotedges(iu));
      else {
	etop.Nullify();
	if (!Precision::IsInfinite(vpars(iv+1))) {
	  B.MakeEdge(etop);
	  if (!vlt.IsNull()) {
	    vlt.Orientation(TopAbs_FORWARD);
	    B.Add(etop,vlt);
	  }
	  if (!vrt.IsNull()) {
	    vrt.Orientation(TopAbs_REVERSED);
	    B.Add(etop,vrt);
	  }
	}
      }

      if ( vperiodic && nv == 1) {
	if (!ebot.IsNull() && !etop.IsNull()) {
	  B.UpdateEdge(ebot,visos(1),visos(2),F,tol);
	  B.Range(ebot,F,vpars(iv),vpars(iv+1));
	}
      }
      else {
	if (!ebot.IsNull()) {
	  B.UpdateEdge(ebot,visos(iv),F,tol);
	  B.Range(ebot,F,upars(iu),upars(iu+1));
	}
	if (!etop.IsNull()) {
	  B.UpdateEdge(etop,visos(iv+1),F,tol);
	  B.Range(etop,F,upars(iu),upars(iu+1));
	}
      }
      
      botedges(iu) = etop;

      if (!eleft.IsNull()) {
	eleft.Orientation(TopAbs_REVERSED);
	B.Add(W,eleft);
      }
      if (!ebot.IsNull()) {
	ebot.Orientation(TopAbs_FORWARD);
	B.Add(W,ebot);
      }
      if (!eright.IsNull()) {
	eright.Orientation(TopAbs_FORWARD);
	B.Add(W,eright);
      }
      if (!etop.IsNull()) {
	etop.Orientation(TopAbs_REVERSED);
	B.Add(W,etop);
      }
      
      B.Add(F,W);
      B.Add(myShape,F);
    }
  }
  
  // codage des courbes 3d et regularites.
  BRepLib::BuildCurves3d(myShape,tol);
  BRepLib::EncodeRegularity(myShape);
  myShape.Closed (BRep_Tool::IsClosed (myShape));

  // Additional checking for degenerated edges
  Standard_Boolean isDegenerated;
  Standard_Real aFirst, aLast;
  Standard_Real aTol = Precision::Confusion();
  Standard_Real anActTol;
  TopExp_Explorer anExp(myShape, TopAbs_EDGE);
  for ( ; anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
    isDegenerated = BRepLib_MakeFace::IsDegenerated(aCurve, aTol, anActTol);
    B.Degenerated(anEdge, isDegenerated);
  }
  
  myError = BRepLib_ShellDone;
  Done();
}


//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepLib_ShellError BRepLib_MakeShell::Error() const 
{
  return myError;
}


//=======================================================================
//function : TopoDS_Shell&
//purpose  : 
//=======================================================================

const TopoDS_Shell& BRepLib_MakeShell::Shell() const 
{
  return TopoDS::Shell(myShape);
}



//=======================================================================
//function : TopoDS_Shell
//purpose  : 
//=======================================================================

BRepLib_MakeShell::operator TopoDS_Shell() const
{
  return Shell();
}


