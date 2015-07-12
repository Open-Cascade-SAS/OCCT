// Created on: 1994-12-16
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StepToTopoDS_TranslateEdge_HeaderFile
#define _StepToTopoDS_TranslateEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_TranslateEdgeError.hxx>
#include <TopoDS_Shape.hxx>
#include <StepToTopoDS_Root.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class StepShape_Edge;
class StepToTopoDS_Tool;
class StepToTopoDS_NMTool;
class StepGeom_Curve;
class StepShape_EdgeCurve;
class StepShape_Vertex;
class TopoDS_Edge;
class TopoDS_Vertex;
class Geom2d_Curve;
class StepGeom_Pcurve;
class Geom_Surface;
class TopoDS_Shape;



class StepToTopoDS_TranslateEdge  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepToTopoDS_TranslateEdge();
  
  Standard_EXPORT StepToTopoDS_TranslateEdge(const Handle(StepShape_Edge)& E, StepToTopoDS_Tool& T, StepToTopoDS_NMTool& NMTool);
  
  Standard_EXPORT void Init (const Handle(StepShape_Edge)& E, StepToTopoDS_Tool& T, StepToTopoDS_NMTool& NMTool);
  
  //! Warning! C3D is assumed to be a Curve 3D ...
  //! other cases to checked before calling this
  Standard_EXPORT void MakeFromCurve3D (const Handle(StepGeom_Curve)& C3D, const Handle(StepShape_EdgeCurve)& EC, const Handle(StepShape_Vertex)& Vend, const Standard_Real preci, TopoDS_Edge& E, TopoDS_Vertex& V1, TopoDS_Vertex& V2, StepToTopoDS_Tool& T);
  
  Standard_EXPORT Handle(Geom2d_Curve) MakePCurve (const Handle(StepGeom_Pcurve)& PCU, const Handle(Geom_Surface)& ConvSurf) const;
  
  Standard_EXPORT const TopoDS_Shape& Value() const;
  
  Standard_EXPORT StepToTopoDS_TranslateEdgeError Error() const;




protected:





private:



  StepToTopoDS_TranslateEdgeError myError;
  TopoDS_Shape myResult;


};







#endif // _StepToTopoDS_TranslateEdge_HeaderFile
