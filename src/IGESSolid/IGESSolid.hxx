// Created on: 1993-01-11
// Created by: SIVA
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESSolid_HeaderFile
#define _IGESSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESSolid_Protocol;


//! This package consists of B-Rep and CSG Solid entities
class IGESSolid 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESSolid_Protocol) Protocol();




protected:





private:




friend class IGESSolid_Block;
friend class IGESSolid_RightAngularWedge;
friend class IGESSolid_Cylinder;
friend class IGESSolid_ConeFrustum;
friend class IGESSolid_Sphere;
friend class IGESSolid_Torus;
friend class IGESSolid_SolidOfRevolution;
friend class IGESSolid_SolidOfLinearExtrusion;
friend class IGESSolid_Ellipsoid;
friend class IGESSolid_BooleanTree;
friend class IGESSolid_SelectedComponent;
friend class IGESSolid_SolidAssembly;
friend class IGESSolid_ManifoldSolid;
friend class IGESSolid_PlaneSurface;
friend class IGESSolid_CylindricalSurface;
friend class IGESSolid_ConicalSurface;
friend class IGESSolid_SphericalSurface;
friend class IGESSolid_ToroidalSurface;
friend class IGESSolid_SolidInstance;
friend class IGESSolid_VertexList;
friend class IGESSolid_EdgeList;
friend class IGESSolid_Loop;
friend class IGESSolid_Face;
friend class IGESSolid_Shell;
friend class IGESSolid_ToolBlock;
friend class IGESSolid_ToolRightAngularWedge;
friend class IGESSolid_ToolCylinder;
friend class IGESSolid_ToolConeFrustum;
friend class IGESSolid_ToolSphere;
friend class IGESSolid_ToolTorus;
friend class IGESSolid_ToolSolidOfRevolution;
friend class IGESSolid_ToolSolidOfLinearExtrusion;
friend class IGESSolid_ToolEllipsoid;
friend class IGESSolid_ToolBooleanTree;
friend class IGESSolid_ToolSelectedComponent;
friend class IGESSolid_ToolSolidAssembly;
friend class IGESSolid_ToolManifoldSolid;
friend class IGESSolid_ToolPlaneSurface;
friend class IGESSolid_ToolCylindricalSurface;
friend class IGESSolid_ToolConicalSurface;
friend class IGESSolid_ToolSphericalSurface;
friend class IGESSolid_ToolToroidalSurface;
friend class IGESSolid_ToolSolidInstance;
friend class IGESSolid_ToolVertexList;
friend class IGESSolid_ToolEdgeList;
friend class IGESSolid_ToolLoop;
friend class IGESSolid_ToolFace;
friend class IGESSolid_ToolShell;
friend class IGESSolid_Protocol;
friend class IGESSolid_ReadWriteModule;
friend class IGESSolid_GeneralModule;
friend class IGESSolid_SpecificModule;
friend class IGESSolid_TopoBuilder;

};







#endif // _IGESSolid_HeaderFile
