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


#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
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
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_ReadWriteModule.hxx>
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
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_ReadWriteModule,IGESData_ReadWriteModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESGeom_ReadWriteModule::IGESGeom_ReadWriteModule ()    {  }


    Standard_Integer  IGESGeom_ReadWriteModule::CaseIGES
  (const Standard_Integer typenum, const Standard_Integer formnum) const 
{
  switch (typenum) {
    case 100 : return  5;
    case 102 : return  6;
    case 104 : return  7;
    case 106 : if (formnum < 20 || formnum > 40) return 8;
      break;
    case 108 : return 15;
    case 110 : return 12;
    case 112 : return 18;
    case 114 : return 19;
    case 116 : return 16;
    case 118 : return 17;
    case 120 : return 20;
    case 122 : return 21;
    case 123 : return 10;
    case 124 : return 22;
    case 125 : return 11;
    case 126 : return  1;
    case 128 : return  2;
    case 130 : return 13;
    case 140 : return 14;
    case 141 : return  3;
    case 142 : return  9;
    case 143 : return  4;
    case 144 : return 23;
    default : break;
  }
  return 0;
}


    void  IGESGeom_ReadWriteModule::ReadOwnParams
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineCurve tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundary tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundedSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCircularArc tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCompositeCurve tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolConicArc tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCopiousData tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCurveOnSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolDirection tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolFlash tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolLine tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetCurve tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPlane tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPoint tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolRuledSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineCurve tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSurfaceOfRevolution tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTabulatedCylinder tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTransformationMatrix tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTrimmedSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    default : break;
  }
}


    void  IGESGeom_ReadWriteModule::WriteOwnParams
  (const Standard_Integer CN,  const Handle(IGESData_IGESEntity)& ent,
   IGESData_IGESWriter& IW) const
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESGeom_BSplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineCurve tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESGeom_BSplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBSplineSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESGeom_Boundary,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundary tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESGeom_BoundedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolBoundedSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESGeom_CircularArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCircularArc tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESGeom_CompositeCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCompositeCurve tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESGeom_ConicArc,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolConicArc tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESGeom_CopiousData,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCopiousData tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESGeom_CurveOnSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolCurveOnSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESGeom_Direction,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolDirection tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESGeom_Flash,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolFlash tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESGeom_Line,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolLine tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESGeom_OffsetCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetCurve tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESGeom_OffsetSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolOffsetSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESGeom_Plane,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPlane tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESGeom_Point,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolPoint tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESGeom_RuledSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolRuledSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESGeom_SplineCurve,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineCurve tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESGeom_SplineSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSplineSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESGeom_SurfaceOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolSurfaceOfRevolution tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESGeom_TabulatedCylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTabulatedCylinder tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESGeom_TransformationMatrix,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTransformationMatrix tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESGeom_TrimmedSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESGeom_ToolTrimmedSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    default : break;
  }
}
