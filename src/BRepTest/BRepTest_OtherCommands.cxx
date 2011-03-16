// File:	BRepTest_OtherCommands.cxx
// Created:	Thu Apr 13 15:06:24 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <BRepTest.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>

#include <BRepIntCurveSurface_Inter.hxx>
#include <Geom_Line.hxx>
#include <DrawTrSurf.hxx>

#include <string.h>

#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>

#include <Standard_ErrorHandler.hxx>

//#ifdef WNT
#include <stdio.h>
//#endif

//=======================================================================
// shape : shape name V/E/W/F/SH/SO/CS/C
//=======================================================================

static Standard_Integer emptyshape(Draw_Interpretor& , Standard_Integer n, const char** a)
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
// subshape
//=======================================================================

static Standard_Integer subshape(Draw_Interpretor& di, Standard_Integer n, const char** a)
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
    Standard_Integer isub = atoi(a[2]);
    TopoDS_Iterator itr(S);
    while (itr.More()) {
      i++;
      if ( i == isub ) {
	sprintf(p,"%d",i);
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
    
    Standard_Integer isub = atoi(a[3]);
    TopTools_MapOfShape M;
    M.Add(S);
    TopExp_Explorer ex(S,typ);
    while (ex.More()) {
      if (M.Add(ex.Current())) {
	i++;
	if ( i == isub ) {
	  sprintf(p,"%d",i);
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
// subshape
//=======================================================================

static Standard_Integer brepintcs(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n <= 2) return 1;
  TopoDS_Shape S = DBRep::Get(a[n-1]);
  if (S.IsNull()) return 3;

  static BRepIntCurveSurface_Inter theAlg;
  static double tol=1e-6;
  static int nbpi=0;
  static gp_Pnt curp;

  if (n==3) {
    Handle(Geom_Curve) C= DrawTrSurf::GetCurve(a[1]);
    if (C.IsNull()) return 2;
    GeomAdaptor_Curve acur(C);
    theAlg.Init(S, acur, tol);
    for (; theAlg.More(); theAlg.Next()) {
      curp=theAlg.Pnt();
      nbpi++;
      char name[64];
      char* temp = name; // pour portage WNT
      sprintf(temp, "%s_%d", "brics", nbpi); 
      DrawTrSurf::Set(temp, curp);
    }
  }
  else {
    Handle(Geom_Line) hl;
    gp_Lin thel;
    for (Standard_Integer il = 1; il<n ; il++) {
      hl= Handle(Geom_Line)::DownCast(DrawTrSurf::GetCurve(a[il]));
      if (!hl.IsNull()) {
	thel=hl->Lin();
	  theAlg.Init(S, thel, tol);
	for (; theAlg.More(); theAlg.Next()) {
	  curp=theAlg.Pnt();
	  nbpi++;
	  char name[64];
	  char* temp = name; // pour portage WNT
	  sprintf(temp, "%s_%d", "brics", nbpi); 
	  DrawTrSurf::Set(temp, curp);
	}
      }
    }
  }
  //POP pour NT
  return 0;
}



#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <ElCLib.hxx>
#include <BRepFeat_MakeDPrism.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>
//#include <SaveShape.hxx>
#include <gp_Ax1.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_SequenceOfLin.hxx>
#include <TopAbs_Orientation.hxx>
#include <LocOpe_PntFace.hxx>

static void SampleEdges (const TopoDS_Shape&   theShape, TColgp_SequenceOfPnt& theSeq);
static TopoDS_Face NextFaceForPrism (const TopoDS_Shape& shape, 
				     const TopoDS_Shape& basis,
				     const gp_Ax1&       ax1);

//=======================================================================
// intgo
//=======================================================================
static Standard_Integer MakeBoss(Draw_Interpretor& , Standard_Integer , const char** a)
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

#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset.hxx>
//#include <SaveShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <GeomAbs_JoinType.hxx>

//=======================================================================
// Create a shell
//=======================================================================
static Standard_Integer MakeShell(Draw_Interpretor& , Standard_Integer , const char** a)
{

  TopoDS_Shape aShape = DBRep::Get( a[1] ); 
  TopTools_ListOfShape Lst;
  TopExp_Explorer Exp(aShape, TopAbs_FACE);
  TopoDS_Shape InputShape(DBRep::Get( a[2] ));
  TopoDS_Face F = TopoDS::Face(InputShape);
//  TopoDS_Face F = TopoDS::Face(DBRep::Get( a[2] ));
  
  Standard_Real Off = -atof( a[3] );

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
		  "Calcul d'intersection entre face et curve : BRepIntCS curve shape"
		  ,__FILE__,brepintcs,g);

  theCommands.Add("makeboss",
		  "create a boss on the shape myS", __FILE__, MakeBoss, g);

  theCommands.Add("mksh",
		  "create a shell on Shape", __FILE__, MakeShell, g);

}



//=======================================================================
//function : NextFaceForPrism
//purpose  : Search a face from <shape> which intersects with a line of
//           direction <ax1> and location a point of <basis>.
//=======================================================================
static TopoDS_Face NextFaceForPrism (const TopoDS_Shape& shape, 
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

#include <BRep_Tool.hxx>

//=======================================================================
//function : SampleEdges
//purpose  : Sampling of <theShape>.
//design   : Collect the vertices and points on the edges
//=======================================================================
static void SampleEdges (const TopoDS_Shape&   theShape, TColgp_SequenceOfPnt& theSeq)
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

