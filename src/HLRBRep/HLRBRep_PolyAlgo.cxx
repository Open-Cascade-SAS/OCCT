// Created on: 1995-05-05
// Created by: Christophe MARION
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

// Modified by cma, Tue Apr  1 11:39:48 1997
// Modified by cma, Tue Apr  1 11:40:30 1997

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>
#include <HLRAlgo_PolyAlgo.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_PolyInternalData.hxx>
#include <HLRAlgo_PolyMask.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_PolyAlgo,Standard_Transient)

enum
{
  NMsk_Vert =  1,
  NMsk_OutL =  2,
  NMsk_Norm =  4,
  NMsk_Fuck =  8,
  NMsk_Edge = 16,
  NMsk_Move = 32
};

#ifdef OCCT_DEBUG
static Standard_Integer DoTrace = Standard_False; 
static Standard_Integer DoError = Standard_False; 
#endif
//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo () :
myDebug     (Standard_False),
myAngle     (5 * M_PI / 180.),
myTolSta    (0.1),
myTolEnd    (0.9),
myTolAngular(0.001)
{
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const Handle(HLRBRep_PolyAlgo)& A)
{
  myDebug      = A->Debug();
  myAngle      = A->Angle();
  myTolAngular = A->TolAngular();
  myTolSta     = A->TolCoef();
  myTolEnd     = 1 - myTolSta;
  myAlgo       = A->Algo();
  myProj       = A->Projector();

  Standard_Integer n = A->NbShapes();

  for (Standard_Integer i = 1; i <= n; i++)
    Load(A->Shape(i));
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const TopoDS_Shape& S) :
myDebug     (Standard_False),
myAngle     (5 * M_PI / 180.),
myTolSta    (0.1),
myTolEnd    (0.9),
myTolAngular(0.001)
{
  myShapes.Append(S);
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape & HLRBRep_PolyAlgo::Shape (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if (I == 0 || I > myShapes.Length(),
			       "HLRBRep_PolyAlgo::Shape : unknown Shape");
  return myShapes(I);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Remove (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if (I == 0 || I > myShapes.Length(),
				"HLRBRep_PolyAlgo::Remove : unknown Shape");
  myShapes.Remove(I);
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_PolyAlgo::Index (const TopoDS_Shape& S) const
{
  Standard_Integer n = myShapes.Length();

  for (Standard_Integer i = 1; i <= n; i++)
    if (myShapes(i) == S) return i;

  return 0;
}

//=======================================================================
//function : Algo
//purpose  : 
//=======================================================================

Handle(HLRAlgo_PolyAlgo) HLRBRep_PolyAlgo::Algo () const
{
  return myAlgo;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Update ()
{
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();
  TopoDS_Shape Shape = MakeShape();

  if (!Shape.IsNull()) {
    TopExp_Explorer exshell;
    Standard_Boolean IsoledF,IsoledE;//,closed;
    TopLoc_Location L;
    TopTools_MapOfShape ShapeMap1,ShapeMap2;
    TopExp::MapShapes(Shape,TopAbs_EDGE,myEMap);
    TopExp::MapShapes(Shape,TopAbs_FACE,myFMap);
    Standard_Integer nbEdge = myEMap.Extent();
    Standard_Integer nbFace = myFMap.Extent();
    TColStd_Array1OfInteger   ES (0,nbEdge); // index of the Shell
    TColStd_Array1OfTransient PD (0,nbFace); // HLRAlgo_PolyData
    TColStd_Array1OfTransient PID(0,nbFace); // PolyInternalData
    Standard_Integer nbShell = InitShape(Shape,IsoledF,IsoledE);
    if (nbShell > 0) {
      TColStd_Array1OfTransient& Shell = myAlgo->PolyShell();
      Standard_Integer iShell = 0;
      
      for (exshell.Init(Shape, TopAbs_SHELL);
	   exshell.More(); 
	   exshell.Next())
	StoreShell(exshell.Current(),iShell,Shell,
		   Standard_False,Standard_False,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      if (IsoledF)
	StoreShell(Shape,iShell,Shell,IsoledF,Standard_False,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      if (IsoledE)
	StoreShell(Shape,iShell,Shell,Standard_False,IsoledE,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      myAlgo->Update();
    }
  }
}

//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================

TopoDS_Shape HLRBRep_PolyAlgo::MakeShape () const
{
  Standard_Integer n = myShapes.Length();
  Standard_Boolean FirstTime = Standard_True;
  BRep_Builder B;
  TopoDS_Shape Shape;
  
  for (Standard_Integer i = 1; i <= n; i++) {
    if (FirstTime) {
      FirstTime = Standard_False;
      B.MakeCompound(TopoDS::Compound(Shape));
    }
    B.Add(Shape,myShapes(i));
  }
  return Shape;
}
  
//=======================================================================
//function : InitShape
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_PolyAlgo::InitShape (const TopoDS_Shape& Shape,
			     Standard_Boolean& IsoledF,
			     Standard_Boolean& IsoledE)
{
  TopTools_MapOfShape ShapeMap0;
  Standard_Integer nbShell = 0;
  IsoledF = Standard_False;
  IsoledE = Standard_False;
  TopExp_Explorer exshell,exface,exedge;
  TopLoc_Location L;
  
  for (exshell.Init(Shape, TopAbs_SHELL);
       exshell.More(); 
       exshell.Next()) {
    Standard_Boolean withTrian = Standard_False;
    
    for (exface.Init(exshell.Current(), TopAbs_FACE);
	 exface.More(); 
	 exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());
      if (!BRep_Tool::Triangulation(F,L).IsNull()) {
	if (ShapeMap0.Add(F))
	  withTrian = Standard_True;
      }
    }
    if (withTrian) nbShell++;
  }
  
  for (exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
       exface.More() && !IsoledF; 
       exface.Next()) {
    const TopoDS_Face& F = TopoDS::Face(exface.Current());
    if (!BRep_Tool::Triangulation(F,L).IsNull()) {
      if (ShapeMap0.Add(F))
	IsoledF = Standard_True;
    }
  }
  if (IsoledF) nbShell++;
  
  for (exedge.Init(Shape, TopAbs_EDGE, TopAbs_FACE);
       exedge.More() && !IsoledE; 
       exedge.Next())
    IsoledE = Standard_True;
  if (IsoledE) nbShell++;
  if (nbShell > 0)
    myAlgo->Init(new TColStd_HArray1OfTransient(1,nbShell));
  return nbShell;
}

//=======================================================================
//function : StoreShell
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::StoreShell (const TopoDS_Shape& Shape,
				   Standard_Integer& iShell,
				   TColStd_Array1OfTransient& Shell,
				   const Standard_Boolean IsoledF,
				   const Standard_Boolean IsoledE,
				   TColStd_Array1OfInteger& ES,
				   TColStd_Array1OfTransient& PD,
				   TColStd_Array1OfTransient& PID,
				   TopTools_MapOfShape& ShapeMap1,
				   TopTools_MapOfShape& ShapeMap2)
{
  TopLoc_Location L;
  TopExp_Explorer exface,exedge;
  Standard_Integer f = 0,i,j;
  Standard_Integer nbFaceShell = 0;
  Standard_Boolean reversed;
  Standard_Boolean closed    = Standard_False;
  const gp_Trsf& T  = myProj.Transformation();
  const gp_Trsf& TI = myProj.InvertedTransformation();
  const gp_XYZ& tloc = T.TranslationPart();
  TLoc[0] = tloc.X();
  TLoc[1] = tloc.Y();
  TLoc[2] = tloc.Z();
  const gp_Mat& tmat = T.VectorialPart();
  TMat[0][0] = tmat.Value(1,1);
  TMat[0][1] = tmat.Value(1,2);
  TMat[0][2] = tmat.Value(1,3);
  TMat[1][0] = tmat.Value(2,1);
  TMat[1][1] = tmat.Value(2,2);
  TMat[1][2] = tmat.Value(2,3);
  TMat[2][0] = tmat.Value(3,1);
  TMat[2][1] = tmat.Value(3,2);
  TMat[2][2] = tmat.Value(3,3);
  const gp_XYZ& tilo = TI.TranslationPart();
  TILo[0] = tilo.X();
  TILo[1] = tilo.Y();
  TILo[2] = tilo.Z();
  const gp_Mat& tima = TI.VectorialPart();
  TIMa[0][0] = tima.Value(1,1);
  TIMa[0][1] = tima.Value(1,2);
  TIMa[0][2] = tima.Value(1,3);
  TIMa[1][0] = tima.Value(2,1);
  TIMa[1][1] = tima.Value(2,2);
  TIMa[1][2] = tima.Value(2,3);
  TIMa[2][0] = tima.Value(3,1);
  TIMa[2][1] = tima.Value(3,2);
  TIMa[2][2] = tima.Value(3,3);
  if (!IsoledE) {
    if (!IsoledF) {
      closed = Shape.Closed();
      if (!closed) {
	TopTools_IndexedMapOfShape EM;
	TopExp::MapShapes(Shape,TopAbs_EDGE,EM);
	Standard_Integer ie;
	Standard_Integer nbEdge = EM.Extent ();
	Standard_Integer *flag = new Standard_Integer[nbEdge + 1];

	for (ie = 1; ie <= nbEdge; ie++)
	  flag[ie] = 0;
	
	for (exedge.Init(Shape, TopAbs_EDGE);
	     exedge.More(); 
	     exedge.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(exedge.Current());
	  ie = EM.FindIndex(E);
	  TopAbs_Orientation orient = E.Orientation();
	  if (!BRep_Tool::Degenerated(E)) {
	    if      (orient == TopAbs_FORWARD ) flag[ie] += 1;
	    else if (orient == TopAbs_REVERSED) flag[ie] -= 1;
	  }
	}
	closed = Standard_True;
	
	for (ie = 1; ie <= nbEdge && closed; ie++)
	  closed = (flag[ie] == 0); 
	delete [] flag;
	flag = NULL;
      }
      
      exface.Init(Shape, TopAbs_FACE);
    }
    else
      exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
    
    for (; exface.More(); exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());
      if (!BRep_Tool::Triangulation(F,L).IsNull()) {
	if (ShapeMap1.Add(F))
	  nbFaceShell++;
      }
    }
  }
  if (nbFaceShell > 0 || IsoledE) {
    iShell++;
    Shell(iShell) = new HLRAlgo_PolyShellData(nbFaceShell);
  }
  if (nbFaceShell > 0) {
    const Handle(HLRAlgo_PolyShellData)& psd =
      *(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell));
    Standard_Integer iFace = 0;
    if (!IsoledF) exface.Init(Shape, TopAbs_FACE);
    else          exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
    TopTools_MapOfShape ShapeMapBis;
    
    for (; exface.More(); exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());  
      const Handle(Poly_Triangulation)& Tr = BRep_Tool::Triangulation(F,L);
      if (!Tr.IsNull()) {
	if (ShapeMap2.Add(F)) {
	  iFace++;
	  f = myFMap.FindIndex(F);
	  reversed = F.Orientation() == TopAbs_REVERSED;
	  gp_Trsf TT = L.Transformation();
	  TT.PreMultiply(T);
	  const gp_XYZ& ttlo = TT.TranslationPart();
	  TTLo[0] = ttlo.X();
	  TTLo[1] = ttlo.Y();
	  TTLo[2] = ttlo.Z();
	  const gp_Mat& ttma = TT.VectorialPart();
	  TTMa[0][0] = ttma.Value(1,1);
	  TTMa[0][1] = ttma.Value(1,2);
	  TTMa[0][2] = ttma.Value(1,3);
	  TTMa[1][0] = ttma.Value(2,1);
	  TTMa[1][1] = ttma.Value(2,2);
	  TTMa[1][2] = ttma.Value(2,3);
	  TTMa[2][0] = ttma.Value(3,1);
	  TTMa[2][1] = ttma.Value(3,2);
	  TTMa[2][2] = ttma.Value(3,3);
	  Poly_Array1OfTriangle & Tri = Tr->ChangeTriangles();
	  TColgp_Array1OfPnt    & Nod = Tr->ChangeNodes();
	  Standard_Integer nbN = Nod.Upper();
	  Standard_Integer nbT = Tri.Upper();
	  PD (f) = new HLRAlgo_PolyData();
	  psd->PolyData().ChangeValue(iFace) = PD(f);
	  PID(f) = new HLRAlgo_PolyInternalData(nbN,nbT);
	  Handle(HLRAlgo_PolyInternalData)& pid = 
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(f));
	  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
	  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
	    S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
	  GeomAdaptor_Surface AS(S);
	  pid->Planar(AS.GetType() == GeomAbs_Plane);
	  HLRAlgo_Array1OfTData* TData = &pid->TData();
	  HLRAlgo_Array1OfPISeg* PISeg = &pid->PISeg();
	  HLRAlgo_Array1OfPINod* PINod = &pid->PINod();
	  Poly_Triangle       * OT = &(Tri.ChangeValue(1));
	  HLRAlgo_TriangleData* NT = &TData->ChangeValue(1);

	  for (i = 1; i <= nbT; i++) {
	    OT->Get(NT->Node1, NT->Node2, NT->Node3);
	    NT->Flags = 0;
	    if (reversed) {
	      j         = NT->Node1;
	      NT->Node1 = NT->Node3;
	      NT->Node3 = j;
	    }
	    OT++;
	    NT++;
	  }

	  gp_Pnt                          * ON = &(Nod.ChangeValue(1));
	  Handle(HLRAlgo_PolyInternalNode)* NN = &PINod->ChangeValue(1);

	  for (i = 1; i <= nbN; i++) {
	    HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = (*NN)->Data();
	    HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices = (*NN)->Indices();
	    aNodIndices.NdSg = 0;
	    aNodIndices.Flag = 0;
      Nod1RValues.Point = ON->Coord();
	    TTMultiply(Nod1RValues.Point);
	    ON++;
	    NN++;
	  }
	  pid->UpdateLinks(TData,PISeg,PINod);
	  if (Tr->HasUVNodes()) {
	    myBSurf.Initialize(F,Standard_False);
	    TColgp_Array1OfPnt2d & UVN = Tr->ChangeUVNodes();
	    gp_Pnt2d* OUVN = &(UVN.ChangeValue(1));
	    NN             = &(((HLRAlgo_Array1OfPINod*)PINod)->
			       ChangeValue(1));
	    
	    for (i = 1; i <= nbN; i++) {
	      HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices = (*NN)->Indices();
	      HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = (*NN)->Data();
        Nod1RValues.UV = OUVN->Coord();
	      if (Normal(i,aNodIndices,Nod1RValues,
			 TData,PISeg,PINod,Standard_False))
		aNodIndices.Flag |=  NMsk_Norm;
	      else {
		aNodIndices.Flag &= ~NMsk_Norm;
		Nod1RValues.Scal = 0;
	      }
	      OUVN++;
	      NN++;
	    }
	  }
#ifdef OCCT_DEBUG
	  else if (DoError) {
	    cout << " HLRBRep_PolyAlgo::StoreShell : Face ";
	    cout << f << " non triangulated" << endl;
	  }
#endif
	  NT = &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));

	  for (i = 1; i <= nbT; i++) {
	    const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
        &PINod->ChangeValue(NT->Node1);
	    const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
        &PINod->ChangeValue(NT->Node2);
	    const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
        &PINod->ChangeValue(NT->Node3);
	    HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = (*PN1)->Data();
	    HLRAlgo_PolyInternalNode::NodeData& Nod2RValues = (*PN2)->Data();
	    HLRAlgo_PolyInternalNode::NodeData& Nod3RValues = (*PN3)->Data();
	    OrientTriangle(i,*NT,
			   (*PN1)->Indices(),Nod1RValues,
			   (*PN2)->Indices(),Nod2RValues,
			   (*PN3)->Indices(),Nod3RValues);
	    NT++;
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::StoreShell : Face ";
	cout << f << " deja stockee" << endl;
      }
#endif
    }
    Standard_Integer nbFace = myFMap.Extent();
    HLRAlgo_ListOfBPoint& List = psd->Edges();
    TopTools_IndexedDataMapOfShapeListOfShape EF;
    TopExp::MapShapesAndAncestors(Shape,TopAbs_EDGE,TopAbs_FACE,EF);
    Handle(HLRAlgo_PolyInternalData)* pid = 
	(Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      if (!(*pid).IsNull()) {

	for (exedge.Init(myFMap(f),TopAbs_EDGE);
	     exedge.More(); 
	     exedge.Next()) {
	  TopoDS_Edge E = TopoDS::Edge(exedge.Current());
	  if (ShapeMap1.Add(E)) {
	    Standard_Integer e = myEMap.FindIndex(E);
	    ES(e) = iShell;
	    Standard_Integer anIndexE = EF.FindIndex(E);
	    if (anIndexE > 0) {
	      TopTools_ListOfShape& LS = EF(anIndexE);
	      InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_True);
	    }
	    else {
	      TopTools_ListOfShape LS;
	      InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_False);
	    }
	  }
	}
      }
      pid++;
    }
    InsertOnOutLine(PID);
    CheckFrBackTriangles(List,PID);
    UpdateOutLines(List,PID);
    UpdateEdgesBiPoints(List,PID,closed);
    UpdatePolyData(PD,PID,closed);
    pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      (*pid).Nullify();  
      pid++;
    }
  }
  else if (IsoledE) {
    const Handle(HLRAlgo_PolyShellData)& psd =
      *(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell));
    HLRAlgo_ListOfBPoint& List = psd->Edges();
    
    for (exedge.Init(Shape, TopAbs_EDGE, TopAbs_FACE);
	 exedge.More(); 
	 exedge.Next()) {
      TopoDS_Edge E = TopoDS::Edge(exedge.Current());
      if (ShapeMap1.Add(E)) {
	Standard_Integer e = myEMap.FindIndex(E);
	ES(e) = iShell;
	TopTools_ListOfShape LS;
	InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_False);
      }
    }
  }
}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_PolyAlgo::
Normal (const Standard_Integer iNode,
	HLRAlgo_PolyInternalNode::NodeIndices& theNodIndices,
	HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
        HLRAlgo_Array1OfTData*& TData,
        HLRAlgo_Array1OfPISeg*& PISeg,
        HLRAlgo_Array1OfPINod*& PINod,
        const Standard_Boolean orient) const
{
  gp_Vec D1U,D1V,D2U,D2V,D2UV;
  gp_Pnt P;
  gp_Dir Norma;
  Standard_Boolean OK;
  CSLib_DerivativeStatus aStatus;
  CSLib_NormalStatus NStat;
  myBSurf.D1(Nod1RValues.UV.X(), Nod1RValues.UV.Y(), P, D1U, D1V);
  CSLib::Normal(D1U,D1V,Standard_Real(Precision::Angular()),
		aStatus,Norma);
  if (aStatus != CSLib_Done) {
    myBSurf.D2(Nod1RValues.UV.X(), Nod1RValues.UV.Y(), P, D1U, D1V, D2U, D2V, D2UV);
    CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,
		  Precision::Angular(),OK,NStat,Norma);
    if (!OK)
      return Standard_False;
  }
  Standard_Real EyeX =  0;
  Standard_Real EyeY =  0;
  Standard_Real EyeZ = -1;
  if (myProj.Perspective()) {
    EyeX = Nod1RValues.Point.X();
    EyeY = Nod1RValues.Point.Y();
    EyeZ = Nod1RValues.Point.Z() - myProj.Focus();
    Standard_Real d = sqrt(EyeX * EyeX + EyeY * EyeY + EyeZ * EyeZ);
    if (d > 0) {
      EyeX /= d;
      EyeY /= d;
      EyeZ /= d;
    }
  }
  Nod1RValues.Normal = Norma.XYZ();
