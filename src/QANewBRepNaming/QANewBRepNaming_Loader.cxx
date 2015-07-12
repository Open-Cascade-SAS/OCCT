// Created on: 1999-10-25
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999 Matra Datavision
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


#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : LoadGeneratedShapes
//purpose  :   Load in   the naming data-structure   the  shape
//           generated  from FACE,  EDGE, VERTEX,..., after the
//           MakeShape   operation.  <ShapeIn>  is  the initial
//           shape.   <GeneratedFrom>   defines  the   kind  of
//           shape generation    to    record  in   the  naming
//           data-structure. The <builder> is used to store the
//           set of evolutions in the data-framework of TDF.
//=======================================================================
void QANewBRepNaming_Loader::LoadGeneratedShapes (BRepBuilderAPI_MakeShape& MS, 
					     const TopoDS_Shape&     ShapeIn,
					     const TopAbs_ShapeEnum  KindOfShape,
					     TNaming_Builder&        Builder)
{
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Generated (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      if (!Root.IsSame (newShape)) Builder.Generated (Root,newShape );
    }
  }
}



//=======================================================================
//function : LoadAndOrientGeneratedShapes
//purpose  : The same as LoadGeneratedShapes plus performs orientation of
//           loaded shapes according orientation of SubShapes
//=======================================================================

void QANewBRepNaming_Loader::LoadAndOrientGeneratedShapes (BRepBuilderAPI_MakeShape&     MS,
						      const TopoDS_Shape&           ShapeIn,
						      const TopAbs_ShapeEnum        KindOfShape,
						      TNaming_Builder&              Builder,
						      const TopTools_DataMapOfShapeShape& SubShapes)
{
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Generated (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      TopoDS_Shape newShape = ShapesIterator.Value ();
      if (SubShapes.IsBound(newShape)) {
	newShape.Orientation((SubShapes(newShape)).Orientation());
      }
      if (!Root.IsSame (newShape)) Builder.Generated (Root,newShape );
    }
  }
}

//=======================================================================
//function : LoadModifiedShapes
//purpose  :   Load in  the naming data-structure     the shape
//           modified from FACE, EDGE, VERTEX,...,    after the
//           MakeShape operation.      <ShapeIn> is the initial
//           shape. <ModifiedFrom> defines the kind of    shape
//            modification    to      record  in   the   naming
//           data-structure. The <builder> is used to store the
//           set of evolutions in the data-framework of TDF.
//=======================================================================

void QANewBRepNaming_Loader::LoadModifiedShapes (BRepBuilderAPI_MakeShape&    MS,
					    const TopoDS_Shape&     ShapeIn,
					    const TopAbs_ShapeEnum  KindOfShape,
					    TNaming_Builder&        Builder,
					    const Standard_Boolean  theBool)
{ 
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  TopTools_ListOfShape Shapes;
  BRepAlgoAPI_BooleanOperation* pMS = NULL;
  if (theBool) 
    pMS = (reinterpret_cast<BRepAlgoAPI_BooleanOperation *>(&MS));
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
//    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    if (theBool) 
      Shapes = pMS->Modified (Root);
    else
      Shapes = MS.Modified (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      if (!Root.IsSame (newShape)) {
	Builder.Modify (Root,newShape );
      }
    }
  }
}


//=======================================================================
//function : LoadAndOrientModifiedShapes
//purpose  : The same as LoadModifiedShapes plus performs orientation of
//           loaded shapes according orientation of SubShapes
//=======================================================================

void QANewBRepNaming_Loader::LoadAndOrientModifiedShapes (BRepBuilderAPI_MakeShape&     MS,
						     const TopoDS_Shape&           ShapeIn,
						     const TopAbs_ShapeEnum        KindOfShape,
						     TNaming_Builder&              Builder,
						     const TopTools_DataMapOfShapeShape& SubShapes)
{
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified(Root); 
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      TopoDS_Shape newShape = ShapesIterator.Value ();
      if (SubShapes.IsBound(newShape)) {
	newShape.Orientation((SubShapes(newShape)).Orientation());
      }
      if (!Root.IsSame (newShape)) Builder.Modify (Root,newShape );
    }
  }
}


