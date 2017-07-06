// Created on: 1998-09-07
// Created by: Denis PASCAL
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

#ifndef _DDataStd_DrawDriver_HeaderFile
#define _DDataStd_DrawDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Draw_ColorKind.hxx>
#include <Standard_Boolean.hxx>
class Draw_Drawable3D;
class TDF_Label;
class TDataXtd_Constraint;
class TopoDS_Shape;


class DDataStd_DrawDriver;
DEFINE_STANDARD_HANDLE(DDataStd_DrawDriver, Standard_Transient)

//! Root class of drivers to build draw variables from TDF_Label.
//! Priority rule to display standard attributes is :
//! * 1 Constraint
//! * 2 Object
//! * 3 Datum      (Point,Axis,Plane)
//! * 4 Geometry
//! * 5 NamedShape
class DDataStd_DrawDriver : public Standard_Transient
{

public:

  
  //! access to the current DrawDriver
  //! ================================
  Standard_EXPORT static void Set (const Handle(DDataStd_DrawDriver)& DD);
  
  Standard_EXPORT static Handle(DDataStd_DrawDriver) Get();
  
  //! next method is called by DrawPresentation (may be redefined)
  //! ============================================================
  Standard_EXPORT DDataStd_DrawDriver();
  
  //! reusable methods (may used when redefined <Drawable>)
  //! =====================================================
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Drawable (const TDF_Label& L) const;
  
  Standard_EXPORT Handle(Draw_Drawable3D) DrawableConstraint (const Handle(TDataXtd_Constraint)& C) const;
  
  Standard_EXPORT Handle(Draw_Drawable3D) DrawableShape (const TDF_Label& L, const Draw_ColorKind color, const Standard_Boolean current = Standard_True) const;
  
  //! May be used for temporary display of a shape
  Standard_EXPORT static Handle(Draw_Drawable3D) DrawableShape (const TopoDS_Shape& s, const Draw_ColorKind color);




  DEFINE_STANDARD_RTTIEXT(DDataStd_DrawDriver,Standard_Transient)

protected:




private:




};







#endif // _DDataStd_DrawDriver_HeaderFile