//  TMultiply(Nod1NrmX,Nod1NrmY,Nod1NrmZ);
  TMultiply(Nod1RValues.Normal,myProj.Perspective()); //OCC349
  gp_XYZ Norm;
  
  if (AverageNormal(iNode,theNodIndices,TData,PISeg,PINod,Norm))
  {
    if (Nod1RValues.Normal * Norm < 0)
    {
      Nod1RValues.Normal.Reverse();
    }
    Nod1RValues.Scal = (Nod1RValues.Normal * gp_XYZ(EyeX, EyeY, EyeZ));
  }
  else {
    Nod1RValues.Scal = 0;
    Nod1RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
    if (DoError) {
      cout << "HLRBRep_PolyAlgo::Normal : AverageNormal error";
      cout << endl;
    }
#endif
  }
  if (Nod1RValues.Scal > 0) {
    if ( Nod1RValues.Scal < myTolAngular) {
      Nod1RValues.Scal  = 0;
      theNodIndices.Flag |= NMsk_OutL;
    }
  }
  else {
    if (-Nod1RValues.Scal < myTolAngular) {
      Nod1RValues.Scal  = 0;
      theNodIndices.Flag |= NMsk_OutL;
    }
  }
  if (orient) UpdateAroundNode(iNode,theNodIndices,
			       TData,PISeg,PINod);
  return Standard_True;
}

//=======================================================================
//function : AverageNormal
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::AverageNormal(const Standard_Integer iNode,
				HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
				HLRAlgo_Array1OfTData*& TData,
				HLRAlgo_Array1OfPISeg*& PISeg,
				HLRAlgo_Array1OfPINod*& PINod,
				Standard_Real& X,
				Standard_Real& Y,
				Standard_Real& Z) const
{
  Standard_Boolean OK = Standard_False;
  Standard_Integer jNode = 0,kNode,iiii,iTri1,iTri2;
  X = 0;
  Y = 0;
  Z = 0;
  iiii = theNodeIndices.NdSg;

  while (iiii != 0 && !OK) {
    HLRAlgo_PolyInternalSegment& aSegIndices =
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii);
    iTri1 = aSegIndices.Conex1;
    iTri2 = aSegIndices.Conex2;
    if ( iTri1 != 0) AddNormalOnTriangle
      (iTri1,iNode,jNode,TData,PINod,X,Y,Z,OK);
    if ( iTri2 != 0) AddNormalOnTriangle
      (iTri2,iNode,jNode,TData,PINod,X,Y,Z,OK);
    if (aSegIndices.LstSg1 == iNode) iiii = aSegIndices.NxtSg1;
    else                     iiii = aSegIndices.NxtSg2;
  }

  if (jNode != 0) {
    iiii = theNodeIndices.NdSg;
    
    while (iiii != 0 && !OK) {
      HLRAlgo_PolyInternalSegment& aSegIndices = PISeg->ChangeValue(iiii);
      iTri1 = aSegIndices.Conex1;
      iTri2 = aSegIndices.Conex2;
      if ( iTri1 != 0) AddNormalOnTriangle
	(iTri1,jNode,kNode,TData,PINod,X,Y,Z,OK);
      if ( iTri2 != 0) AddNormalOnTriangle
	(iTri2,jNode,kNode,TData,PINod,X,Y,Z,OK);
      if (aSegIndices.LstSg1 == jNode) iiii = aSegIndices.NxtSg1;
      else                     iiii = aSegIndices.NxtSg2;
    }
  }
  Standard_Real d = sqrt (X * X + Y * Y + Z * Z);
  if (OK && d < 1.e-10) {
    OK = Standard_False;
#ifdef OCCT_DEBUG
    if (DoError) {
      cout << "HLRAlgo_PolyInternalData:: inverted normals on ";
      cout << "node " << iNode << endl;
    }
#endif
  }
  return OK;
}

//=======================================================================
//function : AddNormalOnTriangle
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
AddNormalOnTriangle(const Standard_Integer iTri,
		    const Standard_Integer iNode,
		    Standard_Integer& jNode,
		    HLRAlgo_Array1OfTData*& TData,
		    HLRAlgo_Array1OfPINod*& PINod,
		    Standard_Real& X,
		    Standard_Real& Y,
		    Standard_Real& Z,
		    Standard_Boolean& OK) const
{
  HLRAlgo_TriangleData& aTriangle = TData->ChangeValue(iTri);
  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues =
    PINod->ChangeValue(aTriangle.Node1)->Data();
  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues =
    PINod->ChangeValue(aTriangle.Node2)->Data();
  HLRAlgo_PolyInternalNode::NodeData& Nod3RValues =
    PINod->ChangeValue(aTriangle.Node3)->Data();
  const gp_XYZ aD1 = Nod2RValues.Point - Nod1RValues.Point;
  const Standard_Real aD1Norm = aD1.Modulus();
  if (aD1Norm < 1.e-10) {
    if      (aTriangle.Node1 == iNode) jNode = aTriangle.Node2;
    else if (aTriangle.Node2 == iNode) jNode = aTriangle.Node1;
  }
  else {
    const gp_XYZ aD2 = Nod3RValues.Point - Nod2RValues.Point;
    const Standard_Real aD2Norm = aD2.Modulus();
    if (aD2Norm < 1.e-10) {
      if      (aTriangle.Node2 == iNode) jNode = aTriangle.Node3;
      else if (aTriangle.Node3 == iNode) jNode = aTriangle.Node2;
    }
    else {
      const gp_XYZ aD3 = Nod1RValues.Point - Nod3RValues.Point;
      const Standard_Real aD3Norm = aD3.Modulus();
      if (aD3Norm < 1.e-10) {
	if      (aTriangle.Node3 == iNode) jNode = aTriangle.Node1;
	else if (aTriangle.Node1 == iNode) jNode = aTriangle.Node3;
      }
      else {
        const gp_XYZ aDN = (1 / (aD1Norm * aD2Norm)) * (aD1 ^ aD2);
        const Standard_Real aDNNorm = aDN.Modulus();
	if (aDNNorm > 1.e-10) {
	  OK = Standard_True;
	  X += aDN.X();
	  Y += aDN.Y();
	  Z += aDN.Z();
	}
      }
    }
  }
}

//=======================================================================
//function : InitBiPointsWithConnexity
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
InitBiPointsWithConnexity (const Standard_Integer e,
			   TopoDS_Edge& E,
			   HLRAlgo_ListOfBPoint& List,
			   TColStd_Array1OfTransient& PID,
			   TopTools_ListOfShape& LS,
			   const Standard_Boolean connex)
{
  Standard_Integer iPol,nbPol,i1,i1p1,i1p2,i2,i2p1,i2p2;
  Standard_Real X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ;
  Standard_Real XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2;
  Standard_Real U1,U2 = 0.;
  Handle(Poly_PolygonOnTriangulation) HPol[2];
  TopLoc_Location L;
  myBCurv.Initialize(E);
  if (connex) {
    Standard_Integer nbConnex = LS.Extent();
    if      (nbConnex == 1) {
      TopTools_ListIteratorOfListOfShape itn(LS);
      const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
      i1      = myFMap.FindIndex(F1);
      const Handle(Poly_Triangulation)& Tr1 = BRep_Tool::Triangulation(F1,L);
      HPol[0] = BRep_Tool::PolygonOnTriangulation(E,Tr1,L);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i1));
      if (!HPol[0].IsNull()) {
	myPC.Initialize(E,F1);
	const Handle(TColStd_HArray1OfReal)& par = HPol[0]->Parameters();
	const TColStd_Array1OfInteger&      Pol1 = HPol[0]->Nodes();
	nbPol = Pol1.Upper();
	HLRAlgo_Array1OfTData* TData1 = &pid1->TData();
	HLRAlgo_Array1OfPISeg* PISeg1 = &pid1->PISeg();
	HLRAlgo_Array1OfPINod* PINod1 = &pid1->PINod();
  HLRAlgo_PolyInternalNode::NodeIndices* aNode11Indices;
  HLRAlgo_PolyInternalNode::NodeIndices* aNode12Indices;
	const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(1    )));
	aNode11Indices = &(*pi1p1)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod11RValues = &(*pi1p1)->Data();
	const Handle(HLRAlgo_PolyInternalNode)* pi1p2 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(nbPol)));
	aNode12Indices = &(*pi1p2)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod12RValues = &(*pi1p2)->Data();
	aNode11Indices->Flag |=  NMsk_Vert;
	aNode12Indices->Flag |=  NMsk_Vert;
	
	for (iPol = 1; iPol <= nbPol; iPol++) {
	  const Handle(HLRAlgo_PolyInternalNode)* pi1pA =
	    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	  HLRAlgo_PolyInternalNode::NodeIndices& aNodeIndices1A = (*pi1pA)->Indices();
	  HLRAlgo_PolyInternalNode::NodeData& Nod1ARValues = (*pi1pA)->Data();
	  if (aNodeIndices1A.Edg1 == 0 || aNodeIndices1A.Edg1 == e) {
	    aNodeIndices1A.Edg1 = e;
	    Nod1ARValues.PCu1 = par->Value(iPol);
	  }
	  else {
	    aNodeIndices1A.Edg2 = e;
	    Nod1ARValues.PCu2 = par->Value(iPol);
	  }
	}
	
	i1p2 = Pol1(1);
	aNode12Indices = aNode11Indices;
	Nod12RValues = Nod11RValues;
	XTI2 = X2 = Nod12RValues->Point.X();
	YTI2 = Y2 = Nod12RValues->Point.Y();
	ZTI2 = Z2 = Nod12RValues->Point.Z();
	if      (aNode12Indices->Edg1 == e) U2 = Nod12RValues->PCu1;
	else if (aNode12Indices->Edg2 == e) U2 = Nod12RValues->PCu2;