//=======================================================================
//function : LoadDeletedShapes
//purpose  : Load in the naming data-structure the shape
//             deleted after the MakeShape operation.
//           <ShapeIn> is the initial shape.
//           <KindOfDeletedShape> defines the kind of
//             deletion to record in the naming data-structure.
//           The <builder> is used to store the set of evolutions
//             in the data-framework of TDF.
//=======================================================================

void QANewBRepNaming_Loader::LoadDeletedShapes (BRepBuilderAPI_MakeShape& MS,
					   const TopoDS_Shape&     ShapeIn,
					   const TopAbs_ShapeEnum  KindOfShape,
					   TNaming_Builder&        Builder)
{  
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    if (MS.IsDeleted (Root)) Builder.Delete (Root);
  }
}


//=======================================================================
//function : ModifyPart
//purpose  : Internal Tool
//=======================================================================

void QANewBRepNaming_Loader::ModifyPart (const TopoDS_Shape& PartShape,
				    const TopoDS_Shape& Primitive,
				    const TDF_Label&    Label) 
{

  TNaming_Builder Builder (Label);

  TopLoc_Location PartLocation = PartShape.Location ();
  if (!PartLocation.IsIdentity ()) {
    TopLoc_Location Identity;
    Builder.Modify (PartShape.Located(Identity), Primitive);
    TNaming::Displace (Label, PartLocation);
  }
  else Builder.Modify (PartShape, Primitive);
}

//=======================================================================
//function : HasDangleShapes
//purpose  : 
//=======================================================================

Standard_Boolean QANewBRepNaming_Loader::HasDangleShapes(const TopoDS_Shape& ShapeIn) {
  if (ShapeIn.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(ShapeIn);
    for (; itr.More(); itr.Next()) 
      if (itr.Value().ShapeType() != TopAbs_SOLID) return Standard_True;
    return Standard_False;
  } else if (ShapeIn.ShapeType() == TopAbs_COMPSOLID || 
	     ShapeIn.ShapeType() == TopAbs_SOLID) {
    return Standard_False;
  } else if (ShapeIn.ShapeType() == TopAbs_SHELL ||
	     ShapeIn.ShapeType() == TopAbs_FACE ||
	     ShapeIn.ShapeType() == TopAbs_WIRE ||
	     ShapeIn.ShapeType() == TopAbs_EDGE ||
	     ShapeIn.ShapeType() == TopAbs_VERTEX)
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes Generator - Dangle.
//=======================================================================

Standard_Boolean QANewBRepNaming_Loader::GetDangleShapes(const TopoDS_Shape& ShapeIn,
						    const TopAbs_ShapeEnum GeneratedFrom,
						    TopTools_DataMapOfShapeShape& Dangles) 
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Bind(ancestors.First(), mayBeDangle);
  }
  return Dangles.Extent();
}

//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes.
//=======================================================================
Standard_Boolean QANewBRepNaming_Loader::GetDangleShapes(const TopoDS_Shape& ShapeIn,
                                                    const TopAbs_ShapeEnum GeneratedFrom,
                                                    TopTools_MapOfShape& Dangles)
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Add(mayBeDangle);
  }
  return Dangles.Extent();
}

//=======================================================================
//function : LoadGeneratedDangleShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_Loader::LoadGeneratedDangleShapes(const TopoDS_Shape&          ShapeIn,
						  const TopAbs_ShapeEnum       GeneratedFrom,
						  TNaming_Builder&             Builder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!QANewBRepNaming_Loader::GetDangleShapes(ShapeIn, GeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next()) Builder.Generated(itr.Key(), itr.Value());
}

//=======================================================================
//function : LoadGeneratedDangleShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_Loader::LoadGeneratedDangleShapes(const TopoDS_Shape&          ShapeIn,
						  const TopAbs_ShapeEnum       GeneratedFrom,
						  const TopTools_MapOfShape&   OnlyThese,
						  TNaming_Builder&             Builder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!QANewBRepNaming_Loader::GetDangleShapes(ShapeIn, GeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next()) {
    if (!OnlyThese.Contains(itr.Value())) continue;
    Builder.Generated(itr.Key(), itr.Value());
  }
}

