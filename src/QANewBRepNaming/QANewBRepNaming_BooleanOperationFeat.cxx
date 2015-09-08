// Created on: 1999-09-27
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepGProp.hxx>
#include <Geom_Ellipse.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <QANewBRepNaming_BooleanOperationFeat.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <Standard_NullObject.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
#include <TDataStd_Name.hxx>
#endif

#ifdef OCCT_DEBUG
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>
#include <TNaming_Tool.hxx>
static void ModDbgTools_Write(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  ofstream save;
  save.open(filename);
  save << "DBRep_DrawableShape" << endl << endl;
  if(!shape.IsNull()) BRepTools::Write(shape, save);
  save.close();
}

#endif

//=======================================================================
//function : QANewBRepNaming_BooleanOperationFeat
//purpose  : 
//=======================================================================

QANewBRepNaming_BooleanOperationFeat::QANewBRepNaming_BooleanOperationFeat() {}

//=======================================================================
//function : QANewBRepNaming_BooleanOperationFeat
//purpose  : 
//=======================================================================

QANewBRepNaming_BooleanOperationFeat::QANewBRepNaming_BooleanOperationFeat(const TDF_Label& ResultLabel):QANewBRepNaming_TopNaming(ResultLabel) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::Init(const TDF_Label& ResultLabel) {  
  if(ResultLabel.IsNull()) 
    Standard_NullObject::Raise("QANewBRepNaming_BooleanOperationFeat::Init The Result label is Null ..."); 
  myResultLabel = ResultLabel; 
}

//=======================================================================
//function : ModifiedFaces
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::ModifiedFaces() const {
#ifdef OCCT_DEBUG
  const TDF_Label& ModifiedFacesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(ModifiedFacesLabel, "ModifiedFaces");
  return ModifiedFacesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : ModifiedEdges
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::ModifiedEdges() const {
#ifdef OCCT_DEBUG
  const TDF_Label& ModifiedEdgesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(ModifiedEdgesLabel, "ModifiedEdges");
  return ModifiedEdgesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : DeletedFaces
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::DeletedFaces() const {
#ifdef OCCT_DEBUG
  const TDF_Label& DeletedFacesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(DeletedFacesLabel, "DeletedFaces");
  return DeletedFacesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : DeletedEdges
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::DeletedEdges() const {
#ifdef OCCT_DEBUG
  const TDF_Label& DeletedEdgesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(DeletedEdgesLabel, "DeletedEdges");
  return DeletedEdgesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : DeletedVertices
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::DeletedVertices() const {
#ifdef OCCT_DEBUG
  const TDF_Label& DeletedVerticesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(DeletedVerticesLabel, "DeletedVertices");
  return DeletedVerticesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : NewShapes
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::NewShapes() const {
#ifdef OCCT_DEBUG
  const TDF_Label& NewShapesLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(NewShapesLabel, "NewShapes");
  return NewShapesLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : Content
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::Content() const {
#ifdef OCCT_DEBUG
  const TDF_Label& ContentLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(ContentLabel, "Content");
  return ContentLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : DeletedDegeneratedEdges
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_BooleanOperationFeat::DeletedDegeneratedEdges() const {
#ifdef OCCT_DEBUG
  const TDF_Label& DegeneratedLabel = ResultLabel().NewChild();
  TDataStd_Name::Set(DegeneratedLabel, "DeletedDegeneratedEdges");
  return DegeneratedLabel;
#else
  return ResultLabel().NewChild();
#endif
}

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum QANewBRepNaming_BooleanOperationFeat::ShapeType(const TopoDS_Shape& theShape) {
  TopAbs_ShapeEnum TypeSh = theShape.ShapeType();
  if (TypeSh == TopAbs_COMPOUND || TypeSh == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(theShape);
    if (!itr.More()) return TypeSh; 
    TypeSh = ShapeType(itr.Value());
    if(TypeSh == TopAbs_COMPOUND) return TypeSh;
    itr.Next();
    for(; itr.More(); itr.Next()) 
      if(ShapeType(itr.Value()) != TypeSh) return TopAbs_COMPOUND;      
  }
  return TypeSh;
} 

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

TopoDS_Shape QANewBRepNaming_BooleanOperationFeat::GetShape(const TopoDS_Shape& theShape) const {
  if (theShape.ShapeType() == TopAbs_COMPOUND || theShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(theShape);
    if (itr.More()) return itr.Value();
  }
  return theShape;
}

//=======================================================================
//function : LoadWire
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadWire(BRepAlgoAPI_BooleanOperation& MS) const {
  // Naming of modified edges:
  TNaming_Builder ModBuilder(ModifiedEdges());
  QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape1(), TopAbs_EDGE, ModBuilder);

  // load generated vertexes
  if(MS.HasGenerated()) {  
    TNaming_Builder nBuilder (NewShapes());
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape1(), TopAbs_EDGE, nBuilder);
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_FACE, nBuilder);
  }
  // Naming of deleted edges, dangle vertices
  if(MS.HasDeleted()){ 
    TNaming_Builder DelEBuilder(DeletedEdges());
    QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_EDGE, DelEBuilder);
    TNaming_Builder DelVBuilder(DeletedVertices());
    QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_VERTEX, DelEBuilder);
  }
 }

//=======================================================================
//function : LoadShell
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadShell(BRepAlgoAPI_BooleanOperation& MS) const {
// Naming of modified faces and dangle edges
  TNaming_Builder ModFBuilder(ModifiedFaces());
  QANewBRepNaming_Loader::LoadModifiedShapes(MS, MS.Shape1(), TopAbs_FACE, ModFBuilder);
  TNaming_Builder ModEBuilder(ModifiedEdges());    
  QANewBRepNaming_Loader::LoadModifiedShapes(MS, MS.Shape1(), TopAbs_EDGE, ModEBuilder);
  
  if(MS.HasGenerated()) {  
    TNaming_Builder nBuilder (NewShapes());
//  generated Edges
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_FACE, nBuilder);
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape1(), TopAbs_FACE, nBuilder);
  }
  // Naming of deleted faces edges:
  if(MS.HasDeleted()){ 
    TNaming_Builder DelFBuilder(DeletedFaces());
    QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_FACE, DelFBuilder);

    TNaming_Builder DelEBuilder(DeletedEdges());
    QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_EDGE, DelEBuilder);
  }
}

//=======================================================================
//function : LoadContent
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadContent(BRepAlgoAPI_BooleanOperation& MS) const {
  if (MS.Shape().ShapeType() == TopAbs_COMPSOLID || MS.Shape().ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(MS.Shape());
    Standard_Integer nbShapes = 0;
    while (itr.More()) {
      nbShapes++;
      itr.Next();
    }
    if (nbShapes > 1) {
      for (itr.Initialize(MS.Shape()); itr.More(); itr.Next()) {
	TNaming_Builder bContent(Content());
	bContent.Generated(itr.Value());      
      }
    }
  } 
}  

//=======================================================================
//function : LoadResult
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadResult(BRepAlgoAPI_BooleanOperation& MS) const {
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);
  TNaming_Builder Builder (ResultLabel());
  TopoDS_Shape aResult = MS.Shape();
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(aResult);
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(aResult);
      if (itr.More()) aResult = itr.Value();
    }
  }
  if (MS.Shape1().IsNull()) Builder.Generated(aResult);
  else Builder.Modify(MS.Shape1(), aResult);  
}

