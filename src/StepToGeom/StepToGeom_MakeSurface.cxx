// Created on: 1993-07-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//:n7 abv 15.02.99: S4132: adding translation of surface_replica
//:p0 abv 19.02.99: management of 'done' flag improved
//:s5 abv 22.04.99  Adding debug printouts in catch {} blocks
//sln 03.10.01. BUC61003. creation of  offset surface is corrected

#include <StepToGeom_MakeSurface.ixx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StepGeom_Surface.hxx>
#include <StepToGeom_MakeSurface.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepToGeom_MakeBoundedSurface.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepToGeom_MakeElementarySurface.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <StepToGeom_MakeSweptSurface.hxx>

//:d4
#include <StepGeom_OffsetSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <StepGeom_SurfaceReplica.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepToGeom_MakeTransformation3d.hxx>
#include <UnitsMethods.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>  
#include <gp_Trsf.hxx>
#include <Precision.hxx>

//=============================================================================
// Creation d' une Surface de Geom a partir d' une Surface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSurface::Convert (const Handle(StepGeom_Surface)& SS, Handle(Geom_Surface)& CS)
{
   // sln 01.10.2001 BUC61003. If entry shell is NULL do nothing
  if(SS.IsNull()) {
//#ifdef DEB
//      cout<<"Warning: StepToGeom_MakeSurface: Null Surface:"; 
//#endif
    return Standard_False;
  }

  try {
    OCC_CATCH_SIGNALS
    if (SS->IsKind(STANDARD_TYPE(StepGeom_BoundedSurface))) {
      const Handle(StepGeom_BoundedSurface) S1 = Handle(StepGeom_BoundedSurface)::DownCast(SS);
      return StepToGeom_MakeBoundedSurface::Convert(S1,*((Handle(Geom_BoundedSurface)*)&CS));
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_ElementarySurface))) {
      const Handle(StepGeom_ElementarySurface) S1 = Handle(StepGeom_ElementarySurface)::DownCast(SS);
      return StepToGeom_MakeElementarySurface::Convert(S1,*((Handle(Geom_ElementarySurface)*)&CS));
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_SweptSurface))) {
      const Handle(StepGeom_SweptSurface) S1 = Handle(StepGeom_SweptSurface)::DownCast(SS);
      return StepToGeom_MakeSweptSurface::Convert(S1,*((Handle(Geom_SweptSurface)*)&CS));
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_OffsetSurface))) { //:d4 abv 12 Mar 98
      const Handle(StepGeom_OffsetSurface) OS = Handle(StepGeom_OffsetSurface)::DownCast(SS);
      Handle(Geom_Surface) aBasisSurface;
      if (StepToGeom_MakeSurface::Convert(OS->BasisSurface(),aBasisSurface))
      {
        // sln 03.10.01. BUC61003. creation of  offset surface is corrected
        const Standard_Real anOffset = OS->Distance() * UnitsMethods::LengthFactor();
        if (aBasisSurface->Continuity() == GeomAbs_C0)
        {
          const BRepBuilderAPI_MakeFace aBFace(aBasisSurface, Precision::Confusion());
          if (aBFace.IsDone())
          {
            const TopoDS_Shape aResult = ShapeAlgo::AlgoContainer()->C0ShapeToC1Shape(aBFace.Face(), Abs(anOffset));
            if (aResult.ShapeType() == TopAbs_FACE) 
            {
              aBasisSurface = BRep_Tool::Surface(TopoDS::Face(aResult));
            }
          }
        }
        if(aBasisSurface->Continuity() != GeomAbs_C0)
        {
          CS = new Geom_OffsetSurface ( aBasisSurface, anOffset );
          return Standard_True;
        }
      }
    }
    else if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceReplica))) { //:n7 abv 16 Feb 99
      const Handle(StepGeom_SurfaceReplica) SR = Handle(StepGeom_SurfaceReplica)::DownCast(SS);
      const Handle(StepGeom_Surface) PS = SR->ParentSurface();
      const Handle(StepGeom_CartesianTransformationOperator3d) T = 
        Handle(StepGeom_CartesianTransformationOperator3d)::DownCast(SR->Transformation());
      // protect against cyclic references and wrong type of cartop
      if ( !T.IsNull() && PS != SS ) {
        Handle(Geom_Surface) S1;
        if (StepToGeom_MakeSurface::Convert(PS,S1))
        {
          gp_Trsf T1;
          if (StepToGeom_MakeTransformation3d::Convert(T,T1))
          {
            S1->Transform ( T1 );
            CS = S1;
            return Standard_True;
		  }
        }
      }
    }
  }
  catch(Standard_Failure) {
//   ShapeTool_DB ?
#ifdef DEB //:s5
    cout<<"Warning: StepToGeom_MakeSurface: Exception:"; 
    Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
  }
  return Standard_False;
}	 
