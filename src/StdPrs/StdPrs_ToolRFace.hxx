// Created on: 1993-01-26
// Created by: Jean-Louis FRENKEL
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

#ifndef _StdPrs_ToolRFace_HeaderFile
#define _StdPrs_ToolRFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Boolean.hxx>
#include <Adaptor2d_Curve2dPtr.hxx>
#include <TopAbs_Orientation.hxx>
class BRepAdaptor_HSurface;



class StdPrs_ToolRFace 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StdPrs_ToolRFace();
  
  Standard_EXPORT StdPrs_ToolRFace(const Handle(BRepAdaptor_HSurface)& aSurface);
  
  Standard_EXPORT Standard_Boolean IsOriented() const;
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT Adaptor2d_Curve2dPtr Value() const;
  
  Standard_EXPORT TopAbs_Orientation Orientation() const;




protected:





private:



  TopoDS_Face myFace;
  TopExp_Explorer myExplorer;
  Geom2dAdaptor_Curve DummyCurve;


};







#endif // _StdPrs_ToolRFace_HeaderFile