//=======================================================================
//function : LoadDegenerated
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadDegenerated(BRepAlgoAPI_BooleanOperation& MS) const {
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes(MS.Shape1(), TopAbs_EDGE, allEdges);
  Standard_Integer i = 1;
  for (; i <= allEdges.Extent(); i++) {
    if (BRep_Tool::Degenerated(TopoDS::Edge(allEdges.FindKey(i)))) {
      if (MS.IsDeleted(allEdges.FindKey(i))) {
	TNaming_Builder DegeneratedBuilder(DeletedDegeneratedEdges()); 
	DegeneratedBuilder.Generated(allEdges.FindKey(i));
#ifdef OCCT_DEBUG
	TDataStd_Name::Set(DegeneratedBuilder.NamedShape()->Label(), "DeletedDegenerated");
#endif
      }      
    }
  }
}

//=======================================================================
//function : IsResultChanged
//purpose  : 
//=======================================================================

Standard_Boolean QANewBRepNaming_BooleanOperationFeat::IsResultChanged(BRepAlgoAPI_BooleanOperation& MS) const {
  TopoDS_Shape ResSh = MS.Shape();
  if (MS.Shape().ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(MS.Shape());
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(MS.Shape());
      if (itr.More()) ResSh = itr.Value();
    }
  }
  return MS.Shape1().IsSame(ResSh);
}
//=======================================================================
// Workaround for evolution 1:n
//=======================================================================
static Standard_Boolean IsValidSurfType(const TopoDS_Face& theFace) {
  BRepAdaptor_Surface anAdapt(theFace);
  Handle( Adaptor3d_HCurve ) aBasisCurve;
  const GeomAbs_SurfaceType& aType = anAdapt.GetType();
  if(aType == GeomAbs_Cylinder || aType == GeomAbs_Cone)
    return Standard_True;
  else if(aType == GeomAbs_SurfaceOfRevolution){
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Line)
      return Standard_True;
  }
  else if(aType == GeomAbs_SurfaceOfExtrusion) {
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Circle || aBasisCurve->GetType() == GeomAbs_Ellipse)
      return Standard_True;
  }
#ifdef OCCT_DEBUG
  ModDbgTools_Write(theFace, "Surf");
#endif
  return Standard_False;
}
//=======================================================================
//function : IsWRCase
//purpose  : 
//=======================================================================

Standard_Boolean QANewBRepNaming_BooleanOperationFeat::IsWRCase(const BRepAlgoAPI_BooleanOperation& MS) {
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();
  const TopAbs_ShapeEnum& Type1 = ShapeType(ObjSh);
  if(Type1  == TopAbs_COMPOUND || Type1 > TopAbs_FACE) return Standard_False;
  const TopAbs_ShapeEnum& Type2 = ShapeType(ToolSh);
  if(Type2  == TopAbs_COMPOUND || Type2 > TopAbs_FACE) return Standard_False;
  TopTools_ListOfShape aList;

  
  if(Type1 != TopAbs_FACE) {
    TopExp_Explorer anExp(ObjSh, TopAbs_FACE);
    for(;anExp.More();anExp.Next()) {      
      if(IsValidSurfType(TopoDS::Face(anExp.Current())))
	aList.Append(anExp.Current());
    }
  } else 
       if(IsValidSurfType(TopoDS::Face(ObjSh)))
	aList.Append(ObjSh);
  if(aList.Extent() == 1) {
    if(Type2 != TopAbs_FACE) {
      TopExp_Explorer anExp(ToolSh, TopAbs_FACE);
      for(;anExp.More();anExp.Next()) {      
	if(IsValidSurfType(TopoDS::Face(anExp.Current())))
	  aList.Append(anExp.Current());
      }
    } else 
      if(IsValidSurfType(TopoDS::Face(ToolSh)))
	aList.Append(ToolSh);
    if(aList.Extent() == 2) return Standard_True;      
  }
  return Standard_False;      
}

//=======================================================================
static gp_Ax1 ComputeAxis(const TopoDS_Shape& theShape) {
  TopoDS_Face aFace;
  TopExp_Explorer anExp(theShape, TopAbs_FACE);
  for(;anExp.More();anExp.Next()) {      
    aFace = TopoDS::Face(anExp.Current());
    BRepAdaptor_Surface anAdapt(aFace);
    Handle( Adaptor3d_HCurve ) aBasisCurve;
    const GeomAbs_SurfaceType& aType = anAdapt.GetType();
    if(aType == GeomAbs_Cylinder) 
      return anAdapt.Cylinder().Axis();
    else if(aType == GeomAbs_Cone)
      return anAdapt.Cone().Axis();
    else if(aType == GeomAbs_SurfaceOfRevolution)
      return anAdapt.AxeOfRevolution();   
    else if(aType == GeomAbs_SurfaceOfExtrusion) {
      aBasisCurve = anAdapt.BasisCurve();
      if (aBasisCurve->GetType() == GeomAbs_Circle)
	return aBasisCurve->Circle().Axis();
      else if(aBasisCurve->GetType() == GeomAbs_Ellipse)
	return aBasisCurve->Ellipse().Axis();
    }
  }
  return gp::OX();
}
//==============================================================================
//
//==========================================================================
static Standard_Integer Identify(const TopoDS_Face& theFace, const gp_Ax1& theAx) {
  GProp_GProps aGProp;
  BRepGProp::SurfaceProperties(theFace, aGProp);
  gp_Pnt aPoint = aGProp.CentreOfMass();
  gp_Vec aV1(theAx.Direction());
  gp_Vec aV2(theAx.Location(), aPoint);
#ifdef OCCT_DEBUG
  gp_Vec v1 = aV1.Crossed(aV2);
  cout <<" Z of V1 = " << v1.XYZ().Z() << endl;
#endif
  if((aV1.Crossed(aV2)).XYZ().Z() >= 0) return 1; //right orientation
  return (-1); //left orientation
}

//=======================================================================
//function : LoadModified11
//purpose  : 1 : 1
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadModified11 (BRepAlgoAPI_BooleanOperation& MS,
						      const TopoDS_Shape&     ShapeIn,
						      const TopAbs_ShapeEnum  KindOfShape) const

