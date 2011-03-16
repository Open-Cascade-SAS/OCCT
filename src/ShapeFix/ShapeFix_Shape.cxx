// File:	ShapeFix_Shape.cxx
// Created:	Wed Aug 09 12:27:47 1999
// Author:	Galina Kulikova
//		<gka@nnov.matra-dtv.fr>

#include <ShapeFix_Shape.ixx>

#include <Precision.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

#include <ShapeFix.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Solid.hxx>

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

Standard_Boolean ShapeFix_Shape::Perform() 
{
  Standard_Integer savFixSmallAreaWireMode = 0;

  Handle(ShapeFix_Face) fft = Handle(ShapeFix_Face)::DownCast ( FixFaceTool() );
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
  switch ( st ) {
  case TopAbs_COMPOUND:  
  case TopAbs_COMPSOLID: {
    TopoDS_Shape shape = myShape;
    Standard_Boolean savFixSameParameterMode = myFixSameParameterMode;
    myFixSameParameterMode = Standard_False;
    for( TopoDS_Iterator iter(S); iter.More(); iter.Next()) {
      myShape = iter.Value();
      if ( Perform() ) status = Standard_True;
    }
    myFixSameParameterMode = savFixSameParameterMode;
    myShape = shape;
//    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE5 );
    break;
  }
  case TopAbs_SOLID: {  
    if ( ! NeedFix ( myFixSolidMode ) ) break;
    myFixSolid->Init(TopoDS::Solid(S)); 
    myFixSolid->SetContext(Context());
    if(myFixSolid->Perform()) {
//      Context()->Replace(S,myFixSolid->Solid());
      status = Standard_True;
    }
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
    break;
  }
  case TopAbs_SHELL:  {
    if ( ! NeedFix ( myFixShellMode ) ) break;
    Handle(ShapeFix_Shell) sfsh = FixShellTool(); 
    sfsh->Init(TopoDS::Shell(S)); 
    sfsh->SetContext(Context());
    if(sfsh->Perform()) {
//      Context()->Replace(S,sfsh->Shell());
      status = Standard_True;
    }
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4 );
    break;
  }
  case TopAbs_FACE: {
    if ( ! NeedFix ( myFixFaceMode ) ) break;
    Handle(ShapeFix_Face) sff = FixFaceTool();
    Standard_Integer savTopoMode = sff->FixWireTool()->ModifyTopologyMode();
    sff->FixWireTool()->ModifyTopologyMode() = Standard_True;
    sff->Init(TopoDS::Face(S)); 
    sff->SetContext(Context());
    if(sff->Perform()) {
//      Context()->Replace(S,sff->Face());
      status = Standard_True;
    }
    sff->FixWireTool()->ModifyTopologyMode() = savTopoMode;
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    break;
  }
  case TopAbs_WIRE: {
    if ( ! NeedFix ( myFixWireMode ) ) break;
    Handle(ShapeFix_Wire) sfw = FixWireTool();
    Standard_Integer savTopoMode = sfw->ModifyTopologyMode();
    Standard_Integer savClosedMode = sfw->ClosedWireMode();
    sfw->ModifyTopologyMode() = Standard_True;
    if ( ! S.Closed() ) 
      sfw->ClosedWireMode() = Standard_False;
//    sfw->FixEdgeCurvesMode() =0;
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
    if(sfe->FixVertexTolerance(TopoDS::Edge(S)))
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    break;
  }
  case TopAbs_VERTEX:
  case TopAbs_SHAPE :    
  default           : break;
  }
  
  myResult  = Context()->Apply(S);  
  if ( NeedFix ( myFixSameParameterMode ) ) 
    SameParameter (myResult,Standard_False);

  if ( !fft.IsNull() )
    fft->FixSmallAreaWireMode() = savFixSmallAreaWireMode;

  return status;
}  

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void ShapeFix_Shape::SameParameter(const TopoDS_Shape& sh, const Standard_Boolean enforce)
{
  ShapeFix::SameParameter(sh, enforce);
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
