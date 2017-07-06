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

#ifndef _XSControl_Vars_HeaderFile
#define _XSControl_Vars_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
class Geom_Geometry;
class Geom2d_Curve;
class Geom_Curve;
class Geom_Surface;
class gp_Pnt;
class gp_Pnt2d;
class TopoDS_Shape;


class XSControl_Vars;
DEFINE_STANDARD_HANDLE(XSControl_Vars, Standard_Transient)

//! Defines a receptacle for externally defined variables, each
//! one has a name
//!
//! I.E. a WorkSession for XSTEP is generally used inside a
//! context, which brings variables, especially shapes and
//! geometries. For instance DRAW or an application engine
//!
//! This class provides a common form for this. It also provides
//! a default implementation (locally recorded variables in a
//! dictionary), but which is aimed to be redefined
class XSControl_Vars : public Standard_Transient
{

public:

  
  Standard_EXPORT XSControl_Vars();
  
  Standard_EXPORT virtual void Set (const Standard_CString name, const Handle(Standard_Transient)& val);
  
  Standard_EXPORT virtual Handle(Standard_Transient) Get (Standard_CString& name) const;
  
  Standard_EXPORT virtual Handle(Geom_Geometry) GetGeom (Standard_CString& name) const;
  
  Standard_EXPORT virtual Handle(Geom2d_Curve) GetCurve2d (Standard_CString& name) const;
  
  Standard_EXPORT virtual Handle(Geom_Curve) GetCurve (Standard_CString& name) const;
  
  Standard_EXPORT virtual Handle(Geom_Surface) GetSurface (Standard_CString& name) const;
  
  Standard_EXPORT virtual void SetPoint (const Standard_CString name, const gp_Pnt& val);
  
  Standard_EXPORT virtual void SetPoint2d (const Standard_CString name, const gp_Pnt2d& val);
  
  Standard_EXPORT virtual Standard_Boolean GetPoint (Standard_CString& name, gp_Pnt& pnt) const;
  
  Standard_EXPORT virtual Standard_Boolean GetPoint2d (Standard_CString& name, gp_Pnt2d& pnt) const;
  
  Standard_EXPORT virtual void SetShape (const Standard_CString name, const TopoDS_Shape& val);
  
  Standard_EXPORT virtual TopoDS_Shape GetShape (Standard_CString& name) const;




  DEFINE_STANDARD_RTTIEXT(XSControl_Vars,Standard_Transient)

protected:




private:


  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thevars;


};







#endif // _XSControl_Vars_HeaderFile
