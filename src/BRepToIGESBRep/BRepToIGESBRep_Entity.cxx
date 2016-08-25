// Created on: 1995-04-25
// Created by: Marie Jose MARTZ
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

// modif le 25/03/96 mjm
// implement ShapeCustom::DirectModification for indirect surfaces (out of norm IGES)
//:l4 abv 12 Jan 99: CTS22022-2: correct writing reversed shells
//:n3 abv 8 Feb 99: PRO17820: BRepTools::OuterWire() -> ShapeAnalysis::OuterWire
//szv#4 S4163
//S4181 pdn 20.04.99 implementing of writing IGES elementary surfaces.
//      abv 31.01.00 inheriting from BRepToIGES_BREntity to remove code duplication
//eap: Tue Aug 29 11:02:56 2000: Shape Processing moved to upper levels

#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepToIGES_BRWire.hxx>
#include <BRepToIGESBRep_Entity.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dToIGES_Geom2dCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_XYZ.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfFace.hxx>
#include <IGESSolid_HArray1OfLoop.hxx>
#include <IGESSolid_HArray1OfShell.hxx>
#include <IGESSolid_HArray1OfVertexList.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressIndicator.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <TransferBRep_OrientedShapeMapper.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//#include <GeomConvert.hxx>
//#include <Geom2dConvert.hxx>
//#include <ShapeAnalysis.hxx>
//#include <ShapeCustom.hxx>
//=======================================================================
//function : BRepToIGESBRep_Entity
//purpose  : 
//=======================================================================
BRepToIGESBRep_Entity::BRepToIGESBRep_Entity()
{  
  Init();
  if (myEdgeList.IsNull()) 
    myEdgeList = new IGESSolid_EdgeList;
  if (myVertexList.IsNull())
    myVertexList = new IGESSolid_VertexList;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepToIGESBRep_Entity::Clear()
{
  myVertices.Clear();
  myEdges.Clear();
  myCurves.Clear();
}


//=============================================================================
// TransferVertexList
// 
//=============================================================================

void BRepToIGESBRep_Entity::TransferVertexList()
{

  Standard_Integer nbvertices = myVertices.Extent();
  if(!nbvertices)
    return;
  Handle(TColgp_HArray1OfXYZ) vertices= new TColgp_HArray1OfXYZ(1,nbvertices);
  Standard_Real Unit = GetUnit();
  Standard_Real X,Y,Z;

  for (Standard_Integer ivertex = 1; ivertex <= nbvertices; ivertex++) {
    TopoDS_Shape myshape = myVertices(ivertex);
    TopoDS_Vertex myvertex = TopoDS::Vertex(myshape);
    gp_Pnt Point = BRep_Tool::Pnt(myvertex);
    Point.Coord(X,Y,Z);
    vertices->SetValue(ivertex, gp_XYZ(X/Unit,Y/Unit,Z/Unit));
  }

  myVertexList->Init(vertices);
}


//=============================================================================
// IndexVertex
//=============================================================================
Standard_Integer BRepToIGESBRep_Entity::IndexVertex(const TopoDS_Vertex& myvertex) const
{
  TopoDS_Shape V = myvertex;
  return myVertices.FindIndex(V);
}


//=============================================================================
// AddVertex
// 
//=============================================================================

Standard_Integer BRepToIGESBRep_Entity::AddVertex(const TopoDS_Vertex& myvertex) 
{
  if ( myvertex.IsNull()) return 0;
  
  TopoDS_Shape V = myvertex;
  Standard_Integer index = myVertices.FindIndex(V);
  if (index == 0) {
    index = myVertices.Add(V);
  }
  
  return index;
}


//=============================================================================
// TransferEdgeList
// 
//=============================================================================

void BRepToIGESBRep_Entity::TransferEdgeList()
{

  Handle(IGESSolid_VertexList) TheVertexList = myVertexList;

  Handle(IGESData_IGESEntity) mycurve;
  Standard_Integer mystartindex, myendindex;
  Handle(IGESSolid_VertexList) mystartlist;
  Handle(IGESSolid_VertexList) myendlist;

  Standard_Integer nbedges = myEdges.Extent();
  if(!nbedges)
    return;
  Handle(IGESData_HArray1OfIGESEntity) Curves= 
    new IGESData_HArray1OfIGESEntity(1,nbedges);
  Handle(IGESSolid_HArray1OfVertexList) startVertexList = 
    new IGESSolid_HArray1OfVertexList(1,nbedges);
  Handle(TColStd_HArray1OfInteger) startVertexIndex = 
    new TColStd_HArray1OfInteger(1,nbedges);
  Handle(IGESSolid_HArray1OfVertexList) endVertexList = 
    new IGESSolid_HArray1OfVertexList(1,nbedges);
  Handle(TColStd_HArray1OfInteger) endVertexIndex = 
    new TColStd_HArray1OfInteger(1,nbedges);

  for (Standard_Integer iedge = 1; iedge <= nbedges; iedge++) {
    TopoDS_Shape myshape = myEdges(iedge);
    TopoDS_Edge myedge = TopoDS::Edge(myshape);
    //  the curve 3D
    DeclareAndCast(IGESData_IGESEntity, amycurve, myCurves(iedge));
    Curves->SetValue(iedge, amycurve);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(myedge, V1, V2);
    //  vertices follow the orientation of curve 3d
    mystartindex = IndexVertex(V1);
    myendindex = IndexVertex(V2);
    startVertexIndex->SetValue(iedge, mystartindex);
    endVertexIndex->SetValue(iedge, myendindex);
    startVertexList->SetValue(iedge, TheVertexList);
    endVertexList->SetValue(iedge, TheVertexList);
  }

  myEdgeList->Init
    (Curves, startVertexList, startVertexIndex, endVertexList, endVertexIndex);
}


//=============================================================================
// IndexEdge
//=============================================================================
Standard_Integer BRepToIGESBRep_Entity::IndexEdge(const TopoDS_Edge& myedge) const
{
  TopoDS_Shape E = myedge;
  return myEdges.FindIndex(E);
}


//=============================================================================
// AddEdge
// 
//=============================================================================

Standard_Integer BRepToIGESBRep_Entity::AddEdge(const TopoDS_Edge& myedge,
						const Handle(IGESData_IGESEntity)& mycurve3d)
{
  if ( myedge.IsNull()) return 0;
  
  TopoDS_Shape E = myedge;
  Handle(IGESData_IGESEntity) C = mycurve3d;
  Standard_Integer index = myEdges.FindIndex(E);
  if (index == 0) {
    index = myEdges.Add(E);
    myCurves.Add(C);
  }
  
  return index;
}


//=======================================================================
//function : TransferShape
//purpose  : 
//=======================================================================
Handle(IGESData_IGESEntity) BRepToIGESBRep_Entity::TransferShape
(const TopoDS_Shape& start)
{
  Handle(IGESData_IGESEntity) res;
  //TopoDS_Shape theShape;

  if (start.IsNull()) return res;

  if (start.ShapeType() == TopAbs_VERTEX) {
    AddWarning (start, " A Vertex alone is not a IGESBRep Entity");
    TopoDS_Vertex V = TopoDS::Vertex(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferVertex(V);
    return res;
  }  
  else if (start.ShapeType() == TopAbs_EDGE) {
    AddWarning (start, " An Edge alone is not a IGESBRep Entity");
    TopoDS_Edge E =  TopoDS::Edge(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferEdge(E, Standard_False);
    return res;
  }  
  else if (start.ShapeType() == TopAbs_WIRE) {
    AddWarning (start, " An Wire alone is not a IGESBRep Entity");
    TopoDS_Wire W =  TopoDS::Wire(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferWire(W);
    return res;
  }  
  else { 
//    theShape = ShapeCustom::DirectFaces(start);
    if (start.ShapeType() == TopAbs_FACE) {
      TopoDS_Face F =  TopoDS::Face(start);
      res = TransferFace(F);
    }  
    else if (start.ShapeType() == TopAbs_SHELL) {
      TopoDS_Shell S =  TopoDS::Shell(start);
      res = TransferShell(S);
    }  
    else if (start.ShapeType() == TopAbs_SOLID) {
      TopoDS_Solid M =  TopoDS::Solid(start);
      res = TransferSolid(M);
    }  
    else if (start.ShapeType() == TopAbs_COMPSOLID) {
      TopoDS_CompSolid C =  TopoDS::CompSolid(start);
      res = TransferCompSolid(C);
    }  
    else if (start.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Compound C =  TopoDS::Compound(start);
      res = TransferCompound(C);
    }  
    else {
      // error message
    }  
  }
  if(res.IsNull())
    return res;

  TransferVertexList();
  TransferEdgeList();
  return res;
}


//=============================================================================
// TransferEdge
//=============================================================================

Handle(IGESData_IGESEntity)  BRepToIGESBRep_Entity::TransferEdge (const TopoDS_Edge& myedge)
{
  BRepToIGES_BRWire BR(*this);
  BR.SetModel(GetModel());
  return BR.TransferEdge (myedge, Standard_True);
}


//=============================================================================
// TransferEdge
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGESBRep_Entity::TransferEdge (const TopoDS_Edge& myedge,
								 const TopoDS_Face& myface,
								 const Standard_Real Length)
{
  Handle(IGESData_IGESEntity) ICurve3d;
  Handle(IGESData_IGESEntity) ICurve2d;
  if ( myedge.IsNull()) return ICurve2d;

  BRepToIGES_BRWire BR(*this);
  BR.SetModel(GetModel());
  ICurve2d = BR.TransferEdge (myedge, myface, Length, Standard_True);

  // curve 3d is obligatory. If it does not exist it is created and stored in "myCurves".
  // If the edge is degenerated, there is no associated 3d. So "edge-tuple" 
  // will be a Vertex.

  if (!BRep_Tool::Degenerated(myedge)) {
    ICurve3d = TransferEdge(myedge);
    if (ICurve3d.IsNull()) {
      AddFail (myedge, " Transfer Failed : no Curve 3D ");
    }
    AddEdge(myedge, ICurve3d);
  }
    
  return ICurve2d;
}


//=============================================================================
// TransferWire
//=============================================================================

Handle(IGESSolid_Loop) BRepToIGESBRep_Entity::TransferWire (const TopoDS_Wire& mywire,
							    const TopoDS_Face& myface,
							    const Standard_Real Length)
{
  Handle(IGESSolid_Loop) myLoop = new IGESSolid_Loop;
  if ( mywire.IsNull()) return myLoop;
  Handle(IGESData_IGESEntity) Pointeur;

  TColStd_SequenceOfInteger Seqindex;
  TColStd_SequenceOfInteger Seqorient;
  TColStd_SequenceOfInteger Seqtype;
  Handle(IGESData_IGESEntity) ent2d ;
  Handle(IGESData_IGESEntity) ent3d ;
  Handle(TColStd_HSequenceOfTransient) Seq2d = new TColStd_HSequenceOfTransient();

  BRepTools_WireExplorer WE;
  //Standard_Integer nbedge = 0; //szv#4:S4163:12Mar99 unused
  TopExp_Explorer TE(mywire, TopAbs_VERTEX);
  if ( TE.More()) {
    for ( WE.Init(mywire,myface); WE.More(); WE.Next()) { 
      TopoDS_Edge E = WE.Current();
      if (E.IsNull()) {
	AddWarning(mywire, "an Edge is a null entity");
      }
      else {
	ent2d = TransferEdge(E, myface, Length);
	Seq2d->Append(ent2d);
	Standard_Integer myindex; 

	// add Vertices in the Map "myVertices"
	TopoDS_Vertex V1, V2;
	TopExp::Vertices(E, V1, V2);
	//Standard_Integer Ivertex1, Ivertex2; //szv#4:S4163:12Mar99 not needed
	if (!BRep_Tool::Degenerated(E)) {
	  if ( !V1.IsNull()) {
	    AddVertex(V1); //szv#4:S4163:12Mar99 `Ivertex1=` not needed
	  }
	  if ( !V2.IsNull()) {
	    AddVertex(V2); //szv#4:S4163:12Mar99 `Ivertex2=` not needed
	  }
	  myindex = IndexEdge(E);
	  Seqtype.Append(0);
	}
	else {
	  myindex = AddVertex(V1);
	  Seqtype.Append(1);
	}
	Seqindex.Append(myindex);
	if (E.Orientation() == TopAbs_FORWARD ) Seqorient.Append(1);
	if (E.Orientation() == TopAbs_REVERSED) Seqorient.Append(0); 
      }
    }
  }
  else 
    AddWarning(mywire, " no Vertex associated to the Wire");

  Standard_Integer nbedges = Seq2d->Length();
  Handle(TColStd_HArray1OfInteger) types = new TColStd_HArray1OfInteger(1,nbedges);
  Standard_Integer mytype;
  Handle(IGESData_HArray1OfIGESEntity) edges = new IGESData_HArray1OfIGESEntity(1,nbedges);
  Handle(IGESData_IGESEntity) myedge;
  Handle(TColStd_HArray1OfInteger) index = new TColStd_HArray1OfInteger(1,nbedges);
  Standard_Integer myindex;
  Handle(TColStd_HArray1OfInteger) orient = new TColStd_HArray1OfInteger(1,nbedges);
  Standard_Integer myorient;
  Handle(TColStd_HArray1OfInteger) nbcurves = new TColStd_HArray1OfInteger(1,nbedges);
  Standard_Integer mynbcurve;
  Handle(TColStd_HArray1OfInteger) flag;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) isoflags = 
    new IGESBasic_HArray1OfHArray1OfInteger(1,nbedges);
  Standard_Integer myisoflag;
  Handle(IGESData_HArray1OfIGESEntity) curve;
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) curves = 
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1,nbedges);
  Handle(IGESData_IGESEntity) mycurve;


  for (Standard_Integer itab = 1; itab <= nbedges; itab++) {
    mytype = Seqtype.Value(itab);
    types->SetValue(itab,mytype);
    if ( mytype == 0) 
      Pointeur = myEdgeList;
    else
      Pointeur = myVertexList;
    edges->SetValue(itab,Pointeur);
    myindex = Seqindex.Value(itab);
    index->SetValue(itab, myindex);
    myorient = Seqorient.Value(itab);
    orient->SetValue(itab, myorient);
    mynbcurve = ( Seq2d->Value(itab).IsNull() ? 0 : 1 ); // abv 31 Jan 00: to be able not to write pcurves: was 1
    nbcurves->SetValue(itab, mynbcurve);
    myisoflag = 0;
    flag = new TColStd_HArray1OfInteger(1,1);  
    flag->SetValue(1,myisoflag);
    isoflags->SetValue(itab,flag);
    mycurve = GetCasted(IGESData_IGESEntity, Seq2d->Value(itab));
    curve = new IGESData_HArray1OfIGESEntity(1,1);
    curve->SetValue(1,mycurve);
    curves->SetValue(itab,curve);
  }
  
  myLoop->Init(types, edges, index, orient, nbcurves, isoflags, curves);

  SetShapeResult ( mywire, myLoop );

  return myLoop;
}


//=============================================================================
// TransferFace
// 
//=============================================================================

Handle(IGESSolid_Face) BRepToIGESBRep_Entity ::TransferFace(const TopoDS_Face& start)
{
  Handle(Message_ProgressIndicator) progress = GetTransferProcess()->GetProgress();
  if ( ! progress.IsNull() ) {
    if ( progress->UserBreak() ) return 0;
    progress->Increment();
  }
  
  Handle(IGESSolid_Face) myent = new IGESSolid_Face;
  if ( start.IsNull()) return myent;
  Handle(IGESData_IGESEntity) ISurf;
  Standard_Real Length = 1.;

  // returns the face surface, the face tolerance, the face natural restriction flag.
  // --------------------------------------------------------------------------------
  Handle(Geom_Surface) Surf = BRep_Tool::Surface(start);
  if (!Surf.IsNull()) {
    Standard_Real U1, U2, V1, V2;
    BRepTools::UVBounds(start, U1, U2, V1, V2);  // to limit the base surfaces
    GeomToIGES_GeomSurface GS;
    //S4181 pdn 17.04.99 Boolean flags in order to define write of elementary surfaces added.
    GS.SetBRepMode(Standard_True);
    GS.SetAnalyticMode ( Interface_Static::IVal("write.convertsurface.mode") ==0 );
    GS.SetModel(GetModel());

    Handle(Geom_Surface) st;
    if (Surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) { 
      DeclareAndCast(Geom_RectangularTrimmedSurface, rectang, Surf);
      st = rectang->BasisSurface();
    }
    else 
      st= Surf;

    //S4181 pdn 17.04.99 Geom_Plane translated into GeomToIGES_GeomSurface
    ISurf = GS.TransferSurface(st, U1, U2, V1, V2);
    if (ISurf.IsNull()) {
      AddWarning (start, "the basic surface is a null entity");
      return myent;
    }
    Length = GS.Length();
  }


  // returns the wires of start
  // --------------------------

  // to explore the face , it is required to set it FORWARD.
  TopoDS_Face myface = start;
  Standard_Boolean IsReversed = Standard_False;
  if (start.Orientation() == TopAbs_REVERSED) {
    myface.Reverse();
    IsReversed = Standard_True;
  }

  // outer wire
//:n3  TopoDS_Wire Outer = BRepTools::OuterWire(myface);
  TopoDS_Wire Outer = ShapeAlgo::AlgoContainer()->OuterWire(myface); //:n3 
  Handle(IGESSolid_Loop) OuterLoop = new IGESSolid_Loop;
  Standard_Boolean OuterLoopFlag = Standard_False;
  if (!Outer.IsNull()) {
    OuterLoopFlag = Standard_True;
    OuterLoop = TransferWire(Outer, myface, Length);
  }

  // inner wires
  TopExp_Explorer Ex;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(myface,TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Wire W = TopoDS::Wire(Ex.Current());
    Handle(IGESSolid_Loop) InnerLoop = new IGESSolid_Loop;
    if (W.IsNull()) {
      AddWarning(start," a Wire is a null entity");
    }
    else if (!W.IsSame(Outer)) {
      InnerLoop = TransferWire(W, myface, Length);
      if (!InnerLoop.IsNull()) Seq->Append(InnerLoop);
    }
  }

  // all inner edges not in a wire
  for (Ex.Init(myface,TopAbs_EDGE,TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Edge E = TopoDS::Edge(Ex.Current());
    AddWarning ( E, "An edge alone is not transfer as an IGESBRep Entity");
  }

  Standard_Integer nbent = Seq->Length();
  Handle(IGESSolid_HArray1OfLoop) TabLoop;
  TabLoop = new IGESSolid_HArray1OfLoop(1,nbent+1);
  TabLoop->SetValue(1,OuterLoop);
  if ( nbent >= 1) {
    for (Standard_Integer itab = 1; itab <= nbent; itab++) {
      Handle(IGESSolid_Loop) item = GetCasted(IGESSolid_Loop, Seq->Value(itab));
      TabLoop->SetValue(itab+1,item);
    }
  }

  // returns the Face
  // ----------------
  myent-> Init (ISurf, OuterLoopFlag, TabLoop);

  if (IsReversed) myface.Reverse();

  SetShapeResult ( start, myent );

  return myent;
}


//=============================================================================
// TransferShell
//=============================================================================

Handle(IGESSolid_Shell) BRepToIGESBRep_Entity ::TransferShell(const TopoDS_Shell& start)
{
  Handle(IGESSolid_Shell) myshell = new IGESSolid_Shell;
  if ( start.IsNull()) return myshell;

  TopExp_Explorer Ex;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();
  TColStd_SequenceOfInteger SeqFlag;
  Handle(IGESSolid_Face) IFace;

  for (Ex.Init(start,TopAbs_FACE); Ex.More(); Ex.Next()) {
    TopoDS_Face F = TopoDS::Face(Ex.Current());
    if ( start.Orientation() == TopAbs_REVERSED ) F.Reverse(); //:l4 abv 12 Jan 99: CTS22022-2: writing reversed shells
    if (F.IsNull()) {
      AddWarning(start," a Face is a null entity");
    }
    else {
      IFace = TransferFace(F);
      if (!IFace.IsNull()) {
	Seq->Append(IFace);
	if (F.Orientation() == TopAbs_FORWARD ) SeqFlag.Append(1);
	if (F.Orientation() == TopAbs_REVERSED) SeqFlag.Append(0); 
      }
    }
  }


  Standard_Integer nbfaces = Seq->Length();  
  Handle(IGESSolid_HArray1OfFace) TabFace = new IGESSolid_HArray1OfFace(1,nbfaces);
  Handle(TColStd_HArray1OfInteger) TabFlag = new TColStd_HArray1OfInteger(1,nbfaces);
  for (Standard_Integer itab = 1; itab <= nbfaces; itab++) {
    Handle(IGESSolid_Face) itemface = GetCasted(IGESSolid_Face, Seq->Value(itab));
    TabFace->SetValue(itab,itemface);
    Standard_Integer item = SeqFlag.Value(itab);
    TabFlag->SetValue(itab, item);
  }

  myshell->Init(TabFace,TabFlag);

  SetShapeResult ( start, myshell );

  return myshell;
}


//=============================================================================
// TransferSolid
// with a Solid
//=============================================================================

Handle(IGESSolid_ManifoldSolid) BRepToIGESBRep_Entity ::TransferSolid (const TopoDS_Solid& start)
{
  Handle(IGESSolid_ManifoldSolid) mysol = new IGESSolid_ManifoldSolid;
  if ( start.IsNull()) return mysol;

  TopExp_Explorer Ex;
  Handle(IGESSolid_Shell) IShell, FirstShell;
  Standard_Integer ShellFlag = 1;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();
  TColStd_SequenceOfInteger SeqFlag;

  for (Ex.Init(start,TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Shell S = TopoDS::Shell(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Shell is a null entity");
    }
    else {
      IShell = TransferShell(S);
      if (!IShell.IsNull()) { 
	Seq->Append(IShell);
	if (S.Orientation() == TopAbs_FORWARD ) SeqFlag.Append(1);
	if (S.Orientation() == TopAbs_REVERSED) SeqFlag.Append(0); 
      }
    }
  }


  Standard_Integer nbshells = Seq->Length();
  Handle(IGESSolid_HArray1OfShell) Tab;
  Handle(TColStd_HArray1OfInteger) TabFlag;
  if ( nbshells > 1) {
    Tab     =  new IGESSolid_HArray1OfShell(1,nbshells-1);
    TabFlag =  new TColStd_HArray1OfInteger(1,nbshells-1);
    for (Standard_Integer itab = 1; itab <= nbshells; itab++) {
      Handle(IGESSolid_Shell) itemShell = GetCasted(IGESSolid_Shell, Seq->Value(itab));
      Standard_Integer item = SeqFlag.Value(itab);
      if (itab == 1) {
	FirstShell = itemShell;
	ShellFlag = item;
      }
      else {
	Tab->SetValue(itab-1,itemShell);
	TabFlag->SetValue(itab-1, item);
      }
    }
  }

  if (nbshells == 1) {
    FirstShell = GetCasted(IGESSolid_Shell, Seq->Value(1));
    ShellFlag = SeqFlag.Value(1);
    Tab.Nullify();  TabFlag.Nullify();
    mysol->Init(FirstShell, ShellFlag != 0, Tab, TabFlag);
  }
  else if (nbshells >=2 ) {
    mysol->Init(FirstShell, ShellFlag != 0, Tab, TabFlag);
  }
  else
    AddWarning (start, " no Result ");

  SetShapeResult ( start, mysol );

  return mysol;
}


//=============================================================================
// TransferCompSolid
// with a CompSolid
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGESBRep_Entity::TransferCompSolid (const TopoDS_CompSolid& start)
{
  Handle(IGESData_IGESEntity) myent;
  if ( start.IsNull()) return myent;

  TopExp_Explorer Ex;
  Handle(IGESSolid_ManifoldSolid) ISolid = new IGESSolid_ManifoldSolid;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(start,TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Solid S = TopoDS::Solid(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," an Solid is a null entity");
    }
    else {
      ISolid = TransferSolid(S);
      if (!ISolid.IsNull()) Seq->Append(ISolid);
    }
  }


  Standard_Integer nbsolids = Seq->Length();
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if ( nbsolids > 1) {
    Tab =  new IGESData_HArray1OfIGESEntity(1,nbsolids);
    for (Standard_Integer itab = 1; itab <= nbsolids; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  if (nbsolids == 1) {
    myent = ISolid;
  }
  else if(nbsolids > 1 ){
    Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
    IGroup->Init(Tab);
    myent = IGroup;
  }

  SetShapeResult ( start, myent );

  return myent;
}


//=============================================================================
// TransferCompound
// with a Compound
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGESBRep_Entity::TransferCompound (const TopoDS_Compound& start)
{
  Handle(IGESData_IGESEntity) res;
  if ( start.IsNull()) return res;


  TopExp_Explorer Ex;
  Handle(IGESData_IGESEntity) IShape;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  // take all Solids
  for (Ex.Init(start, TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Solid S = TopoDS::Solid(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Solid is a null entity");
    }
    else {
      IShape = TransferSolid(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }

  // take all isolated Shells 
  for (Ex.Init(start, TopAbs_SHELL, TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Shell S = TopoDS::Shell(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Shell is a null entity");
    }
    else {
      IShape = TransferShell(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Faces
  for (Ex.Init(start, TopAbs_FACE, TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Face S = TopoDS::Face(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Face is a null entity");
    }
    else {
      IShape = TransferFace(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Wires 
  for (Ex.Init(start, TopAbs_WIRE, TopAbs_FACE); Ex.More(); Ex.Next()) {
    TopoDS_Wire S = TopoDS::Wire(Ex.Current());
    AddWarning(S," a Wire alone is not an IGESBRep entity : no Transfer");

    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    IShape = BW.TransferWire(S);
    if (!IShape.IsNull()) Seq->Append(IShape);
  }


  // take all isolated Edges 
  for (Ex.Init(start, TopAbs_EDGE, TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Edge S = TopoDS::Edge(Ex.Current());
    AddWarning(S," a Edge alone is not an IGESBRep entity : no Transfer");

    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    IShape = BW.TransferEdge(S, Standard_False);
    if (!IShape.IsNull()) Seq->Append(IShape);
  }


  // take all isolated Vertices 
  for (Ex.Init(start, TopAbs_VERTEX, TopAbs_EDGE); Ex.More(); Ex.Next()) {
    TopoDS_Vertex S = TopoDS::Vertex(Ex.Current());
    AddWarning(S," a Vertex alone is not an IGESBRep entity : no Transfer");

    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    IShape = BW.TransferVertex(S);
    if (!IShape.IsNull()) Seq->Append(IShape);
  }

  // construct the group
  Standard_Integer nbshapes = Seq->Length();
  if (nbshapes > 0) {
    Handle(IGESData_HArray1OfIGESEntity) Tab =
      new IGESData_HArray1OfIGESEntity(1,nbshapes);
    for (Standard_Integer itab = 1; itab <= nbshapes; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }

    Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
    IGroup->Init(Tab);
    res = IGroup;
  }  

  SetShapeResult ( start, res );

  return res;
}
