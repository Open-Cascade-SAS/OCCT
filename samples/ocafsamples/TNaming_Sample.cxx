// Created on: 1999-12-29
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <TopOpeBRepBuild_HBuilder.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>

#include <BRepAlgo_Cut.hxx>
#include <BRepAlgo.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming.hxx>

// =======================================================================================
// This sample contains template for typical actions with OCAF Topologigal Naming services
// =======================================================================================

#ifdef DEB

#define Box1POS          1
#define Box2POS          2
#define SelectedEdgesPOS 3
#define FilletPOS        4
#define CutPOS           5

void Sample()
{
  // Starting with data framework 
  Handle(TDF_Data) DF = new TDF_Data();
  TDF_Label aLabel = DF->Root();

  TopoDS_Shape Shape, Context;

  // ======================================================
  // Creating  NamedShapes with different type of Evolution
  // Scenario:
  // 1.Create Box1 and push it as PRIMITIVE in DF
  // 2.Create Box2 and push it as PRIMITIVE in DF
  // 3.Move Box2 (applying a transformation)
  // 4.Push a selected edges of top face of Box1 in DF,
  //   create Fillet (using selected edges) and push result as modification of Box1
  // 5.Create a Cut (Box1, Box2) as modification of Box1 and push it in DF
  // 6.Recover result from DF
  // ======================================================

  // =====================================
  // 1.Box1, TNaming_Evolution == PRIMITIVE
  // =====================================
  BRepPrimAPI_MakeBox MKBOX1( 100, 100, 100); // creating Box1

 //Load the faces of the box in DF
  TDF_Label Box1Label = aLabel.FindChild(Box1POS);
  TDF_Label Top1      = Box1Label.FindChild(1);
  TDF_Label Bottom1   = Box1Label.FindChild(2);
  TDF_Label Right1    = Box1Label.FindChild(3);
  TDF_Label Left1     = Box1Label.FindChild(4);
  TDF_Label Front1    = Box1Label.FindChild(5);
  TDF_Label Back1     = Box1Label.FindChild(6);

  TNaming_Builder Box1Ins (Box1Label);
  Box1Ins.Generated (MKBOX1.Shape());
  
  TNaming_Builder Top1FaceIns (Top1);
  TopoDS_Face Top1Face = MKBOX1.TopFace ();
  Top1FaceIns.Generated (Top1Face);  

  TopoDS_Face Bottom1Face = MKBOX1.BottomFace ();
  TNaming_Builder Bottom1FaceIns (Bottom1); 
  Bottom1FaceIns.Generated (Bottom1Face);
 
  TopoDS_Face Right1Face = MKBOX1.RightFace ();
  TNaming_Builder Right1FaceIns (Right1); 
  Right1FaceIns.Generated (Right1Face); 

  TopoDS_Face Left1Face = MKBOX1.LeftFace ();
  TNaming_Builder Left1FaceIns (Left1); 
  Left1FaceIns.Generated (Left1Face); 

  TopoDS_Face Front1Face = MKBOX1.FrontFace ();
  TNaming_Builder Front1FaceIns (Front1);
  Front1FaceIns.Generated (Front1Face); 

  TopoDS_Face Back1Face = MKBOX1.BackFace ();
  TNaming_Builder Back1FaceIns (Back1); 
  Back1FaceIns.Generated (Back1Face); 

  // =====================================
  // 2.Box2, TNaming_Evolution == PRIMITIVE
  // =====================================
  BRepPrimAPI_MakeBox MKBOX2( 150, 150, 150); // creating Box2

 //Load the faces of the box2 in DF
  TDF_Label Box2Label = aLabel.FindChild(Box2POS);
  TDF_Label Top2      = Box2Label.FindChild(1);
  TDF_Label Bottom2   = Box2Label.FindChild(2);
  TDF_Label Right2    = Box2Label.FindChild(3);
  TDF_Label Left2     = Box2Label.FindChild(4);
  TDF_Label Front2    = Box2Label.FindChild(5);
  TDF_Label Back2     = Box2Label.FindChild(6);

  TNaming_Builder Box2Ins (Box2Label);
  Box2Ins.Generated (MKBOX2.Shape());
  
  TNaming_Builder Top2FaceIns (Top2);
  TopoDS_Face Top2Face = MKBOX2.TopFace ();
  Top2FaceIns.Generated (Top2Face);  

  TopoDS_Face Bottom2Face = MKBOX2.BottomFace ();
  TNaming_Builder Bottom2FaceIns (Bottom2); 
  Bottom2FaceIns.Generated (Bottom2Face);
 
  TopoDS_Face Right2Face = MKBOX2.RightFace ();
  TNaming_Builder Right2FaceIns (Right2); 
  Right2FaceIns.Generated (Right2Face); 

  TopoDS_Face Left2Face = MKBOX2.LeftFace ();
  TNaming_Builder Left2FaceIns (Left2); 
  Left2FaceIns.Generated (Left2Face); 

  TopoDS_Face Front2Face = MKBOX2.FrontFace ();
  TNaming_Builder Front2FaceIns (Front2);
  Front2FaceIns.Generated (Front2Face); 

  TopoDS_Face Back2Face = MKBOX2.BackFace ();
  TNaming_Builder Back2FaceIns (Back2); 
  Back2FaceIns.Generated (Back2Face); 

  // ====================================
  // 3.Applying a transformation to Box2
  // ====================================
  gp_Vec vec1(gp_Pnt(0.,0.,0.),gp_Pnt(50.,50.,20.));
  gp_Trsf TRSF;
  TRSF.SetTranslation(vec1);
  TopLoc_Location loc(TRSF);
  TDF_LabelMap scope;
  TDF_ChildIterator itchild;
  for (itchild.Initialize(Box2Label,Standard_True); itchild.More();itchild.Next()) {
    if (itchild.Value().IsAttribute(TNaming_NamedShape::GetID())) scope.Add(itchild.Value());
  }
  if (Box2Label.IsAttribute(TNaming_NamedShape::GetID())) scope.Add(Box2Label);
  TDF_MapIteratorOfLabelMap it(scope);
  for (;it.More();it.Next()) 
    TNaming::Displace(it.Key(), loc, Standard_True);//with oldshapes


  //============================================================================
  // 4.Push a selected edges of top face of Box1 in DF,
  // create Fillet (using selected edges) and push result as modification of Box1
  //=============================================================================
  Handle(TNaming_NamedShape) B1NS;
  Box1Label.FindAttribute(TNaming_NamedShape::GetID(), B1NS);
  const TopoDS_Shape& box1  = TNaming_Tool::GetShape(B1NS);
  Handle(TNaming_NamedShape) Top1NS;
  Top1.FindAttribute(TNaming_NamedShape::GetID(), Top1NS);
  const TopoDS_Shape& top1face  = TNaming_Tool::GetShape(Top1NS);

  BRepFilletAPI_MakeFillet MKFILLET(box1);// fillet's algo
  TDF_Label SelectedEdgesLabel = aLabel.FindChild(SelectedEdgesPOS); //Label for selected edges
  TopExp_Explorer exp(top1face, TopAbs_EDGE);
  Standard_Integer i=1;
  for(;exp.More();exp.Next(),i++) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    const TDF_Label& SelEdge  = SelectedEdgesLabel.FindChild(i);
    // Creating TNaming_Selector on label
    TNaming_Selector Selector(SelEdge);

    // Inserting shape into data framework, we need the context to find neighbourhood of shape
    // For example the context for a lateral face of cone is cone itself
    // If a shape is standalone the context will be the shape itself

    // Selector.Select(Shape, Context);
    // TNaming_Evolution == SELECTED
    Selector.Select(E, box1);
    // Recover selected edge from DF, only for example
    const TopoDS_Edge& FE  = TopoDS::Edge(Selector.NamedShape()->Get());
    MKFILLET.Add(5., FE);
  }

  MKFILLET.Build();
  if(!MKFILLET.IsDone()) return; //Algorithm failed

  // ...put fillet in the DataFramework as modification of Box1
  TDF_Label FilletLabel       = aLabel.FindChild(FilletPOS);
  TDF_Label DeletedFaces      = FilletLabel.FindChild(i++);
  TDF_Label ModifiedFaces     = FilletLabel.FindChild(i++);
  TDF_Label FacesFromEdges    = FilletLabel.FindChild(i++);
  TDF_Label FacesFromVertices = FilletLabel.FindChild(i);

  // TNaming_Evolution == MODIFY
  TNaming_Builder bFillet(FilletLabel);
  bFillet.Modify(box1, MKFILLET.Shape());

 //New faces generated from edges
  TopTools_MapOfShape View;
  TNaming_Builder FaceFromEdgeBuilder(FacesFromEdges);  
  TopExp_Explorer ShapeExplorer (box1, TopAbs_EDGE);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MKFILLET.Generated (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      // TNaming_Evolution == GENERATED
      if (!Root.IsSame (newShape)) FaceFromEdgeBuilder.Generated (Root,newShape );
    }
  }

  //Faces of the initial shape modified by MKFILLET
  View.Clear();
  TNaming_Builder ModFacesBuilder(ModifiedFaces);
  ShapeExplorer.Init(box1,TopAbs_FACE);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MKFILLET.Modified (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      // TNaming_Evolution == MODIFY
      if (!Root.IsSame (newShape)) ModFacesBuilder.Modify (Root,newShape );
    }
  }

  //Deleted faces of the initial shape
  View.Clear();
  TNaming_Builder DelFacesBuilder(DeletedFaces);
  ShapeExplorer.Init(box1, TopAbs_FACE);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    // TNaming_Evolution == DELETE
    if (MKFILLET.IsDeleted (Root)) DelFacesBuilder.Delete (Root);
  }

 //New faces generated from vertices
  View.Clear();
  TNaming_Builder FaceFromVertexBuilder(FacesFromVertices);
  ShapeExplorer.Init(box1, TopAbs_VERTEX);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MKFILLET.Generated (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      // TNaming_Evolution == GENERATED
      if (!Root.IsSame (newShape)) FaceFromVertexBuilder.Generated (Root,newShape );
    }
  }
  // =====================================================================
  // 5.Create a Cut (Box1, Box2) as modification of Box1 and push it in DF
  // Boolean operation - CUT Object=Box1, Tool=Box2
  // =====================================================================

  TDF_Label CutLabel = aLabel.FindChild(CutPOS);

  // recover Object
  Handle(TNaming_NamedShape) ObjectNS;
  FilletLabel.FindAttribute(TNaming_NamedShape::GetID(), ObjectNS);
  TopoDS_Shape OBJECT = ObjectNS->Get();

  // Select Tool
  TDF_Label ToolLabel = CutLabel.FindChild(1);
  TNaming_Selector ToolSelector(ToolLabel);
  Handle(TNaming_NamedShape) ToolNS;
  Box2Label.FindAttribute(TNaming_NamedShape::GetID(), ToolNS);
  const TopoDS_Shape& Tool = ToolNS->Get();
  //TNaming_Evolution == SELECTED
  ToolSelector.Select(Tool, Tool);
  const TopoDS_Shape& TOOL = ToolSelector.NamedShape()->Get();

  BRepAlgo_Cut mkCUT (OBJECT, TOOL);

  if (!mkCUT.IsDone()) {
    cout << "CUT: Algorithm failed" << endl;
    return; 
  } else 
    {
      TopTools_ListOfShape Larg;
      Larg.Append(OBJECT);
      Larg.Append(TOOL);

      if (!BRepAlgo::IsValid(Larg, mkCUT.Shape(), Standard_True, Standard_False)) {

	cout << "CUT: Result is not valid" << endl;
	return;
      } else 
	{
	 // push CUT results in DF as modification of Box1
	  TDF_Label Modified      = CutLabel.FindChild(2);
	  TDF_Label Deleted       = CutLabel.FindChild(3);
	  TDF_Label Intersections = CutLabel.FindChild(4);
	  TDF_Label NewFaces      = CutLabel.FindChild(5);

	  TopoDS_Shape newS1 = mkCUT.Shape();
	  const TopoDS_Shape& ObjSh = mkCUT.Shape1();

	  //push in the DF result of CUT
	  TNaming_Builder CutBuilder (CutLabel);
	  // TNaming_Evolution == MODIFY
	  CutBuilder.Modify (ObjSh, newS1);

	  //push in the DF modified faces
	  View.Clear();
	  TNaming_Builder ModBuilder(Modified);
	  ShapeExplorer.Init(ObjSh, TopAbs_FACE);
	  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
	    const TopoDS_Shape& Root = ShapeExplorer.Current ();
	    if (!View.Add(Root)) continue;
	    const TopTools_ListOfShape& Shapes = mkCUT.Modified (Root);
	    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
	    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
	      const TopoDS_Shape& newShape = ShapesIterator.Value ();
	      // TNaming_Evolution == MODIFY
	      if (!Root.IsSame (newShape)) ModBuilder.Modify (Root,newShape );
	    }
	  }

	  //push in the DF deleted faces
	  View.Clear();
	  TNaming_Builder DelBuilder(Deleted);
	  ShapeExplorer.Init (ObjSh,TopAbs_FACE);
	  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
	    const TopoDS_Shape& Root = ShapeExplorer.Current ();
	    if (!View.Add(Root)) continue;
	    // TNaming_Evolution == DELETE
	    if (mkCUT.IsDeleted (Root)) DelBuilder.Delete (Root);
	  }

	  // push in the DF section edges
	  TNaming_Builder IntersBuilder(Intersections);
	  Handle(TopOpeBRepBuild_HBuilder) build = mkCUT.Builder();  
	  TopTools_ListIteratorOfListOfShape its = build->Section();
	  for (; its.More(); its.Next()) {
	    // TNaming_Evolution == SELECTED
	    IntersBuilder.Select(its.Value(),its.Value());
	  }

	  // push in the DF new faces added to the object:
	  const TopoDS_Shape& ToolSh = mkCUT.Shape2();
	  TNaming_Builder newBuilder (NewFaces);
	  ShapeExplorer.Init(ToolSh, TopAbs_FACE);
	  for (; ShapeExplorer.More(); ShapeExplorer.Next()) {
	    const TopoDS_Shape& F = ShapeExplorer.Current();
	    const TopTools_ListOfShape& modified = mkCUT.Modified(F);
	    if (!modified.IsEmpty()) {
	      TopTools_ListIteratorOfListOfShape itr(modified);
	      for (; itr.More (); itr.Next ()) {
		const TopoDS_Shape& newShape = itr.Value();
		Handle(TNaming_NamedShape) NS = TNaming_Tool::NamedShape(newShape, NewFaces);
		if (NS.IsNull() || NS->Evolution() != TNaming_MODIFY) {
		  // TNaming_Evolution == GENERATED
		  newBuilder.Generated(F, newShape); 	
		}
	      }
	    }
	  }
	} 
    }
  // end of CUT

  // =================================================
  // 6.Recover result from DF
  // get final result - Box1 shape after CUT operation
  // =================================================
  Handle(TNaming_NamedShape) ResultNS;
  CutLabel.FindAttribute(TNaming_NamedShape::GetID(), ResultNS);
  const TopoDS_Shape& Result_1 = ResultNS->Get(); // here is result of cut operation
  ResultNS.Nullify();
  Box1Label.FindAttribute(TNaming_NamedShape::GetID(), ResultNS);
  const TopoDS_Shape& Result_2 = TNaming_Tool::CurrentShape(ResultNS);//here is also result of cut operation

  //
  //Result_1 and Result_2 are the same shapes
  //=========================================
}
#endif
