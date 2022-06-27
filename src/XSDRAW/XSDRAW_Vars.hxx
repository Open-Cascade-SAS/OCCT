// Created on: 1998-07-22
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _XSDRAW_Vars_HeaderFile
#define _XSDRAW_Vars_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XSControl_Vars.hxx>
class Standard_Transient;
class Geom_Geometry;
class Geom2d_Curve;
class Geom_Curve;
class Geom_Surface;
class gp_Pnt;
class gp_Pnt2d;
class TopoDS_Shape;


class XSDRAW_Vars;
DEFINE_STANDARD_HANDLE(XSDRAW_Vars, XSControl_Vars)

//! Vars for DRAW session (i.e. DBRep and DrawTrSurf)
class XSDRAW_Vars : public XSControl_Vars
{

public:

  
  Standard_EXPORT XSDRAW_Vars();
  
  Standard_EXPORT virtual void Set (const Standard_CString name, const Handle(Standard_Transient)& val) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Geom_Geometry) GetGeom (Standard_CString& name) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Geom2d_Curve) GetCurve2d (Standard_CString& name) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Geom_Curve) GetCurve (Standard_CString& name) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Geom_Surface) GetSurface (Standard_CString& name) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetPoint (const Standard_CString name, const gp_Pnt& val) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetPoint2d (const Standard_CString name, const gp_Pnt2d& val) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean GetPoint (Standard_CString& name, gp_Pnt& pnt) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean GetPoint2d (Standard_CString& name, gp_Pnt2d& pnt) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetShape (const Standard_CString name, const TopoDS_Shape& val) Standard_OVERRIDE;
  
  Standard_EXPORT virtual TopoDS_Shape GetShape (Standard_CString& name) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XSDRAW_Vars,XSControl_Vars)

protected:




private:




};







#endif // _XSDRAW_Vars_HeaderFile