//=======================================================================
//function : LoadModifiedDangleShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_Loader::LoadModifiedDangleShapes (BRepBuilderAPI_MakeShape& MS,
						  const TopoDS_Shape&       ShapeIn,
						  const TopAbs_ShapeEnum    KindOfShape,
						  TNaming_Builder&          Builder)
{ 
  TopTools_MapOfShape OnlyThese;
  TopAbs_ShapeEnum neighbour = TopAbs_EDGE;
  if (KindOfShape == TopAbs_EDGE) neighbour = TopAbs_FACE;
  if (!QANewBRepNaming_Loader::GetDangleShapes(ShapeIn, neighbour, OnlyThese)) return;

  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root) || !OnlyThese.Contains(Root)) continue;
    const TopTools_ListOfShape& Shapes = MS.Modified (Root);
    TopTools_ListIteratorOfListOfShape ShapesIterator (Shapes);
    for (;ShapesIterator.More (); ShapesIterator.Next ()) {
      const TopoDS_Shape& newShape = ShapesIterator.Value ();
      if (!Root.IsSame (newShape)) {
	Builder.Modify (Root,newShape );
      }
    }
  }
}

//=======================================================================
//function : IsDangle
//purpose  : Don't use this method inside an iteration process!
//=======================================================================
Standard_Boolean QANewBRepNaming_Loader::IsDangle (const TopoDS_Shape& theDangle, 
					      const TopoDS_Shape& theShape) {
  TopTools_MapOfShape dangles;
  TopAbs_ShapeEnum neighbour = TopAbs_EDGE;
  if (theDangle.ShapeType() == TopAbs_EDGE) neighbour = TopAbs_FACE;
  if (!QANewBRepNaming_Loader::GetDangleShapes(theShape, neighbour, dangles)) return Standard_False;
  return dangles.Contains(theDangle);
}  

//=======================================================================
//function : LoadDeletedDangleShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Loader::LoadDeletedDangleShapes (BRepBuilderAPI_MakeShape& MS,
						 const TopoDS_Shape&       ShapeIn,
						 const TopAbs_ShapeEnum    KindOfShape,
						 TNaming_Builder&          Builder)
{ 
  if (KindOfShape != TopAbs_EDGE && KindOfShape != TopAbs_VERTEX) return; // not implemented ...
  TopTools_MapOfShape View;
  TopExp_Explorer ShapeExplorer (ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& Root = ShapeExplorer.Current ();
    if (!View.Add(Root)) continue;
    if (!QANewBRepNaming_Loader::IsDangle(Root, ShapeIn)) continue;
    if (MS.IsDeleted (Root)) Builder.Delete (Root);
  }
}  

//=======================================================================
//function : LoadDangleShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Loader::LoadDangleShapes(const TopoDS_Shape& theShape,const TDF_Label& theLabelGenerator) {
  QANewBRepNaming_Loader::LoadDangleShapes(theShape, TopoDS_Shape(), theLabelGenerator);
}

//=======================================================================
//function : LoadDangleShapes
//purpose  : 
//=======================================================================
void QANewBRepNaming_Loader::LoadDangleShapes(const TopoDS_Shape& theShape,
					 const TopoDS_Shape& theIgnoredShape,
					 const TDF_Label& theLabelGenerator)
{
  TopTools_MapOfShape dangles, ignored;
  TopAbs_ShapeEnum GeneratedFrom = TopAbs_EDGE; // theShape.ShapeType() == TopAbs_WIRE or TopAbs_EDGE
  if (theShape.ShapeType() == TopAbs_SHELL || theShape.ShapeType() == TopAbs_FACE)
    GeneratedFrom = TopAbs_FACE;
  if (!QANewBRepNaming_Loader::GetDangleShapes(theShape, GeneratedFrom, dangles)) return;
  if (!theIgnoredShape.IsNull()) {
    TopoDS_Iterator itrI(theIgnoredShape);
    for (; itrI.More(); itrI.Next()) {
      TopoDS_Shape ignoredShape = itrI.Value();
      ignored.Add(ignoredShape);
    }
  }
  TopTools_MapIteratorOfMapOfShape itr (dangles);
  for (; itr.More(); itr.Next()) {
    const TopoDS_Shape& aDangle = itr.Key();
    if (ignored.Contains(aDangle)) continue;
    TNaming_Builder aBuilder(theLabelGenerator.NewChild());
#ifdef OCCT_DEBUG
    TDataStd_Name::Set(aBuilder.NamedShape()->Label(), "NewShapes");
#endif
    aBuilder.Generated(aDangle);
  }
}
