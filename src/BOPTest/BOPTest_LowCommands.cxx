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

#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopTools_MapOfShape.hxx>

#include <TopLoc_Location.hxx>

#include <TopExp_Explorer.hxx>

#include <TopAbs.hxx>
#include <TopAbs_State.hxx>

#include <DBRep.hxx>

#include <DrawTrSurf.hxx>

#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>

#include <IntTools_Tools.hxx>

#include <BOP_Refiner.hxx>

#include <TCollection_AsciiString.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_FClass2d.hxx>
#include <gp_Pnt2d.hxx>
#include <OSD_Chronometer.hxx>

#include <BRepTools.hxx>
#include <BOPTColStd_CArray1OfInteger.hxx>

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
static
  void PrintState (Draw_Interpretor& aDI,
		   const TopAbs_State& aState);

//modified by NIZNHY-PKV Thu Nov 10 12:11:15 2011f
static
  void DumpArray(const BOPTColStd_CArray1OfInteger& aC,
	       Draw_Interpretor& aDI);
//modified by NIZNHY-PKV Thu Nov 10 12:11:18 2011t

static  Standard_Integer bhaspc      (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer baddve      (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bisclosed   (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bexplode    (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bupdateedge (Draw_Interpretor& , Standard_Integer , const char**);
static  Standard_Integer bremovesim  (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bopsetdisp  (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bfaceprepare(Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer brefine     (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bclassify   (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer b2dclassify (Draw_Interpretor& , Standard_Integer , const char** );

static  Standard_Integer bhole       (Draw_Interpretor& , Standard_Integer , const char** );
static  Standard_Integer bxhole      (Draw_Interpretor& , Standard_Integer , const char** );
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
  //
  // Whether Edge has PCurve on Face
  theCommands.Add("bhaspc"       , "Use >bhaspc Edge Face"    , __FILE__, bhaspc      , g);
  theCommands.Add("baddve"       , "Use >addve  E V1 V2 p1 p2", __FILE__, baddve      , g);
  theCommands.Add("bisclosed"    , "Use >bisclosed Edge Face" , __FILE__, bisclosed   , g);
  theCommands.Add("bexplode"     , ""                         , __FILE__, bexplode    , g);
  theCommands.Add("bupdateedge"  , "Use bupdateedge Edge Face", __FILE__, bupdateedge , g);
  theCommands.Add("bremovesim"   , "Use bremovesim Face"      , __FILE__, bremovesim  , g);
  theCommands.Add("bopsetdisp"   , "Use bopsetdisp"           , __FILE__, bopsetdisp  , g);
  

  theCommands.Add("bfaceprepare" , "Use bfaceprepare F1 "     , __FILE__, bfaceprepare, g);

  theCommands.Add("brefine"      , "Use brefine Shape "       , __FILE__, brefine     , g);
  theCommands.Add("bclassify"    , "Use >bclassify Solid Point [Tolerance=1.e-7]",
		                                                __FILE__, bclassify   , g);
  theCommands.Add("b2dclassify"  , "Use >bclassify Face Point2d [Tol2D=Tol(Face)] ",
		                                                __FILE__, b2dclassify , g);
  theCommands.Add("bhole"   , "Use bhole"                     , __FILE__, bhole       , g);
  theCommands.Add("bxhole"  , "Use bxhole"                    , __FILE__, bxhole      , g);
}

//=======================================================================
//function : brefine
//purpose  : 
//=======================================================================
Standard_Integer  brefine (Draw_Interpretor& di,
			   Standard_Integer n, 
			   const char** a)
{
  if (n<2) {
    di << " Use >brefine Shape1\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
 
  
  if (aS.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }

  Standard_Boolean bIsDone;
  BOP_Refiner aRefiner;
  aRefiner.SetShape(aS);

  aRefiner.Do();

  bIsDone=aRefiner.IsDone();
  
  if (bIsDone) {
    Standard_Integer aNbRemovedVertices, aNbRemovedEdges, iFlag=0;
    aNbRemovedVertices=aRefiner.NbRemovedVertices();
    aNbRemovedEdges=aRefiner.NbRemovedEdges();

    if (aNbRemovedVertices) {
      di << " Vertices removed=" << aNbRemovedVertices << "%d\n";
      iFlag++;
    }

    if (aNbRemovedEdges) {
      di << " Edges    removed=" << aNbRemovedEdges << "\n";
      iFlag++;
    }

    // REFRESH
    if (iFlag) {
      TCollection_AsciiString aNm(a[1]);
      Standard_CString pstr;
      
      pstr=aNm.ToCString();
      DBRep::Set(pstr, aS);
    }
  }

  else {
    di << "Not Done, err=" << aRefiner.ErrorStatus() << "\n";
  }
  return 0;
}
//=======================================================================
//function : bfaceprepare
//purpose  : 
//=======================================================================
Standard_Integer  bfaceprepare(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<2) {
    di << " Use bfaceprepare> F1\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
 
  
  if (S1.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }
  if (S1.ShapeType()!=TopAbs_FACE) {
    di << " Type mismatch\n";
    return 1;
  }

  const TopoDS_Face& aF1=TopoDS::Face(S1);

  TopAbs_Orientation anOr;
  anOr=aF1.Orientation();
  if (anOr==TopAbs_FORWARD) {
    return 0;
  }
  //
  Standard_Real aTol;
  TopoDS_Face newFace;
  BRep_Builder aBB;
  
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS=Handle(Geom_Surface)::DownCast(aS1->Copy());
  aS->VReverse();

  aTol=BRep_Tool::Tolerance(aF1);

  aBB.MakeFace (newFace, aS, aTol);

  TopExp_Explorer anExp(aF1, TopAbs_WIRE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape& newWire=anExp.Current();
    aBB.Add(newFace, newWire);
  }

  TCollection_AsciiString aNm(a[1]), aNmNew("_r");
  aNm=aNm+aNmNew;
  Standard_CString an=aNm.ToCString();
  di << an << "\n";
  DBRep::Set(an, newFace); 
  return 0;
}
//=======================================================================
//function : bopsetdisp
//purpose  : 
//=======================================================================
Standard_Integer bopsetdisp (Draw_Interpretor& di, Standard_Integer , const char** )
{
  Standard_Integer aFlag;
  
  char* xr=getenv("MDISP");
  if (xr==NULL) {
    aFlag=putenv((char*)"MDISP=yes");
    di << " MDISP=yes. Return code=" << aFlag << "\n";
  }
  else if (!strcmp (xr, "yes")) {
    aFlag=putenv((char*)"MDISP=no");
    di << " MDISP=no. Return code=" << aFlag << "\n";
  }
  else {
    aFlag=putenv((char*)"MDISP=yes");
    di << " MDISP=yes. Return code=" << aFlag << "\n";
  }
  

  return 0;
}
//=======================================================================
//function : bremovesim
//purpose  : 
//=======================================================================
Standard_Integer bremovesim (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<2) {
    di << " Use bremovesim> Face\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
 
  
  if (S1.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }
  if (S1.ShapeType()!=TopAbs_FACE) {
    di << " Type mismatch\n";
    return 1;
  }
  

  const TopoDS_Face& aF=TopoDS::Face(S1);
  //
  Handle(IntTools_Context) aCtx=new IntTools_Context;
  BOPTools_Tools3D::RemoveSims (aF, aCtx);
  //
  di << " Ok\n";
  return 0;
}
//=======================================================================
//function : bupdateedge
//purpose  : 
//=======================================================================
Standard_Integer bupdateedge (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<3) {
    di << " Use bupdateedge> Edge Face\n";
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
  
  const TopoDS_Edge& aE=TopoDS::Edge(S1);
  const TopoDS_Face& aF=TopoDS::Face(S2);
  Handle(Geom2d_Curve) aC2D;

  Standard_Real aTol;
  aTol=BRep_Tool::Tolerance(aE);
  BRep_Builder BB;
  BB.UpdateEdge(aE, aC2D, aF, aTol);
  di << " Ok\n";
  return 0;
}

//=======================================================================
//function : bisclosed
//purpose  : 
//=======================================================================
Standard_Integer bisclosed (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<3) {
    di << " Use bisclosed> Edge Face\n";
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
  
  const TopoDS_Edge& aE=TopoDS::Edge(S1);
  const TopoDS_Face& aF=TopoDS::Face(S2);

  Standard_Boolean anIsClosed;
  anIsClosed=BRep_Tool::IsClosed(aE, aF); 

  if (!anIsClosed) {
    di << " Edge is not closed on this face\n";
  }
  else {
    di << " Edge is closed on this face\n";
  }
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
      BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF);  
    }
  }

  return 0;
}
  
//=======================================================================
//function : baddve
//purpose  : 
//=======================================================================
Standard_Integer baddve (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<6) {
    di << " Use baddve> E V1 V2 p1 p2\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  TopoDS_Shape S3 = DBRep::Get(a[3]);
  if (S1.IsNull() || S2.IsNull() || S3.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }

  if (S1.ShapeType()!=TopAbs_EDGE || 
      S2.ShapeType()!=TopAbs_VERTEX ||
      S3.ShapeType()!=TopAbs_VERTEX ) {
    di << " Type mismatch\n";
    return 1;
  }
  const TopoDS_Edge& aE=TopoDS::Edge(S1);
  const TopoDS_Vertex& aV1=TopoDS::Vertex(S2);
  const TopoDS_Vertex& aV2=TopoDS::Vertex(S3);


  Standard_Real p1, p2;
  p1=atof(a[4]);
  p2=atof(a[5]);
  
  TopoDS_Edge E=aE;
  E.EmptyCopy();

  BRep_Builder BB;
  BB.Add  (E, aV1);
  BB.Add  (E, aV2);
  BB.Range(E, p1, p2);

  DBRep::Set("ne", E);
  return 0;
}

static Handle(Geom2d_Curve) nullPCurve;
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

//=======================================================================
//function : bexplode
//purpose  : 
//=======================================================================
static Standard_Integer bexplode(Draw_Interpretor& di,
				 Standard_Integer n, 
				 const char** a)
{
  if (n <= 1) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  Standard_Integer i = 0;
  if (n == 2) {
    TopoDS_Iterator itr(S);
    while (itr.More()) {
      i++;
      sprintf(p,"%d",i);
      DBRep::Set(newname,itr.Value());
      di.AppendElement(newname);
      itr.Next();
    }
  }
  else {
    // explode a type
    TopAbs_ShapeEnum typ;
    switch (a[2][0]) {
      
    case 'C' :
    case 'c' :
      if ((a[2][1] == 'd')||(a[2][1] == 'D')) 
	typ = TopAbs_COMPOUND;
      else
	typ = TopAbs_COMPSOLID;
      break;
      
    case 'S' :
    case 's' :
      if ((a[2][1] == 'O')||(a[2][1] == 'o')) 
	typ = TopAbs_SOLID;
      else if ((a[2][1] == 'H')||(a[2][1] == 'h')) 
	typ = TopAbs_SHELL;
      else
	return 1;
      break;
      
    case 'F' :
    case 'f' :
      typ = TopAbs_FACE;
      break;
      
    case 'W' :
    case 'w' :
      typ = TopAbs_WIRE;
      break;
      
    case 'E' :
    case 'e' :
      typ = TopAbs_EDGE;
      break;
      
    case 'V' :
    case 'v' :
      typ = TopAbs_VERTEX;
      break;
      
      default :
	return 1;
    }
    
    
    TopExp_Explorer ex(S,typ);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Shape& aS = ex.Current();
      i++;
      sprintf(p,"%d",i);
      DBRep::Set(newname,aS);
      di.AppendElement(newname);
    }
  }
  return 0;
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
    sprintf(sbf, " Use >bclassify Solid Point [Tolerance=1.e-7]\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  
  if (aS.ShapeType()!=TopAbs_SOLID) {
    sprintf(sbf, " Shape type must be SOLID\n");
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
    aTol=atof(a[3]);
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
    sprintf(sbf, " Use >bclassify Face Point2d [Tol2D=Tol(Face)]\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  
  if (aS.ShapeType()!=TopAbs_FACE) {
    sprintf(sbf, " Shape type must be FACE\n");
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
    aTol=atof(a[3]);
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
//function : PrintState
//purpose  : 
//=======================================================================
void PrintState (Draw_Interpretor& aDI,
		 const TopAbs_State& aState)
{
  char sbf[512];	
  TCollection_AsciiString sIN("IN"), sOUT("OUT of"), sON("ON"), sUNKNOWN("UNKNOWN"); 
  //
  sprintf(sbf, "The point is "); aDI<<sbf;
  //
  switch (aState) {
  case TopAbs_IN:		
    sprintf(sbf, sIN.ToCString());
    break;
  case TopAbs_OUT:		
    sprintf(sbf, sOUT.ToCString());
    break;
  case TopAbs_ON:		
    sprintf(sbf, sON.ToCString());
    break;
  case TopAbs_UNKNOWN:		
    sprintf(sbf, sUNKNOWN.ToCString());
    break;
  default:
    sprintf(sbf, sUNKNOWN.ToCString()); 
    break;
  }
  aDI<<sbf; 
	//
  sprintf(sbf, " shape\n");
  aDI<<sbf;
  
}
//
//=======================================================================
//function : bhole
//purpose  : 
//=======================================================================
Standard_Integer bhole (Draw_Interpretor& aDI,
			Standard_Integer n, 
			const char** a)
{
  char sbf[512];	
  
  if (n!=2) {
    sprintf(sbf, " Use bhole Face\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  
  if (aS.ShapeType()!=TopAbs_FACE) {
    sprintf(sbf, " Shape type must be FACE\n");
    aDI<<sbf;
    return 1;
  }
  //
  Standard_Boolean bIsHole;
  Standard_Real aTol=1.e-7;
  IntTools_FClass2d aFClass2d;
  //
  const TopoDS_Face& aF=TopoDS::Face(aS);
  aTol=BRep_Tool::Tolerance(aF); 
  //
  aFClass2d.Init(aF, aTol);
  //
  bIsHole=aFClass2d.IsHole();
  printf(" bIsHole=%d\n", bIsHole);
  //
  return 0;
}
//


//
//=======================================================================
//class : X_Chr
//purpose  : 
//=======================================================================
class X_Chr {
 public:
//  X_Chr::X_Chr() {
  X_Chr() {
    Reset();
  }
  void Reset() {
    myTime=0.;
    myCnt=0;
  }
  void Start() {
    myChronometer.Reset();
    myChronometer.Start();
  }
  void Stop() {
    Standard_Real aTime;
    //
    myChronometer.Stop();
    myChronometer.Show(aTime);
    myTime=myTime+aTime;
    ++myCnt;
  }
  void Show(const char *pComment) {
    printf("  Tps: %7.2lf\tCnt: %d\tfor %s\n", myTime, myCnt, pComment);
  }
  
 protected:
  Standard_Real myTime;
  Standard_Integer myCnt;
  OSD_Chronometer myChronometer;
  
};

//
//=======================================================================
//function : bxhole
//purpose  : 
//=======================================================================
Standard_Integer bxhole (Draw_Interpretor& aDI,
			 Standard_Integer n, 
			 const char** a)
{
  char sbf[512];	
  Standard_Integer aNbIter;
  //
  if (n!=3) {
    sprintf(sbf, " Use bxhole Face NbIter\n");
    aDI<<sbf;
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    sprintf(sbf, " Null Shape is not allowed here\n");
    aDI<<sbf;
    return 1;
  }
  if (aS.ShapeType()!=TopAbs_FACE) {
    sprintf(sbf, " Shape type must be FACE\n");
    aDI<<sbf;
    return 1;
  }
  //
  aNbIter=atoi(a[2]);
  if (aNbIter<1) {
    aNbIter=1;
  }
  //
  Standard_Integer i;
  Standard_Boolean bIsHole;
  Standard_Real aTol, aUMin, aUMax, aVMin, aVMax;
  gp_Pnt2d aP2DInf;
  TopAbs_State aState;
  X_Chr aChr;
  //
  aTol=1.e-7;
  const TopoDS_Face& aF=TopoDS::Face(aS);
  aTol=BRep_Tool::Tolerance(aF); 
  //
  BRepTools::UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
  aP2DInf.SetCoord(aUMin, aVMin);
  //
  // 1. New
  aChr.Reset();
  aChr.Start();
  for (i=1; i<=aNbIter; ++i) {
    IntTools_FClass2d aFClass2d;
    //
    aFClass2d.Init(aF, aTol);
    //
    bIsHole=aFClass2d.IsHole();
  }
  aChr.Stop();
  aChr.Show(" new");
  printf(" new bIsHole=%d\n", bIsHole);
  //``````````````````````````````````````````
  aChr.Reset();
  aChr.Start();
  for (i=1; i<=aNbIter; ++i) {
    IntTools_FClass2d aFClass2d;
    //
    aFClass2d.Init(aF, aTol);
    //
    aState=aFClass2d.Perform(aP2DInf);
    bIsHole=(aState==TopAbs_IN);
  }
  aChr.Stop();
  aChr.Show(" was");
  printf(" was bIsHole=%d\n", bIsHole);
  //
  return 0;
}