{
  TopTools_MapOfShape View;
  Standard_Boolean found = Standard_False;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    if(Shapes.Extent() == 1) {found = Standard_True; break;}
  }
    
  if(found) {
    View.Clear();
    ShapeExplorer.Init (ShapeIn, KindOfShape);
    TNaming_Builder Builder(ModifiedFaces());
    for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
      const TopoDS_Shape& Root = ShapeExplorer.Current ();
      if (!View.Add(Root)) continue;
      const TopTools_ListOfShape& Shapes = MS.Modified (Root);
      if(Shapes.Extent() > 1) continue;
      TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
      for (;ShapesIterator.More (); ShapesIterator.Next ()) {
	const TopoDS_Shape& newShape = ShapesIterator.Value ();
	if (!Root.IsSame (newShape)) {
	  //put shapes with evolution 1:1 (may be Compound)
#ifdef OCCT_DEBUG
	  TCollection_AsciiString entry;
	  TDF_Tool::Entry(Builder.NamedShape()->Label(), entry);
	  cout << "Add shape to Compound at Label = "<< entry <<endl;
#endif
	  Builder.Modify (Root,newShape);
	}
      }
    }
  }
}
//======================================================================
static gp_Pnt GetCenterPoint(const TopoDS_Shape& theEdge)
{
  GProp_GProps aGProp;
  BRepGProp::LinearProperties(theEdge, aGProp);
  return aGProp.CentreOfMass();
}
//===================================================================
static void SortRootFaces(TopTools_ListOfShape& theList, const TopoDS_Shape& theShape)
{
  TopTools_ListOfShape aList;
  Standard_Integer aNum = theList.Extent();
  if(aNum <= 1) return;
  gp_Ax1 anAx  = ComputeAxis(theShape);
  TopTools_Array1OfShape  ArS(1, aNum);
  TColgp_Array1OfPnt      ArP(1, aNum);
  TColStd_Array1OfInteger ArI(1, aNum); 
  TopTools_ListIteratorOfListOfShape It(theList);
  Standard_Integer i;
  for(i=1;It.More();It.Next(),i++) {
    ArS.SetValue(i, It.Value ());
    ArI.SetValue(i,0);
    ArP.SetValue(i, GetCenterPoint(It.Value()));

  }
  gp_Pnt aPnt = anAx.Location();
  Standard_Integer I, j;
  for(j=1;j <= aNum; j++) {
    if(ArI.Value(j) == -1) continue;
    Standard_Real aD1 = aPnt.Distance(ArP(j));
    I = 0;
    for(i=1;i <= aNum; i++) {
      if(i==j) continue;
      if(ArI.Value(i) == -1) continue;
	Standard_Real aD2 = aPnt.Distance(ArP(i));
	if(aD2 < aD1) {
	  I = i;
	  aD1 = aD2;
	}
      }
    if (I == 0) continue;
    ArI.SetValue(I, -1);
    aList.Append(ArS.Value(I));
    if(aList.Extent() == aNum -1) {
      for(i=1; i<=aNum;i++)
	if(ArI.Value(i) != -1) aList.Append(ArS.Value(i));
    }
  }
  theList.Assign(aList);
}
//=======================================================================
static void Sort2Faces(const TopTools_ListOfShape& Shapes,
		       const gp_Ax1& theAx, TopTools_ListOfShape& theList)
{

  TopTools_ListIteratorOfListOfShape It(Shapes);
  for(;It.More();It.Next()) {
    if(Identify(TopoDS::Face(It.Value()), theAx) == 1)
      theList.Prepend(It.Value());   //Pos
    else theList.Append(It.Value()); //Neg
  }
}

//=======================================================================
static void Sort3Faces(const TopTools_ListOfShape& theListIn, TopTools_ListOfShape& theListOut)
{
  TopTools_ListIteratorOfListOfShape It (theListIn);
  TopTools_Array1OfShape  ArS(1, theListIn.Extent());
  TColgp_Array1OfPnt      ArP(1, theListIn.Extent());

  Standard_Integer i;
  for(i=1;It.More();It.Next(),i++) {
    ArS.SetValue(i, It.Value());
    ArP.SetValue(i, GetCenterPoint(It.Value()));
  }

  Standard_Boolean found = Standard_False;
  Standard_Integer j, i1 = 0, i2 = 0, i3 = 0; 
  TopoDS_Edge anEdge;
  for(i=1;i<=3;i++) {
    TopExp_Explorer anExp1(ArS.Value(i), TopAbs_EDGE);
    for(;anExp1.More();anExp1.Next()) {         
      for(j=1;j<=3;j++) {
	if(i==j) continue;      
	TopExp_Explorer anExp2(ArS.Value(j), TopAbs_EDGE);
	for(;anExp2.More();anExp2.Next()) {  
	  if(anExp1.Current().IsSame(anExp2.Current())){
	    found = Standard_True;
	    anEdge = TopoDS::Edge(anExp1.Current());
	    break;
	  }
	}
	if(found) break;
      }
      if(found) {
	switch(i+j) {
	case 3: //12
	  i1=1;i2=2; i3=3;
	  break;
	case 4: //13	  
	  i1=1;i2=3; i3=2;
	  break;
	case 5: //23
	  i1=2;i2=3; i3=1;
	}
	break;
      }
    }
    if(found) break;
  }

//i1,i2 - two adjacent faces via sim-edge
  gp_Pnt aPnt1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
  gp_Pnt aPnt2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));
  gp_Vec aVec(aPnt1, aPnt2);
  gp_Ax1 anAx(aPnt1, gp_Dir(aVec));
  if (Identify(TopoDS::Face(ArS.Value(i1)), anAx) == -1) {//neg
    i=i2; i2=i1; //i1 < = > i2
    i1=i;
  }
  theListOut.Append(ArS.Value(i1));
  theListOut.Append(ArS.Value(i2));
  theListOut.Append(ArS.Value(i3)); //single
}
//=======================================================================
//function : Load1nFaces
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::Load1nFaces(BRepAlgoAPI_BooleanOperation& MS, const TopoDS_Shape& ShapeIn) const
{

  TopTools_MapOfShape View;
  TopTools_ListOfShape aListR;
  TopExp_Explorer ShapeExplorer (ShapeIn, TopAbs_FACE);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    if(Shapes.Extent() < 2) continue; 
    aListR.Append(Root);
  }
  if(ShapeIn.IsEqual(MS.Shape1()))
    if(aListR.Extent() > 1) SortRootFaces(aListR, ShapeIn);
   
  TopTools_ListIteratorOfListOfShape Itr(aListR);
  for(;Itr.More();Itr.Next()) {
    const TopoDS_Shape& Root = Itr.Value();
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    TopTools_ListOfShape aList;
    gp_Ax1 anAx = ComputeAxis(MS.Shape2());	
    if(Shapes.Extent() == 2)
      Sort2Faces(Shapes, anAx, aList);
    else if(Shapes.Extent() == 3)
      Sort3Faces(Shapes, aList);
    TopTools_ListIteratorOfListOfShape It(aList);
    for(;It.More();It.Next()) {
      TNaming_Builder aBuilder(NewShapes());
//      aBuilder.Modify(Root,It.Value ());
      aBuilder.Generated(It.Value ());
    }
  }
}

//=======================================================================
//function : LoadModified faces
//purpose  : 1 : n modification
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadModified1n (BRepAlgoAPI_BooleanOperation& MS,
						    const TopoDS_Shape&     ShapeIn,
						    const TopAbs_ShapeEnum  KindOfShape) const

