// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Kiran)
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

#ifndef _IGESGeom_BoundedSurface_HeaderFile
#define _IGESGeom_BoundedSurface_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESGeom_Boundary.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESGeom_Boundary;

//! defines BoundedSurface, Type <143> Form <0>
//! in package IGESGeom
//! A bounded surface is used to communicate trimmed
//! surfaces. The surface and trimming curves are assumed
//! to be represented parametrically.
class IGESGeom_BoundedSurface : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_BoundedSurface();

  //! This method is used to set the fields of the class
  //! BoundedSurface
  //! - aType     : Type of bounded surface representation
  //! - aSurface  : Surface entity to be bounded
  //! - allBounds : Array of boundary entities
  Standard_EXPORT void Init(const int                    aType,
                            const occ::handle<IGESData_IGESEntity>&        aSurface,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESGeom_Boundary>>>& allBounds);

  //! returns the type of Bounded surface representation
  //! 0 = The boundary entities may only reference model space curves
  //! 1 = The boundary entities may reference both model space curves
  //! and associated parameter space curve representations
  Standard_EXPORT int RepresentationType() const;

  //! returns the bounded surface
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Surface() const;

  //! returns the number of boundaries
  Standard_EXPORT int NbBoundaries() const;

  //! returns boundary entity
  //! raises exception if Index <= 0 or Index > NbBoundaries()
  Standard_EXPORT occ::handle<IGESGeom_Boundary> Boundary(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_BoundedSurface, IGESData_IGESEntity)

private:
  int                   theType;
  occ::handle<IGESData_IGESEntity>        theSurface;
  occ::handle<NCollection_HArray1<occ::handle<IGESGeom_Boundary>>> theBoundaries;
};

#endif // _IGESGeom_BoundedSurface_HeaderFile
