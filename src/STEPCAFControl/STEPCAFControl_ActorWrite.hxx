// Created on: 2000-10-05
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_ActorWrite_HeaderFile
#define _STEPCAFControl_ActorWrite_HeaderFile

#include <Standard.hxx>

#include <Standard_Boolean.hxx>
#include <TopTools_MapOfShape.hxx>
#include <STEPControl_ActorWrite.hxx>
class TopoDS_Shape;


class STEPCAFControl_ActorWrite;
DEFINE_STANDARD_HANDLE(STEPCAFControl_ActorWrite, STEPControl_ActorWrite)

//! Extends ActorWrite from STEPControl by analysis of
//! whether shape is assembly (based on information from DECAF)
class STEPCAFControl_ActorWrite : public STEPControl_ActorWrite
{

public:

  
  Standard_EXPORT STEPCAFControl_ActorWrite();
  
  //! Check whether shape S is assembly
  //! Returns True if shape is registered in assemblies map
  Standard_EXPORT virtual Standard_Boolean IsAssembly (const Handle(StepData_StepModel)& theModel,
                                                       TopoDS_Shape& S) const Standard_OVERRIDE;
  
  //! Set standard mode of work
  //! In standard mode Actor (default) behaves exactly as its
  //! ancestor, also map is cleared
  Standard_EXPORT void SetStdMode (const Standard_Boolean stdmode = Standard_True);
  
  //! Clears map of shapes registered as assemblies
  Standard_EXPORT void ClearMap();
  
  //! Registers shape to be written as assembly
  //! The shape should be TopoDS_Compound (else does nothing)
  Standard_EXPORT void RegisterAssembly (const TopoDS_Shape& S);




  DEFINE_STANDARD_RTTIEXT(STEPCAFControl_ActorWrite,STEPControl_ActorWrite)

protected:




private:


  Standard_Boolean myStdMode;
  TopTools_MapOfShape myMap;


};







#endif // _STEPCAFControl_ActorWrite_HeaderFile