{ 
//fill modification 1:n
  TopTools_MapOfShape View;
  Standard_Integer aNum = 0;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    if(Shapes.Extent() >= 2) aNum += Shapes.Extent();
  }
  
  View.Clear();
  const TopoDS_Shape&     Tool = MS.Shape2();
  ShapeExplorer.Init (Tool, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    if(Shapes.Extent() >= 2) aNum += Shapes.Extent();
  }

  Handle(TDataStd_IntegerArray) aSAR;
  if(!ResultLabel().FindAttribute(TDataStd_IntegerArray::GetID(), aSAR) ) {
    //not find
    aSAR = TDataStd_IntegerArray::Set(ResultLabel(), 1, 2);
    aSAR->SetValue(1, 0); //tag num for faces structure
    aSAR->SetValue(2, 0); //tag num for edges structure
  }

  if(aSAR->Value(2))  {
    Standard_Integer aNE =0;
    TDF_Label aLab = ResultLabel().FindChild(aSAR->Value(2));
    Handle(TDataStd_Integer) anAtt;
    if(aLab.FindAttribute(TDataStd_Integer::GetID(), anAtt) ) 
      aNE = anAtt->Get();    
    TDF_Label aFLab = ResultLabel().FindChild(aLab.Tag() + aNE);
    if(!aFLab.FindAttribute(TDataStd_Integer::GetID(), anAtt))
      aSAR->SetValue(1, 0); 
  }
  TDF_Label aLabelFDS;
  if(aSAR->Value(1)) 
    aLabelFDS = ResultLabel().FindChild(aSAR->Value(1)); // !=0 -already exist
  else {
    // initial creation of FDS structure
    Handle(TDF_TagSource) aTS;
    ResultLabel().FindAttribute (TDF_TagSource::GetID (), aTS);    
    aLabelFDS = NewShapes();
//    aLabelFDS = ResultLabel().FindChild(aSAR->Value(1)); 
    aSAR->SetValue(1, aLabelFDS.Tag()); //keep tag
    aTS->Set(aLabelFDS.Tag()-1);
  }
  Handle(TDataStd_Integer) anAtt;
  if(aLabelFDS.FindAttribute(TDataStd_Integer::GetID(), anAtt) ) {
    // modification : check compatibility
    if(anAtt->Get() != aNum) {
      cout << "WARNING: Case isn't mantained - Number of Faces was changed!"<<endl;
      // mark all structure as Deleted
      Standard_Integer aN = aLabelFDS.Tag()+anAtt->Get();
      for(Standard_Integer i=aLabelFDS.Tag(); i < aN; i++) {
	TDF_Label aLab =  ResultLabel().FindChild(i, Standard_False); 
	if(!aLab.IsNull()) {
	  Handle(TNaming_NamedShape) aNS;
	  if(aLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
	    TopoDS_Shape aShape = aNS->Get();
	    TNaming_Builder aBuilder(aLab);
	    aBuilder.Delete(aShape); //Deleted
	  }
	}
      }
//
      Handle(TDF_TagSource) aTS;
      ResultLabel().FindAttribute (TDF_TagSource::GetID (), aTS);
      if(!aTS.IsNull())
	aTS->Set(aLabelFDS.Tag());
      anAtt->Set(aNum);
    }    
  } else
    TDataStd_Integer::Set(aLabelFDS, aNum); //keep number of faces

  Load1nFaces(MS, ShapeIn);
  Load1nFaces(MS, Tool);   
}


//======================================================================
static Standard_Boolean IsDirectionPositive (const gp_Ax1& theAx, const gp_Pnt thePnt1, 
					     const gp_Pnt thePnt2) {
  Standard_Boolean isPositive;
  gp_Vec aVec1(theAx.Direction());
  gp_Vec aVec2(thePnt1, thePnt2);
#ifdef OCCT_DEBUG
//  gp_Vec v1 = aVec1.Crossed(aVec2);
//  cout <<" Z of V1 = " << v1.XYZ().Z() << endl;
#endif
  if((aVec1.Crossed(aVec2)).XYZ().Z() >= 0) isPositive = Standard_True;
  else 
    isPositive = Standard_False;
  return isPositive;
}
//======================================================================
// i => ArS[i] : ArP[i] ; i = ArI[j]
//======================================================================
static void SortEdges2(const TColgp_Array1OfPnt& theArP, const gp_Ax1& theAx,
		      TColStd_Array1OfInteger& theArI)
{
  gp_Pnt aPnt = theAx.Location();  
  //sort : the nearest point must be first
  Standard_Real aD1 = aPnt.Distance(theArP.Value(1));
  Standard_Real aD2 = aPnt.Distance(theArP.Value(2));
  if(aD1 < aD2) {
    theArI.SetValue(1, 1);
    theArI.SetValue(2, 2);
  } else  {
    theArI.SetValue(1, 2); //change order
    theArI.SetValue(2, 1);
  }
}
//======================================================================
// i => ArS[i] : ArP[i] ; i = ArI[j]
//======================================================================
static void SortEdges3(const TopTools_Array1OfShape& theArS, const TColgp_Array1OfPnt& theArP, 
		       const gp_Ax1& theAx, TColStd_Array1OfInteger& theArI)
{
  Standard_Integer i, j, i1 = 0,i2 = 0, i3 = 0;
  TopoDS_Shape aV;
  Standard_Boolean adjacent = Standard_False;
  for(i=1;i<=3;i++) {    
    for(j=1;j<=3;j++) {
      if(i==j) continue;
      const TopoDS_Shape& aV11 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i)));
      const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(j)));
      const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(j)));      
      if(aV11.IsSame(aV21) || aV11.IsSame(aV22)) {adjacent = Standard_True;aV = aV11;}
      else {
	const TopoDS_Shape& aV12 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i)));
	if(aV12.IsSame(aV21) || aV12.IsSame(aV22)) {adjacent = Standard_True;aV = aV12;}
      }
      if(adjacent) {
	Standard_Integer aSum = i+j;
	switch(aSum) {
	case 3: //12
	  i1 = 3;i2 = 1;i3 = 2;
	  break;
	case 4: //13
	  i1 = 2; i2 = 1;i3 = 3;
	  break;
	case 5: //23
	  i1 = 1; i2 = 2;i3 = 3;
	}
	break;
      }
    }
    if(adjacent) break; 
  }
  gp_Pnt aPnt = theAx.Location(); 
// i1 - index of single edge
  Standard_Real aD1 = aPnt.Distance(theArP.Value(i1));
  Standard_Real aD2 = aPnt.Distance(theArP.Value(i2));
  if(aD1 > aD2) { //cyclic shift
    Standard_Integer aN = i3;// i1 => i3 - to the end
    i3 = i1; i1 = aN;
    // pair of adjacent i1, i2
    gp_Pnt aCP = BRep_Tool::Pnt(TopoDS::Vertex(aV)); 
    if(!IsDirectionPositive(theAx, aCP, theArP.Value(i1))) {//first must be positive direction       
      // change i1 <=>i2
      aN = i2; i2 = i1;
      i1 = aN;
    }    
  } else {
    // pair of adjacent i2, i3
    gp_Pnt aCP = BRep_Tool::Pnt(TopoDS::Vertex(aV)); 
    if(!IsDirectionPositive(theAx, aCP, theArP.Value(i2))) {//first must be positive direction
      // change i2 <=>i3
      Standard_Integer aN = i3; i3 = i2;
      i2 = aN;
      }
  }
// order i1, i2, i3 
  theArI.SetValue(1, i1); 
  theArI.SetValue(2, i2);
  theArI.SetValue(3, i3);
}

//======================================================================
// i => ArS[i] : ArP[i] ; i = ArI[j]
//======================================================================
static void SortEdges4(const TopTools_Array1OfShape& theArS, const TColgp_Array1OfPnt& theArP, 
		       const gp_Ax1& theAx, TColStd_Array1OfInteger& theArI)
{
// 1. find adjacent edges, build pairs in ArI
// 2. find nearest pair, reorganize ArI
// 3. sort inside pairs
// =======================================
  Standard_Integer i, j, i1 = 0,i2 = 0, i3 = 0, i4 = 0;
// 1.
  TopoDS_Shape aV1;
  for(i=1;i<=4;i++) { 
    const TopoDS_Shape& aV11 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i)));
    const TopoDS_Shape& aV12 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i)));
    Standard_Boolean aDjacent = Standard_False;
    for(j=1;j<=4;j++) {
      if(i==j) continue;
      const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(j)));
      const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(j)));
      aDjacent = Standard_False;
      if(aV11.IsSame(aV21) || aV11.IsSame(aV22)) {aDjacent = Standard_True;aV1 = aV11;}
      else 
	if(aV12.IsSame(aV21) || aV12.IsSame(aV22)) {aDjacent = Standard_True;aV1 = aV12;}
      if(aDjacent) {
	aDjacent = Standard_True;
	Standard_Integer aSum = i+j;
	i1 = i; i2 = j; 
	switch(aSum) {
	case 3: //12
	  i3 = 3; i4 = 4;
	  break;
	case 4: //13
	  i3 = 2; i4 = 4;
	  break;
	case 5: //14
	  i3 = 2; i4 = 3;
	  break;
	}
	break;
      }
    }
    if(aDjacent) break;
  }
