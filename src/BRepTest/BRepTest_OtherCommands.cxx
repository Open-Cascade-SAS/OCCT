// Created on: 1995-04-13
// Created by: Jean Yves LEBEY
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

#include <BRepTest.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>

#include <string.h>
#include <stdio.h>

#include <Standard_ErrorHandler.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>

#include <ElCLib.hxx>

#include <TColgp_SequenceOfPnt.hxx>

#include <GeomAbs_JoinType.hxx>
#include <Geom_Line.hxx>

#include <IntCurvesFace_Intersector.hxx>

#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopExp_Explorer.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_SequenceOfLin.hxx>
#include <LocOpe_PntFace.hxx>
#include <BRepFeat_MakeDPrism.hxx>

#include <BRepTools.hxx>
#include <BRepIntCurveSurface_Inter.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <GeomAdaptor_Curve.hxx>

static 
  void SampleEdges (const TopoDS_Shape&   theShape, 
		    TColgp_SequenceOfPnt& theSeq);
static 
  TopoDS_Face NextFaceForPrism (const TopoDS_Shape& shape, 
				const TopoDS_Shape& basis,
				const gp_Ax1&       ax1);
static
  void PrintState (Draw_Interpretor& aDI, 
		   const TopAbs_State& aState);
//
static Standard_Integer emptyshape(Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer subshape  (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer brepintcs (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer MakeBoss  (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer MakeShell (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer xbounds   (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer xclassify (Draw_Interpretor&, Standard_Integer, const char** );

//=======================================================================
//function : OtherCommands
//purpose  : 
//=======================================================================
void  BRepTest::OtherCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "TOPOLOGY other commands";

  theCommands.Add("shape",
		  "shape name V/E/W/F/Sh/So/CS/C; make a empty shape",__FILE__,emptyshape,g);

  theCommands.Add("subshape",
		  "subshape name V/E/W/F/Sh/So/CS/C index; get subsshape <index> of given type"
		  ,__FILE__,subshape,g);

  theCommands.Add("BRepIntCS",
    "Calcul d'intersection entre face et curve : BRepIntCS curve1 [curve2 ...] shape [res] [tol]"
		  ,__FILE__,brepintcs,g);

  theCommands.Add("makeboss",  "create a boss on the shape myS", __FILE__, MakeBoss, g);
  theCommands.Add("mksh", "create a shell on Shape", __FILE__, MakeShell, g);
  theCommands.Add("xbounds",  "xbounds face", __FILE__, xbounds, g);
  theCommands.Add("xclassify",  "use xclassify Solid [Tolerance=1.e-7]", __FILE__, xclassify, g);
  

}
//=======================================================================
//function : emptyshape
//purpose  : shape : shape name V/E/W/F/SH/SO/CS/C
//=======================================================================
Standard_Integer emptyshape(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;

  BRep_Builder B;
  TopoDS_Shape S;

  if (n == 3) {
    TCollection_AsciiString as(a[2]); as.LowerCase();
    const char* a2 = as.ToCString();

    if      ( ! strcmp(a2,"c") ) {
      TopoDS_Compound SS; B.MakeCompound(SS); S = SS;
    }
    else if ( ! strcmp(a2,"cs") ) {
      TopoDS_CompSolid SS; B.MakeCompSolid(SS); S = SS;
    }
    else if ( ! strcmp(a2,"so") ) {
      TopoDS_Solid SS; B.MakeSolid(SS); S = SS;
    }
    else if ( ! strcmp(a2,"sh") ) {
      TopoDS_Shell SS; B.MakeShell(SS); S = SS;
    }
    else if ( ! strcmp(a2,"f") ) {
      TopoDS_Face SS; B.MakeFace(SS); S = SS;
    }
    else if ( ! strcmp(a2,"w") ) {
      TopoDS_Wire SS; B.MakeWire(SS); S = SS;
    }
    else if ( ! strcmp(a2,"e") ) {
      TopoDS_Edge SS; B.MakeEdge(SS); S = SS;
    }
    else if ( ! strcmp(a2,"v") ) {
      TopoDS_Vertex SS; B.MakeVertex(SS); S = SS;
    }
    else {
      return 1;
    }
  }

  const char *shapename = a[1];
  DBRep::Set(shapename,S);
  return 0;
}

//=======================================================================
//function : subshape
//purpose  : 
//=======================================================================
Standard_Integer subshape(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 2) return 1;

  
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  Standard_Integer i = 0;
  if (n == 3) {
    Standard_Integer isub = Draw::Atoi(a[2]);
    TopoDS_Iterator itr(S);
    while (itr.More()) {
      i++;
      if ( i == isub ) {
	Sprintf(p,"%d",i);
	DBRep::Set(newname,itr.Value());
	di.AppendElement(newname);
	break;
      }
      itr.Next();
    }
  }
  else {
    // explode a type
    TopAbs_ShapeEnum typ;
    switch (a[2][0]) {
      
    case 'C' :
    case 'c' :
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
    
    Standard_Integer isub = Draw::Atoi(a[3]);
    TopTools_MapOfShape M;
    M.Add(S);
    TopExp_Explorer ex(S,typ);
    while (ex.More()) {
      if (M.Add(ex.Current())) {
	i++;
	if ( i == isub ) {
	  Sprintf(p,"%d",i);
	  DBRep::Set(newname,ex.Current());
	  di.AppendElement(newname);
	  break;
	}
      }
      ex.Next();
    }
  }
  return 0;
}
//=======================================================================
//function : brepintcs
//purpose  : 
//=======================================================================
Standard_Integer brepintcs(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 2) 
  {
    cout<<"Invalid input arguments. Should be: curve1 [curve2 ...] shape [result] [tol]"<<endl;
    return 1;
  }
  Standard_Integer indshape = 2;
  TopoDS_Shape S;
  for( ; indshape <= n-1 ; indshape++)
  {
    S = DBRep::Get(a[indshape]);
    if(!S.IsNull())
      break;
  }
  if (S.IsNull()) 
  {
    cout<<"Invalid input shape"<<endl;
    return 1;
  }

  BRepIntCurveSurface_Inter theAlg;
  double tol=1e-6;
  if( indshape < n-1)
  {
    Standard_Real preci = atof(a[n-1]);
    if(preci >= Precision::Confusion())
      tol = preci;
  }
  int nbpi=0;
  gp_Pnt curp;
  TopoDS_Compound aComp;
  BRep_Builder aB;
  aB.MakeCompound(aComp);
  if (indshape == 2) {
    Handle(Geom_Curve) C= DrawTrSurf::GetCurve(a[1]);
    if (C.IsNull()) return 2;
    GeomAdaptor_Curve acur(C);
    theAlg.Init(S, acur, tol);

    for (; theAlg.More(); theAlg.Next()) {
      curp=theAlg.Pnt();
      TopoDS_Vertex aV;

      aB.MakeVertex(aV, curp, 0);
      aB.Add(aComp, aV);
      nbpi++;
      di<<"Point "<<nbpi<<" : "<<curp.X()<<" "<<curp.Y()<<" "<<curp.Z()<<"\n";
      char name[64];
      char* temp = name; // pour portage WNT
      Sprintf(temp, "%s_%d", "brics", nbpi); 
      DrawTrSurf::Set(temp, curp);
    }
  }
  else {
    theAlg.Load(S,tol );
    for (Standard_Integer il = 1; il<indshape ; il++) 
    {
      Handle(Geom_Curve) hl= DrawTrSurf::GetCurve(a[il]);
      if (!hl.IsNull()) {
        theAlg.Init(hl);
        for (; theAlg.More(); theAlg.Next()) {
          curp=theAlg.Pnt();
          nbpi++;
          TopoDS_Vertex aV;
          aB.MakeVertex(aV, curp, 0);
          aB.Add(aComp, aV);
          di<<"Point "<<nbpi<<" : "<<curp.X()<<" "<<curp.Y()<<" "<<curp.Z()<<"\n";
          char name[64];
          char* temp = name; // pour portage WNT
          Sprintf(temp, "%s_%d", "brics", nbpi); 
          DrawTrSurf::Set(temp, curp);
        }
      }
    }
  }
  if(!nbpi)
    di<<"Points of intersections are not found\n";
  if(indshape < n-1)
    DBRep::Set(a[n-1], aComp);
  //POP pour NT
  return 0;
}
//=======================================================================
//function : MakeBoss
//purpose  : 
//=======================================================================
Standard_Integer MakeBoss(Draw_Interpretor& , Standard_Integer , const char** a)
{
  TopoDS_Shape myS = DBRep::Get( a[2] );

  TopoDS_Shape myBasis = DBRep::Get( a[3] ) ;

  Standard_Real ang = -0.05235987901687622;


  TopoDS_Face basis = TopoDS::Face(myBasis);

  BRepFeat_MakeDPrism DPRISM(myS, basis, basis, ang, 1, Standard_True);

  TopoDS_Shape myFaceOnShape;
  gp_Pnt Pnt(0.0, 0.0, 50.0);
  gp_Dir Dir(-0.0, -0.0, -1.0);
  gp_Ax1 ax(Pnt, Dir);
  
  myFaceOnShape = NextFaceForPrism(myS, myBasis, ax);

  DPRISM.Perform (myFaceOnShape);
  DPRISM.Build();

  if( DPRISM.IsDone() )  DBRep::Set( a[1], DPRISM.Shape() );

  return 0;
}
//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================
Standard_Integer MakeShell(Draw_Interpretor& , Standard_Integer , const char** a)
{

  TopoDS_Shape aShape = DBRep::Get( a[1] ); 
  TopTools_ListOfShape Lst;
  TopExp_Explorer Exp(aShape, TopAbs_FACE);
  TopoDS_Shape InputShape(DBRep::Get( a[2] ));
  TopoDS_Face F = TopoDS::Face(InputShape);
//  TopoDS_Face F = TopoDS::Face(DBRep::Get( a[2] ));
  
  Standard_Real Off = -Draw::Atof( a[3] );

  BRepOffset_MakeOffset Offset;

  Offset.Initialize( aShape, Off,  1.0e-3, BRepOffset_Skin, 
			       Standard_True , Standard_False , GeomAbs_Arc );
  Offset.AddFace( F );
  Offset.MakeThickSolid();

  if( Offset.IsDone() ) {
  //    SaveShape::Save(Offset.Shape(), "ss");
    DBRep::Set( a[1], Offset.Shape() );
  }
  return 0;
}
//=======================================================================
//function : xbounds
//purpose  : 
//=======================================================================
Standard_Integer xbounds(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n<2) {
    di << "Usage : " << a[0] << " face\n";
    return 0;
  }
  //
 
  Standard_Real aUMin, aUMax, aVMin, aVMax;
  TopoDS_Shape aS;
  TopoDS_Face aF;
  //
  aS=DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << " null shapes is not allowed here\n";
    return 0;
  }
  if (aS.ShapeType()!=TopAbs_FACE) {
    di << " shape" << a[1] <<" must be a face\n";
    return 0;
  }
  //
  aF=*((TopoDS_Face*)&aS);
  //
  BRepTools::UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
  //
  TCollection_AsciiString aStr;
  TCollection_AsciiString sUMin(aUMin);
  TCollection_AsciiString sUMax(aUMax);
  TCollection_AsciiString sVMin(aVMin);
  TCollection_AsciiString sVMax(aVMax);
  //
  aStr=aStr+sUMin + "\n";
  aStr=aStr+sUMax + "\n";
  aStr=aStr+sVMin + "\n";
  aStr=aStr+sVMax + "\n";
  di <<aStr.ToCString();
  //
  return 0;
}
//=======================================================================
//function : xclassify
//purpose  : 
//=======================================================================
Standard_Integer xclassify (Draw_Interpretor& aDI, Standard_Integer n, const char** a)
{
  if (n < 2) {
    aDI<<" use xclassify Solid [Tolerance=1.e-7]\n";
    return 1;
  }
  
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    aDI<<" Null Shape is not allowed here\n";
    return 0;
  }
  
  if (aS.ShapeType()!=TopAbs_SOLID) {
    aDI<< " Shape type must be SOLID\n";
    return 0;
  }
  //
  Standard_Real aTol=1.e-7;
  TopAbs_State aState = TopAbs_UNKNOWN;
  //
  aTol=1.e-7; 
  if (n==3) {
    aTol=Draw::Atof(a[2]);
  }
  //
  BRepClass3d_SolidClassifier aSC(aS);
  aSC.PerformInfinitePoint(aTol);
  
  aState = aSC.State();
  PrintState(aDI, aState);
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
  aDI<<"state is: ";
  switch (aState) {
  case TopAbs_IN:
    aDI<<"IN\n"; 
    break;
  case TopAbs_OUT:
    aDI<<"OUT\n";		
    break;
  case TopAbs_ON:	 
    aDI<<"ON\n";	
    break;
  case TopAbs_UNKNOWN:
  default:
    aDI<<"UNKNOWN\n";		
    break;
  }
}
//=======================================================================
//function : NextFaceForPrism
//purpose  : Search a face from <shape> which intersects with a line of
//           direction <ax1> and location a point of <basis>.
//=======================================================================
TopoDS_Face NextFaceForPrism (const TopoDS_Shape& shape, 
			      const TopoDS_Shape& basis,
			      const gp_Ax1&       ax1)
{
  TopoDS_Face nextFace;

  TColgp_SequenceOfPnt seqPnts;
  SampleEdges(basis, seqPnts);
  
  for (Standard_Integer i=1; i<=seqPnts.Length(); i++) {
    const gp_Pnt& pt = seqPnts(i);
    // find a axis through a face
    gp_Dir dir = ax1.Direction();
    gp_Ax1 ax1b(pt, dir);
    
    LocOpe_CSIntersector ASI(shape);
    LocOpe_SequenceOfLin slin;
    slin.Append(ax1b);
    ASI.Perform(slin);
    
    if (ASI.IsDone()) {
      Standard_Integer no=1, IndFrom, IndTo;
      TopAbs_Orientation theOr;
      Standard_Real min = 1.e-04, Tol = -Precision::Confusion();
      if (ASI.LocalizeAfter (no, min, Tol, theOr, IndFrom, IndTo))  {
	nextFace = ASI.Point(no, IndFrom).Face();  
	break;
      }
    }
  }
  
  return nextFace;
}


//=======================================================================
//function : SampleEdges
//purpose  : Sampling of <theShape>.
//design   : Collect the vertices and points on the edges
//=======================================================================
void SampleEdges (const TopoDS_Shape&   theShape, TColgp_SequenceOfPnt& theSeq)
{

  theSeq.Clear();

   
  TopTools_MapOfShape theMap;
  TopExp_Explorer exp;
  
  // Adds all vertices/pnt
  for (exp.Init(theShape,TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (theMap.Add(exp.Current())) {
      theSeq.Append (BRep_Tool::Pnt(TopoDS::Vertex(exp.Current())));
    }
  }

  // Computes points on edge, but does not take the extremities into account
  Standard_Integer NECHANT = 5;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
  for (exp.Init (theShape,TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (theMap.Add(edg)) {
      if (!BRep_Tool::Degenerated(edg)) {
	C = BRep_Tool::Curve(edg,f,l);
	for (Standard_Integer i=1; i < NECHANT; i++) {
	  prm = ((NECHANT-i)*f+i*l)/NECHANT;
	  theSeq.Append (C->Value(prm));
	}
      }
    }
  }
}

