// Created on: 1997-11-20
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <BRepAlgo_BooleanOperations.ixx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_Substitution.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Geom_Surface.hxx>
#include <TopLoc_Location.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================
BRepAlgo_BooleanOperations::BRepAlgo_BooleanOperations() :
myApproxNbPntMax (30) ,
myApproxTol3D (1.e-7) ,
myApproxTol2D (1.e-7) ,
myApproxRelativeTol (Standard_True)
{
}

//=======================================================================
//function : Shapes2d
//purpose  : 
//=======================================================================
  void BRepAlgo_BooleanOperations::Shapes2d (const TopoDS_Shape& S1,
					    const TopoDS_Shape& S2) 
{
  // S1 doit etre une face ou un ensemble de faces
  // S2 doit etre une edge.

  if (S2.ShapeType() != TopAbs_EDGE) return;

  BRep_Builder Builder ;
  TopoDS_Wire Wire ;
  Builder.MakeWire (Wire);
  Builder.Add (Wire, S2);
  
  TopExp_Explorer Exp (S1, TopAbs_FACE) ;
  if (!Exp.More()) return ;
  const TopoDS_Face& FirstFace = TopoDS::Face (Exp.Current()) ;

  TopLoc_Location Loc;
  const Handle(Geom_Surface)& Surf = BRep_Tool::Surface (FirstFace, Loc) ; 

  TopoDS_Face Face ;
  Builder.MakeFace (Face, Surf, Loc, BRep_Tool::Tolerance (FirstFace)) ;
  Builder.Add (Face, Wire) ;
  Face.Orientation (FirstFace.Orientation()) ;
  
  myS1 = S1 ;
  myS2 = Face ;

  myDSA.Init() ;
  myDSA.Load (myS1, myS2) ;
  Handle (TopOpeBRepDS_HDataStructure)& HDS = myDSA.ChangeDS() ;
  myDSA.myDSFiller.Insert2d (myS1, myS2, HDS) ;
}

//=======================================================================
//function : Shapes
//purpose  : Defines the arguments.
//=======================================================================
  void BRepAlgo_BooleanOperations::Shapes (const TopoDS_Shape& S1,
					   const TopoDS_Shape& S2)
{
  myS1 = S1;
  myS2 = S2;
  myDSA.Init();
  myDSA.Load(myS1, myS2);
  Handle(TopOpeBRepDS_HDataStructure)& HDS = myDSA.ChangeDS();
  myDSA.myDSFiller.Insert(myS1,myS2,HDS);
  //  const Standard_Boolean CheckShapes = Standard_True;
  //  Standard_Boolean esp = HDS->EdgesSameParameter();
  //  Standard_Boolean localcheck = CheckShapes;
}

//=======================================================================
//function : SetApproxParameters
//purpose  : Sets the parameters for the approximations.
//=======================================================================
  void BRepAlgo_BooleanOperations::SetApproxParameters (const Standard_Integer NbPntMax,
							const Standard_Real Tol3D,
							const Standard_Real Tol2D,
							const Standard_Boolean RelativeTol)
{
  myApproxNbPntMax = NbPntMax ;
  myApproxTol3D = Tol3D ;
  myApproxTol2D = Tol2D ;
  myApproxRelativeTol = RelativeTol ;
}

//=======================================================================
//function : Define
//purpose  : 
//=======================================================================
  void BRepAlgo_BooleanOperations::Define (const TopoDS_Shape& S1,
					   const TopoDS_Shape& S2,
					   Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  ChangeDataStructure() = HDS;
  myS1 = S1;
  myS2 = S2;
}

//=======================================================================
//function : Perform
//purpose  : Performs the global boolean operation.
//=======================================================================
  void BRepAlgo_BooleanOperations::Perform ()
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myDSA.myHB->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();
  GTofBTofBuilder.SetNbPntMax(myApproxNbPntMax);
  GTofBTofBuilder.SetTolerances (myApproxTol3D, myApproxTol2D, myApproxRelativeTol) ;
  Handle(TopOpeBRepBuild_HBuilder)& HB = myDSA.myHB;
  Handle(TopOpeBRepDS_HDataStructure)& HDS = myDSA.ChangeDS();
  HB->Perform(HDS,myS1,myS2);
}

//=======================================================================
//function : Perform
//purpose  : Performs the global boolean operation in regards of the
//           given states.
//=======================================================================
  void BRepAlgo_BooleanOperations::Perform (const TopAbs_State State1,
					   const TopAbs_State State2)
{
  Perform() ;

  myShape.Nullify() ;
  myResult.Nullify() ;
  myMapShape.Clear() ;

  Handle(TopOpeBRepBuild_HBuilder)& HBuilder = ChangeBuilder() ;
  HBuilder->MergeShapes (myS1, State1, myS2, State2) ;

  const TopTools_ListOfShape& ListResults = HBuilder->Merged (myS1, State1) ;
  Standard_Integer NbResults = ListResults.Extent() ;
  if (NbResults > 0) {
    if (NbResults == 1) {
      myShape = ListResults.First() ;
    } else {
      BRep_Builder Builder ;
      Builder.MakeCompound (TopoDS::Compound (myShape)) ;
      TopTools_ListIteratorOfListOfShape Iter ;
      for (Iter.Initialize (ListResults) ; Iter.More() ; Iter.Next())
	Builder.Add (myShape, Iter.Value()) ;
    }
    TopExp_Explorer Explorer ;
    for (Explorer.Init (myShape, TopAbs_FACE) ; Explorer.More() ; Explorer.Next()) {
      myMapShape.Add (Explorer.Current()) ;
    }
    for (Explorer.Init (myShape, TopAbs_EDGE) ; Explorer.More() ; Explorer.Next()) {
      myMapShape.Add (Explorer.Current()) ;
    }
  }
}

