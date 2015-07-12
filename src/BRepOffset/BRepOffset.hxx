// Created on: 1995-10-12
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

#ifndef _BRepOffset_HeaderFile
#define _BRepOffset_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <BRepOffset_Status.hxx>
class Geom_Surface;
class BRepOffset_MakeOffset;
class BRepOffset_Inter3d;
class BRepOffset_Inter2d;
class BRepOffset_Offset;
class BRepOffset_Analyse;
class BRepOffset_MakeLoops;
class BRepOffset_Tool;
class BRepOffset_Interval;



class BRepOffset 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns   the  Offset  surface  computed from  the
  //! surface <Surface> at an OffsetDistance <Offset>.
  //!
  //! If possible, this method returns  the real type of
  //! the surface ( e.g. An Offset of a plane is a plane).
  //!
  //! If  no particular  case  is detected, the returned
  //! surface will have the Type Geom_OffsetSurface.
  Standard_EXPORT static Handle(Geom_Surface) Surface (const Handle(Geom_Surface)& Surface, const Standard_Real Offset, BRepOffset_Status& Status);




protected:





private:




friend class BRepOffset_MakeOffset;
friend class BRepOffset_Inter3d;
friend class BRepOffset_Inter2d;
friend class BRepOffset_Offset;
friend class BRepOffset_Analyse;
friend class BRepOffset_MakeLoops;
friend class BRepOffset_Tool;
friend class BRepOffset_Interval;

};







#endif // _BRepOffset_HeaderFile
