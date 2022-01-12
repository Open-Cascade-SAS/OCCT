// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : BRepLib_CheckCurveOnSurface
//purpose  : 
//=======================================================================
BRepLib_CheckCurveOnSurface::BRepLib_CheckCurveOnSurface
                                            ( const TopoDS_Edge& theEdge,
                                              const TopoDS_Face& theFace)
  : myIsParallel(Standard_False)
{
  Init(theEdge, theFace);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Init(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace)
{
  myCOnSurfGeom.Init();

  if (theEdge.IsNull() || theFace.IsNull())
  {
    return;
  }
  //
  if (BRep_Tool::Degenerated(theEdge) ||
      !BRep_Tool::IsGeometric(theEdge))
  {
    return;
  }
  
  // 3D curve initialization
  const Handle(Adaptor3d_Curve) anAdaptor3dCurve = new BRepAdaptor_Curve(theEdge);

  // Surface initialization

  TopLoc_Location aLocation;
  Standard_Real aFirstParam, aLastParam;

  Handle(Geom2d_Curve) aGeom2dCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirstParam, aLastParam);
  Handle(Geom_Surface) aGeomSurface = BRep_Tool::Surface(theFace);

  // 2D curves initialization 
  Handle(Adaptor2d_Curve2d) anAdaptorCurve = 
    new Geom2dAdaptor_Curve(aGeom2dCurve, aFirstParam, aLastParam);
  Handle(GeomAdaptor_Surface) aGeomAdaptorSurface = new GeomAdaptor_Surface(aGeomSurface);

  myAdaptorCurveOnSurface = new Adaptor3d_CurveOnSurface(anAdaptorCurve, aGeomAdaptorSurface);

  if(BRep_Tool::IsClosed(theEdge, theFace))
  {
    Handle(Geom2d_Curve) aGeom2dReversedCurve = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(theEdge.Reversed()), theFace, aFirstParam, aLastParam);
    Handle(Adaptor2d_Curve2d) anAdaptorReversedCurve =
      new Geom2dAdaptor_Curve(aGeom2dReversedCurve, aFirstParam, aLastParam);
     myAdaptorCurveOnSurface2 = new Adaptor3d_CurveOnSurface(anAdaptorReversedCurve, aGeomAdaptorSurface);
  }

  myCOnSurfGeom.Init(anAdaptor3dCurve);
}

//=======================================================================
//function : Perform
//purpose  : if isTheMTDisabled == TRUE parallelization is not used
//=======================================================================
void BRepLib_CheckCurveOnSurface::Perform()
{
  // Compute the max distance
  Compute(myAdaptorCurveOnSurface);
  if (ErrorStatus())
  {
    return;
  }
  //
  if (!myAdaptorCurveOnSurface2.IsNull())
  {
    // compute max distance for myAdaptorCurveOnSurface2
    // (for the second curve on closed surface)
    Compute(myAdaptorCurveOnSurface2);
  }
}

//=======================================================================
//function : Compute
//purpose  : if isTheMTDisabled == TRUE parallelization is not used
//=======================================================================
void BRepLib_CheckCurveOnSurface::Compute(const Handle(Adaptor3d_CurveOnSurface)& theCurveOnSurface)
{
  myCOnSurfGeom.SetParallel(myIsParallel);
  myCOnSurfGeom.Perform(theCurveOnSurface);
}