#ifdef OCCT_DEBUG
	else {
	  cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	  cout << "Parameter error on Node " << i1p2 << endl;
	}
#endif
	aNode12Indices->Flag |= NMsk_Edge;
	TIMultiply(XTI2,YTI2,ZTI2);
	if (Pol1(1) == Pol1(nbPol) && myPC.IsPeriodic())
	  U2 = U2 - myPC.Period();
	
	if (nbPol == 2 && BRep_Tool::Degenerated(E)) {
	  CheckDegeneratedSegment(*aNode11Indices,*Nod11RValues,
				  *aNode12Indices,*Nod12RValues);
	  UpdateAroundNode(Pol1(1    ),*aNode11Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol1(nbPol),*aNode12Indices,TData1,PISeg1,PINod1);
	}
	else {

	  for (iPol = 2; iPol <= nbPol; iPol++) {
	    i1p1 = i1p2;
	    aNode11Indices = aNode12Indices;
	    Nod11RValues = Nod12RValues;
	    i1p2 = Pol1(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2iPol =
	      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	    aNode12Indices = &(*pi1p2iPol)->Indices();
	    Nod12RValues = &(*pi1p2iPol)->Data();
#ifdef OCCT_DEBUG
	    if (DoError) {
	      if (Nod11RValues->Normal.X()*Nod12RValues->Normal.X() +
		  Nod11RValues->Normal.Y()*Nod12RValues->Normal.Y() +
		  Nod11RValues->Normal.Z()*Nod12RValues->Normal.Z() < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "Too big angle between " << i1p1 << setw(6);
		cout << " and " << i1p2 << setw(6);
		cout << " in face " << i1 << endl;
	      }
	    }
#endif
	    X1   = X2;
	    Y1   = Y2;
	    Z1   = Z2;
	    XTI1 = XTI2;
	    YTI1 = YTI2;
	    ZTI1 = ZTI2;
	    U1   = U2;
	    XTI2 = X2 = Nod12RValues->Point.X();
	    YTI2 = Y2 = Nod12RValues->Point.Y();
	    ZTI2 = Z2 = Nod12RValues->Point.Z();
	    if      (aNode12Indices->Edg1 == e) U2 = Nod12RValues->PCu1;
	    else if (aNode12Indices->Edg2 == e) U2 = Nod12RValues->PCu2;
#ifdef OCCT_DEBUG
	    else {
	      cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	      cout << "Parameter error on Node " << i1p2 << endl;
	    }
#endif
	    aNode12Indices->Flag |= NMsk_Edge;
	    TIMultiply(XTI2,YTI2,ZTI2);
	    Interpolation(List,
			  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			  XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			  e,U1,U2,
			  *aNode11Indices,*Nod11RValues,
			  *aNode12Indices,*Nod12RValues,
			  i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1);
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
	cout << e << " connex 1 sans PolygonOnTriangulation" << endl;
      }
#endif
    }
    else if (nbConnex == 2) {
      TopTools_ListIteratorOfListOfShape itn(LS);
      const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
      i1      = myFMap.FindIndex(F1);
      const Handle(Poly_Triangulation)& Tr1 = BRep_Tool::Triangulation(F1,L);
      HPol[0] = BRep_Tool::PolygonOnTriangulation(E,Tr1,L);
      itn.Next();
      const TopoDS_Face& F2 = TopoDS::Face(itn.Value());
      i2      = myFMap.FindIndex(F2);
      if (i1 == i2) E.Reverse();
      const Handle(Poly_Triangulation)& Tr2 = BRep_Tool::Triangulation(F2,L);
      HPol[1] = BRep_Tool::PolygonOnTriangulation(E,Tr2,L);
      GeomAbs_Shape rg = BRep_Tool::Continuity(E,F1,F2);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i1));
      const Handle(HLRAlgo_PolyInternalData)& pid2 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i2));
      if (!HPol[0].IsNull() && !HPol[1].IsNull()) {
	myPC.Initialize(E,F1);
	const TColStd_Array1OfInteger&      Pol1 = HPol[0]->Nodes();
	const TColStd_Array1OfInteger&      Pol2 = HPol[1]->Nodes();
	const Handle(TColStd_HArray1OfReal)& par = HPol[0]->Parameters();
	Standard_Integer nbPol1 = Pol1.Upper();
	HLRAlgo_Array1OfTData* TData1 = &pid1->TData();
	HLRAlgo_Array1OfPISeg* PISeg1 = &pid1->PISeg();
	HLRAlgo_Array1OfPINod* PINod1 = &pid1->PINod();
	HLRAlgo_Array1OfTData* TData2 = &pid2->TData();
	HLRAlgo_Array1OfPISeg* PISeg2 = &pid2->PISeg();
	HLRAlgo_Array1OfPINod* PINod2 = &pid2->PINod();
	const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	  &PINod1->ChangeValue(Pol1(1));
	HLRAlgo_PolyInternalNode::NodeIndices* aNode11Indices = &(*pi1p1)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod11RValues = &(*pi1p1)->Data();
	const Handle(HLRAlgo_PolyInternalNode)* pi1p2nbPol1 =
	  &PINod1->ChangeValue(Pol1(nbPol1));
	HLRAlgo_PolyInternalNode::NodeIndices* aNode12Indices = &(*pi1p2nbPol1)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod12RValues = &(*pi1p2nbPol1)->Data();
	const Handle(HLRAlgo_PolyInternalNode)* pi2p1 =
	  &PINod2->ChangeValue(Pol2(1));
	HLRAlgo_PolyInternalNode::NodeIndices* Nod21Indices = &(*pi2p1)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod21RValues = &(*pi2p1)->Data();
	const Handle(HLRAlgo_PolyInternalNode)* pi2p2 =
    &PINod2->ChangeValue(Pol2(nbPol1));
	HLRAlgo_PolyInternalNode::NodeIndices* Nod22Indices = &(*pi2p2)->Indices();
  HLRAlgo_PolyInternalNode::NodeData* Nod22RValues = &(*pi2p2)->Data();
	aNode11Indices->Flag |=  NMsk_Vert;
	aNode12Indices->Flag |=  NMsk_Vert;
	Nod21Indices->Flag |=  NMsk_Vert;
	Nod22Indices->Flag |=  NMsk_Vert;
	
	for (iPol = 1; iPol <= nbPol1; iPol++) {
	  const Handle(HLRAlgo_PolyInternalNode)* pi1pA =
	    &PINod1->ChangeValue(Pol1(iPol));
	  HLRAlgo_PolyInternalNode::NodeIndices* Nod1AIndices = &(*pi1pA)->Indices();
    HLRAlgo_PolyInternalNode::NodeData* Nod1ARValues = &(*pi1pA)->Data();
	  const Handle(HLRAlgo_PolyInternalNode)* pi2pA =
      &PINod2->ChangeValue(Pol2(iPol));
	  HLRAlgo_PolyInternalNode::NodeIndices* Nod2AIndices = &(*pi2pA)->Indices();
    HLRAlgo_PolyInternalNode::NodeData* Nod2ARValues = &(*pi2pA)->Data();
	  Standard_Real PCu = par->Value(iPol);
	  if (Nod1AIndices->Edg1 == 0 || Nod1AIndices->Edg1 == e) {
	    Nod1AIndices->Edg1 = e;
	    Nod1ARValues->PCu1 = PCu;
	  }
	  else {
	    Nod1AIndices->Edg2 = e;
	    Nod1ARValues->PCu2 = PCu;
	  }
	  if (Nod2AIndices->Edg1 == 0 || Nod2AIndices->Edg1 == e) {
	    Nod2AIndices->Edg1 = e;
	    Nod2ARValues->PCu1 = PCu;
	  }
	  else {
	    Nod2AIndices->Edg2 = e;
	    Nod2ARValues->PCu2 = PCu;
	  }
	}

	i1p2 = Pol1(1);
	aNode12Indices = aNode11Indices;
	Nod12RValues = Nod11RValues;
	i2p2 = Pol2(1);
	Nod22Indices = Nod21Indices;
	Nod22RValues = Nod21RValues;
	XTI2 = X2 = Nod12RValues->Point.X();
	YTI2 = Y2 = Nod12RValues->Point.Y();
	ZTI2 = Z2 = Nod12RValues->Point.Z();
	if      (aNode12Indices->Edg1 == e) U2 = Nod12RValues->PCu1;
	else if (aNode12Indices->Edg2 == e) U2 = Nod12RValues->PCu2;
#ifdef OCCT_DEBUG
	else {
	  cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	  cout << "Parameter error on Node " << i1p2 << endl;
	}
#endif
	aNode12Indices->Flag |= NMsk_Edge;
	Nod22Indices->Flag |= NMsk_Edge;
	TIMultiply(XTI2,YTI2,ZTI2);
	if (Pol1(1) == Pol1(nbPol1) && myPC.IsPeriodic())
	  U2 = U2 - myPC.Period();
	
	if (nbPol1 == 2 && BRep_Tool::Degenerated(E)) {
	  CheckDegeneratedSegment(*aNode11Indices,*Nod11RValues,
				  *aNode12Indices,*Nod12RValues);
	  CheckDegeneratedSegment(*Nod21Indices,*Nod21RValues,
				  *Nod22Indices,*Nod22RValues);
	  UpdateAroundNode(Pol1(1    ),*aNode11Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol1(nbPol1),*aNode12Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol2(1    ),*Nod21Indices,TData2,PISeg2,PINod2);
	  UpdateAroundNode(Pol2(nbPol1),*Nod22Indices,TData2,PISeg2,PINod2);
	}
	else {

	  for (iPol = 2; iPol <= nbPol1; iPol++) {
	    i1p1 = i1p2;
	    aNode11Indices = aNode12Indices;
	    Nod11RValues = Nod12RValues;
	    i2p1 = i2p2;
	    Nod21Indices = Nod22Indices;
	    Nod21RValues = Nod22RValues;
	    i1p2 = Pol1(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2iPol =
	      &PINod1->ChangeValue(Pol1(iPol));
	    aNode12Indices = &(*pi1p2iPol)->Indices();
      Nod12RValues = &(*pi1p2iPol)->Data();
	    i2p2 = Pol2(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi2p2iPol =
        &PINod2->ChangeValue(Pol2(iPol));
	    Nod22Indices = &(*pi2p2iPol)->Indices();
      Nod22RValues = &(*pi2p2iPol)->Data();
#ifdef OCCT_DEBUG
	    if (DoError) {
	      if (Nod11RValues->Normal.X()*Nod12RValues->Normal.X() +
		  Nod11RValues->Normal.Y()*Nod12RValues->Normal.Y() +
		  Nod11RValues->Normal.Z()*Nod12RValues->Normal.Z() < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "To big angle between " << i1p1 << setw(6);
		cout << " and " << i1p2 << setw(6);
		cout << " in face " << i1 << endl;
	      }
	      if (Nod21RValues->Normal.X()*Nod22RValues->Normal.X() +
		  Nod21RValues->Normal.Y()*Nod22RValues->Normal.Y() +
		  Nod21RValues->Normal.Z()*Nod22RValues->Normal.Z() < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "To big angle between " << i2p1 << setw(6);
		cout << " and " << i2p2 << setw(6);
		cout<< " in face " << i2 << endl;
	      }
	    }
#endif
	    X1   = X2;
	    Y1   = Y2;
	    Z1   = Z2;
	    XTI1 = XTI2;
	    YTI1 = YTI2;
	    ZTI1 = ZTI2;
	    U1   = U2;
	    XTI2 = X2 = Nod12RValues->Point.X();
	    YTI2 = Y2 = Nod12RValues->Point.Y();
	    ZTI2 = Z2 = Nod12RValues->Point.Z();
	    if      (aNode12Indices->Edg1 == e) U2 = Nod12RValues->PCu1;
	    else if (aNode12Indices->Edg2 == e) U2 = Nod12RValues->PCu2;
#ifdef OCCT_DEBUG
	    else {
	      cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	      cout << "Parameter error on Node " << i1p2 << endl;
	    }
#endif
	    aNode12Indices->Flag |= NMsk_Edge;
	    aNode12Indices->Flag |= NMsk_Edge;
	    TIMultiply(XTI2,YTI2,ZTI2);
	    Interpolation(List,
			  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			  XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			  e,U1,U2,rg,
			  *aNode11Indices,*Nod11RValues,
			  *aNode12Indices,*Nod12RValues,
			  i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			  *Nod21Indices,*Nod21RValues,
			  *Nod22Indices,*Nod22RValues,
			  i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2);
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
	cout << e << " connect 2 without PolygonOnTriangulation" << endl;
      }
#endif
    }
  }
  else {  // no connexity
    const Handle(Poly_Polygon3D)& Polyg = BRep_Tool::Polygon3D(E,L);
    if (!Polyg.IsNull()) {
      const TColgp_Array1OfPnt& Pol = Polyg->Nodes();
      gp_Trsf TT       = L.Transformation();
      const gp_Trsf& T = myProj.Transformation();
      TT.PreMultiply(T);
      const gp_XYZ& ttlo = TT.TranslationPart();
      TTLo[0] = ttlo.X();
      TTLo[1] = ttlo.Y();
      TTLo[2] = ttlo.Z();
      const gp_Mat& ttma = TT.VectorialPart();
      TTMa[0][0] = ttma.Value(1,1);
      TTMa[0][1] = ttma.Value(1,2);
      TTMa[0][2] = ttma.Value(1,3);
      TTMa[1][0] = ttma.Value(2,1);
      TTMa[1][1] = ttma.Value(2,2);
      TTMa[1][2] = ttma.Value(2,3);
      TTMa[2][0] = ttma.Value(3,1);
      TTMa[2][1] = ttma.Value(3,2);
      TTMa[2][2] = ttma.Value(3,3);
      Standard_Integer nbPol1 = Pol.Upper();
      const gp_XYZ& P1 = Pol(1).XYZ();
      X2 = P1.X();
      Y2 = P1.Y();
      Z2 = P1.Z();
      TTMultiply(X2,Y2,Z2);
      XTI2 = X2;
      YTI2 = Y2;
      ZTI2 = Z2;
      TIMultiply(XTI2,YTI2,ZTI2);
      
      for (Standard_Integer jPol = 2; jPol <= nbPol1; jPol++) {
	X1   = X2;
	Y1   = Y2;
	Z1   = Z2;
	XTI1 = XTI2;
	YTI1 = YTI2;
	ZTI1 = ZTI2;
	const gp_XYZ& P2 = Pol(jPol).XYZ();
	X2 = P2.X();
	Y2 = P2.Y();
	Z2 = P2.Z();
	TTMultiply(X2,Y2,Z2);
	XTI2 = X2;
	YTI2 = Y2;
	ZTI2 = Z2;
	TIMultiply(XTI2,YTI2,ZTI2);
	List.Prepend(HLRAlgo_BiPoint
		     (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		      X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		      0));
      }
    }
#ifdef OCCT_DEBUG
    else if (DoError) {
      cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
      cout << e << " Isolated, without Polygone 3D" << endl;
    }
#endif
  }
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
Interpolation (HLRAlgo_ListOfBPoint& List,
	       Standard_Real& X1,
	       Standard_Real& Y1,
	       Standard_Real& Z1,
	       Standard_Real& X2,
	       Standard_Real& Y2,
	       Standard_Real& Z2,
	       Standard_Real& XTI1,
	       Standard_Real& YTI1,
	       Standard_Real& ZTI1,
	       Standard_Real& XTI2,
	       Standard_Real& YTI2,
	       Standard_Real& ZTI2,
	       const Standard_Integer e,
	       Standard_Real& U1,
	       Standard_Real& U2,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod11RValues,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod12RValues,
	       const Standard_Integer i1p1,
	       const Standard_Integer i1p2,
	       const Standard_Integer i1,
	       const Handle(HLRAlgo_PolyInternalData)& pid1,
	       HLRAlgo_Array1OfTData*& TData1,
	       HLRAlgo_Array1OfPISeg*& PISeg1,
	       HLRAlgo_Array1OfPINod*& PINod1) const
{
  Standard_Boolean insP3,mP3P1;
  Standard_Real X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3;
//  gp_Pnt P3,PT3;
  insP3 = Interpolation(U1,U2,Nod11RValues,Nod12RValues,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,mP3P1);
  MoveOrInsertPoint(List,
		    X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
		    XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		    e,U1,U2,
		    Nod11Indices,Nod11RValues,
		    Nod12Indices,Nod12RValues,
		    i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
		    X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,0);
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
Interpolation (HLRAlgo_ListOfBPoint& List,
	       Standard_Real& X1,
	       Standard_Real& Y1,
	       Standard_Real& Z1,
	       Standard_Real& X2,
	       Standard_Real& Y2,
	       Standard_Real& Z2,
	       Standard_Real& XTI1,
	       Standard_Real& YTI1,
	       Standard_Real& ZTI1,
	       Standard_Real& XTI2,
	       Standard_Real& YTI2,
	       Standard_Real& ZTI2,
	       const Standard_Integer e,
	       Standard_Real& U1,
	       Standard_Real& U2,
	       const GeomAbs_Shape rg,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod11RValues,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod12RValues,
	       const Standard_Integer i1p1,
	       const Standard_Integer i1p2,
	       const Standard_Integer i1,
	       const Handle(HLRAlgo_PolyInternalData)& pid1,
	       HLRAlgo_Array1OfTData*& TData1,
	       HLRAlgo_Array1OfPISeg*& PISeg1,
	       HLRAlgo_Array1OfPINod*& PINod1,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod21RValues,
	       HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices,
	       HLRAlgo_PolyInternalNode::NodeData& Nod22RValues,
	       const Standard_Integer i2p1,
	       const Standard_Integer i2p2,
	       const Standard_Integer i2,
	       const Handle(HLRAlgo_PolyInternalData)& pid2,
	       HLRAlgo_Array1OfTData*& TData2,
	       HLRAlgo_Array1OfPISeg*& PISeg2,
	       HLRAlgo_Array1OfPINod*& PINod2) const
{
  Standard_Boolean insP3,mP3P1,insP4,mP4P1;
  Standard_Real X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3;
  Standard_Real X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4;
//  gp_Pnt P3,PT3,P4,PT4;
  Standard_Integer flag = 0;
  if (rg >= GeomAbs_G1) flag += 1;
  if (rg >= GeomAbs_G2) flag += 2;
  insP3 = Interpolation(U1,U2,Nod11RValues,Nod12RValues,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,mP3P1);
  insP4 = Interpolation(U1,U2,Nod21RValues,Nod22RValues,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,mP4P1);
  Standard_Boolean OK = insP3 || insP4;
  if (OK) {
    if      (!insP4)                               // p1 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else if (!insP3)                               // p1 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else if (Abs(coef4 - coef3) < myTolSta)       // p1 i1p3-i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else if (coef4 < coef3)                        // p1 i2p4 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else                                           // p1 i1p3 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::
Interpolation (const Standard_Real U1,
	       const Standard_Real U2,
	       HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
	       HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
	       Standard_Real& X3,
	       Standard_Real& Y3,
	       Standard_Real& Z3,
	       Standard_Real& XTI3,
	       Standard_Real& YTI3,
	       Standard_Real& ZTI3,
	       Standard_Real& coef3,
	       Standard_Real& U3,
	       Standard_Boolean& mP3P1) const
{
  if (NewNode(Nod1RValues,Nod2RValues,coef3,mP3P1)) {
    U3 = U1 + (U2 - U1) * coef3;
    const gp_Pnt& P3 = myBCurv.Value(U3);
    XTI3 = X3 = P3.X();
    YTI3 = Y3 = P3.Y();
    ZTI3 = Z3 = P3.Z();
    TMultiply(X3,Y3,Z3);
    return Standard_True;
  }
  X3 = Y3 = Z3 = XTI3 = YTI3 = ZTI3 = coef3 = U3 = 0.;
  return Standard_False;
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod11RValues,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   HLRAlgo_Array1OfTData*& TData1,
		   HLRAlgo_Array1OfPISeg*& PISeg1,
		   HLRAlgo_Array1OfPINod*& PINod1,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Integer flag) const
{
  HLRAlgo_Array1OfTData* TData2 = 0;
  HLRAlgo_Array1OfPISeg* PISeg2 = 0;
  HLRAlgo_Array1OfPINod* PINod2 = 0;
  Standard_Boolean ins3 = insP3;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Indices.Flag & NMsk_Vert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod11Indices.Edg1 == e) Nod11RValues.PCu1 = U3;
      else if (Nod11Indices.Edg2 == e) Nod11RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11RValues.Scal  = 0;
      Nod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      HLRAlgo_BiPoint::PointsT& aPoints = List.First().Points();
      aPoints.PntP2 = gp_XYZ(X3, Y3, Z3);
      aPoints.Pnt2 = gp_XYZ(XTI3, YTI3, ZTI3);
    }
  }
  if (ins3 && !mP3P1) {                            // P2 ---> P3
    if (!(Nod12Indices.Flag & NMsk_Vert) && coef3 > myTolEnd) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData1,PISeg1,PINod1);
      X2   = X3;
      Y2   = Y3;
      Z2   = Z3;
      XTI2 = XTI3;
      YTI2 = YTI3;
      ZTI2 = ZTI3;
      U2   = U3;
      Nod12RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod12Indices.Edg1 == e) Nod12RValues.PCu1 = U3;
      else if (Nod12Indices.Edg2 == e) Nod12RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12RValues.Scal  = 0;
      Nod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
    }
  }
  if (ins3) {                                      // p1 i1p3 p2
    Standard_Integer i1p3 = pid1->AddNode
      (Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
    HLRAlgo_PolyInternalNode::NodeIndices& Nod13Indices = (*pi1p3)->Indices();
    HLRAlgo_PolyInternalNode::NodeData& Nod13RValues = (*pi1p3)->Data();
    Nod13Indices.Edg1  = e;
    Nod13RValues.PCu1  = U3;
    Nod13RValues.Scal  = 0;
    Nod13Indices.Flag |= NMsk_OutL;
    Nod13Indices.Flag |= NMsk_Edge;
    pid1->UpdateLinks(i1p1,i1p2,i1p3,
		      TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
    UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		  X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		  i1  ,i1p1,i1p3,flag));
    List.Prepend(HLRAlgo_BiPoint
		 (XTI3,YTI3,ZTI3,XTI2,YTI2,ZTI2,
		  X3  ,Y3  ,Z3  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p3,i1p2,flag));
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,flag));
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod11RValues,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   HLRAlgo_Array1OfTData*& TData1,
		   HLRAlgo_Array1OfPISeg*& PISeg1,
		   HLRAlgo_Array1OfPINod*& PINod1,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod21RValues,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod22RValues,
		   const Standard_Integer i2p1,
		   const Standard_Integer i2p2,
		   const Standard_Integer i2,
		   const Handle(HLRAlgo_PolyInternalData)& pid2,
		   HLRAlgo_Array1OfTData*& TData2,
		   HLRAlgo_Array1OfPISeg*& PISeg2,
		   HLRAlgo_Array1OfPINod*& PINod2,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Integer flag) const
{
  Standard_Boolean ins3 = insP3;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Indices.Flag & NMsk_Vert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData2,PISeg2,PINod2);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod11Indices.Edg1 == e) Nod11RValues.PCu1 = U3;
      else if (Nod11Indices.Edg2 == e) Nod11RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11RValues.Scal  = 0;
      Nod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      Nod21RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod21Indices.Edg1 == e) Nod21RValues.PCu1 = U3;
      else if (Nod21Indices.Edg2 == e) Nod21RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p1 << endl;
      }
