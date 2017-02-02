// Created on: 1998-05-18
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================
void  ChFi3d_Builder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      , 
				const Handle(ChFiDS_Spine)&         , 
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const TopAbs_Orientation            ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}

//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      , 
				const Handle(ChFiDS_Spine)&         , 
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const TopAbs_Orientation            ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}


//=======================================================================
//function : SimulSurf
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::SimulSurf(Handle(ChFiDS_SurfData)&            ,
				const Handle(ChFiDS_HElSpine)&      ,
				const Handle(ChFiDS_Spine)&         ,
				const Standard_Integer              ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const TopAbs_Orientation            ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(Adaptor3d_TopolTool)&    ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				const Handle(BRepAdaptor_HSurface)& ,
				const Handle(BRepAdaptor_HCurve2d)& ,
				Standard_Boolean&                   ,
				const TopAbs_Orientation            ,
				const Standard_Real                 ,
				const Standard_Real                 ,
				Standard_Real&                      ,
				Standard_Real&                      ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const Standard_Boolean              ,
				const math_Vector&                  )
{
  throw Standard_Failure("SimulSurf Not Implemented");
}



//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      , 
				  const Handle(ChFiDS_Spine)&         , 
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const TopAbs_Orientation            ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");
}

//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      , 
				  const Handle(ChFiDS_Spine)&         , 
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const TopAbs_Orientation            ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");

}





//=======================================================================
//function : PerformSurf
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::PerformSurf(ChFiDS_SequenceOfSurfData&          ,
				  const Handle(ChFiDS_HElSpine)&      ,
				  const Handle(ChFiDS_Spine)&         ,
				  const Standard_Integer              ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const TopAbs_Orientation            ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(Adaptor3d_TopolTool)&    ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  const Handle(BRepAdaptor_HSurface)& ,
				  const Handle(BRepAdaptor_HCurve2d)& ,
				  Standard_Boolean&                   ,
				  const TopAbs_Orientation            ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  const Standard_Real                 ,
				  Standard_Real&                      ,
				  Standard_Real&                      ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const Standard_Boolean              ,
				  const math_Vector&                  )
{
  throw Standard_Failure("PerformSurf Not Implemented");

}
