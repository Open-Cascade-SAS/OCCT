// Created on: 2001-03-28
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPTest.ixx>

#include <stdio.h>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_State.hxx>

#include <TCollection_AsciiString.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>

#include <BRep_Tool.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_GCurve.hxx>

#include <Draw.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>

#include <BOPTools_AlgoTools2D.hxx>

static
  void PrintState (Draw_Interpretor& aDI,
                   const TopAbs_State& aState);
static
  Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                      const TopoDS_Face& F,
                                      Standard_Real& First,
                                      Standard_Real& Last);
static
  Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                      const Handle(Geom_Surface)& S,
                                      const TopLoc_Location& L,
                                      Standard_Real& First,
                                      Standard_Real& Last);

static  Standard_Integer bclassify   (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer b2dclassify (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bhaspc      (Draw_Interpretor& , Standard_Integer , const char** );

//=======================================================================
//function : LowCommands
//purpose  : 
//=======================================================================
  void  BOPTest::LowCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";
  theCommands.Add("bclassify"    , "Use >bclassify Solid Point [Tolerance=1.e-7]",
                  __FILE__, bclassify   , g);
  theCommands.Add("b2dclassify"  , "Use >bclassify Face Point2d [Tol2D=Tol(Face)] ",
                  __FILE__, b2dclassify , g);
  theCommands.Add("bhaspc"       , "Use >bhaspc Edge Face [do]",
                  __FILE__, bhaspc      , g);
}

//=======================================================================
//function : bclassify
//purpose  : 
//=======================================================================
Standard_Integer bclassify (Draw_Interpretor& aDI,
                            Standard_Integer n, 
                            const char** a)
{
  char sbf[512];	
  
  if (n < 3) {
    Sprintf(sbf, " Use >bclassify Solid Point [Tolerance=1.e-7]\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    Sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  
  if (aS.ShapeType()!=TopAbs_SOLID) {
    Sprintf(sbf, " Shape type must be SOLID\n");
    aDI<<sbf;
    return 1;
  }
  //
  Standard_Real aTol=1.e-7;
  TopAbs_State aState = TopAbs_UNKNOWN;
  gp_Pnt aP(8., 9., 10.);
  
  DrawTrSurf::GetPoint(a[2], aP);
  
  aTol=1.e-7; 
  if (n==4) {
    aTol=Draw::Atof(a[3]);
  }
  //
  BRepClass3d_SolidClassifier aSC(aS);
  aSC.Perform(aP,aTol);
  //
  aState = aSC.State();
  //
  PrintState (aDI, aState);
  //
  return 0;
}
//
//=======================================================================
//function : b2dclassify
//purpose  : 
//=======================================================================
Standard_Integer b2dclassify (Draw_Interpretor& aDI,
                              Standard_Integer n, 
                              const char** a)
{
  char sbf[512];	
  
  if (n < 3) {
    Sprintf(sbf, " Use >bclassify Face Point2d [Tol2D=Tol(Face)]\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    Sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  
  if (aS.ShapeType()!=TopAbs_FACE) {
    Sprintf(sbf, " Shape type must be FACE\n");
    aDI<<sbf;
    return 1;
  }
  //
  Standard_Real aTol;
  TopAbs_State aState = TopAbs_UNKNOWN;
  gp_Pnt2d aP(8., 9.);
  
  DrawTrSurf::GetPoint2d(a[2], aP);
  
  const TopoDS_Face& aF=TopoDS::Face(aS);
  aTol=BRep_Tool::Tolerance(aF); 
  if (n==4) {
    aTol=Draw::Atof(a[3]);
  }
  //
  BRepClass_FaceClassifier aClassifier;
  aClassifier.Perform(aF, aP, aTol);
  //
  aState = aClassifier.State();
  //
  PrintState (aDI, aState);
  //
  return 0;
}

//=======================================================================
//function : bhaspc
//purpose  : 
//=======================================================================
Standard_Integer bhaspc (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<3) {
    di << " Use bhaspc> Edge Face [do]\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }
  if (S1.ShapeType()!=TopAbs_EDGE || S2.ShapeType()!=TopAbs_FACE) {
    di << " Type mismatch\n";
    return 1;
  }
  //
  const TopoDS_Edge& aE=TopoDS::Edge(S1);
  const TopoDS_Face& aF=TopoDS::Face(S2);
  Standard_Real f2D, l2D; 

  Handle(Geom2d_Curve) C2D=CurveOnSurface(aE, aF, f2D, l2D);
  
  if (C2D.IsNull()) {
    di << " No 2D Curves detected\n";
  }
  else {
    di << " Ok Edge has P-Curve on this Face\n";
  }
  
  if (n==4) {
    if (!strcmp(a[3], "do")) {
      BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF);  
    }
  }

  return 0;
}

//=======================================================================
//function : PrintState
//purpose  : 
//=======================================================================
void PrintState (Draw_Interpretor& aDI,
                 const TopAbs_State& aState)
{
  char sbf[512];	
  TCollection_AsciiString sIN("IN"), sOUT("OUT of"), sON("ON"), sUNKNOWN("UNKNOWN"); 
  //
  Sprintf(sbf, "The point is "); aDI<<sbf;
  //
  switch (aState) {
  case TopAbs_IN:
    Sprintf(sbf, sIN.ToCString());
    break;
  case TopAbs_OUT:
    Sprintf(sbf, sOUT.ToCString());
    break;
  case TopAbs_ON:
    Sprintf(sbf, sON.ToCString());
    break;
  case TopAbs_UNKNOWN:
    Sprintf(sbf, sUNKNOWN.ToCString());
    break;
  default:
    Sprintf(sbf, sUNKNOWN.ToCString()); 
    break;
  }
  aDI<<sbf; 
  //
  Sprintf(sbf, " shape\n");
  aDI<<sbf;
  
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                    const TopoDS_Face& F,
                                    Standard_Real& First,
                                    Standard_Real& Last)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,l);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
  }
  return CurveOnSurface(aLocalEdge,S,l,First,Last);
}

static Handle(Geom2d_Curve) nullPCurve;
//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) CurveOnSurface(const TopoDS_Edge& E, 
                                    const Handle(Geom_Surface)& S,
                                    const TopLoc_Location& L,
                                    Standard_Real& First,
                                    Standard_Real& Last)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      const Handle(BRep_GCurve)& GC = *((Handle(BRep_GCurve)*)&cr);
      GC->Range(First,Last);
      if (GC->IsCurveOnClosedSurface() && Eisreversed)
        return GC->PCurve2();
      else
        return GC->PCurve();
    }
    itcr.Next();
  }
  return nullPCurve;
}

