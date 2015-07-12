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
  return BRepTools_ReShape::Apply (shape,until,buildmode);
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
  Replace ( shape, result );

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
