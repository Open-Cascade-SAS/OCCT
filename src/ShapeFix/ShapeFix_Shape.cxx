// Created on: 1999-08-09
// Created by: Galina Kulikova
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


#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Solid.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Shape,ShapeFix_Root)

//=======================================================================
//function : ShapeFix_Shape
//purpose  : 
//=======================================================================
ShapeFix_Shape::ShapeFix_Shape()
{  
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixSolidMode = -1;
  myFixShellMode = -1;
  myFixFaceMode  = -1;
  myFixWireMode  = -1;
  myFixSameParameterMode = -1;
  myFixVertexPositionMode =0;
  myFixVertexTolMode = -1;
  myFixSolid = new ShapeFix_Solid;
}

//=======================================================================
//function : ShapeFix_Shape
//purpose  : 
//=======================================================================

ShapeFix_Shape::ShapeFix_Shape(const TopoDS_Shape& shape)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  myFixSolidMode = -1;
  myFixShellMode = -1;
  myFixFaceMode  = -1;
  myFixWireMode  = -1;
  myFixSameParameterMode = -1;
  myFixSolid = new ShapeFix_Solid;
  myFixVertexPositionMode =0;
  myFixVertexTolMode = -1;
  Init(shape);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_Shape::Init(const TopoDS_Shape& shape) 
{
  myShape = shape;
  if ( Context().IsNull() ) {
    SetContext ( new ShapeBuild_ReShape );
    Context()->ModeConsiderLocation() = Standard_True;
  }
  myResult = myShape;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Shape::Perform(const Handle(Message_ProgressIndicator)& theProgress) 
{
  Standard_Integer savFixSmallAreaWireMode = 0;
  Standard_Integer savFixVertexTolMode =  myFixVertexTolMode;
  Handle(ShapeFix_Face) fft = FixFaceTool();
  if ( !fft.IsNull() ) {
    savFixSmallAreaWireMode = fft->FixSmallAreaWireMode();
    if ( savFixSmallAreaWireMode == -1 &&
         myShape.ShapeType() == TopAbs_FACE ) {
      fft->FixSmallAreaWireMode() = Standard_True;
    }
  }

  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  Standard_Boolean status = Standard_False;
  TopAbs_ShapeEnum st;
  
  //gka fix for sharing assembly
  TopLoc_Location nullLoc,L;
  L = myShape.Location();
  TopoDS_Shape aShapeNullLoc = myShape;
  aShapeNullLoc.Location(nullLoc);
  if(myMapFixingShape.Contains(aShapeNullLoc)) {
    myShape.Location(L);
    myResult = Context()->Apply(myShape);
    status = Standard_True;
    return status;
  }
  else myMapFixingShape.Add(aShapeNullLoc);
  //---------------------------------------
  myShape.Location(L);
  TopoDS_Shape S = Context()->Apply(myShape);
  if ( NeedFix (  myFixVertexPositionMode ) )
    ShapeFix::FixVertexPosition(S,Precision(),Context());

  st = S.ShapeType();

  // Open progress indication scope for the following fix stages:
  // - Fix on Solid or Shell;
  // - Fix same parameterization;
  Message_ProgressSentry aPSentry(theProgress, "Fixing stage", 0, 2, 1);

  switch ( st ) {
  case TopAbs_COMPOUND:  
  case TopAbs_COMPSOLID: {
    TopoDS_Shape shape = myShape;
    Standard_Integer savFixSameParameterMode = myFixSameParameterMode;
    myFixSameParameterMode = Standard_False;
    myFixVertexTolMode = Standard_False;
    Standard_Integer aShapesNb = 0;
    for ( TopoDS_Iterator anIter(S); anIter.More(); anIter.Next() )
      ++aShapesNb;

    // Open progress indication scope for sub-shape fixing
    Message_ProgressSentry aPSentrySubShape(theProgress, "Fixing sub-shape", 0, aShapesNb, 1);
    for ( TopoDS_Iterator anIter(S); anIter.More() && aPSentrySubShape.More(); anIter.Next(), aPSentrySubShape.Next() )
    {
      myShape = anIter.Value();
      if ( Perform(theProgress) )
        status = Standard_True;
    }
    if ( !aPSentrySubShape.More() )
      return Standard_False; // aborted execution

    myFixSameParameterMode = savFixSameParameterMode;
    myFixVertexTolMode = savFixVertexTolMode;
    myShape = shape;
    break;
  }
  case TopAbs_SOLID: {  
    if ( !NeedFix(myFixSolidMode) )
      break;
    myFixSolid->Init(TopoDS::Solid(S)); 
    myFixSolid->SetContext(Context());

    if ( myFixSolid->Perform(theProgress) )
      status = Standard_True;

    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
    break;
  }
  case TopAbs_SHELL:  {
    if ( !NeedFix(myFixShellMode) )
      break;
    Handle(ShapeFix_Shell) sfsh = FixShellTool(); 
    sfsh->Init( TopoDS::Shell(S) ); 
    sfsh->SetContext( Context() );

    if ( sfsh->Perform(theProgress) )
      status = Standard_True;

    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
    break;
  }
  case TopAbs_FACE: {
    if ( ! NeedFix ( myFixFaceMode ) ) break;
    Handle(ShapeFix_Face) sff = FixFaceTool();
    Standard_Boolean savTopoMode = sff->FixWireTool()->ModifyTopologyMode();
    sff->FixWireTool()->ModifyTopologyMode() = Standard_True;
    sff->Init(TopoDS::Face(S)); 
    sff->SetContext(Context());

    if(sff->Perform()) {
      status = Standard_True;
    }
    sff->FixWireTool()->ModifyTopologyMode() = savTopoMode;
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    break;
  }
  case TopAbs_WIRE: {
    if ( ! NeedFix ( myFixWireMode ) ) break;
    Handle(ShapeFix_Wire) sfw = FixWireTool();
    Standard_Boolean savTopoMode = sfw->ModifyTopologyMode();
    Standard_Boolean savClosedMode = sfw->ClosedWireMode();
    sfw->ModifyTopologyMode() = Standard_True;
    if ( ! S.Closed() ) 
      sfw->ClosedWireMode() = Standard_False;
    sfw->SetFace(TopoDS_Face());
    sfw->Load(TopoDS::Wire(S));
    sfw->SetContext(Context());
    if(sfw->Perform()) {
      status = Standard_True;
      Context()->Replace(S,sfw->Wire()); // replace for wire only
    }
    sfw->ModifyTopologyMode() = savTopoMode;
    sfw->ClosedWireMode() = savClosedMode;
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    break;
  }
  case TopAbs_EDGE: {
    Handle(ShapeFix_Edge) sfe = FixEdgeTool();
    sfe->SetContext(Context());
    if(sfe->FixVertexTolerance(TopoDS::Edge(S)))
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    break;
  }
  case TopAbs_VERTEX:
  case TopAbs_SHAPE :    
  default           : break;
  }

  // Switch to the second progress indication scope if it exists
  aPSentry.Next();

  myResult = Context()->Apply(S);

  if ( NeedFix(myFixSameParameterMode) )
  {
    SameParameter(myResult, Standard_False, theProgress);
  }
  if( NeedFix( myFixVertexTolMode))
  {
    Standard_Integer nbF = 0;
    TopExp_Explorer anExpF(myResult, TopAbs_FACE);
    for( ; anExpF.More() && nbF <= 1; anExpF.Next())
      nbF++;
    if( nbF > 1)
    {
      //fix for bug  0025455 
      // for case when vertex belong to the different faces it is necessary to check vertices tolerances
      //after all fixes.
      //This fix it should be performed for example for case when cutting edge was performed.
      Handle(ShapeFix_Edge) sfe = FixEdgeTool();
      for (anExpF.ReInit(); anExpF.More(); anExpF.Next()) 
      {
        TopoDS_Face aF = TopoDS::Face(anExpF.Current());
        TopExp_Explorer anExpE (aF, TopAbs_EDGE);
        for ( ; anExpE.More(); anExpE.Next()) 
          sfe->FixVertexTolerance( TopoDS::Edge (anExpE.Current()), aF);
      }
    }
  }

  myResult = Context()->Apply(myResult);

  if ( !fft.IsNull() )
    fft->FixSmallAreaWireMode() = savFixSmallAreaWireMode;

  return status;
}  

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SameParameter(const TopoDS_Shape& sh,
                                   const Standard_Boolean enforce,
                                   const Handle(Message_ProgressIndicator)& theProgress)
{
  ShapeFix::SameParameter(sh, enforce, 0.0, theProgress);
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeFix_Shape::Shape() const
{
  return myResult;
}

//=======================================================================
//function : SetMsgRegistrator
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SetMsgRegistrator(const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg)
{
  ShapeFix_Root::SetMsgRegistrator ( msgreg );
  myFixSolid->SetMsgRegistrator ( msgreg );
}

//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SetPrecision (const Standard_Real preci) 
{
  ShapeFix_Root::SetPrecision ( preci );
  myFixSolid->SetPrecision ( preci );
}

//=======================================================================
//function : SetMinTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SetMinTolerance (const Standard_Real mintol) 
{
  ShapeFix_Root::SetMinTolerance ( mintol );
  myFixSolid->SetMinTolerance ( mintol );
}

//=======================================================================
//function : SetMaxTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SetMaxTolerance (const Standard_Real maxtol) 
{
  ShapeFix_Root::SetMaxTolerance ( maxtol );
  myFixSolid->SetMaxTolerance ( maxtol );
}
//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_Shape::Status (const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status ); 
}