// i1,i2 - first pair of adjacent: aV1.
// i3,i4 - next  pair of adjacent: aV2.
// find agjacent V (i3-i4)
  TopoDS_Shape aV2;
  const TopoDS_Shape& aV11 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i3)));
  const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i4)));
  const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i4)));
  if(aV11.IsSame(aV21) || aV11.IsSame(aV22)) aV2 = aV11;
  else {
    const TopoDS_Shape& aV12 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i3)));
    if(aV12.IsSame(aV21) || aV12.IsSame(aV22)) aV2 = aV12;
  }

// 2. find nearest pair
  gp_Pnt aCP1 = BRep_Tool::Pnt(TopoDS::Vertex(aV1)); 
  gp_Pnt aCP2 = BRep_Tool::Pnt(TopoDS::Vertex(aV2)); 
  gp_Pnt aPnt = theAx.Location();
  Standard_Real aD1 = aPnt.Distance(aCP1);//i1-i2
  Standard_Real aD2 = aPnt.Distance(aCP2);//i3-i4
  if(aD1 > aD2) { //change order of pairs
    Standard_Integer a3 = i3;// i1,i2 => i3,i4 - to the end
    Standard_Integer a4 = i4;
    i3 = i1; i4 = i2;
    i1 = a3; i2 = a4;
    gp_Pnt aP = aCP2;
    aCP2 = aCP1;
    aCP1 = aP;
    // pair of adjacent i1-i2 is the nearest
  }    

// 3. sort inside pairs
  if(!IsDirectionPositive(theAx, aCP1, theArP.Value(i1))) {//first must be positive direction
    // change i1 <=> i2
    Standard_Integer aN = i2; i2 = i1;
    i1 = aN;
  }

  if(!IsDirectionPositive(theAx, aCP2, theArP.Value(i3))) {//first must be positive direction
    // change i3 <=> i4
#ifdef OCCT_DEBUG
  cout << "SortEdges4: i3 = "<<i3<< "i4 = "<< i4 << endl;
#endif
    Standard_Integer aN = i4; i4 = i3;
    i3 = aN;
  }

// 4. final order i1, i2, i3, i4 - Ok
#ifdef OCCT_DEBUG
  cout << "SortEdges4: i1 = " <<i1<<" i2 = "<<i2<< " i3 = "<<i3<< "i4 = "<< i4 << endl;
#endif
  theArI.SetValue(1, i1); 
  theArI.SetValue(2, i2);
  theArI.SetValue(3, i3);
  theArI.SetValue(4, i4);
}
// ======================================================================
static void SortEdges5 (const TopTools_Array1OfShape& theArS, const TColgp_Array1OfPnt& theArP, 
		       const gp_Ax1& theAx, TColStd_Array1OfInteger& theArI)
{
// =======================================
// 1. find middle edge from gr. of 3 edges, build two groups in ArI
// 2. find nearest group, reorganize ArI - nerest => top
// 3. sort inside groups
// 3.1. sort inside group of 2 edges
// 3.2. sort inside group of 3 edges
// =======================================
  Standard_Integer i, j, i1,i2, i3, i4, i5;
// 1.
  TopoDS_Shape aV1, aV2, aV;
  Standard_Integer I=0, J1=0, J2=0;
  for(i=1;i<=5;i++) {    
    Standard_Boolean found = Standard_False;
    const TopoDS_Shape& aV11 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i)));
    for(j=1;j<=5;j++) {
      if(i==j) continue;
      const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(j)));
      const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(j)));
      if(aV11.IsSame(aV21) || aV11.IsSame(aV22)) {
	aV1 = aV11; I = i; J1 = j;
	found = Standard_True;
	break;
      }
    }
    if (found) {
      found = Standard_False;
      const TopoDS_Shape& aV12 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i)));
      for(j=1;j<=5;j++) {
	if(i==j) continue;
	const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(j)));
	const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(j)));
	if(aV12.IsSame(aV21) || aV12.IsSame(aV22)) {
	  aV2 = aV12; J2 = j;
#ifdef OCCT_DEBUG
	  if(I != i) cout << "WARNING:: I != i, I = " << I << ", i = " << i <<endl; 
#endif
	  found = Standard_True;
	  break;
	}
      }
    }
    if (found) break;
  }
// aV1, aV2 - vetexes of middle Edge, I - index of middle Edge, J1, J2 = indexes of 
// adjacent edges of the middle edge

// init & shift group from 3 edges on the top 
  i1=J1; i2=I; i3 = J2; i4=0; i5=0;
  for(i=1; i<=5;i++) {
    if(i==i1 || i==i2 ||i==i3) continue;
    if(!i4) i4=i;
    else i5 = i;
  }

// find agjacent V (i4-i5)
  TopoDS_Shape aV3;
  const TopoDS_Shape& aV11 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i4)));
  const TopoDS_Shape& aV21 = TopExp::FirstVertex(TopoDS::Edge(theArS.Value(i5)));
  const TopoDS_Shape& aV22 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i5)));
  if(aV11.IsSame(aV21) || aV11.IsSame(aV22)) aV3 = aV11;
  else {
    const TopoDS_Shape& aV12 = TopExp::LastVertex(TopoDS::Edge(theArS.Value(i4)));
    if(aV12.IsSame(aV21) || aV12.IsSame(aV22)) aV3 = aV12;
  }
  
// 2. find nearest group (aV1, aV3), reorganize ArI - nerest => top
  gp_Pnt aDP1 = BRep_Tool::Pnt(TopoDS::Vertex(aV1)); 
  gp_Pnt aDP3 = BRep_Tool::Pnt(TopoDS::Vertex(aV3)); 
  gp_Pnt aPnt = theAx.Location();
  Standard_Real aD1 = aPnt.Distance(aDP1);//i1-i2-i3
  Standard_Real aD2 = aPnt.Distance(aDP3);//i4-i5
  Standard_Integer aTop = 3;
  if(aD1 > aD2) { //change order of groups
    aTop = 2;
    Standard_Integer a4 = i4;// i1,i2 => i4,i5 - to the end
    Standard_Integer a5 = i5;
    i4 = i2; i5 = i1; // i4 - middle edge
    i1 = a4; i2 = a5;
    gp_Pnt aP1 = aDP1;    
    aDP1 = aDP3;
    aDP3 = aP1;
    // goup of 2 edges i1-i2 is the nearest
  }
// 3.1. sort inside group of 2 edges
  gp_Pnt aDP, aCP;
  if(aTop == 2) {aDP = aDP1; aCP = theArP.Value(i1);} //i1,i2 
  else {aDP = aDP3; aCP = theArP.Value(i4);} //i4, i5 - group of 2 edges at the bottom
  if(!IsDirectionPositive(theAx, aDP, aCP)) {//first must be positive direction
    Standard_Integer aN;
    if(aTop == 2) {
    // change i1 <=> i2
      aN = i2; i2 = i1;
      i1 = aN;
    } else {
      // change i4 <=> i5
      aN = i5; i5 = i4;
      i4 = aN;
    }
  }
