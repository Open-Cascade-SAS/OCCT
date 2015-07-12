// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_HeaderFile
#define _IGESGeom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESGeom_Protocol;
class IGESGeom_CircularArc;
class IGESGeom_CompositeCurve;
class IGESGeom_ConicArc;
class IGESGeom_CopiousData;
class IGESGeom_Plane;
class IGESGeom_Line;
class IGESGeom_SplineCurve;
class IGESGeom_SplineSurface;
class IGESGeom_Point;
class IGESGeom_RuledSurface;
class IGESGeom_SurfaceOfRevolution;
class IGESGeom_TabulatedCylinder;
class IGESGeom_Direction;
class IGESGeom_TransformationMatrix;
class IGESGeom_Flash;
class IGESGeom_BSplineCurve;
class IGESGeom_BSplineSurface;
class IGESGeom_OffsetCurve;
class IGESGeom_OffsetSurface;
class IGESGeom_Boundary;
class IGESGeom_CurveOnSurface;
class IGESGeom_BoundedSurface;
class IGESGeom_TrimmedSurface;
class IGESGeom_ToolCircularArc;
class IGESGeom_ToolCompositeCurve;
class IGESGeom_ToolConicArc;
class IGESGeom_ToolCopiousData;
class IGESGeom_ToolPlane;
class IGESGeom_ToolLine;
class IGESGeom_ToolSplineCurve;
class IGESGeom_ToolSplineSurface;
class IGESGeom_ToolPoint;
class IGESGeom_ToolRuledSurface;
class IGESGeom_ToolSurfaceOfRevolution;
class IGESGeom_ToolTabulatedCylinder;
class IGESGeom_ToolDirection;
class IGESGeom_ToolTransformationMatrix;
class IGESGeom_ToolFlash;
class IGESGeom_ToolBSplineCurve;
class IGESGeom_ToolBSplineSurface;
class IGESGeom_ToolOffsetCurve;
class IGESGeom_ToolOffsetSurface;
class IGESGeom_ToolBoundary;
class IGESGeom_ToolCurveOnSurface;
class IGESGeom_ToolBoundedSurface;
class IGESGeom_ToolTrimmedSurface;
class IGESGeom_Protocol;
class IGESGeom_ReadWriteModule;
class IGESGeom_GeneralModule;
class IGESGeom_SpecificModule;


//! This package consists of B-Rep and CSG Solid entities
class IGESGeom 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dymanic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESGeom_Protocol) Protocol();




protected:





private:




friend class IGESGeom_CircularArc;
friend class IGESGeom_CompositeCurve;
friend class IGESGeom_ConicArc;
friend class IGESGeom_CopiousData;
friend class IGESGeom_Plane;
friend class IGESGeom_Line;
friend class IGESGeom_SplineCurve;
friend class IGESGeom_SplineSurface;
friend class IGESGeom_Point;
friend class IGESGeom_RuledSurface;
friend class IGESGeom_SurfaceOfRevolution;
friend class IGESGeom_TabulatedCylinder;
friend class IGESGeom_Direction;
friend class IGESGeom_TransformationMatrix;
friend class IGESGeom_Flash;
friend class IGESGeom_BSplineCurve;
friend class IGESGeom_BSplineSurface;
friend class IGESGeom_OffsetCurve;
friend class IGESGeom_OffsetSurface;
friend class IGESGeom_Boundary;
friend class IGESGeom_CurveOnSurface;
friend class IGESGeom_BoundedSurface;
friend class IGESGeom_TrimmedSurface;
friend class IGESGeom_ToolCircularArc;
friend class IGESGeom_ToolCompositeCurve;
friend class IGESGeom_ToolConicArc;
friend class IGESGeom_ToolCopiousData;
friend class IGESGeom_ToolPlane;
friend class IGESGeom_ToolLine;
friend class IGESGeom_ToolSplineCurve;
friend class IGESGeom_ToolSplineSurface;
friend class IGESGeom_ToolPoint;
friend class IGESGeom_ToolRuledSurface;
friend class IGESGeom_ToolSurfaceOfRevolution;
friend class IGESGeom_ToolTabulatedCylinder;
friend class IGESGeom_ToolDirection;
friend class IGESGeom_ToolTransformationMatrix;
friend class IGESGeom_ToolFlash;
friend class IGESGeom_ToolBSplineCurve;
friend class IGESGeom_ToolBSplineSurface;
friend class IGESGeom_ToolOffsetCurve;
friend class IGESGeom_ToolOffsetSurface;
friend class IGESGeom_ToolBoundary;
friend class IGESGeom_ToolCurveOnSurface;
friend class IGESGeom_ToolBoundedSurface;
friend class IGESGeom_ToolTrimmedSurface;
friend class IGESGeom_Protocol;
friend class IGESGeom_ReadWriteModule;
friend class IGESGeom_GeneralModule;
friend class IGESGeom_SpecificModule;

};







#endif // _IGESGeom_HeaderFile
