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

#ifndef _IGESGeom_Flash_HeaderFile
#define _IGESGeom_Flash_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;
class gp_Pnt;

//! defines IGESFlash, Type <125> Form <0 - 4>
//! in package IGESGeom
//! A flash entity is a point in the ZT=0 plane that locates
//! a particular closed area. That closed area can be defined
//! in one of two ways. First, it can be an arbitrary closed
//! area defined by any entity capable of defining a closed
//! area. The points of this entity must all lie in the ZT=0
//! plane. Second, it can be a member of a predefined set of
//! flash shapes.
class IGESGeom_Flash : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_Flash();

  //! This method is used to set the fields of the class Flash
  //! - aPoint     : Reference of flash
  //! - aDim       : First flash sizing parameter
  //! - anotherDim : Second flash sizing parameter
  //! - aRotation  : Rotation of flash about reference point
  //! in radians
  //! - aReference : Pointer to the referenced entity or Null
  Standard_EXPORT void Init(const gp_XY&                       aPoint,
                            const double                aDim,
                            const double                anotherDim,
                            const double                aRotation,
                            const occ::handle<IGESData_IGESEntity>& aReference);

  //! Changes FormNumber (indicates the Nature of the Flash :
  //! 0 Unspecified, then given by Reference, 1->4 various
  //! Specialisations (Circle,Rectangle, etc...) )
  //! Error if not in range [0-4]
  Standard_EXPORT void SetFormNumber(const int form);

  //! returns the referenced point, Z = 0 always
  Standard_EXPORT gp_Pnt2d ReferencePoint() const;

  //! returns the referenced point after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedReferencePoint() const;

  //! returns first flash sizing parameter
  Standard_EXPORT double Dimension1() const;

  //! returns second flash sizing parameter
  Standard_EXPORT double Dimension2() const;

  //! returns the angle in radians of the rotation of flash about the
  //! reference point
  Standard_EXPORT double Rotation() const;

  //! returns the referenced entity or Null handle.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> ReferenceEntity() const;

  //! returns True if referenced entity is present.
  Standard_EXPORT bool HasReferenceEntity() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_Flash, IGESData_IGESEntity)

private:
  gp_XY                       thePoint;
  double               theDim1;
  double               theDim2;
  double               theRotation;
  occ::handle<IGESData_IGESEntity> theReference;
};

#endif // _IGESGeom_Flash_HeaderFile