#endif
      Nod21RValues.Scal  = 0;
      Nod21Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i2p1,Nod21Indices,TData2,PISeg2,PINod2);
      HLRAlgo_BiPoint::PointsT& aPoints = List.First().Points();
      aPoints.PntP2 = gp_XYZ(X3, Y3, Z3);
      aPoints.Pnt2 = gp_XYZ(XTI3, YTI3, ZTI3);
    }
  }
  if (ins3 && !mP3P1) {                            // P2 ---> P3
    if (!(Nod12Indices.Flag & NMsk_Vert) && coef3 > myTolEnd) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData2,PISeg2,PINod2);
      X2   = X3;
      Y2   = Y3;
      Z2   = Z3;
      XTI2 = XTI3;
      YTI2 = YTI3;
      ZTI2 = ZTI3;
      U2   = U3;
      Nod12RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod12Indices.Edg1 == e) Nod12RValues.PCu1 = U3;
      else if (Nod12Indices.Edg2 == e) Nod12RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12RValues.Scal  = 0;
      Nod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
      Nod22RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod22Indices.Edg1 == e) Nod22RValues.PCu1 = U3;
      else if (Nod22Indices.Edg2 == e) Nod22RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p2 << endl;
      }
#endif
      Nod22RValues.Scal = 0;
      Nod22Indices.Flag |=  NMsk_OutL;
      UpdateAroundNode(i2p2,Nod22Indices,TData2,PISeg2,PINod2);
    }
  }
  if (ins3) {                                      // p1 i1p3 p2
    Standard_Integer i1p3 = pid1->AddNode
      (Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
    Standard_Integer i2p3 = pid2->AddNode
      (Nod21RValues,Nod22RValues,PINod2,PINod1,coef3,X3,Y3,Z3); 
    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
    HLRAlgo_PolyInternalNode::NodeIndices& Nod13Indices = (*pi1p3)->Indices();
    HLRAlgo_PolyInternalNode::NodeData& Nod13RValues = (*pi1p3)->Data();
    const Handle(HLRAlgo_PolyInternalNode)* pi2p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p3));
    HLRAlgo_PolyInternalNode::NodeIndices& Nod23Indices = (*pi2p3)->Indices();
    HLRAlgo_PolyInternalNode::NodeData& Nod23RValues = (*pi2p3)->Data();
    Nod13Indices.Edg1  = e;
    Nod13RValues.PCu1  = U3;
    Nod13RValues.Scal  = 0;
    Nod13Indices.Flag |= NMsk_OutL;
    Nod13Indices.Flag |= NMsk_Edge;
    Nod23Indices.Edg1  = e;
    Nod23RValues.PCu1  = U3;
    Nod23RValues.Scal  = 0;
    Nod23Indices.Flag |= NMsk_OutL;
    Nod23Indices.Flag |= NMsk_Edge;
    pid1->UpdateLinks(i1p1,i1p2,i1p3,
		      TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
    pid2->UpdateLinks(i2p1,i2p2,i2p3,
		      TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
    UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
    UpdateAroundNode(i2p3,Nod23Indices,TData2,PISeg2,PINod2);
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		  X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		  i1  ,i1p1,i1p3,i2  ,i2p1,i2p3,flag));
    List.Prepend(HLRAlgo_BiPoint
		 (XTI3,YTI3,ZTI3,XTI2,YTI2,ZTI2,
		  X3  ,Y3  ,Z3  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p3,i1p2,i2  ,i2p3,i2p2,flag));
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod11RValues,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod12Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   HLRAlgo_Array1OfTData*& TData1,
		   HLRAlgo_Array1OfPISeg*& PISeg1,
		   HLRAlgo_Array1OfPINod*& PINod1,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod21RValues,
		   HLRAlgo_PolyInternalNode::NodeIndices& Nod22Indices,
		   HLRAlgo_PolyInternalNode::NodeData& Nod22RValues,
		   const Standard_Integer i2p1,
		   const Standard_Integer i2p2,
		   const Standard_Integer i2,
		   const Handle(HLRAlgo_PolyInternalData)& pid2,
		   HLRAlgo_Array1OfTData*& TData2,
		   HLRAlgo_Array1OfPISeg*& PISeg2,
		   HLRAlgo_Array1OfPINod*& PINod2,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Real X4,
		   const Standard_Real Y4,
		   const Standard_Real Z4,
		   const Standard_Real XTI4,
		   const Standard_Real YTI4,
		   const Standard_Real ZTI4,
		   const Standard_Real coef4,
		   const Standard_Real U4,
		   const Standard_Boolean insP4,
		   const Standard_Boolean mP4P1,
		   const Standard_Integer flag) const
{
  Standard_Boolean ins3 = insP3;
  Standard_Boolean ins4 = insP4;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Indices.Flag & NMsk_Vert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData2,PISeg2,PINod2);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod11Indices.Edg1 == e) Nod11RValues.PCu1 = U3;
      else if (Nod11Indices.Edg2 == e) Nod11RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11RValues.Scal  = 0;
      Nod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      Nod21RValues.Point = gp_XYZ(X3, Y3, Z3);
      if      (Nod21Indices.Edg1 == e) Nod21RValues.PCu1 = U3;
      else if (Nod21Indices.Edg2 == e) Nod21RValues.PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p1 << endl;
      }
