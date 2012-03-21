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

//    abv 28.04.99 S4137: ading method Apply for work on all types of shapes

#include <ShapeBuild_ReShape.ixx>

#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <ShapeExtend.hxx>
#include <ShapeBuild_Edge.hxx>
#include <TopoDS.hxx>

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
      (!aConsLoc &&! newsh.IsSame ( shape )) ) {
  
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
