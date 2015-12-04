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
#include <IGESSolid_Block.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_Cylinder.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_ReadWriteModule.hxx>
#include <IGESSolid_RightAngularWedge.hxx>
#include <IGESSolid_SelectedComponent.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_SolidAssembly.hxx>
#include <IGESSolid_SolidInstance.hxx>
#include <IGESSolid_SolidOfLinearExtrusion.hxx>
#include <IGESSolid_SolidOfRevolution.hxx>
#include <IGESSolid_Sphere.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToolBlock.hxx>
#include <IGESSolid_ToolBooleanTree.hxx>
#include <IGESSolid_ToolConeFrustum.hxx>
#include <IGESSolid_ToolConicalSurface.hxx>
#include <IGESSolid_ToolCylinder.hxx>
#include <IGESSolid_ToolCylindricalSurface.hxx>
#include <IGESSolid_ToolEdgeList.hxx>
#include <IGESSolid_ToolEllipsoid.hxx>
#include <IGESSolid_ToolFace.hxx>
#include <IGESSolid_ToolLoop.hxx>
#include <IGESSolid_ToolManifoldSolid.hxx>
#include <IGESSolid_ToolPlaneSurface.hxx>
#include <IGESSolid_ToolRightAngularWedge.hxx>
#include <IGESSolid_ToolSelectedComponent.hxx>
#include <IGESSolid_ToolShell.hxx>
#include <IGESSolid_ToolSolidAssembly.hxx>
#include <IGESSolid_ToolSolidInstance.hxx>
#include <IGESSolid_ToolSolidOfLinearExtrusion.hxx>
#include <IGESSolid_ToolSolidOfRevolution.hxx>
#include <IGESSolid_ToolSphere.hxx>
#include <IGESSolid_ToolSphericalSurface.hxx>
#include <IGESSolid_ToolToroidalSurface.hxx>
#include <IGESSolid_ToolTorus.hxx>
#include <IGESSolid_ToolVertexList.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESSolid_Torus.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_ReadWriteModule,IGESData_ReadWriteModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESSolid_ReadWriteModule::IGESSolid_ReadWriteModule ()    {  }


    Standard_Integer  IGESSolid_ReadWriteModule::CaseIGES
  (const Standard_Integer typenum, const Standard_Integer /*formnum*/) const 
{
  switch (typenum) {
    case 150 : return  1;
    case 152 : return 13;
    case 154 : return  5;
    case 156 : return  3;
    case 158 : return 20;
    case 160 : return 23;
    case 162 : return 19;
    case 164 : return 18;
    case 168 : return  8;
    case 180 : return  2;
    case 182 : return 14;
    case 184 : return 16;
    case 186 : return 11;
    case 190 : return 12;
    case 192 : return  6;
    case 194 : return  4;
    case 196 : return 21;
    case 198 : return 22;
    case 430 : return 17;
    case 502 : return 24;
    case 504 : return  7;
    case 508 : return 10;
    case 510 : return  9;
    case 514 : return 15;
    default : break;
  }
  return 0;
}


    void  IGESSolid_ReadWriteModule::ReadOwnParams
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBlock tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBooleanTree tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConeFrustum tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConicalSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylinder tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylindricalSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEdgeList tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEllipsoid tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolFace tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolLoop tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolManifoldSolid tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolPlaneSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolRightAngularWedge tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSelectedComponent tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolShell tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidAssembly tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidInstance tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfRevolution tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphere tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphericalSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolToroidalSurface tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolTorus tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolVertexList tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    default : break;
  }
}


    void  IGESSolid_ReadWriteModule::WriteOwnParams
  (const Standard_Integer CN,  const Handle(IGESData_IGESEntity)& ent,
   IGESData_IGESWriter& IW) const
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBlock tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBooleanTree tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConeFrustum tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConicalSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylinder tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylindricalSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEdgeList tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEllipsoid tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolFace tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolLoop tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolManifoldSolid tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolPlaneSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolRightAngularWedge tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSelectedComponent tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolShell tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidAssembly tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidInstance tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfRevolution tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphere tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphericalSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolToroidalSurface tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolTorus tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolVertexList tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    default : break;
  }
}