#endif
      Nod21RValues.Scal  = 0;
      Nod21Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i2p1,Nod21Indices,TData2,PISeg2,PINod2);
      HLRAlgo_BiPoint::PointsT& aPoints = List.First().Points();
      aPoints.PntP2 = gp_XYZ(X3, Y3, Z3);
      aPoints.Pnt2 = gp_XYZ(XTI3, YTI3, ZTI3);
    }
  }
  if (ins4 && !mP4P1) {                            // P2 ---> P4
    if (!(Nod12Indices.Flag & NMsk_Vert) && coef4 > myTolEnd) {
      ins4 = Standard_False;
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef4,X4,Y4,Z4,Standard_False,
		 TData2,PISeg2,PINod2);
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef4,X4,Y4,Z4,Standard_False,
		 TData1,PISeg1,PINod1);
      X2   = X4;
      Y2   = Y4;
      Z2   = Z4;
      XTI2 = XTI4;
      YTI2 = YTI4;
      ZTI2 = ZTI4;
      U2   = U4;
      Nod12RValues.Point = gp_XYZ(X4, Y4, Z4);
      if      (Nod12Indices.Edg1 == e) Nod12RValues.PCu1 = U4;
      else if (Nod12Indices.Edg2 == e) Nod12RValues.PCu2 = U4;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12RValues.Scal  = 0;
      Nod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
      Nod22RValues.Point = gp_XYZ(X4, Y4, Z4);
      if      (Nod22Indices.Edg1 == e) Nod22RValues.PCu1 = U4;
      else if (Nod22Indices.Edg2 == e) Nod22RValues.PCu2 = U4;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p2 << endl;
      }
