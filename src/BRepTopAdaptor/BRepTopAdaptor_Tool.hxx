// Created on: 1997-10-07
// Created by: Laurent BUCHARD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepTopAdaptor_Tool_HeaderFile
#define _BRepTopAdaptor_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class BRepTopAdaptor_TopolTool;
class Adaptor3d_HSurface;
class TopoDS_Face;



class BRepTopAdaptor_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepTopAdaptor_Tool();
  
  Standard_EXPORT BRepTopAdaptor_Tool(const TopoDS_Face& F, const Standard_Real Tol2d);
  
  Standard_EXPORT BRepTopAdaptor_Tool(const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real Tol2d);
  
  Standard_EXPORT void Init (const TopoDS_Face& F, const Standard_Real Tol2d);
  
  Standard_EXPORT void Init (const Handle(Adaptor3d_HSurface)& Surface, const Standard_Real Tol2d);
  
  Standard_EXPORT Handle(BRepTopAdaptor_TopolTool) GetTopolTool();
  
  Standard_EXPORT void SetTopolTool (const Handle(BRepTopAdaptor_TopolTool)& TT);
  
  Standard_EXPORT Handle(Adaptor3d_HSurface) GetSurface();
  
  Standard_EXPORT void Destroy();
~BRepTopAdaptor_Tool()
{
  Destroy();
}




protected:





private:



  Standard_Boolean myloaded;
  Handle(BRepTopAdaptor_TopolTool) myTopolTool;
  Handle(Adaptor3d_HSurface) myHSurface;


};







#endif // _BRepTopAdaptor_Tool_HeaderFile
