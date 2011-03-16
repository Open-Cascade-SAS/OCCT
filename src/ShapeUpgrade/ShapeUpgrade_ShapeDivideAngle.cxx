// File:	ShapeUpgrade_ShapeDivideAngle.cxx
// Created:	Thu May  6 10:38:17 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox.nnov.matra-dtv.fr>


#include <ShapeUpgrade_ShapeDivideAngle.ixx>
#include <ShapeUpgrade_SplitSurfaceAngle.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>

//=======================================================================
//function : ShapeUpgrade_ShapeDivideAngle
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideAngle::ShapeUpgrade_ShapeDivideAngle (const Standard_Real MaxAngle)
{
  InitTool ( MaxAngle );
}

//=======================================================================
//function : ShapeUpgrade_ShapeDivideAngle
//purpose  : 
//=======================================================================

ShapeUpgrade_ShapeDivideAngle::ShapeUpgrade_ShapeDivideAngle(const Standard_Real MaxAngle,
							     const TopoDS_Shape& S):
       ShapeUpgrade_ShapeDivide(S)
{
  InitTool ( MaxAngle );
}

//=======================================================================
//function : InitTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideAngle::InitTool (const Standard_Real MaxAngle)
{
  Handle(ShapeUpgrade_FaceDivide) tool = GetSplitFaceTool();
  tool->SetSplitSurfaceTool ( new ShapeUpgrade_SplitSurfaceAngle (MaxAngle) );
  tool->SetWireDivideTool ( 0 ); // no splitting of wire
  SetSplitFaceTool(tool);
}
     
//=======================================================================
//function : SetMaxAngle
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShapeDivideAngle::SetMaxAngle (const Standard_Real MaxAngle)
{
  InitTool ( MaxAngle );
}
     
//=======================================================================
//function : MaxAngle
//purpose  : 
//=======================================================================

double ShapeUpgrade_ShapeDivideAngle::MaxAngle () const
{
  Handle(ShapeUpgrade_FaceDivide) faceTool = GetSplitFaceTool();
  if ( faceTool.IsNull() ) return 0.;
  Handle(ShapeUpgrade_SplitSurfaceAngle) tool = 
    Handle(ShapeUpgrade_SplitSurfaceAngle)::DownCast ( faceTool );
  return ( tool.IsNull() ? 0. : tool->MaxAngle() );
}
     