// 3.2. sort inside group of 3 edges
  if(aTop == 2) {
    //i3,i4,i5
    aDP = theArP.Value(i4); //center of middle edge
    aCP = theArP.Value(i3); 
  } else {
    //i1,i2,i3
    aDP = theArP.Value(i2);
    aCP = theArP.Value(i1);
  }

  if(!IsDirectionPositive(theAx, aDP, aCP)) {//first must be positive direction
    Standard_Integer aN;
    if(aTop == 2) {
    // change i3 <=> i5
      aN = i5; i5 = i3;
      i3 = aN;
    } else {
      // change i1 <=> i3
      aN = i3; i3 = i1;
      i1 = aN;
    }
  }
// 4. final order i1, i2, i3, i4, i5 - Ok
  theArI.SetValue(1, i1); 
  theArI.SetValue(2, i2);
  theArI.SetValue(3, i3);
  theArI.SetValue(4, i4);
  theArI.SetValue(5, i5);
}
//=======================================================================
static void FindAdjacent2(const TopTools_ListOfShape& theList, 
			 TopTools_ListOfShape& theListOfEdges) {
  TopTools_ListIteratorOfListOfShape It (theList);
  const TopoDS_Shape& aShape1 = It.Value (); It.Next ();
  const TopoDS_Shape& aShape2 = It.Value ();
  if(!aShape1.IsNull() && !aShape2.IsNull()) {
    TopExp_Explorer anExp1(aShape1, TopAbs_EDGE);
    for(;anExp1.More();anExp1.Next()) {      
      TopExp_Explorer anExp2(aShape2, TopAbs_EDGE);
      for(;anExp2.More();anExp2.Next()) {  
	if(anExp1.Current().IsSame(anExp2.Current()))
	  theListOfEdges.Append(anExp1.Current());
      }
    }
  }
}
//=======================================================================
static void FindAdjacent3(const TopTools_ListOfShape& theList, 
			  TopTools_ListOfShape& theListOfEdges) {
  TopTools_ListIteratorOfListOfShape It (theList);
  TopTools_Array1OfShape  ArS(1, theList.Extent());
  TColgp_Array1OfPnt      ArP(1, theList.Extent());
  TColgp_Array1OfDir      ArD(1, theList.Extent());
  Standard_Integer i;
  for(i=1;It.More();It.Next(),i++) {
    ArS.SetValue(i, It.Value());
    gp_Ax1 anAx = ComputeAxis(It.Value()); 
    ArP.SetValue(i, anAx.Location());
    ArD.SetValue(i, anAx.Direction());
  }
  Standard_Boolean aDjacent = Standard_False;
  Standard_Integer j, i2 = 0, i3 = 0; //i2, i3 - indexes of two adjacent faces having the same surface
  Standard_Integer i1 = 0; //single face
  for(i=1;i<=3;i++) {    
    for(j=1;j<=3;j++) {
      if(i==j) continue;
      if(ArP.Value(i).IsEqual(ArP.Value(j), Precision::Confusion()) 
	 && ArD.Value(i).IsEqual(ArD.Value(j), Precision::Angular())) { 
	aDjacent = Standard_True;
	Standard_Integer aSum = i+j;
	switch(aSum) {
	case 3: //12
	  i1 = 3; i2 = 1; i3 = 2;
	  break;
	case 4: //13
	  i1 = 2; i2 = 1; i3 = 3;
	  break;
	case 5: //23
	  i1 = 1; i2 = 2; i3 = 3;
	  break;
	default:
	  i1 = 1; i2 = 2; i3 = 3;
	}
	break;
      }
    }
    if(aDjacent) break;
  }

  TopExp_Explorer anExp1(ArS.Value(i1), TopAbs_EDGE);
  for(;anExp1.More();anExp1.Next()) {
    Standard_Boolean found = Standard_False;
    TopExp_Explorer anExp2(ArS.Value(i2), TopAbs_EDGE);
    for(;anExp2.More();anExp2.Next()) {  
      if(anExp1.Current().IsSame(anExp2.Current()))
	{theListOfEdges.Append(anExp1.Current()); found=Standard_True; break;}
    }
    if(!found) {
      TopExp_Explorer anExp3(ArS.Value(i3), TopAbs_EDGE);
      for(;anExp3.More();anExp3.Next()) {  
	if(anExp1.Current().IsSame(anExp3.Current()))
	  {theListOfEdges.Append(anExp1.Current());break;}
      }
    }
  }
}
//=======================================================================
static void FindAdjacent4(const TopTools_ListOfShape& theList, 
			  TopTools_ListOfShape& theListOfEdges) {
  TopTools_ListIteratorOfListOfShape It (theList);
  TopTools_Array1OfShape  ArS(1, theList.Extent());
  TColgp_Array1OfPnt      ArP(1, theList.Extent());
  TColgp_Array1OfDir      ArD(1, theList.Extent());
  Standard_Integer i;
  for(i=1;It.More();It.Next(),i++) {
    ArS.SetValue(i, It.Value());
    gp_Ax1 anAx = ComputeAxis(It.Value()); 
    ArP.SetValue(i, anAx.Location());
    ArD.SetValue(i, anAx.Direction());
  }
  //find pairs
  Standard_Integer j, i3=0, i4 = 0;//i3, i4 - indexes of two adjacent faces having the same surface
  Standard_Integer i1 = 0, i2 = 0; 
  Standard_Boolean aDjacent = Standard_False;
  for(i=1;i<=4;i++) {    
    for(j=1;j<=4;j++) {
      if(i==j) continue;
      if(ArP.Value(i).IsEqual(ArP.Value(j), Precision::Confusion()) 
	 && ArD.Value(i).IsEqual(ArD.Value(j), Precision::Angular())) {
	aDjacent = Standard_True;
	Standard_Integer aSum = i+j;
	i1 = i; i2 = j; 
	switch(aSum) {
	case 3: //12
	  i3 = 3; i4 = 4;
	  break;
	case 4: //13
	  i3 = 2; i4 = 4;
	  break;
	case 5: //14
	  i3 = 2; i4 = 3;
	  break;
	default:
	  i3 = 3; i4 = 4;
	}
	break;
      }
    }
    if(aDjacent) break;
  }

  TopExp_Explorer anExp1(ArS.Value(i1), TopAbs_EDGE);
  for(;anExp1.More();anExp1.Next()) {
    Standard_Boolean found = Standard_False;
    TopExp_Explorer anExp2(ArS.Value(i3), TopAbs_EDGE);
    for(;anExp2.More();anExp2.Next()) {  
      if(anExp1.Current().IsSame(anExp2.Current()))
	{theListOfEdges.Append(anExp1.Current()); found=Standard_True; break;}
    }
    if(!found) {
      TopExp_Explorer anExp3(ArS.Value(i4), TopAbs_EDGE);
      for(;anExp3.More();anExp3.Next()) {  
	if(anExp1.Current().IsSame(anExp3.Current()))
	  {theListOfEdges.Append(anExp1.Current());break;}
      }
    }
  }
//
  anExp1.Init(ArS.Value(i2), TopAbs_EDGE);
  for(;anExp1.More();anExp1.Next()) {
    Standard_Boolean found = Standard_False;
    TopExp_Explorer anExp2(ArS.Value(i3), TopAbs_EDGE);
    for(;anExp2.More();anExp2.Next()) {  
      if(anExp1.Current().IsSame(anExp2.Current()))
	{theListOfEdges.Append(anExp1.Current()); found=Standard_True; break;}
    }
    if(!found) {
      TopExp_Explorer anExp3(ArS.Value(i4), TopAbs_EDGE);
      for(;anExp3.More();anExp3.Next()) {  
	if(anExp1.Current().IsSame(anExp3.Current()))
	  {theListOfEdges.Append(anExp1.Current());break;}
      }
    }
  }
}