#endif
      Nod22RValues.Scal  = 0;
      Nod22Indices.Flag |= NMsk_OutL;
      UpdateAroundNode(i2p2,Nod22Indices,TData2,PISeg2,PINod2);
    }
  }
  if (ins3 || ins4) {
    if      (!ins4)                                // p1 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else if (!ins3)                                // p1 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else {                                         // p1 i1p3 i2p4 p2
      Standard_Integer i1p3 = pid1->AddNode
	(Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
      Standard_Integer i2p3 = pid2->AddNode
	(Nod21RValues,Nod22RValues,PINod2,PINod1,coef3,X3,Y3,Z3);
      Standard_Integer i1p4 = pid1->AddNode
	(Nod11RValues,Nod12RValues,PINod1,PINod2,coef4,X4,Y4,Z4);
      Standard_Integer i2p4 = pid2->AddNode
	(Nod21RValues,Nod22RValues,PINod2,PINod1,coef4,X4,Y4,Z4);
      const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
	&(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
      HLRAlgo_PolyInternalNode::NodeIndices& Nod13Indices = (*pi1p3)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& Nod13RValues = (*pi1p3)->Data();
      const Handle(HLRAlgo_PolyInternalNode)* pi1p4 =
	&(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p4));
      HLRAlgo_PolyInternalNode::NodeIndices& Nod14Indices = (*pi1p4)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& Nod14RValues = (*pi1p4)->Data();
      const Handle(HLRAlgo_PolyInternalNode)* pi2p3 =
	&(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p3));
      HLRAlgo_PolyInternalNode::NodeIndices& Nod23Indices = (*pi2p3)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& Nod23RValues = (*pi2p3)->Data();
      const Handle(HLRAlgo_PolyInternalNode)* pi2p4 = 
	&(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p4));
      HLRAlgo_PolyInternalNode::NodeIndices& Nod24Indices = (*pi2p4)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& Nod24RValues = (*pi2p4)->Data();
      Nod13Indices.Edg1  = e;
      Nod13RValues.PCu1  = U3;
      Nod13RValues.Scal  = 0;
      Nod13Indices.Flag |= NMsk_OutL;
      Nod13Indices.Flag |= NMsk_Edge;
      Nod23Indices.Edg1  = e;
      Nod23RValues.PCu1  = U3;
      Nod23RValues.Scal  = 0;
      Nod23Indices.Flag |= NMsk_OutL;
      Nod23Indices.Flag |= NMsk_Edge;
      Nod14Indices.Edg1  = e;
      Nod14RValues.PCu1  = U4;
      Nod14RValues.Scal  = 0;
      Nod14Indices.Flag |= NMsk_OutL;
      Nod14Indices.Flag |= NMsk_Edge;
      Nod24Indices.Edg1  = e;
      Nod24RValues.PCu1  = U4;
      Nod24RValues.Scal  = 0;
      Nod24Indices.Flag |= NMsk_OutL;
      Nod24Indices.Flag |= NMsk_Edge;
      pid1->UpdateLinks(i1p1,i1p2,i1p3,
			TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
      pid2->UpdateLinks(i2p1,i2p2,i2p3,
			TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
      pid2->UpdateLinks(i2p3,i2p2,i2p4,
			TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
      pid1->UpdateLinks(i1p3,i1p2,i1p4,
			TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
      UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
      UpdateAroundNode(i2p3,Nod23Indices,TData2,PISeg2,PINod2);
      UpdateAroundNode(i1p4,Nod14Indices,TData1,PISeg1,PINod1);
      UpdateAroundNode(i2p4,Nod24Indices,TData2,PISeg2,PINod2);
      List.Prepend(HLRAlgo_BiPoint
		   (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		    X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		    i1  ,i1p1,i1p3,i2  ,i2p1,i2p3,flag));
      List.Prepend(HLRAlgo_BiPoint
		   (XTI3,YTI3,ZTI3,XTI4,YTI4,ZTI4,
		    X3  ,Y3  ,Z3  ,X4  ,Y4  ,Z4  ,   e,
		    i1  ,i1p3,i1p4,i2  ,i2p3,i2p4,flag));
      List.Prepend(HLRAlgo_BiPoint
		   (XTI4,YTI4,ZTI4,XTI2,YTI2,ZTI2,
		    X4  ,Y4  ,Z4  ,X2  ,Y2  ,Z2  ,   e,
		    i1  ,i1p4,i1p2,i2  ,i2p4,i2p2,flag));
    }
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : InsertOnOutLine
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::InsertOnOutLine (TColStd_Array1OfTransient& PID)
{
  HLRAlgo_Array1OfTData* TData2 = 0;
  HLRAlgo_Array1OfPISeg* PISeg2 = 0;
  HLRAlgo_Array1OfPINod* PINod2 = 0;
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*) (&(PID.ChangeValue(1)));

  TopLoc_Location L;
  Standard_Boolean insP3,mP3P1,IntOutL;
  Standard_Integer f,ip1,ip2,ip3;//, i;
  Standard_Real U3,V3,coef3,X3 = 0.,Y3 = 0.,Z3 = 0.;

  const gp_Trsf& T  = myProj.Transformation();
  
  Standard_Integer nbFace = myFMap.Extent();
  for (f = 1; f <= nbFace; f++) {

    if (!((*pid).IsNull())) {
      IntOutL = Standard_False;
      HLRAlgo_Array1OfTData* TData1= &((*pid)->TData());
      HLRAlgo_Array1OfPISeg* PISeg1= &(*pid)->PISeg();
      HLRAlgo_Array1OfPINod* PINod1= &((*pid)->PINod());
      TopoDS_Shape LocalShape = myFMap(f);
      const TopoDS_Face& F = TopoDS::Face(LocalShape);
      myBSurf.Initialize(F,Standard_False);
      myGSurf = BRep_Tool::Surface(F,L);
      gp_Trsf TT = L.Transformation();
      TT.PreMultiply(T);
      const gp_XYZ& ttlo = TT.TranslationPart();
      TTLo[0] = ttlo.X();
      TTLo[1] = ttlo.Y();
      TTLo[2] = ttlo.Z();
      const gp_Mat& ttma = TT.VectorialPart();
      TTMa[0][0] = ttma.Value(1,1);
      TTMa[0][1] = ttma.Value(1,2);
      TTMa[0][2] = ttma.Value(1,3);
      TTMa[1][0] = ttma.Value(2,1);
      TTMa[1][1] = ttma.Value(2,2);
      TTMa[1][2] = ttma.Value(2,3);
      TTMa[2][0] = ttma.Value(3,1);
      TTMa[2][1] = ttma.Value(3,2);
      TTMa[2][2] = ttma.Value(3,3);

#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << " InsertOnOutLine : NbTData " << (*pid)->NbTData() << endl;
	cout << " InsertOnOutLine : NbPISeg " << (*pid)->NbPISeg() << endl;
	cout << " InsertOnOutLine : NbPINod " << (*pid)->NbPINod() << endl;
      }
#endif

      Standard_Integer iseg,nbS;
      nbS = (*pid)->NbPISeg();
      for (iseg = 1; iseg <= nbS; iseg++) {
        HLRAlgo_PolyInternalSegment& aSegIndices = PISeg1->ChangeValue(iseg);
//	Standard_Boolean Cutted = Standard_False;
	if (aSegIndices.Conex1 != 0 && aSegIndices.Conex2 != 0) {
	  ip1 = aSegIndices.LstSg1;
	  ip2 = aSegIndices.LstSg2;
	  const Handle(HLRAlgo_PolyInternalNode)* pip1 = &PINod1->ChangeValue(ip1);
	  HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices = (*pip1)->Indices();
	  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = (*pip1)->Data();
	  const Handle(HLRAlgo_PolyInternalNode)* pip2 = &PINod1->ChangeValue(ip2);
	  HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices = (*pip2)->Indices();
	  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues = (*pip2)->Data();
	  if (Nod1Indices.Flag & NMsk_OutL && Nod2Indices.Flag & NMsk_OutL)
	    IntOutL = Standard_True;
	  else if ((Nod1RValues.Scal >=  myTolAngular &&
		    Nod2RValues.Scal <= -myTolAngular) ||
		   (Nod2RValues.Scal >=  myTolAngular &&
		    Nod1RValues.Scal <= -myTolAngular)) {
	    IntOutL = Standard_True;
	    insP3 = NewNode(Nod1RValues,Nod2RValues,coef3,mP3P1);
	    if (insP3) {
	      UVNode(Nod1RValues,Nod2RValues,coef3,U3,V3);
	      const gp_Pnt& PT3 = myGSurf->Value(U3,V3);
	      X3 = PT3.X();
	      Y3 = PT3.Y();
	      Z3 = PT3.Z();
	      TTMultiply(X3,Y3,Z3);
	    }
	    
	    if (insP3 && mP3P1) {                        // P1 ---> P3
	      if ((Nod1Indices.Flag & NMsk_Edge) == 0 && coef3 < myTolSta) {
		insP3 = Standard_False;
		ChangeNode(ip1,ip2,
			   Nod1Indices,Nod1RValues,
			   Nod2Indices,Nod2RValues,
			   coef3,X3,Y3,Z3,Standard_True,
			   TData1,PISeg1,PINod1);
		Nod1RValues.Scal  = 0;
		Nod1Indices.Flag |= NMsk_OutL;
	      }
	    }
	    if (insP3 && !mP3P1) {                       // P2 ---> P3
	      if ((Nod2Indices.Flag & NMsk_Edge) == 0 && coef3 > myTolEnd) {
		insP3 = Standard_False;
		ChangeNode(ip1,ip2,
			   Nod1Indices,Nod1RValues,
			   Nod2Indices,Nod2RValues,
			   coef3,X3,Y3,Z3,Standard_False,
			   TData1,PISeg1,PINod1);
		Nod2RValues.Scal  = 0;
		Nod2Indices.Flag |= NMsk_OutL;
	      }
	    }
	    if (insP3) {                                 // p1 ip3 p2
	      ip3 = (*pid)->AddNode(Nod1RValues,Nod2RValues,PINod1,PINod2,
				    coef3,X3,Y3,Z3);
	      const Handle(HLRAlgo_PolyInternalNode)* pip3 =
		(&((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip3));
	      HLRAlgo_PolyInternalNode::NodeIndices& Nod3Indices = (*pip3)->Indices();
	      HLRAlgo_PolyInternalNode::NodeData& Nod3RValues = (*pip3)->Data();
	      (*pid)->UpdateLinks(ip1,ip2,ip3,
				  TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
	      UpdateAroundNode(ip3,Nod3Indices,TData1,PISeg1,PINod1);
	      Nod3RValues.Scal  = 0;
	      Nod3Indices.Flag |= NMsk_OutL;
	    }
	  }
	}
      }
      if (IntOutL)
	(*pid)->IntOutL(Standard_True);

      nbS = (*pid)->NbPISeg();

#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << " InsertOnOutLine : NbTData " << (*pid)->NbTData() << endl;
	cout << " InsertOnOutLine : NbPISeg " << (*pid)->NbPISeg() << endl;
	cout << " InsertOnOutLine : NbPINod " << (*pid)->NbPINod() << endl;
      }
#endif
    }
    pid++;
  }
}

//=======================================================================
//function : CheckFrBackTriangles
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::CheckFrBackTriangles (HLRAlgo_ListOfBPoint& List,
					TColStd_Array1OfTransient& PID)
{
  Standard_Integer f,i,nbN,nbT,nbFace;
  Standard_Real X1 =0.,Y1 =0.,X2 =0.,Y2 =0.,X3 =0.,Y3 =0.;
  Standard_Real D1,D2,D3;
  Standard_Real dd,dX,dY,nX,nY;
  Standard_Boolean FrBackInList;
  HLRAlgo_Array1OfTData* TData;
  HLRAlgo_Array1OfPISeg* PISeg;
  HLRAlgo_Array1OfPINod* PINod;

  //Standard_Address IndexPtr = NULL; 
  //const Handle(HLRAlgo_PolyInternalData)& pid1 =
  //  *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F1Index));
  //Standard_Address TData1 = &pid1->TData(),
  //PISeg1 = &pid1->PISeg(),
  //PINod1 = &pid1->PINod();

  //const Handle(HLRAlgo_PolyInternalData)& pid2 =
  //  *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F2Index));
  //Standard_Address TData2 = &pid2->TData(),
  //PISeg2 = &pid2->PISeg(),
  //PINod2 = &pid2->PISeg();

  HLRAlgo_Array1OfTData* TData1 = NULL;
  HLRAlgo_Array1OfPISeg* PISeg1 = NULL;
  HLRAlgo_Array1OfPINod* PINod1 = NULL;
  HLRAlgo_Array1OfTData* TData2 = NULL;
  HLRAlgo_Array1OfPISeg* PISeg2 = NULL;
  HLRAlgo_Array1OfPINod* PINod2 = NULL;
  HLRAlgo_PolyInternalNode::NodeIndices* Nod11Indices;
  HLRAlgo_PolyInternalNode::NodeIndices* Nod12Indices;
  HLRAlgo_PolyInternalNode::NodeIndices* Nod13Indices;
  HLRAlgo_PolyInternalNode::NodeData* Nod11RValues;
  HLRAlgo_PolyInternalNode::NodeData* Nod12RValues;
  HLRAlgo_PolyInternalNode::NodeData* Nod13RValues;

  Handle(HLRAlgo_PolyInternalData)* pid;

  nbFace = myFMap.Extent();
  Standard_Boolean Modif = Standard_True;
  Standard_Integer iLoop = 0;
  
  while (Modif && iLoop < 4) {
    iLoop++;
    Modif        = Standard_False;
    FrBackInList = Standard_False;
    pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      if (!(*pid).IsNull()) {
	nbT   =  (*pid)->NbTData();
	TData = &(*pid)->TData();
	PISeg = &(*pid)->PISeg();
	PINod = &(*pid)->PINod();
	HLRAlgo_TriangleData* tdata = &TData->ChangeValue(1);
	
	for (i = 1; i <= nbT; i++) {
	  if ((tdata->Flags & HLRAlgo_PolyMask_FMskSide) == 0 &&
	      (tdata->Flags & HLRAlgo_PolyMask_FMskFrBack)) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << " face : " << f << " , triangle " << i << endl;
#endif
	    Modif        = Standard_True;
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	      &PINod->ChangeValue(tdata->Node1);
	    Nod11Indices = &(*pi1p1)->Indices();
	    Nod11RValues = &(*pi1p1)->Data();
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2 =
	      &PINod->ChangeValue(tdata->Node2);
	    Nod12Indices = &(*pi1p2)->Indices();
	    Nod12RValues = &(*pi1p2)->Data();
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
        &PINod->ChangeValue(tdata->Node3);
	    Nod13Indices = &(*pi1p3)->Indices();
	    Nod13RValues = &(*pi1p3)->Data();
	    D1 = 0.; D2 = 0.; D3 = 0.;
	    if (((Nod11Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
		((Nod11Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
		((Nod11Indices->Flag & NMsk_Vert) == 0)) {
        dX = Nod13RValues->Point.X() - Nod12RValues->Point.X();
        dY = Nod13RValues->Point.Y() - Nod12RValues->Point.Y();
	      D1 = dX * dX + dY * dY;
	      D1 = sqrt(D1);
	      nX = - dY / D1; nY =   dX / D1;
	      dX = Nod11RValues->Point.X() - Nod12RValues->Point.X();
        dY = Nod11RValues->Point.Y() - Nod12RValues->Point.Y();
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D1 * 0.01;
	      else        dd += D1 * 0.01;
	      X1 = nX * dd; Y1 = nY * dd;
	    }
	    if (((Nod12Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
		((Nod12Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
		((Nod12Indices->Flag & NMsk_Vert) == 0)) {
        dX = Nod11RValues->Point.X() - Nod13RValues->Point.X();
        dY = Nod11RValues->Point.Y() - Nod13RValues->Point.Y();
	      D2 = dX * dX + dY * dY;
	      D2 = sqrt(D2);
	      nX = - dY / D2; nY =   dX / D2;
        dX = Nod12RValues->Point.X() - Nod13RValues->Point.X();
        dY = Nod12RValues->Point.Y() - Nod13RValues->Point.Y();
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D2 * 0.01;
	      else        dd += D2 * 0.01;
	      X2 = nX * dd; Y2 = nY * dd;
	    }
	    if (((Nod13Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
		((Nod13Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
		((Nod13Indices->Flag & NMsk_Vert) == 0)) {
        dX = Nod12RValues->Point.X() - Nod11RValues->Point.X();
        dY = Nod12RValues->Point.Y() - Nod11RValues->Point.Y();
	      D3 = dX * dX + dY * dY;
	      D3 = sqrt(D3);
	      nX = - dY / D3; nY =   dX / D3;
        dX = Nod13RValues->Point.X() - Nod11RValues->Point.X();
        dY = Nod13RValues->Point.Y() - Nod11RValues->Point.Y();
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D3 * 0.01;
	      else        dd += D3 * 0.01;
	      X3 = nX * dd; Y3 = nY * dd;
	    }
	    if      (D1 > D2 && D1 > D3) {
        Nod11RValues->Point.ChangeCoord(1) += X1;
        Nod11RValues->Point.ChangeCoord(2) += Y1;
	      Nod11Indices->Flag |= NMsk_Move;
	      UpdateAroundNode(tdata->Node1,*Nod11Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << tdata->Node1 << " modifies  : DX,DY ";
		cout << X1 << " , " << Y1 << endl;
	      }
#endif
	    }
	    else if (D2 > D3 && D2 > D1) {
        Nod12RValues->Point.ChangeCoord(1) += X2;
        Nod12RValues->Point.ChangeCoord(2) += Y2;
	      Nod12Indices->Flag |= NMsk_Move;
	      UpdateAroundNode(tdata->Node2,*Nod12Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << tdata->Node2 << " modifies  : DX,DY ";
		cout << X2 << " , " << Y2 << endl;
	      }
#endif
	    }
	    else if (D3 > D1 && D3 > D2) {
        Nod13RValues->Point.ChangeCoord(1) += X3;
        Nod13RValues->Point.ChangeCoord(2) += Y3;
	      Nod13Indices->Flag |= NMsk_Move;
	      UpdateAroundNode(tdata->Node3,*Nod13Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << tdata->Node3 << " modifies  : DX,DY ";
		cout << X3 << " , " << Y3 << endl;
	      }
#endif
	    }
#ifdef OCCT_DEBUG
	    else if (DoTrace)
	      cout << "modification error" << endl;
#endif
	  }
	  tdata++;
	}
      }
      pid++;
    }
    if (FrBackInList) {
      HLRAlgo_ListIteratorOfListOfBPoint it;
      
      for (it.Initialize(List); it.More(); it.Next()) {      
	HLRAlgo_BiPoint& BP = it.Value();
	HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
	if (theIndices.FaceConex1 != 0) {
	  const Handle(HLRAlgo_PolyInternalData)& pid1 =
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(theIndices.FaceConex1));
	  TData1 = &pid1->TData();
	  PISeg1 = &pid1->PISeg();
	  PINod1 = &pid1->PINod();
	}
	if (theIndices.FaceConex2 != 0) {
	  if (theIndices.FaceConex1 == theIndices.FaceConex2) {
	    TData2 = TData1;
	    PISeg2 = PISeg1;
	    PINod2 = PINod1;
	  }
	  else {
	    const Handle(HLRAlgo_PolyInternalData)& pid2 =
	      *(Handle(HLRAlgo_PolyInternalData)*)&(PID(theIndices.FaceConex2));
	    TData2 = &pid2->TData();
	    PISeg2 = &pid2->PISeg();
	    PINod2 = &pid2->PINod();
	  }
	}
	if (theIndices.FaceConex1 != 0) {
	  Nod11Indices = &PINod1->ChangeValue(theIndices.Face1Pt1)->Indices();
	  if (Nod11Indices->Flag & NMsk_Move) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << theIndices.Face1Pt1 << " modifies 11" << endl;
#endif
	    Nod11RValues = &PINod1->ChangeValue(theIndices.Face1Pt1)->Data();
	    HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
      aPoints.Pnt1 = aPoints.PntP1 = Nod11RValues->Point;
	    TIMultiply(aPoints.Pnt1);
	    if (theIndices.FaceConex2 != 0) {
	      Nod12Indices = &PINod2->ChangeValue(theIndices.Face2Pt1)->Indices();
	      Nod12RValues = &PINod2->ChangeValue(theIndices.Face2Pt1)->Data();
        Nod12RValues->Point.ChangeCoord(1) = Nod11RValues->Point.X();
        Nod12RValues->Point.ChangeCoord(2) = Nod11RValues->Point.Y();
	      UpdateAroundNode(theIndices.Face2Pt1,*Nod12Indices,
			       TData2,PISeg2,PINod2);
	    }
	  }
	  Nod11Indices = &PINod1->ChangeValue(theIndices.Face1Pt2)->Indices();
	  if (Nod11Indices->Flag & NMsk_Move) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << theIndices.Face1Pt2 << " modifies 12" << endl;
#endif
	    Nod11RValues = &PINod1->ChangeValue(theIndices.Face1Pt2)->Data();
	    HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
	    aPoints.Pnt2 = aPoints.PntP2 = Nod11RValues->Point;
	    TIMultiply(aPoints.Pnt2);
	    if (theIndices.FaceConex2 != 0) {
	      Nod12Indices = &PINod2->ChangeValue(theIndices.Face2Pt2)->Indices();
	      Nod12RValues = &PINod2->ChangeValue(theIndices.Face2Pt2)->Data();
        Nod12RValues->Point.ChangeCoord(1) = Nod11RValues->Point.X();
        Nod12RValues->Point.ChangeCoord(2) = Nod11RValues->Point.Y();
	      UpdateAroundNode(theIndices.Face2Pt2,*Nod12Indices,
			       TData2,PISeg2,PINod2);
	    }
	  }
	}
	if (theIndices.FaceConex2 != 0) {
	  const Handle(HLRAlgo_PolyInternalData)& pid2 =
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(theIndices.FaceConex2));
	  PINod2 = &pid2->PINod();
	  Nod11Indices = &PINod2->ChangeValue(theIndices.Face2Pt1)->Indices();
	  if (Nod11Indices->Flag & NMsk_Move) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << theIndices.Face2Pt1 << " modifies 21" << endl;
#endif
	    Nod11RValues = &PINod2->ChangeValue(theIndices.Face2Pt1)->Data();
	    HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
      aPoints.Pnt1 = aPoints.PntP1 = Nod11RValues->Point;
	    TIMultiply(aPoints.Pnt1);
	    if (theIndices.FaceConex1 != 0) {
	      Nod12Indices = &PINod1->ChangeValue(theIndices.Face1Pt1)->Indices();
	      Nod12RValues = &PINod1->ChangeValue(theIndices.Face1Pt1)->Data();
        Nod12RValues->Point.ChangeCoord(1) = Nod11RValues->Point.X();
        Nod12RValues->Point.ChangeCoord(2) = Nod11RValues->Point.Y();
	      UpdateAroundNode(theIndices.Face1Pt1,*Nod12Indices,
			       TData1,PISeg1,PINod1);
	    }
	  }
	  Nod11Indices = &PINod2->ChangeValue(theIndices.Face2Pt2)->Indices();
	  if (Nod11Indices->Flag & NMsk_Move) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << theIndices.Face2Pt2 << " modifies 22" << endl;
#endif
	    Nod11RValues = &PINod2->ChangeValue(theIndices.Face2Pt2)->Data();
	    HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
      aPoints.Pnt2 = aPoints.PntP2 = Nod11RValues->Point;
	    TIMultiply(aPoints.Pnt2);
	    if (theIndices.FaceConex1 != 0) {
	      Nod12Indices = &PINod1->ChangeValue(theIndices.Face1Pt2)->Indices();
	      Nod12RValues = &PINod1->ChangeValue(theIndices.Face1Pt2)->Data();
        Nod12RValues->Point.ChangeCoord(1) = Nod11RValues->Point.X();
        Nod12RValues->Point.ChangeCoord(2) = Nod11RValues->Point.Y();
	      UpdateAroundNode(theIndices.Face1Pt2,*Nod12Indices,
			       TData1,PISeg1,PINod1);
	    }
	  }
	}
      }
      pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
      
      for (f = 1; f <= nbFace; f++) {
	if (!(*pid).IsNull()) {
	  nbN   =  (*pid)->NbPINod();
	  PINod = &(*pid)->PINod();

    for (i = 1; i <= nbN; i++)
    {
      Nod11Indices = &PINod->ChangeValue(i)->Indices();
	    Nod11Indices->Flag   &= ~NMsk_Move;
	  }
	}
	pid++;
      }
    }
  }
}

//=======================================================================
//function : FindEdgeOnTriangle
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
FindEdgeOnTriangle (const HLRAlgo_TriangleData& theTriangle,
		    const Standard_Integer ip1,
		    const Standard_Integer ip2,
		    Standard_Integer& jtrouv,
		    Standard_Boolean& isDirect) const
{
  Standard_Integer n1 = theTriangle.Node1;
  Standard_Integer n2 = theTriangle.Node2;
  Standard_Integer n3 = theTriangle.Node3;
  if      (ip1 == n1 && ip2 == n2) {
    jtrouv = 0;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n1 && ip1 == n2) {
    jtrouv = 0;
    isDirect = Standard_False;
    return;
  }
  else if (ip1 == n2 && ip2 == n3) {
    jtrouv = 1;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n2 && ip1 == n3) {
    jtrouv = 1;
    isDirect = Standard_False;
    return;
  }
  else if (ip1 == n3 && ip2 == n1) {
    jtrouv = 2;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n3 && ip1 == n1) {
    jtrouv = 2;
    isDirect = Standard_False;
    return;
  }
}

//=======================================================================
//function : ChangeNode
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::ChangeNode (const Standard_Integer ip1,
				   const Standard_Integer ip2,
				   HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
				   HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
				   HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices,
				   HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
				   const Standard_Real coef1,
				   const Standard_Real X3,
				   const Standard_Real Y3,
				   const Standard_Real Z3,
				   const Standard_Boolean first,
				   HLRAlgo_Array1OfTData*& TData,
				   HLRAlgo_Array1OfPISeg*& PISeg,
				   HLRAlgo_Array1OfPINod*& PINod) const
{
  Standard_Real coef2 = 1 - coef1;
  if (first) {
    Nod1RValues.Point = gp_XYZ(X3, Y3, Z3);
    Nod1RValues.UV = coef2 * Nod1RValues.UV + coef1 * Nod2RValues.UV;
    Nod1RValues.Scal = Nod1RValues.Scal * coef2 + Nod2RValues.Scal * coef1;
    const gp_XYZ aXYZ = coef2 * Nod1RValues.Normal + coef1 * Nod2RValues.Normal;
    const Standard_Real aNorm = aXYZ.Modulus();
    if (aNorm > 0) {
      Nod1RValues.Normal = (1 / aNorm) * aXYZ;
    }
    else {
      Nod1RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
      if (DoError) {
	cout << "HLRBRep_PolyAlgo::ChangeNode between " << ip1;
	cout << " and " << ip2 << endl;
      }
#endif
    }
    UpdateAroundNode(ip1,Nod1Indices,TData,PISeg,PINod);
  }
  else {
    Nod2RValues.Point = gp_XYZ(X3, Y3, Z3);
    Nod2RValues.UV = coef2 * Nod1RValues.UV + coef1 * Nod2RValues.UV;
    Nod2RValues.Scal = Nod1RValues.Scal * coef2 + Nod2RValues.Scal * coef1;
    const gp_XYZ aXYZ = coef2 * Nod1RValues.Normal + coef1 * Nod2RValues.Normal;
    const Standard_Real aNorm = aXYZ.Modulus();
    if (aNorm > 0) {
      Nod2RValues.Normal = (1 / aNorm) * aXYZ;
    }
    else {
      Nod2RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
      if (DoError) {
	cout << "HLRBRep_PolyAlgo::ChangeNode between " << ip2;
	cout << " and " << ip1 << endl;
      }
#endif
    }
    UpdateAroundNode(ip2,Nod2Indices,TData,PISeg,PINod);
  }
}

//=======================================================================
//function : UpdateAroundNode
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
UpdateAroundNode (const Standard_Integer iNode,
		  HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
      HLRAlgo_Array1OfTData* TData,
      HLRAlgo_Array1OfPISeg* PISeg,
      HLRAlgo_Array1OfPINod* PINod) const
{
  Standard_Integer iiii,iTri1,iTri2;
  iiii = Nod1Indices.NdSg;
  
  while (iiii != 0) {
    HLRAlgo_PolyInternalSegment& aSegIndices = PISeg->ChangeValue(iiii);
    iTri1 = aSegIndices.Conex1;
    iTri2 = aSegIndices.Conex2;
    if ( iTri1 != 0) {
      HLRAlgo_TriangleData& aTriangle = TData->ChangeValue(iTri1);
      const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
        &PINod->ChangeValue(aTriangle.Node1);
      const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
        &PINod->ChangeValue(aTriangle.Node2);
      const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
        &PINod->ChangeValue(aTriangle.Node3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod1Indices = (*PN1)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod2Indices = (*PN2)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod3Indices = (*PN3)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = (*PN1)->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = (*PN2)->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = (*PN3)->Data();
      OrientTriangle(iTri1,aTriangle,
		     aNod1Indices,aNod1RValues,
		     aNod2Indices,aNod2RValues,
		     aNod3Indices,aNod3RValues);
    }
    if ( iTri2 != 0) {
      HLRAlgo_TriangleData& aTriangle2 = TData->ChangeValue(iTri2);
      const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
        &PINod->ChangeValue(aTriangle2.Node1);
      const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
        &PINod->ChangeValue(aTriangle2.Node2);
      const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
        &PINod->ChangeValue(aTriangle2.Node3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod1Indices = (*PN1)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod2Indices = (*PN2)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod3Indices = (*PN3)->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = (*PN1)->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = (*PN2)->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = (*PN3)->Data();
      OrientTriangle(iTri2,aTriangle2,
		     aNod1Indices,aNod1RValues,
		     aNod2Indices,aNod2RValues,
		     aNod3Indices,aNod3RValues);
    }
    if (aSegIndices.LstSg1 == iNode) iiii = aSegIndices.NxtSg1;
    else                     iiii = aSegIndices.NxtSg2;
  }
}

//=======================================================================
//function : OrientTriangle
//purpose  : 
//=======================================================================

void 
#ifdef OCCT_DEBUG
HLRBRep_PolyAlgo::OrientTriangle(const Standard_Integer iTri,
#else
HLRBRep_PolyAlgo::OrientTriangle(const Standard_Integer,
#endif
				 HLRAlgo_TriangleData& theTriangle,
				 HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
				 HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
				 HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices,
				 HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
				 HLRAlgo_PolyInternalNode::NodeIndices& Nod3Indices,
				 HLRAlgo_PolyInternalNode::NodeData& Nod3RValues) const
{
  Standard_Boolean o1 = (Nod1Indices.Flag & NMsk_OutL) != 0;
  Standard_Boolean o2 = (Nod2Indices.Flag & NMsk_OutL) != 0;
  Standard_Boolean o3 = (Nod3Indices.Flag & NMsk_OutL) != 0;
  theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskFlat;
  theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskOnOutL;
  if (o1 && o2 && o3) {
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
    theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskOnOutL;
#ifdef OCCT_DEBUG
    if (DoTrace) {
      cout << "HLRBRep_PolyAlgo::OrientTriangle : OnOutL";
      cout << " triangle " << iTri << endl;
    }
#endif
  }
  else {
    Standard_Real s1 = Nod1RValues.Scal;
    Standard_Real s2 = Nod2RValues.Scal;
    Standard_Real s3 = Nod3RValues.Scal;
    Standard_Real as1 = s1;
    Standard_Real as2 = s2;
    Standard_Real as3 = s3;
    if (s1 < 0) as1 = -s1;
    if (s2 < 0) as2 = -s2;
    if (s3 < 0) as3 = -s3;
    Standard_Real  s = 0;
    Standard_Real as = 0;
    if (!o1            ) {s = s1; as = as1;}
    if (!o2 && as < as2) {s = s2; as = as2;}
    if (!o3 && as < as3) {s = s3; as = as3;}
    if (s > 0) {
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskBack;
    }
    else {
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    }
    gp_XYZ aD12 = Nod2RValues.Point - Nod1RValues.Point;
    const Standard_Real aD12Norm = aD12.Modulus();
    if (aD12Norm <= 1.e-10) {
#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	cout << " triangle " << iTri << endl;
      }
#endif
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    }
    else {
      gp_XYZ aD23 = Nod3RValues.Point - Nod2RValues.Point;
      const Standard_Real aD23Norm = aD23.Modulus();
      if (aD23Norm < 1.e-10) {
#ifdef OCCT_DEBUG
	if (DoTrace) {
	  cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	  cout << " triangle " << iTri << endl;
	}
#endif
	theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
	theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
	theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
      }
      else {
        const gp_XYZ aD31 = Nod1RValues.Point - Nod3RValues.Point;
        const Standard_Real aD31Norm = aD31.Modulus();
        if (aD31Norm < 1.e-10) {
#ifdef OCCT_DEBUG
	  if (DoTrace) {
	    cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	    cout << " triangle " << iTri << endl;
	  }
#endif
	  theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
	  theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
	  theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
	}
	else {
    aD12 *= 1 / aD12Norm;
    aD23 *= 1 / aD23Norm;
    gp_XYZ aD = aD12 ^ aD23;
    const Standard_Real aDNorm = aD.Modulus();
	  if (aDNorm < 1.e-5) {
#ifdef OCCT_DEBUG
	    if (DoTrace) {
	      cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	      cout << " triangle " << iTri << endl;
	    }
#endif
	    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
	    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
	    theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
	  }
	  else {
	    Standard_Real o;
	    if (myProj.Perspective()) {
        aD *= 1 / aDNorm;
	      o = aD.Z() * myProj.Focus() - aD * Nod1RValues.Point;
	    }
	    else
	      o = aD.Z() / aDNorm;
	    if (o < 0) {
	      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskOrBack;
	      o = -o;
	    }
	    else
	      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskOrBack;
	    if (o < 1.e-10) {
	      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
	      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
	    }
	  }
	}
      }
    }
  }
  if ((!(theTriangle.Flags & HLRAlgo_PolyMask_FMskBack) &&  (theTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack)) ||
      ( (theTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && !(theTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack)))
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFrBack;
  else 
    theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskFrBack;
}

//=======================================================================
//function : Triangles
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::Triangles(const Standard_Integer ip1,
			    const Standard_Integer ip2,
			    HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
			    HLRAlgo_Array1OfPISeg*& PISeg,
			    Standard_Integer& iTri1,
			    Standard_Integer& iTri2) const
{
  Standard_Integer iiii = Nod1Indices.NdSg;
  
  while (iiii != 0) {
    HLRAlgo_PolyInternalSegment& aSegIndices =
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii);
    if (aSegIndices.LstSg1 == ip1) {
      if (aSegIndices.LstSg2 == ip2) {
	iTri1 = aSegIndices.Conex1;
	iTri2 = aSegIndices.Conex2;
	return Standard_True;
      }
      else iiii = aSegIndices.NxtSg1;
    }
    else {
      if (aSegIndices.LstSg1 == ip2) {
	iTri1 = aSegIndices.Conex1;
	iTri2 = aSegIndices.Conex2;
	return Standard_True;
      }
      else iiii = aSegIndices.NxtSg2;
    }
  }
  iTri1 = 0;
  iTri2 = 0;
#ifdef OCCT_DEBUG
  if (DoError) {
    cout << "HLRBRep_PolyAlgo::Triangles : error";
    cout << " between " << ip1 << " and " << ip2 << endl;
  }
#endif
  return Standard_False;
}

//=======================================================================
//function : NewNode
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::
NewNode (
  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
  Standard_Real& coef1,
  Standard_Boolean& moveP1) const
{
  Standard_Real TolAng = myTolAngular * 0.5;
  if ((Nod1RValues.Scal >= TolAng && Nod2RValues.Scal <= -TolAng) ||
      (Nod2RValues.Scal >= TolAng && Nod1RValues.Scal <= -TolAng)) {
    coef1 = Nod1RValues.Scal / ( Nod2RValues.Scal - Nod1RValues.Scal );
    if (coef1 < 0) coef1 = - coef1;
    moveP1 = coef1 < 0.5;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : UVNode
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UVNode (
  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
  const Standard_Real coef1,
  Standard_Real& U3,
  Standard_Real& V3) const
{
  Standard_Real coef2 = 1 - coef1;
  const gp_XY aUV3 = coef2 * Nod1RValues.UV + coef1 * Nod2RValues.UV;
  U3 = aUV3.X();
  V3 = aUV3.Y();
}

//=======================================================================
//function : CheckDegeneratedSegment
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
CheckDegeneratedSegment(
  HLRAlgo_PolyInternalNode::NodeIndices& Nod1Indices,
  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
  HLRAlgo_PolyInternalNode::NodeIndices& Nod2Indices,
  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues) const
{
  Nod1Indices.Flag |=  NMsk_Fuck;
  Nod2Indices.Flag |=  NMsk_Fuck;
  if ((Nod1RValues.Scal >= myTolAngular && Nod2RValues.Scal <= -myTolAngular) ||
      (Nod2RValues.Scal >= myTolAngular && Nod1RValues.Scal <= -myTolAngular)) {
    Nod1RValues.Scal  = 0.;
    Nod1Indices.Flag |= NMsk_OutL;
    Nod2RValues.Scal  = 0.;
    Nod2Indices.Flag |= NMsk_OutL;
  }
}

//=======================================================================
//function : UpdateOutLines
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UpdateOutLines (HLRAlgo_ListOfBPoint& List,
				  TColStd_Array1OfTransient& PID)
{
  Standard_Integer f;
  Standard_Integer nbFace = myFMap.Extent();
  Standard_Real X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ;
  Standard_Real XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2;
    
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));

  for (f = 1; f <= nbFace; f++) { 
    if (!(*pid).IsNull()) {
      if ((*pid)->IntOutL()) {
  HLRAlgo_Array1OfTData* TData = &(*pid)->TData();
  HLRAlgo_Array1OfPISeg* PISeg = &(*pid)->PISeg();
	HLRAlgo_Array1OfPINod* PINod = &(*pid)->PINod();
	Standard_Integer i,j,it1,it2,tn1,tn2,tn3,pd,pf;
	Standard_Boolean outl;
	Standard_Integer nbS = (*pid)->NbPISeg();

	for (i = 1; i <= nbS; i++)
  {
    HLRAlgo_PolyInternalSegment* psg = &PISeg->ChangeValue(i);
	  it1 = psg->Conex1;
	  it2 = psg->Conex2;
	  if (it1 != 0 && it2 != 0 && it1 != it2) {  // debile but sure !
	    HLRAlgo_TriangleData& aTriangle = TData->ChangeValue(it1);
	    HLRAlgo_TriangleData& aTriangle2 = TData->ChangeValue(it2);
	    if      (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
	      outl =  (aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) !=  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
	    else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) &&  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
	      outl = Standard_False;
	    else if (  aTriangle.Flags & HLRAlgo_PolyMask_FMskSide)
	      outl = !(aTriangle.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
	    else
	      outl = !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle.Flags & HLRAlgo_PolyMask_FMskBack);
	    
	    if (outl) {
	      pd = psg->LstSg1;
	      pf = psg->LstSg2;
	      tn1 = aTriangle.Node1;
	      tn2 = aTriangle.Node2;
	      tn3 = aTriangle.Node3;
	      if (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack)) {
		j   = tn1;
		tn1 = tn3;
		tn3 = j;
	      }
	      if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
		aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin1;
	      else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
		aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin2;
	      else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
		aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin3;
#ifdef OCCT_DEBUG
	      else if (DoError) {
		cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
		cout << " : segment not found" << endl;
	      }
#endif
	      tn1 = aTriangle2.Node1;
	      tn2 = aTriangle2.Node2;
	      tn3 = aTriangle2.Node3;
	      if (!(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskOrBack)) {
		j   = tn1;
		tn1 = tn3;
		tn3 = j;
	      }
	      if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
		aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin1;
	      else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
		aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin2;
	      else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
		aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin3;
#ifdef OCCT_DEBUG
	      else if (DoError) {
		cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
		cout << " : segment not found" << endl;
	      }
#endif
	      HLRAlgo_PolyInternalNode::NodeData& Nod1RValues =
          PINod->ChangeValue(pd)->Data();
	      HLRAlgo_PolyInternalNode::NodeData& Nod2RValues =
          PINod->ChangeValue(pf)->Data();
	      XTI1 = X1 = Nod1RValues.Point.X();
	      YTI1 = Y1 = Nod1RValues.Point.Y();
	      ZTI1 = Z1 = Nod1RValues.Point.Z();
	      XTI2 = X2 = Nod2RValues.Point.X();
	      YTI2 = Y2 = Nod2RValues.Point.Y();
	      ZTI2 = Z2 = Nod2RValues.Point.Z();
	      TIMultiply(XTI1,YTI1,ZTI1);
	      TIMultiply(XTI2,YTI2,ZTI2);
	      List.Append(HLRAlgo_BiPoint(XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
					  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
					  f,f,pd,pf,f,pd,pf,12));
	    }
	  }
	}
      }
    }
    pid++;
  }
}

//=======================================================================
//function : UpdateEdgesBiPoints
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
UpdateEdgesBiPoints (HLRAlgo_ListOfBPoint& List,
		     const TColStd_Array1OfTransient& PID,
		     const Standard_Boolean closed)
{
  Standard_Integer itri1,itri2,tbid;
  HLRAlgo_ListIteratorOfListOfBPoint it;
  
  for (it.Initialize(List); it.More(); it.Next()) {      
    HLRAlgo_BiPoint& BP = it.Value();
//    Standard_Integer i[5];
    HLRAlgo_BiPoint::IndicesT& aIndices = BP.Indices();
    if (aIndices.FaceConex1 != 0 && aIndices.FaceConex2 != 0) {
      const Handle(HLRAlgo_PolyInternalData)& pid1 =
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(aIndices.FaceConex1));
      const Handle(HLRAlgo_PolyInternalData)& pid2 =
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(aIndices.FaceConex2));
      HLRAlgo_Array1OfPISeg* PISeg1 = &pid1->PISeg();
      HLRAlgo_Array1OfPISeg* PISeg2 = &pid2->PISeg();
      HLRAlgo_PolyInternalNode::NodeIndices& Nod11Indices = 
	pid1->PINod().ChangeValue(aIndices.Face1Pt1)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& Nod21Indices = 
	pid2->PINod().ChangeValue(aIndices.Face2Pt1)->Indices();
      Triangles(aIndices.Face1Pt1,aIndices.Face1Pt2,Nod11Indices,PISeg1,itri1,tbid);
      Triangles(aIndices.Face2Pt1,aIndices.Face2Pt2,Nod21Indices,PISeg2,itri2,tbid);

      if (itri1 != 0 && itri2 != 0) {
	if (aIndices.FaceConex1 != aIndices.FaceConex2 || itri1 != itri2) {
	  HLRAlgo_Array1OfTData* TData1 = &pid1->TData();
	  HLRAlgo_Array1OfTData* TData2 = &pid2->TData();
	  HLRAlgo_TriangleData& aTriangle = TData1->ChangeValue(itri1);
	  HLRAlgo_TriangleData& aTriangle2 = TData2->ChangeValue(itri2);
	  if (closed) {
	    if (((aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack)) ||
		((aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide)) ||
		((aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide)) ||
		((aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack)))
	      BP.Hidden(Standard_True);
	  }
	  Standard_Boolean outl;
	  if      (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
	    outl =  (aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) !=  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
	  else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) &&  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
	    outl = Standard_False;
	  else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide))
	    outl = !(aTriangle.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
	  else
	    outl = !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle.Flags & HLRAlgo_PolyMask_FMskBack);
	  BP.OutLine(outl);
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::UpdateEdgesBiPoints : error ";
	cout << " between " << aIndices.FaceConex1 << setw(6);
	cout << " and " << aIndices.FaceConex2 << endl;
      }
#endif
    }
  }
}

//=======================================================================
//function : UpdatePolyData
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UpdatePolyData (TColStd_Array1OfTransient& PD,
				  TColStd_Array1OfTransient& PID,
				  const Standard_Boolean closed)
{
  Standard_Integer f,i;//,n[3];
  Handle(TColgp_HArray1OfXYZ)    HNodes;
  Handle(HLRAlgo_HArray1OfTData) HTData;
  Handle(HLRAlgo_HArray1OfPHDat) HPHDat;
  Standard_Integer nbFace = myFMap.Extent();
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
  Handle(HLRAlgo_PolyData)* pd =
    (Handle(HLRAlgo_PolyData)*)&(PD.ChangeValue(1));

  for (f = 1; f <= nbFace; f++) {
    if (!(*pid).IsNull()) {
      Standard_Integer nbN = (*pid)->NbPINod();
      Standard_Integer nbT = (*pid)->NbTData();
      HNodes = new TColgp_HArray1OfXYZ   (1,nbN);
      HTData = new HLRAlgo_HArray1OfTData(1,nbT);
      TColgp_Array1OfXYZ&    Nodes = HNodes->ChangeArray1();
      HLRAlgo_Array1OfTData& Trian = HTData->ChangeArray1();
      HLRAlgo_Array1OfTData* TData = &(*pid)->TData();
      HLRAlgo_Array1OfPINod* PINod = &(*pid)->PINod();
      Standard_Integer nbHide = 0;

      for (i = 1; i <= nbN; i++) {
        HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = PINod->ChangeValue(i)->Data();
        Nodes.ChangeValue(i) = Nod1RValues.Point;
      }
      
      for (i = 1; i <= nbT; i++) {
        HLRAlgo_TriangleData* OT = &TData->ChangeValue(i);
        HLRAlgo_TriangleData* NT = &(Trian.ChangeValue(i));
	if (!(OT->Flags & HLRAlgo_PolyMask_FMskSide)) {
#ifdef OCCT_DEBUG
	  if ((OT->Flags & HLRAlgo_PolyMask_FMskFrBack) && DoTrace) {
	    cout << "HLRBRep_PolyAlgo::ReverseBackTriangle :";
	    cout << " face " << f << setw(6);
	    cout << " triangle " << i << endl;
	  }
#endif
	  if (OT->Flags & HLRAlgo_PolyMask_FMskOrBack) {
	    Standard_Integer j = OT->Node1;
	    OT->Node1          = OT->Node3;
	    OT->Node3          = j;
	    OT->Flags |=  HLRAlgo_PolyMask_FMskBack;
	  }
	  else
	    OT->Flags &= ~HLRAlgo_PolyMask_FMskBack;
	    //Tri1Flags |= HLRAlgo_PolyMask_FMskBack;//OCC349
	}
	NT->Node1 = OT->Node1;
	NT->Node2 = OT->Node2;
	NT->Node3 = OT->Node3;
	NT->Flags = OT->Flags;
	if (!(NT->Flags & HLRAlgo_PolyMask_FMskSide) &&
	    (!(NT->Flags & HLRAlgo_PolyMask_FMskBack) || !closed)) {
	  NT->Flags |=  HLRAlgo_PolyMask_FMskHiding;
	  nbHide++;
	}
	else
	  NT->Flags &= ~HLRAlgo_PolyMask_FMskHiding;
      }
      if (nbHide > 0) HPHDat = new HLRAlgo_HArray1OfPHDat(1,nbHide);
      else            HPHDat.Nullify();
      (*pd)->HNodes(HNodes);
      (*pd)->HTData(HTData);
      (*pd)->HPHDat(HPHDat);
      (*pd)->FaceIndex(f);
    }
    pid++;
    pd++;
  }
}

//=======================================================================
//function : TMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TMultiply (Standard_Real& X,
			     Standard_Real& Y,
			     Standard_Real& Z,
			     const Standard_Boolean VPO) const
{
  Standard_Real Xt = TMat[0][0]*X + TMat[0][1]*Y + TMat[0][2]*Z + (VPO ? 0 : TLoc[0]);//OCC349
  Standard_Real Yt = TMat[1][0]*X + TMat[1][1]*Y + TMat[1][2]*Z + (VPO ? 0 : TLoc[1]);//OCC349
  Z                = TMat[2][0]*X + TMat[2][1]*Y + TMat[2][2]*Z + (VPO ? 0 : TLoc[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : TTMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TTMultiply (Standard_Real& X,
			      Standard_Real& Y,
			      Standard_Real& Z,
			      const Standard_Boolean VPO) const
{
  Standard_Real Xt = TTMa[0][0]*X + TTMa[0][1]*Y + TTMa[0][2]*Z + (VPO ? 0 : TTLo[0]);//OCC349
  Standard_Real Yt = TTMa[1][0]*X + TTMa[1][1]*Y + TTMa[1][2]*Z + (VPO ? 0 : TTLo[1]);//OCC349
  Z                = TTMa[2][0]*X + TTMa[2][1]*Y + TTMa[2][2]*Z + (VPO ? 0 : TTLo[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : TIMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TIMultiply (Standard_Real& X,
			      Standard_Real& Y,
			      Standard_Real& Z,
			      const Standard_Boolean VPO) const
{
  Standard_Real Xt = TIMa[0][0]*X + TIMa[0][1]*Y + TIMa[0][2]*Z + (VPO ? 0 : TILo[0]);//OCC349
  Standard_Real Yt = TIMa[1][0]*X + TIMa[1][1]*Y + TIMa[1][2]*Z + (VPO ? 0 : TILo[1]);//OCC349
  Z                = TIMa[2][0]*X + TIMa[2][1]*Y + TIMa[2][2]*Z + (VPO ? 0 : TILo[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::PointsT& HLRBRep_PolyAlgo::Hide (
			     HLRAlgo_EdgeStatus& status,
			     TopoDS_Shape& S,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  Standard_Integer index;
  HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Hide(status,index,reg1,regn,outl,intl);
  if (intl) S = myFMap(index);
  else      S = myEMap(index);
  return aPoints;
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::PointsT& HLRBRep_PolyAlgo::Show (
			     TopoDS_Shape& S,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  Standard_Integer index;
  HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Show(index,reg1,regn,outl,intl);
  if (intl) S = myFMap(index);
  else      S = myEMap(index);
  return aPoints;
}

//=======================================================================
//function : OutLinedShape
//purpose  : 
//=======================================================================

TopoDS_Shape
HLRBRep_PolyAlgo::OutLinedShape (const TopoDS_Shape& S) const
{
  TopoDS_Shape Result;

  if (!S.IsNull()) {
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Result));
    B.Add(Result,S);

    TopTools_MapOfShape Map;
    TopExp_Explorer ex;
    for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next())
      Map.Add(ex.Current());
    for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next())
      Map.Add(ex.Current());

    Standard_Integer nbFace = myFMap.Extent();
    if (nbFace > 0) {
      TopTools_Array1OfShape NewF(1,nbFace);
      TColStd_Array1OfTransient& Shell = myAlgo->PolyShell();
      Standard_Integer nbShell = Shell.Upper();
      HLRAlgo_ListIteratorOfListOfBPoint it;
      
      for (Standard_Integer iShell = 1; iShell <= nbShell; iShell++) {
	HLRAlgo_ListOfBPoint& List =
	  (*(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell)))->Edges();
	
	for (it.Initialize(List); it.More(); it.Next()) {
	  HLRAlgo_BiPoint& BP = it.Value();
	  if (BP.IntLine()) {
	    HLRAlgo_BiPoint::IndicesT& aIndices = BP.Indices();
	    if (Map.Contains(myFMap(aIndices.ShapeIndex))) {
	      HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
	      B.Add(Result,BRepLib_MakeEdge(aPoints.Pnt1, aPoints.Pnt2));
	    }
	  }
	}
      }
    }    
  }
  return Result;
}

