// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
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

#ifndef _Contap_Contour_HeaderFile
#define _Contap_Contour_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Contap_TheSequenceOfLine.hxx>
#include <Contap_TheSearch.hxx>
#include <Contap_TheSearchInside.hxx>
#include <Contap_SurfFunction.hxx>
#include <Contap_ArcFunction.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_ConstructionError;
class gp_Vec;
class gp_Pnt;
class Adaptor3d_HSurface;
class Adaptor3d_TopolTool;
class Contap_Line;
class Contap_SurfFunction;



class Contap_Contour 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Contap_Contour();
  
  Standard_EXPORT Contap_Contour(const gp_Vec& Direction);
  
  Standard_EXPORT Contap_Contour(const gp_Vec& Direction, const Standard_Real Angle);
  
  Standard_EXPORT Contap_Contour(const gp_Pnt& Eye);
  
  //! Creates the contour in a given direction.
  Standard_EXPORT Contap_Contour(const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Vec& Direction);
  
  //! Creates the contour in a given direction.
  Standard_EXPORT Contap_Contour(const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Vec& Direction, const Standard_Real Angle);
  
  //! Creates the contour for a perspective view.
  Standard_EXPORT Contap_Contour(const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Pnt& Eye);
  
  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain);
  
  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Vec& Direction);
  
  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Vec& Direction, const Standard_Real Angle);
  
  //! Creates the contour for a perspective view.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain, const gp_Pnt& Eye);
  
  Standard_EXPORT void Init (const gp_Vec& Direction);
  
  Standard_EXPORT void Init (const gp_Vec& Direction, const Standard_Real Angle);
  
  Standard_EXPORT void Init (const gp_Pnt& Eye);
  
    Standard_Boolean IsDone() const;
  
  //! Returns true if the is no line.
    Standard_Boolean IsEmpty() const;
  
    Standard_Integer NbLines() const;
  
    const Contap_Line& Line (const Standard_Integer Index) const;
  
  //! Returns    a     reference   on     the   internal
  //! SurfaceFunction.  This is used to compute tangents
  //! on the lines.
    Contap_SurfFunction& SurfaceFunction();




protected:





private:

  
  Standard_EXPORT void Perform (const Handle(Adaptor3d_TopolTool)& Domain);
  
  Standard_EXPORT void PerformAna (const Handle(Adaptor3d_TopolTool)& Domain);


  Standard_Boolean done;
  Contap_TheSequenceOfLine slin;
  Contap_TheSearch solrst;
  Contap_TheSearchInside solins;
  Contap_SurfFunction mySFunc;
  Contap_ArcFunction myAFunc;
  Standard_Boolean modeset;


};


#include <Contap_Contour.lxx>





#endif // _Contap_Contour_HeaderFile
