// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGeom_OffsetSurface_HeaderFile
#define _IGESGeom_OffsetSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Vec;


class IGESGeom_OffsetSurface;
DEFINE_STANDARD_HANDLE(IGESGeom_OffsetSurface, IGESData_IGESEntity)

//! defines IGESOffsetSurface, Type <140> Form <0>
//! in package IGESGeom
//! An offset surface is a surface defined in terms of an
//! already existing surface.If S(u, v) is a parametrised
//! regular surface and N(u, v) is a differential field of
//! unit normal vectors defined on the whole surface, and
//! "d" a fixed non zero real number, then offset surface
//! to S is a parametrised surface S(u, v) given by
//! O(u, v) = S(u, v) + d * N(u, v);
//! u1 <= u <= u2; v1 <= v <= v2;
class IGESGeom_OffsetSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_OffsetSurface();
  
  //! This method is used to set the fields of the class
  //! OffsetSurface
  //! - anIndicator : Offset indicator
  //! - aDistance   : Offset distance
  //! - aSurface    : Surface that is offset
  Standard_EXPORT void Init (const gp_XYZ& anIndicatoR, const Standard_Real aDistance, const Handle(IGESData_IGESEntity)& aSurface);
  
  //! returns the offset indicator
  Standard_EXPORT gp_Vec OffsetIndicator() const;
  
  //! returns the offset indicator after applying Transf. Matrix
  Standard_EXPORT gp_Vec TransformedOffsetIndicator() const;
  
  //! returns the distance by which surface is offset
  Standard_EXPORT Standard_Real Distance() const;
  
  //! returns the surface that has been offset
  Standard_EXPORT Handle(IGESData_IGESEntity) Surface() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_OffsetSurface,IGESData_IGESEntity)

protected:




private:


  gp_XYZ theIndicator;
  Standard_Real theDistance;
  Handle(IGESData_IGESEntity) theSurface;


};







#endif // _IGESGeom_OffsetSurface_HeaderFile