//=======================================================================
// SortEdges: returns 
//=======================================================================
static void SortEdges(const TopTools_ListOfShape& theListE, const gp_Ax1& theAx, 
		     TopTools_Array1OfShape& theARS)
{
  
  Standard_Integer aNE1 = theListE.Extent();
  TopTools_Array1OfShape  ArS(1, aNE1);
  TColgp_Array1OfPnt      ArP(1, aNE1);
  TColStd_Array1OfInteger ArI(1, aNE1); 
  TopTools_ListIteratorOfListOfShape It (theListE);//pairs of edges
  //for (Standard_Integer i=1;It.More (); It.Next (),i++) {
  Standard_Integer i;
  for (i=1;It.More (); It.Next (),i++) {
    ArS.SetValue(i, It.Value ());
    ArI.SetValue(i,0);
    ArP.SetValue(i, GetCenterPoint(It.Value()));
  }
  switch(aNE1) {
  case 2:
    // Identify position  
    SortEdges2(ArP, theAx, ArI);
    break;
  case 3:
    SortEdges3(ArS, ArP, theAx, ArI);
    break;
  case 4:
    SortEdges4(ArS, ArP, theAx, ArI);
    break;
  case 5:
    SortEdges5(ArS, ArP, theAx, ArI);
    break;
  }
  
  for(i=1;i<=ArI.Upper();i++) {
#ifdef OCCT_DEBUG
    cout << "SortEdges: i = " <<i<<" ArI.Value(i) = " <<ArI.Value(i)<< endl;
#endif   
    theARS.SetValue(i, ArS.Value(ArI.Value(i)));  

  }
}
//=======================================================================
//function : LoadSymmetricalEdges
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadSymmetricalEdges (BRepAlgoAPI_BooleanOperation& MS) const
{
  const TopoDS_Shape& aResult   = MS.Shape();
  if(aResult.IsNull()) return;
  const TopoDS_Shape& ObjSh     = MS.Shape1();
  const TopoDS_Shape& ToolSh    = MS.Shape2();
  const TopAbs_ShapeEnum& Type1 = ShapeType(ObjSh);
  if(Type1  == TopAbs_COMPOUND || Type1 > TopAbs_FACE) return;
  const TopAbs_ShapeEnum& Type2 = ShapeType(ToolSh);
  if(Type2  == TopAbs_COMPOUND || Type2 > TopAbs_FACE) return;
  TopTools_ListOfShape aList0;
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(aResult);
    for (; itr.More(); itr.Next())
      aList0.Append(itr.Value()); //collect separated entities (bodies)
    
  }
  if(aList0.Extent() > 2) return; // case > 2 ent. is not considered
  TopTools_ListOfShape aList1, aList2;
  TopTools_ListIteratorOfListOfShape It (aList0); //each item (body) must have at least 1 pair 
  // of "cyl/con" surfaces (in some cases may be 3 or 4 faces depending on sim-edge position)
  for (;It.More (); It.Next ()) {
    const TopoDS_Shape& aShape = It.Value (); //1-st solid/shell
    TopTools_ListOfShape aList;
    aList.Clear();
#ifdef OCCT_DEBUG
//    ModDbgTools_Write(aShape, "S0");
#endif
    if(aShape.ShapeType() != TopAbs_FACE) {
      TopExp_Explorer anExp(aShape, TopAbs_FACE);      
      for(;anExp.More();anExp.Next()) {      
	if(IsValidSurfType(TopoDS::Face(anExp.Current()))) 
	  aList.Append(anExp.Current()); // faces of the current entity
      }
    } else 
      if(IsValidSurfType(TopoDS::Face(ObjSh)))
	aList.Append(aShape);
    
    if(aList1.Extent() == 0 )
      aList1.Assign(aList);
    else 
      aList2.Assign(aList);
  }
// aList1,2 contain pairs of faces having more then 1 neghbour edge (each pair)
  const Standard_Integer aNF1 = aList1.Extent(); // keep n of faces of the first entity
  const Standard_Integer aNF2 = aList2.Extent(); // keep n of faces of the second entity
  if(aNF1 + aNF2 < 2) return;

//find Edges
  TopTools_ListOfShape aListE1, aListE2;
  Standard_Integer aNE1=0,aNE2=0;
  if(aNF1 == 2 && !aNF2) {//trivial case - only 2 faces
    FindAdjacent2(aList1, aListE1);
    aNE1 = aListE1.Extent();
  }
  else { // result is compound of two ent.
    if (aNF1 == 2)  //first ent. has 2 valid faces
      FindAdjacent2(aList1, aListE1); // find adjacent edges
    else if (aNF1 == 3) // first ent. has 3 valid faces
      FindAdjacent3(aList1, aListE1);
    else if (aNF1 == 4)  //first ent. has 4 valid faces
      FindAdjacent4(aList1, aListE1);
// set number of symmetry Edges of the first ent. 
    aNE1 = aListE1.Extent();    

// Second ent.
    if (aNF2 == 2)  //second ent. has 2 valid faces
      FindAdjacent2(aList2, aListE2);  
    else if (aNF2 == 3) 
      FindAdjacent3(aList2, aListE2);
    else if (aNF2 == 4) 
      FindAdjacent4(aList2, aListE2);
    
    aNE2 =  aListE2.Extent();
  }
//aListE1, aListE2 - contains Edges
//  if(aNE1 < 2) return;
// check topological compatibility
//if exist
  Handle(TDataStd_IntegerArray) aSAR;
  if(!ResultLabel().FindAttribute(TDataStd_IntegerArray::GetID(), aSAR) ) {
    aSAR = TDataStd_IntegerArray::Set(ResultLabel(), 1, 2);
    aSAR->SetValue(1, 0); //tag num for faces structure
    aSAR->SetValue(2, 0); //tag num for edges structure
  }
  TDF_Label aLabelEDS;
  if(aSAR->Value(2)) aLabelEDS = ResultLabel().FindChild(aSAR->Value(2)); // !=0
  else {
    // initial creation of EDS structure
    aLabelEDS = NewShapes();
    aSAR->SetValue(2, aLabelEDS.Tag()); //keep tag
  }
  Handle(TDataStd_Integer) anAtt;
  if(aLabelEDS.FindAttribute(TDataStd_Integer::GetID(), anAtt) ) {
    // modification : check compatibility
    if(anAtt->Get() != aNE1+aNE2) {
      cout << "WARNING: Case isn't mantained - Number of Edges was changed!"<<endl;
      // mark all structure as Deleted
      Standard_Integer aN = aLabelEDS.Tag()+anAtt->Get();
      for(Standard_Integer i=aLabelEDS.Tag(); i < aN; i++) {
	TDF_Label aLab =  ResultLabel().FindChild(i, Standard_False); 
	if(!aLab.IsNull()) {
	  Handle(TNaming_NamedShape) aNS;
	  if(aLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
	    TopoDS_Shape aShape = aNS->Get();
	    TNaming_Builder aBuilder(aLab);
	    aBuilder.Delete(aShape); //Deleted
	  }
	}
      }
//
      Handle(TDF_TagSource) aTS;
      ResultLabel().FindAttribute (TDF_TagSource::GetID (), aTS);
      if(!aTS.IsNull())
	aTS->Set(aLabelEDS.Tag());
      anAtt->Set(aNE1+aNE2);
    }
    
  } else
    TDataStd_Integer::Set(aLabelEDS, aNE1+aNE2); //keep number of edges 
  