//=======================================================================
//function : Common
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::Common() 
{
  Perform (TopAbs_IN, TopAbs_IN) ;
  return myShape ;
}

//=======================================================================
//function : fus
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::Fus() 
{
  Perform (TopAbs_OUT, TopAbs_OUT) ;
  return myShape ;
}

//=======================================================================
//function : cut
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::Cut() 
{
  Perform (TopAbs_OUT, TopAbs_IN) ;
  return myShape ;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::Section() 
{
//  Standard_Boolean bcw = BuilderCanWork();
//  if ( ! bcw || myshapeisnull) return;

  Perform () ;

  myShape.Nullify() ;
  myResult.Nullify() ;
  myMapShape.Clear() ;
  
  Handle(TopOpeBRepBuild_HBuilder)& HBuilder = myDSA.myHB ;

  const TopTools_ListOfShape& ListResults = HBuilder->Section() ;
  Standard_Integer NbResults = ListResults.Extent() ;
  if (NbResults > 0) {
    if (NbResults == 1) {
      myShape = ListResults.First() ;
    } else {
      BRep_Builder Builder ;
      Builder.MakeCompound (TopoDS::Compound (myShape)) ;
      TopTools_ListIteratorOfListOfShape Iter ;
      for (Iter.Initialize (ListResults) ; Iter.More() ; Iter.Next())
	Builder.Add (myShape, Iter.Value()) ;
    }
    TopExp_Explorer Explorer ;
    for (Explorer.Init (myShape, TopAbs_EDGE) ; Explorer.More() ; Explorer.Next()) {
      myMapShape.Add (Explorer.Current()) ;
    }
  }

  return myShape ;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::Shape() 
{
  return myShape;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BRepAlgo_BooleanOperations::ShapeFrom (const TopoDS_Shape& Shape) 
{
  myResult.Nullify() ;

  if (!myShape.IsNull()) {
    
    TopoDS_Shape ShapeToDel ;
    if (Shape.IsSame (myS1)) {
      ShapeToDel = myS2 ;
    } else {
      ShapeToDel = myS1 ;
    }

    BRepTools_Substitution Substitute ;

    TopTools_ListOfShape NullFaces ;
    NullFaces.Clear() ;

    TopExp_Explorer ExpFac ;
    for (ExpFac.Init (ShapeToDel, TopAbs_FACE) ; ExpFac.More() ; ExpFac.Next()) {
      const TopoDS_Face& Face = TopoDS::Face (ExpFac.Current()) ;
      const TopTools_ListOfShape& ListResults = Modified (Face) ;
      if (ListResults.Extent() == 0) {
	if (myMapShape.Contains (Face)) Substitute.Substitute (Face, NullFaces) ;
      } else {
	TopTools_ListIteratorOfListOfShape ItrFace ;
	for (ItrFace.Initialize (ListResults) ; ItrFace.More() ; ItrFace.Next()) {
	  Substitute.Substitute (TopoDS::Face (ItrFace.Value()), NullFaces) ;
	}
      }
    }

    Substitute.Build (myShape) ;
    if (Substitute.IsCopied (myShape)) {
      const TopTools_ListOfShape& ListResults = Substitute.Copy (myShape) ;
      Standard_Integer NbResults = ListResults.Extent() ;
      if (NbResults == 1) {
	myResult = ListResults.First() ;
      } else if (NbResults > 1) {
	BRep_Builder Builder ;
	Builder.MakeCompound (TopoDS::Compound (myResult)) ;
	TopTools_ListIteratorOfListOfShape ItrResult ;
	for (ItrResult.Initialize (ListResults) ; ItrResult.More() ; ItrResult.Next()) {
	  Builder.Add (myResult, ItrResult.Value()) ;
	}
      }
    } else {
      myResult = myShape ;
    }

  }  
  return myResult ;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BRepAlgo_BooleanOperations::Modified (const TopoDS_Shape& Shape) 
{
  return myDSA.Modified(Shape);
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo_BooleanOperations::IsDeleted (const TopoDS_Shape& Shape)
{
  Standard_Boolean Deleted = Standard_True ; 

  Handle(TopOpeBRepBuild_HBuilder)& HBuilder = myDSA.myHB ;

  if (   myMapShape.Contains (Shape)
      || HBuilder->IsMerged (Shape, TopAbs_OUT)
      || HBuilder->IsMerged (Shape, TopAbs_IN)
      || HBuilder->IsMerged (Shape, TopAbs_ON)
      || HBuilder->IsSplit (Shape, TopAbs_OUT)
      || HBuilder->IsSplit (Shape, TopAbs_IN)
      || HBuilder->IsSplit (Shape, TopAbs_ON))
    Deleted = Standard_False ;

  return Deleted ;    
}

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================
  const Handle(TopOpeBRepDS_HDataStructure)& BRepAlgo_BooleanOperations::DataStructure() const
{
  return myDSA.DS();
}

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================
  Handle(TopOpeBRepDS_HDataStructure)& BRepAlgo_BooleanOperations::ChangeDataStructure()
{
  return myDSA.ChangeDS();
}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================
  const Handle(TopOpeBRepBuild_HBuilder)& BRepAlgo_BooleanOperations::Builder() const 
{
  return myDSA.Builder();
}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================
  Handle(TopOpeBRepBuild_HBuilder)& BRepAlgo_BooleanOperations::ChangeBuilder() 
{
  return myDSA.ChangeBuilder();
}

//=======================================================================
//function : DataStructureAccess
//purpose  : returns the member myDSA.
//=======================================================================
  BRepAlgo_DSAccess& BRepAlgo_BooleanOperations::DataStructureAccess()
{
  return myDSA;
}

