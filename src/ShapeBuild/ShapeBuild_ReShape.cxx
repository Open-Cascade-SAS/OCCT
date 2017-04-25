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

//    abv 28.04.99 S4137: ading method Apply for work on all types of shapes

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeBuild_ReShape,BRepTools_ReShape)

//=======================================================================
//function : ShapeBuild_ReShape
//purpose  : 
//=======================================================================
ShapeBuild_ReShape::ShapeBuild_ReShape()
{
}

//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeBuild_ReShape::Apply (const TopoDS_Shape& shape,
					const TopAbs_ShapeEnum until,
					const Standard_Integer buildmode) 
{
  if (shape.IsNull()) return shape;
  TopoDS_Shape newsh;
  if (Status (shape,newsh,Standard_False) != 0) return newsh;

  TopAbs_ShapeEnum st = shape.ShapeType();
  if (st == until) return newsh;    // critere d arret

  Standard_Integer modif = 0;
  if (st == TopAbs_COMPOUND || st == TopAbs_COMPSOLID) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound (C);
    for (TopoDS_Iterator it (shape); it.More(); it.Next()) {
      TopoDS_Shape sh = it.Value();
      Standard_Integer stat = Status (sh,newsh,Standard_False);
      if (stat != 0) modif = 1;
      if (stat >= 0) B.Add (C,newsh);
    }
    if (modif == 0) return shape;
    return C;
  }

  if (st == TopAbs_SOLID) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound (C);
    TopoDS_Solid S;
    B.MakeSolid (S);
    for (TopoDS_Iterator it (shape); it.More(); it.Next()) {
      TopoDS_Shape sh = it.Value();
      newsh = Apply (sh,until,buildmode);
      if (newsh.IsNull()) {
	modif = -1;
      } 
      else if (newsh.ShapeType() != TopAbs_SHELL) {
	Standard_Integer nbsub = 0;
	for (TopExp_Explorer exh(newsh,TopAbs_SHELL); exh.More(); exh.Next()) {
	  TopoDS_Shape onesh = exh.Current ();
	  B.Add (S,onesh);
	  nbsub ++;
	}
	if (nbsub == 0) modif = -1;
	B.Add (C,newsh);  // c est tout
      } 
      else {
	if (modif == 0 && !sh.IsEqual(newsh)) modif = 1;
	B.Add (C,newsh);
	B.Add (S,newsh);
      }
    }

    if ( (modif < 0 && buildmode < 2) || (modif == 0 && buildmode < 1) )
      return C;
    else
      return S;
  }

  if (st == TopAbs_SHELL) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound (C);
    TopoDS_Shell S;
    B.MakeShell (S);
    for (TopoDS_Iterator it (shape); it.More(); it.Next()) {
      TopoDS_Shape sh = it.Value();
      newsh = Apply (sh,until,buildmode);
      if (newsh.IsNull()) {
	modif = -1;
      } 
      else if (newsh.ShapeType() != TopAbs_FACE) {
	Standard_Integer nbsub = 0;
	for (TopExp_Explorer exf(newsh,TopAbs_FACE); exf.More(); exf.Next()) {
	  TopoDS_Shape onesh = exf.Current ();
	  B.Add (S,onesh);
	  nbsub ++;
	}
	if (nbsub == 0) modif = -1;
	B.Add (C,newsh);  // c est tout
      } 
      else {
	if (modif == 0 && !sh.IsEqual(newsh)) modif = 1;
	B.Add (C,newsh);
	B.Add (S,newsh);
      }
    }
    if ( (modif < 0 && buildmode < 2) || (modif == 0 && buildmode < 1) )
      return C;
    else
    {
      S.Closed (BRep_Tool::IsClosed (S));
      return S;
    }
  }
  cout<<"BRepTools_ReShape::Apply NOT YET IMPLEMENTED"<<endl;
  return shape;
}

//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeBuild_ReShape::Apply (const TopoDS_Shape& shape,
					const TopAbs_ShapeEnum until) 
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  if ( shape.IsNull() ) return shape;

  // apply direct replacement
  TopoDS_Shape newsh = Value ( shape );
  
  // if shape removed, return NULL
  if ( newsh.IsNull() ) {
    myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    return newsh;
  }
  
  // if shape replaced, apply modifications to the result recursively 
  Standard_Boolean aConsLoc = ModeConsiderLocation();
  if ( (aConsLoc && ! newsh.IsPartner (shape)) || 
      (!aConsLoc &&! newsh.IsSame ( shape )) )
  {
    TopoDS_Shape res = Apply ( newsh, until );
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    return res;
  }

  TopAbs_ShapeEnum st = shape.ShapeType();
  if ( st >= until ) return newsh;    // critere d arret
  if(st == TopAbs_VERTEX || st == TopAbs_SHAPE)
    return shape;
  // define allowed types of components

  BRep_Builder B;
  
  TopoDS_Shape result = shape.EmptyCopied();
  TopAbs_Orientation orient = shape.Orientation(); //JR/Hp: or -> orient
  result.Orientation(TopAbs_FORWARD); // protect against INTERNAL or EXTERNAL shapes
  Standard_Boolean modif = Standard_False;
  Standard_Integer locStatus = myStatus;
  
  // apply recorded modifications to subshapes
  for ( TopoDS_Iterator it(shape,Standard_False); it.More(); it.Next() ) {
    TopoDS_Shape sh = it.Value();
    newsh = Apply ( sh, until );
    if ( newsh != sh ) {
      if ( ShapeExtend::DecodeStatus ( myStatus, ShapeExtend_DONE4 ) )
	locStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
      modif = 1;
    }
    if ( newsh.IsNull() ) {
      locStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
      continue;
    }
    locStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    if ( st == TopAbs_COMPOUND || newsh.ShapeType() == sh.ShapeType()) { //fix for SAMTECH bug OCC322 about abcense internal vertices after sewing.
      B.Add ( result, newsh );
      continue;
    }
    Standard_Integer nitems = 0;
    for ( TopoDS_Iterator subit(newsh); subit.More(); subit.Next(), nitems++ ) {
      TopoDS_Shape subsh = subit.Value();
      if ( subsh.ShapeType() == sh.ShapeType() ) B.Add ( result, subsh );
      else locStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
    }
    if ( ! nitems ) locStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL1 );
  }
  if ( ! modif ) return shape;

  // restore Range on edge broken by EmptyCopied()
  if ( st == TopAbs_EDGE ) {
    ShapeBuild_Edge sbe;
    sbe.CopyRanges ( TopoDS::Edge ( result ), TopoDS::Edge ( shape ));
  }
  else if (st == TopAbs_WIRE || st == TopAbs_SHELL)
    result.Closed (BRep_Tool::IsClosed (result));
  result.Orientation(orient);
  myStatus = locStatus;

  replace(shape, result,
    result.IsNull() ? TReplacementKind_Remove : TReplacementKind_Modify);

  return result;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Integer ShapeBuild_ReShape::Status(const TopoDS_Shape& ashape,
					    TopoDS_Shape& newsh,
					    const Standard_Boolean last) 
{
  return BRepTools_ReShape::Status(ashape,newsh,last);
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeBuild_ReShape::Status (const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status );
}