// Identification
//  if(aNE1% 2 > 0) return;
  gp_Ax1 anAx  = ComputeAxis(MS.Shape2());
  Handle(TDF_TagSource) aTS;
  ResultLabel().FindAttribute (TDF_TagSource::GetID (), aTS);
  if(!aNE2 && aNE1) { // only 1 ent.
//Top || Bot ?
    TopTools_Array1OfShape  ArS1(1, aNE1);
    SortEdges(aListE1, anAx, ArS1);
    for(Standard_Integer i=1; i <= aNE1; i++) {
      TopoDS_Shape aShape = ArS1.Value(i);
      Standard_Integer aLabTag = aLabelEDS.Tag() + i -1;
      TDF_Label aLab =  ResultLabel().FindChild(aLabTag); 
      aTS->Set(aLabTag);
      TNaming_Builder aBuilder(aLab);
      aBuilder.Generated(aShape);
    }
  } else if(aNE1 && aNE2) { //2 ent.
    TopTools_Array1OfShape  ArS1(1, aNE1);
    SortEdges(aListE1, anAx, ArS1);
    TopTools_Array1OfShape  ArS2(1, aNE2);
    SortEdges(aListE2, anAx, ArS2);

    gp_Pnt aPnt1 = GetCenterPoint(aListE1.First());
//    gp_Pnt aPnt2 = GetCenterPoint(aListE2.First());
    if(IsDirectionPositive(anAx, anAx.Location(), aPnt1)) {
      Standard_Integer i;
      for(i=1; i <= aNE1; i++) {
	TopoDS_Shape aShape = ArS1.Value(i);
	Standard_Integer aLabTag = aLabelEDS.Tag() + i - 1;
	TDF_Label aLab =  ResultLabel().FindChild(aLabTag); 
	aTS->Set(aLabTag);
	TNaming_Builder aBuilder(aLab);
	aBuilder.Generated(aShape);	
      }
      Standard_Integer start =  aLabelEDS.Tag() + aNE1;
      for(i=1; i <= aNE2; i++) {
	TopoDS_Shape aShape = ArS2.Value(i);
	Standard_Integer aLabTag = start + i - 1;
	TDF_Label aLab =  ResultLabel().FindChild(aLabTag); 
	aTS->Set(aLabTag);
	TNaming_Builder aBuilder(aLab);
	aBuilder.Generated(aShape);	
      }
    }
    else {
      Standard_Integer i;
      for(i=1; i <= aNE2; i++) {
	TopoDS_Shape aShape = ArS2.Value(i);
	Standard_Integer aLabTag = aLabelEDS.Tag() + i - 1;
	TDF_Label aLab =  ResultLabel().FindChild(aLabTag); 
	aTS->Set(aLabTag);
	TNaming_Builder aBuilder(aLab);
	aBuilder.Generated(aShape);	
      }
      Standard_Integer start =  aLabelEDS.Tag() + aNE2;
      for(i=1; i <= aNE1; i++) {
	TopoDS_Shape aShape = ArS1.Value(i);
	Standard_Integer aLabTag = start + i - 1;
	TDF_Label aLab =  ResultLabel().FindChild(aLabTag); 
	aTS->Set(aLabTag);
	TNaming_Builder aBuilder(aLab);
	aBuilder.Generated(aShape);	
      }
    }
  }
}

//=======================================================================
//function : ISWRCase2
//purpose  : 
//=======================================================================
Standard_Boolean QANewBRepNaming_BooleanOperationFeat::IsWRCase2(BRepAlgoAPI_BooleanOperation& MS) {
  const TopoDS_Shape& Result = MS.Shape();
  const TopAbs_ShapeEnum& ResType = ShapeType(Result);
  if(ResType  == TopAbs_COMPOUND || ResType >= TopAbs_FACE) return Standard_False;

  TopExp_Explorer anExp(Result, TopAbs_FACE);
  for(;anExp.More();anExp.Next()) {
    if(IsValidSurfType(TopoDS::Face(anExp.Current()))) {
      TopExp_Explorer anExp1(Result, TopAbs_FACE);
      for(;anExp1.More();anExp1.Next()) {
	if(!anExp1.Current().IsSame(anExp.Current()) && !IsValidSurfType(TopoDS::Face(anExp1.Current()))) {
	  TopTools_ListOfShape aList;
	  aList.Append(anExp.Current());
	  aList.Append(anExp1.Current());
	  TopTools_ListOfShape anEList;
	  FindAdjacent2(aList, anEList);
	  if(anEList.Extent() == 2) {
	    return Standard_True;
	  }
	}
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : LoadWRCase
//purpose  : 
//=======================================================================

void QANewBRepNaming_BooleanOperationFeat::LoadWRCase(BRepAlgoAPI_BooleanOperation& MS) const {
  const TopoDS_Shape& Result = MS.Shape();
  const TopAbs_ShapeEnum& ResType = ShapeType(Result);
  if(ResType  == TopAbs_COMPOUND || ResType >= TopAbs_FACE) return;

  TopExp_Explorer anExp(Result, TopAbs_FACE);
  for(;anExp.More();anExp.Next()) {
    if(IsValidSurfType(TopoDS::Face(anExp.Current()))) {
      TopExp_Explorer anExp1(Result, TopAbs_FACE);
      for(;anExp1.More();anExp1.Next()) {
	if(!anExp1.Current().IsSame(anExp.Current()) && !IsValidSurfType(TopoDS::Face(anExp1.Current()))) {
	  TopTools_ListOfShape aList;
	  aList.Append(anExp.Current());
	  aList.Append(anExp1.Current());
	  TopTools_ListOfShape anEList;
	  FindAdjacent2(aList, anEList);
	  if(anEList.Extent() == 2) {
	    
 	    TopTools_ListIteratorOfListOfShape anEIt(anEList);
	    GProp_GProps anE1Props, anE2Props;
	    BRepGProp::LinearProperties(anEList.First(), anE1Props);
	    BRepGProp::LinearProperties(anEList.Last(), anE2Props);

	    const TDF_Label& WRE1Label = ResultLabel().NewChild();
	    const TDF_Label& WRE2Label = ResultLabel().NewChild();
	    const TDF_Label& WRV1Label = ResultLabel().NewChild();
	    const TDF_Label& WRV2Label = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
	    TDataStd_Name::Set(WRE1Label, "WorkAroundEdge1");
	    TDataStd_Name::Set(WRE2Label, "WorkAroundEdge2");
	    TDataStd_Name::Set(WRV1Label, "WorkAroundVertex1");
	    TDataStd_Name::Set(WRV2Label, "WorkAroundVertex2");
#endif

	    TNaming_Builder anEBuilder1(WRE1Label);
	    TNaming_Builder anEBuilder2(WRE2Label);
	    TNaming_Builder aVBuilder1(WRV1Label);
	    TNaming_Builder aVBuilder2(WRV2Label);

	    if(anE1Props.Mass() > anE2Props.Mass()) {
	      anEBuilder1.Generated(anEList.Last());
	      anEBuilder2.Generated(anEList.First());
	      aVBuilder1.Generated(TopExp::FirstVertex(TopoDS::Edge(anEList.Last())));
	      aVBuilder2.Generated(TopExp::LastVertex(TopoDS::Edge(anEList.Last())));
	    } else {
	      anEBuilder1.Generated(anEList.First());
	      anEBuilder2.Generated(anEList.Last());
	      aVBuilder1.Generated(TopExp::FirstVertex(TopoDS::Edge(anEList.First())));
	      aVBuilder2.Generated(TopExp::LastVertex(TopoDS::Edge(anEList.First())));
	    }
	  }
	}
      }
    }
  }
}
