// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntPatch_TheSearchInside_HeaderFile
#define _IntPatch_TheSearchInside_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntSurf_SequenceOfInteriorPoint.hxx>

class Adaptor3d_HSurfaceTool;
class Adaptor3d_TopolTool;
class IntPatch_HInterTool;
class IntPatch_TheSurfFunction;

class IntPatch_TheSearchInside 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPatch_TheSearchInside();
  
  Standard_EXPORT IntPatch_TheSearchInside(IntPatch_TheSurfFunction& F, const Handle(Adaptor3d_Surface)& Surf, const Handle(Adaptor3d_TopolTool)& T, const Standard_Real Epsilon);
  
  Standard_EXPORT void Perform (IntPatch_TheSurfFunction& F, const Handle(Adaptor3d_Surface)& Surf, const Handle(Adaptor3d_TopolTool)& T, const Standard_Real Epsilon);
  
  Standard_EXPORT void Perform (IntPatch_TheSurfFunction& F, const Handle(Adaptor3d_Surface)& Surf, const Standard_Real UStart, const Standard_Real VStart);
  
    Standard_Boolean IsDone() const;
  
  //! Returns the number of points.
  //! The exception NotDone if raised if IsDone
  //! returns False.
    Standard_Integer NbPoints() const;
  
  //! Returns the point of range Index.
  //! The exception NotDone if raised if IsDone
  //! returns False.
  //! The exception OutOfRange if raised if
  //! Index <= 0 or Index > NbPoints.
    const IntSurf_InteriorPoint& Value (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean done;
  IntSurf_SequenceOfInteriorPoint list;


};

#define ThePSurface Handle(Adaptor3d_Surface)
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheSITool IntPatch_HInterTool
#define TheSITool_hxx <IntPatch_HInterTool.hxx>
#define TheFunction IntPatch_TheSurfFunction
#define TheFunction_hxx <IntPatch_TheSurfFunction.hxx>
#define IntStart_SearchInside IntPatch_TheSearchInside
#define IntStart_SearchInside_hxx <IntPatch_TheSearchInside.hxx>

#include <IntStart_SearchInside.lxx>

#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef Handle_TheTopolTool
#undef TheTopolTool
#undef TheTopolTool_hxx
#undef TheSITool
#undef TheSITool_hxx
#undef TheFunction
#undef TheFunction_hxx
#undef IntStart_SearchInside
#undef IntStart_SearchInside_hxx




#endif // _IntPatch_TheSearchInside_HeaderFile
