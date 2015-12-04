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

#include <BRep_Tool.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <Geom_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Curve.hxx>

//=======================================================================
//function : BRepLib_CheckCurveOnSurface
//purpose  : 
//=======================================================================
BRepLib_CheckCurveOnSurface::BRepLib_CheckCurveOnSurface
                                            ( const TopoDS_Edge& theEdge,
                                              const TopoDS_Face& theFace)
{
  Init(theEdge, theFace);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepLib_CheckCurveOnSurface::Init
  (const TopoDS_Edge& theEdge,
   const TopoDS_Face& theFace)
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
  
  //
  TopLoc_Location aLocE, aLocF, aLocC2D;
  Standard_Real aFirst = 0.0, aLast = 0.0;
  //
  // 3D curve initialization
  const Handle(Geom_Curve)& aC3dTmp = BRep_Tool::Curve(theEdge, aLocE, aFirst, aLast);
  const Handle(Geom_Curve) aC3d(Handle(Geom_Curve)::DownCast(aC3dTmp->Transformed(aLocE.Transformation())));

  // Surface initialization
  const Handle(Geom_Surface)& aSTmp = BRep_Tool::Surface(theFace, aLocF);
  const Handle(Geom_Surface) aS(Handle(Geom_Surface)::DownCast(aSTmp->Transformed(aLocF.Transformation())));
  //
  // 2D curves initialization 
  myPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);

  if(BRep_Tool::IsClosed(theEdge, theFace))
    myPCurve2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(theEdge.Reversed()),
                                          theFace, aFirst, aLast);

  myCOnSurfGeom.Init(aC3d, aS, aFirst, aLast);
}

//=======================================================================
//function : Perform
//purpose  : if isTheMTDisabled == TRUE parallelization is not used
//=======================================================================
void BRepLib_CheckCurveOnSurface::Perform(const Standard_Boolean isTheMTDisabled)
{
  // Compute the max distance
  Compute(myPCurve, isTheMTDisabled);
  if (ErrorStatus())
  {
    return;
  }
  //
  if (!myPCurve2.IsNull())
  {
    // compute max distance for myPCurve2
    // (for the second curve on closed surface)
    Compute(myPCurve2, isTheMTDisabled);
  }
}

//=======================================================================
//function : Compute
//purpose  : if isTheMTDisabled == TRUE parallelization is not used
//=======================================================================
void BRepLib_CheckCurveOnSurface::Compute(const Handle(Geom2d_Curve)& thePCurve,
                                          const Standard_Boolean isTheMTDisabled)
{
  myCOnSurfGeom.Perform(thePCurve, isTheMTDisabled);
}
