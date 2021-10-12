// Created on: 1993-01-14
// Created by: Philippe DAUTRY
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

#ifndef _BRepSweep_Builder_HeaderFile
#define _BRepSweep_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRep_Builder.hxx>
#include <TopAbs_Orientation.hxx>
class TopoDS_Shape;


//! implements the abstract Builder with the BRep Builder
class BRepSweep_Builder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Builder.
  Standard_EXPORT BRepSweep_Builder(const BRep_Builder& aBuilder);
  
    const BRep_Builder& Builder() const;
  
  //! Returns an empty Compound.
  Standard_EXPORT void MakeCompound (TopoDS_Shape& aCompound) const;
  
  //! Returns an empty CompSolid.
  Standard_EXPORT void MakeCompSolid (TopoDS_Shape& aCompSolid) const;
  
  //! Returns an empty Solid.
  Standard_EXPORT void MakeSolid (TopoDS_Shape& aSolid) const;
  
  //! Returns an empty Shell.
  Standard_EXPORT void MakeShell (TopoDS_Shape& aShell) const;
  
  //! Returns an empty Wire.
  Standard_EXPORT void MakeWire (TopoDS_Shape& aWire) const;
  
  //! Adds the Shape 1 in the Shape 2, set to
  //! <Orient> orientation.
  Standard_EXPORT void Add (TopoDS_Shape& aShape1, const TopoDS_Shape& aShape2, const TopAbs_Orientation Orient) const;
  
  //! Adds the Shape 1 in the Shape 2.
  Standard_EXPORT void Add (TopoDS_Shape& aShape1, const TopoDS_Shape& aShape2) const;




protected:





private:



  BRep_Builder myBuilder;


};


#include <BRepSweep_Builder.lxx>





#endif // _BRepSweep_Builder_HeaderFile
