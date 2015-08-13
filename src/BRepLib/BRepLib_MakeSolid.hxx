// Created on: 1995-01-04
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepLib_MakeSolid_HeaderFile
#define _BRepLib_MakeSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <BRepLib_MakeShape.hxx>
#include <BRepLib_ShapeModification.hxx>
class StdFail_NotDone;
class TopoDS_CompSolid;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_Face;


//! Makes a solid from compsolid  or  shells.
class BRepLib_MakeSolid  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Solid covers whole space.
  Standard_EXPORT BRepLib_MakeSolid();
  
  //! Make a solid from a CompSolid.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_CompSolid& S);
  
  //! Make a solid from a shell.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_Shell& S);
  
  //! Make a solid from two shells.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2);
  
  //! Make a solid from three shells.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2, const TopoDS_Shell& S3);
  
  //! Make a solid from a solid. Usefull for adding later.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_Solid& So);
  
  //! Add a shell to a solid.
  Standard_EXPORT BRepLib_MakeSolid(const TopoDS_Solid& So, const TopoDS_Shell& S);
  
  //! Add the shell to the current solid.
  Standard_EXPORT void Add (const TopoDS_Shell& S);
  
  //! Returns the new Solid.
  Standard_EXPORT const TopoDS_Solid& Solid();
  Standard_EXPORT operator TopoDS_Solid();
  
  //! returns the status of the Face after
  //! the shape creation.
  Standard_EXPORT virtual BRepLib_ShapeModification FaceStatus (const TopoDS_Face& F) const Standard_OVERRIDE;




protected:



  TopTools_ListOfShape myDeletedFaces;


private:





};







#endif // _BRepLib_MakeSolid_HeaderFile
