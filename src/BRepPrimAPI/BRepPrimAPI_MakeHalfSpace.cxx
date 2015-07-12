// Created on: 1995-03-08
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
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : FindExtrema
//purpose  : This finction is called to find the nearest normal projection
//           of a point <aPnt> on a face <aFace>.
//           1) return true if extrema are found.
//           2) Set in:
//             - Dist : The lower distance found.
//             - anOppositePnt : The corresponding point lying on the face
//             - U,V : The parameters of <anOppositePnt> on the face <aFace>
//=======================================================================
static Standard_Real FindExtrema(const gp_Pnt&        aPnt,
				 const TopoDS_Face&   aFace,
				       Standard_Real& Dist,
				       gp_Pnt&        anOppositePnt,
				       Standard_Real& U,
				       Standard_Real& V)
{
  Standard_Integer intvalue=0;
  Dist = RealLast();
  TopoDS_Vertex RefVertex = BRepBuilderAPI_MakeVertex(aPnt);
  
  BRepExtrema_ExtPF ext(RefVertex,aFace);
  
  if (ext.IsDone() && ext.NbExt() > 0) {
    // the point projection exist
    Standard_Integer nbext = ext.NbExt();
    for (Standard_Integer iext = 1; iext <= nbext; iext++) {
      if (ext.SquareDistance(iext) < Dist) {
	Dist     = ext.SquareDistance(iext);
	intvalue = iext;
      }
    }
    Dist = sqrt(Dist);
  } else {
    // compute the min distance with the face vertices
    TopExp_Explorer explo(aFace, TopAbs_VERTEX);
    for(; explo.More(); explo.Next()) {
      const TopoDS_Vertex& vertex = TopoDS::Vertex(explo.Current());
      gp_Pnt2d             param  = BRep_Tool::Parameters(vertex, aFace);
      gp_Pnt               Pnt    = BRep_Tool::Pnt(vertex);
      
      Standard_Real        d2      = Pnt.SquareDistance(aPnt);
      if(d2 < Dist) {
	Dist          = d2;
	anOppositePnt = Pnt;
	param.Coord(U, V);
      }
    }
    Dist = sqrt(Dist);
    return Standard_True;
  }

  // Final result.
  anOppositePnt = ext.Point(intvalue);
  ext.Parameter(intvalue,U,V);

  return Standard_True;
}



//=======================================================================
//function : BRepPrimAPI_MakeHalfSpace
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::BRepPrimAPI_MakeHalfSpace(const TopoDS_Face& Face,
					     const gp_Pnt&      RefPnt)
{
  // Set the flag is <IsDone> to False.
  NotDone();

  TopoDS_Shell Shell;

//  gp_Pnt CurPnt, MinPnt;
  gp_Pnt MinPnt;
  Standard_Real U, V;

  Standard_Real MinDist;
  if (FindExtrema(RefPnt,Face,MinDist,MinPnt,U,V)) {
    BRep_Builder myBuilder;
    myBuilder.MakeShell(Shell);
    myBuilder.Add(Shell,Face);
    
    // Normal, scalair product and direction.
    Standard_Real Prec = gp::Resolution();
//    BRepLProp_SLProps Props(BRepAdaptor_Surface(Face),U,V,2,Prec);
    BRepLProp_SLProps Props = BRepLProp_SLProps(BRepAdaptor_Surface(Face),U,V,2,Prec);
    gp_Dir Normale    = Props.Normal();
    gp_Dir OppRef(RefPnt.XYZ()-MinPnt.XYZ());
    Standard_Real Sca = Normale*OppRef;
    
    // Construction of the open solid.
    myBuilder.MakeSolid(mySolid);
    if (Sca > 0.) {
      // Same directions: inverted case.
      Shell.Reverse();
    }
    
    myBuilder.Add(mySolid,Shell);
    Done();
  }
}


//=======================================================================
//function : BRepPrimAPI_MakeHalfSpace
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::BRepPrimAPI_MakeHalfSpace(const TopoDS_Shell& Shell,
					     const gp_Pnt&       RefPnt)
{
  // Set the flag is <IsDone> to False.
  NotDone();

  gp_Pnt CurPnt, MinPnt;
  TopoDS_Face CurFace, MinFace;
  Standard_Real MinDist = RealLast();
  Standard_Real CurDist, U, V, MinU=0, MinV=0;

  // Find the point of the skin closest to the reference point.
  Standard_Boolean YaExt = Standard_False;

  TopoDS_Shell aShell = Shell;
  TopExp_Explorer Exp(aShell,TopAbs_FACE);
  while (Exp.More()) {
    CurFace = TopoDS::Face(Exp.Current());
    if ( FindExtrema(RefPnt,CurFace,CurDist,CurPnt,U,V)) {
      YaExt = Standard_True;
      if (CurDist < MinDist) {
	MinDist = CurDist;
	MinPnt  = CurPnt;
	MinU    = U;
	MinV    = V;
	MinFace = CurFace;
      }
    }
    Exp.Next();
  }

  if ( YaExt) {
    // Normal, scalar product and direction.
    BRep_Builder myBuilder;
    Standard_Real Prec = gp::Resolution();
//    BRepLProp_SLProps Props(BRepAdaptor_Surface(MinFace),MinU,MinV,2,Prec);
    BRepLProp_SLProps Props = BRepLProp_SLProps(BRepAdaptor_Surface(MinFace),MinU,MinV,2,Prec);
    gp_Dir Normale    = Props.Normal();
    gp_Dir OppRef(RefPnt.XYZ()-MinPnt.XYZ());
    Standard_Real Sca = Normale*OppRef;
    
    // Construction of the open solid.
    myBuilder.MakeSolid(mySolid);
    if (Sca > 0.) {
      // Same directions: inverted case.
      aShell.Reverse();
    }
    myBuilder.Add(mySolid,aShell);
    Done();
  }
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid& BRepPrimAPI_MakeHalfSpace::Solid() const
{
  StdFail_NotDone_Raise_if( !IsDone(), "BRepPrimAPI_MakeHalfSpace::Solid");
  return mySolid;
}



//=======================================================================
//function : TopoDS_Solid
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeHalfSpace::operator TopoDS_Solid() const
{
  return Solid();
}


