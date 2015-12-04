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


#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Flash.hxx>
#include <IGESGeom_GeneralModule.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_ToolBoundary.hxx>
#include <IGESGeom_ToolBoundedSurface.hxx>
#include <IGESGeom_ToolBSplineCurve.hxx>
#include <IGESGeom_ToolBSplineSurface.hxx>
#include <IGESGeom_ToolCircularArc.hxx>
#include <IGESGeom_ToolCompositeCurve.hxx>
#include <IGESGeom_ToolConicArc.hxx>
#include <IGESGeom_ToolCopiousData.hxx>
#include <IGESGeom_ToolCurveOnSurface.hxx>
#include <IGESGeom_ToolDirection.hxx>
#include <IGESGeom_ToolFlash.hxx>
#include <IGESGeom_ToolLine.hxx>
#include <IGESGeom_ToolOffsetCurve.hxx>
#include <IGESGeom_ToolOffsetSurface.hxx>
#include <IGESGeom_ToolPlane.hxx>
#include <IGESGeom_ToolPoint.hxx>
#include <IGESGeom_ToolRuledSurface.hxx>
#include <IGESGeom_ToolSplineCurve.hxx>
#include <IGESGeom_ToolSplineSurface.hxx>
#include <IGESGeom_ToolSurfaceOfRevolution.hxx>
#include <IGESGeom_ToolTabulatedCylinder.hxx>
#include <IGESGeom_ToolTransformationMatrix.hxx>
#include <IGESGeom_ToolTrimmedSurface.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_GeneralModule,IGESData_GeneralModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESGeom_GeneralModule::IGESGeom_GeneralModule ()    {  }


    void  IGESGeom_GeneralModule::OwnSharedCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& iter) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineCurve tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundary tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundedSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCircularArc tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCompositeCurve tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolConicArc tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCopiousData tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCurveOnSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolDirection tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolFlash tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolLine tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetCurve tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPlane tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPoint tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolRuledSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineCurve tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSurfaceOfRevolution tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTabulatedCylinder tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTransformationMatrix tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTrimmedSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    default : break;
  }
}


    IGESData_DirChecker  IGESGeom_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBSplineCurve tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBSplineSurface tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBoundary tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolBoundedSurface tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCircularArc tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCompositeCurve tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolConicArc tool;
      return tool.DirChecker(anent);
    }
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCopiousData tool;
      return tool.DirChecker(anent);
    }
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolCurveOnSurface tool;
      return tool.DirChecker(anent);
    }
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolDirection tool;
      return tool.DirChecker(anent);
    }
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolFlash tool;
      return tool.DirChecker(anent);
    }
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolLine tool;
      return tool.DirChecker(anent);
    }
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolOffsetCurve tool;
      return tool.DirChecker(anent);
    }
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolOffsetSurface tool;
      return tool.DirChecker(anent);
    }
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolPlane tool;
      return tool.DirChecker(anent);
    }
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolPoint tool;
      return tool.DirChecker(anent);
    }
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolRuledSurface tool;
      return tool.DirChecker(anent);
    }
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSplineCurve tool;
      return tool.DirChecker(anent);
    }
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSplineSurface tool;
      return tool.DirChecker(anent);
    }
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolSurfaceOfRevolution tool;
      return tool.DirChecker(anent);
    }
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTabulatedCylinder tool;
      return tool.DirChecker(anent);
    }
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTransformationMatrix tool;
      return tool.DirChecker(anent);
    }
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESGeom_ToolTrimmedSurface tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    void  IGESGeom_GeneralModule::OwnCheckCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineCurve tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundary tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundedSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCircularArc tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCompositeCurve tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolConicArc tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCopiousData tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCurveOnSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolDirection tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolFlash tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolLine tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetCurve tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPlane tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPoint tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolRuledSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineCurve tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSurfaceOfRevolution tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTabulatedCylinder tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTransformationMatrix tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTrimmedSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    default : break;
  }
}


    Standard_Boolean  IGESGeom_GeneralModule::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const 
{
  switch (CN) {
    case  1 : ent = new IGESGeom_BSplineCurve;		break;
    case  2 : ent = new IGESGeom_BSplineSurface;	break;
    case  3 : ent = new IGESGeom_Boundary;		break;
    case  4 : ent = new IGESGeom_BoundedSurface;	break;
    case  5 : ent = new IGESGeom_CircularArc;		break;
    case  6 : ent = new IGESGeom_CompositeCurve;	break;
    case  7 : ent = new IGESGeom_ConicArc;		break;
    case  8 : ent = new IGESGeom_CopiousData;		break;
    case  9 : ent = new IGESGeom_CurveOnSurface;	break;
    case 10 : ent = new IGESGeom_Direction;		break;
    case 11 : ent = new IGESGeom_Flash;			break;
    case 12 : ent = new IGESGeom_Line;			break;
    case 13 : ent = new IGESGeom_OffsetCurve;		break;
    case 14 : ent = new IGESGeom_OffsetSurface;		break;
    case 15 : ent = new IGESGeom_Plane; 		break;
    case 16 : ent = new IGESGeom_Point; 		break;
    case 17 : ent = new IGESGeom_RuledSurface;		break;
    case 18 : ent = new IGESGeom_SplineCurve;		break;
    case 19 : ent = new IGESGeom_SplineSurface;		break;
    case 20 : ent = new IGESGeom_SurfaceOfRevolution;	break;
    case 21 : ent = new IGESGeom_TabulatedCylinder;	break;
    case 22 : ent = new IGESGeom_TransformationMatrix;	break;
    case 23 : ent = new IGESGeom_TrimmedSurface;	break;
    default : return Standard_False;    // by default, Failure on Recognize
  }
  return Standard_True;
}


    void  IGESGeom_GeneralModule::OwnCopyCase
  (const Standard_Integer CN,
   const Handle(IGESData_IGESEntity)& entfrom,
   const Handle(IGESData_IGESEntity)& entto,
   Interface_CopyTool& TC) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,enfr,entfrom);
      DeclareAndCast(IGESGeom_BSplineCurve,ento,entto);
      IGESGeom_ToolBSplineCurve tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_BSplineSurface,ento,entto);
      IGESGeom_ToolBSplineSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,enfr,entfrom);
      DeclareAndCast(IGESGeom_Boundary,ento,entto);
      IGESGeom_ToolBoundary tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_BoundedSurface,ento,entto);
      IGESGeom_ToolBoundedSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,enfr,entfrom);
      DeclareAndCast(IGESGeom_CircularArc,ento,entto);
      IGESGeom_ToolCircularArc tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,enfr,entfrom);
      DeclareAndCast(IGESGeom_CompositeCurve,ento,entto);
      IGESGeom_ToolCompositeCurve tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,enfr,entfrom);
      DeclareAndCast(IGESGeom_ConicArc,ento,entto);
      IGESGeom_ToolConicArc tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,enfr,entfrom);
      DeclareAndCast(IGESGeom_CopiousData,ento,entto);
      IGESGeom_ToolCopiousData tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_CurveOnSurface,ento,entto);
      IGESGeom_ToolCurveOnSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,enfr,entfrom);
      DeclareAndCast(IGESGeom_Direction,ento,entto);
      IGESGeom_ToolDirection tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,enfr,entfrom);
      DeclareAndCast(IGESGeom_Flash,ento,entto);
      IGESGeom_ToolFlash tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESGeom_Line,enfr,entfrom);
      DeclareAndCast(IGESGeom_Line,ento,entto);
      IGESGeom_ToolLine tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,enfr,entfrom);
      DeclareAndCast(IGESGeom_OffsetCurve,ento,entto);
      IGESGeom_ToolOffsetCurve tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_OffsetSurface,ento,entto);
      IGESGeom_ToolOffsetSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,enfr,entfrom);
      DeclareAndCast(IGESGeom_Plane,ento,entto);
      IGESGeom_ToolPlane tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESGeom_Point,enfr,entfrom);
      DeclareAndCast(IGESGeom_Point,ento,entto);
      IGESGeom_ToolPoint tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_RuledSurface,ento,entto);
      IGESGeom_ToolRuledSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,enfr,entfrom);
      DeclareAndCast(IGESGeom_SplineCurve,ento,entto);
      IGESGeom_ToolSplineCurve tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_SplineSurface,ento,entto);
      IGESGeom_ToolSplineSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,enfr,entfrom);
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,ento,entto);
      IGESGeom_ToolSurfaceOfRevolution tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,enfr,entfrom);
      DeclareAndCast(IGESGeom_TabulatedCylinder,ento,entto);
      IGESGeom_ToolTabulatedCylinder tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,enfr,entfrom);
      DeclareAndCast(IGESGeom_TransformationMatrix,ento,entto);
      IGESGeom_ToolTransformationMatrix tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,enfr,entfrom);
      DeclareAndCast(IGESGeom_TrimmedSurface,ento,entto);
      IGESGeom_ToolTrimmedSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    default : break;
  }
}


    Standard_Integer  IGESGeom_GeneralModule::CategoryNumber
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& ) const
{
  if (CN == 11) return Interface_Category::Number("Drawing");
  if (CN == 15) {
    DeclareAndCast(IGESGeom_Plane,anent,ent);
    if (anent->HasSymbolAttach()) return Interface_Category::Number("Drawing");
  }
  if (CN == 16) {
    DeclareAndCast(IGESGeom_Point,anent,ent);
    if (anent->HasDisplaySymbol()) return Interface_Category::Number("Drawing");
  }
  if (CN == 22) return Interface_Category::Number("Auxiliary");
  return Interface_Category::Number("Shape");
}
