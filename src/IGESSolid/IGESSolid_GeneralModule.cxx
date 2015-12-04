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
#include <IGESSolid_Block.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ConeFrustum.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_Cylinder.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_GeneralModule.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
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
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_GeneralModule,IGESData_GeneralModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESSolid_GeneralModule::IGESSolid_GeneralModule ()    {  }


    void  IGESSolid_GeneralModule::OwnSharedCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& iter) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBlock tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBooleanTree tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConeFrustum tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConicalSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylinder tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylindricalSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEdgeList tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEllipsoid tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolFace tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolLoop tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolManifoldSolid tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolPlaneSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolRightAngularWedge tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSelectedComponent tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolShell tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidAssembly tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidInstance tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfRevolution tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphere tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphericalSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolToroidalSurface tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolTorus tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolVertexList tool;
      tool.OwnShared(anent,iter);
    }
      break;
    default : break;
  }
}


    IGESData_DirChecker  IGESSolid_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolBlock tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolBooleanTree tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolConeFrustum tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolConicalSurface tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolCylinder tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolCylindricalSurface tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolEdgeList tool;
      return tool.DirChecker(anent);
    }
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolEllipsoid tool;
      return tool.DirChecker(anent);
    }
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolFace tool;
      return tool.DirChecker(anent);
    }
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolLoop tool;
      return tool.DirChecker(anent);
    }
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolManifoldSolid tool;
      return tool.DirChecker(anent);
    }
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolPlaneSurface tool;
      return tool.DirChecker(anent);
    }
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolRightAngularWedge tool;
      return tool.DirChecker(anent);
    }
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSelectedComponent tool;
      return tool.DirChecker(anent);
    }
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolShell tool;
      return tool.DirChecker(anent);
    }
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidAssembly tool;
      return tool.DirChecker(anent);
    }
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidInstance tool;
      return tool.DirChecker(anent);
    }
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      return tool.DirChecker(anent);
    }
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSolidOfRevolution tool;
      return tool.DirChecker(anent);
    }
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSphere tool;
      return tool.DirChecker(anent);
    }
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolSphericalSurface tool;
      return tool.DirChecker(anent);
    }
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolToroidalSurface tool;
      return tool.DirChecker(anent);
    }
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolTorus tool;
      return tool.DirChecker(anent);
    }
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) break;
      IGESSolid_ToolVertexList tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    void  IGESSolid_GeneralModule::OwnCheckCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBlock tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolBooleanTree tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConeFrustum tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolConicalSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylinder tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolCylindricalSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEdgeList tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolEllipsoid tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESSolid_Face,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolFace tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolLoop tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolManifoldSolid tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolPlaneSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolRightAngularWedge tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSelectedComponent tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolShell tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidAssembly tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidInstance tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSolidOfRevolution tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphere tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolSphericalSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolToroidalSurface tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolTorus tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,anent,ent);
      if (anent.IsNull()) return;
      IGESSolid_ToolVertexList tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    default : break;
  }
}


    Standard_Boolean  IGESSolid_GeneralModule::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const 
{
  switch (CN) {
    case  1 : ent = new IGESSolid_Block;	break;
    case  2 : ent = new IGESSolid_BooleanTree;	break;
    case  3 : ent = new IGESSolid_ConeFrustum;	break;
    case  4 : ent = new IGESSolid_ConicalSurface;	break;
    case  5 : ent = new IGESSolid_Cylinder;	break;
    case  6 : ent = new IGESSolid_CylindricalSurface;	break;
    case  7 : ent = new IGESSolid_EdgeList;	break;
    case  8 : ent = new IGESSolid_Ellipsoid;	break;
    case  9 : ent = new IGESSolid_Face;		break;
    case 10 : ent = new IGESSolid_Loop;		break;
    case 11 : ent = new IGESSolid_ManifoldSolid;	break;
    case 12 : ent = new IGESSolid_PlaneSurface;	break;
    case 13 : ent = new IGESSolid_RightAngularWedge;	break;
    case 14 : ent = new IGESSolid_SelectedComponent;	break;
    case 15 : ent = new IGESSolid_Shell;	break;
    case 16 : ent = new IGESSolid_SolidAssembly;	break;
    case 17 : ent = new IGESSolid_SolidInstance;	break;
    case 18 : ent = new IGESSolid_SolidOfLinearExtrusion;	break;
    case 19 : ent = new IGESSolid_SolidOfRevolution;	break;
    case 20 : ent = new IGESSolid_Sphere;	break;
    case 21 : ent = new IGESSolid_SphericalSurface;	break;
    case 22 : ent = new IGESSolid_ToroidalSurface;	break;
    case 23 : ent = new IGESSolid_Torus;	break;
    case 24 : ent = new IGESSolid_VertexList;	break;
    default : return Standard_False;    // by default, Failure on Recognize
  }
  return Standard_True;
}


    void  IGESSolid_GeneralModule::OwnCopyCase
  (const Standard_Integer CN,
   const Handle(IGESData_IGESEntity)& entfrom,
   const Handle(IGESData_IGESEntity)& entto,
   Interface_CopyTool& TC) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESSolid_Block,enfr,entfrom);
      DeclareAndCast(IGESSolid_Block,ento,entto);
      IGESSolid_ToolBlock tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESSolid_BooleanTree,enfr,entfrom);
      DeclareAndCast(IGESSolid_BooleanTree,ento,entto);
      IGESSolid_ToolBooleanTree tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESSolid_ConeFrustum,enfr,entfrom);
      DeclareAndCast(IGESSolid_ConeFrustum,ento,entto);
      IGESSolid_ToolConeFrustum tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESSolid_ConicalSurface,enfr,entfrom);
      DeclareAndCast(IGESSolid_ConicalSurface,ento,entto);
      IGESSolid_ToolConicalSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESSolid_Cylinder,enfr,entfrom);
      DeclareAndCast(IGESSolid_Cylinder,ento,entto);
      IGESSolid_ToolCylinder tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESSolid_CylindricalSurface,enfr,entfrom);
      DeclareAndCast(IGESSolid_CylindricalSurface,ento,entto);
      IGESSolid_ToolCylindricalSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESSolid_EdgeList,enfr,entfrom);
      DeclareAndCast(IGESSolid_EdgeList,ento,entto);
      IGESSolid_ToolEdgeList tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESSolid_Ellipsoid,enfr,entfrom);
      DeclareAndCast(IGESSolid_Ellipsoid,ento,entto);
      IGESSolid_ToolEllipsoid tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESSolid_Face,enfr,entfrom);
      DeclareAndCast(IGESSolid_Face,ento,entto);
      IGESSolid_ToolFace tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESSolid_Loop,enfr,entfrom);
      DeclareAndCast(IGESSolid_Loop,ento,entto);
      IGESSolid_ToolLoop tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESSolid_ManifoldSolid,enfr,entfrom);
      DeclareAndCast(IGESSolid_ManifoldSolid,ento,entto);
      IGESSolid_ToolManifoldSolid tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESSolid_PlaneSurface,enfr,entfrom);
      DeclareAndCast(IGESSolid_PlaneSurface,ento,entto);
      IGESSolid_ToolPlaneSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESSolid_RightAngularWedge,enfr,entfrom);
      DeclareAndCast(IGESSolid_RightAngularWedge,ento,entto);
      IGESSolid_ToolRightAngularWedge tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESSolid_SelectedComponent,enfr,entfrom);
      DeclareAndCast(IGESSolid_SelectedComponent,ento,entto);
      IGESSolid_ToolSelectedComponent tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESSolid_Shell,enfr,entfrom);
      DeclareAndCast(IGESSolid_Shell,ento,entto);
      IGESSolid_ToolShell tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESSolid_SolidAssembly,enfr,entfrom);
      DeclareAndCast(IGESSolid_SolidAssembly,ento,entto);
      IGESSolid_ToolSolidAssembly tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 17 : {
      DeclareAndCast(IGESSolid_SolidInstance,enfr,entfrom);
      DeclareAndCast(IGESSolid_SolidInstance,ento,entto);
      IGESSolid_ToolSolidInstance tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 18 : {
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,enfr,entfrom);
      DeclareAndCast(IGESSolid_SolidOfLinearExtrusion,ento,entto);
      IGESSolid_ToolSolidOfLinearExtrusion tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 19 : {
      DeclareAndCast(IGESSolid_SolidOfRevolution,enfr,entfrom);
      DeclareAndCast(IGESSolid_SolidOfRevolution,ento,entto);
      IGESSolid_ToolSolidOfRevolution tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 20 : {
      DeclareAndCast(IGESSolid_Sphere,enfr,entfrom);
      DeclareAndCast(IGESSolid_Sphere,ento,entto);
      IGESSolid_ToolSphere tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 21 : {
      DeclareAndCast(IGESSolid_SphericalSurface,enfr,entfrom);
      DeclareAndCast(IGESSolid_SphericalSurface,ento,entto);
      IGESSolid_ToolSphericalSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 22 : {
      DeclareAndCast(IGESSolid_ToroidalSurface,enfr,entfrom);
      DeclareAndCast(IGESSolid_ToroidalSurface,ento,entto);
      IGESSolid_ToolToroidalSurface tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 23 : {
      DeclareAndCast(IGESSolid_Torus,enfr,entfrom);
      DeclareAndCast(IGESSolid_Torus,ento,entto);
      IGESSolid_ToolTorus tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case 24 : {
      DeclareAndCast(IGESSolid_VertexList,enfr,entfrom);
      DeclareAndCast(IGESSolid_VertexList,ento,entto);
      IGESSolid_ToolVertexList tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    default : break;
  }
}


    Standard_Integer  IGESSolid_GeneralModule::CategoryNumber
  (const Standard_Integer /*CN*/, const Handle(Standard_Transient)& ,
   const Interface_ShareTool& ) const
{
  return Interface_Category::Number("Shape");
}
