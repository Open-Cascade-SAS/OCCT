// Created on: 1993-07-22
// Created by: Remi LEQUETTE
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

#ifndef _BRepPrimAPI_MakeOneAxis_HeaderFile
#define _BRepPrimAPI_MakeOneAxis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepBuilderAPI_MakeShape.hxx>
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;

//! The abstract class MakeOneAxis is the root class of
//! algorithms used to construct rotational primitives.
class BRepPrimAPI_MakeOneAxis : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! The inherited commands should provide the algorithm.
  //! Returned as a pointer.
  Standard_EXPORT virtual Standard_Address OneAxis() = 0;

  //! Stores the solid in myShape.
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Returns the lateral face of the rotational primitive.
  Standard_EXPORT const TopoDS_Face& Face();
  Standard_EXPORT                    operator TopoDS_Face();

  //! Returns the constructed rotational primitive as a shell.
  Standard_EXPORT const TopoDS_Shell& Shell();
  Standard_EXPORT                     operator TopoDS_Shell();

  //! Returns the constructed rotational primitive as a solid.
  Standard_EXPORT const TopoDS_Solid& Solid();
  Standard_EXPORT                     operator TopoDS_Solid();

protected:
private:
};

#endif // _BRepPrimAPI_MakeOneAxis_HeaderFile
