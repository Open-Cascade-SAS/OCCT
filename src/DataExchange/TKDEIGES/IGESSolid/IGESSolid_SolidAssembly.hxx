// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_SolidAssembly_HeaderFile
#define _IGESSolid_SolidAssembly_HeaderFile

#include <Standard.hxx>

#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Standard_Integer.hxx>
class IGESGeom_TransformationMatrix;

//! defines SolidAssembly, Type <184> Form <0>
//! in package IGESSolid
//! Solid assembly is a collection of items which possess a
//! shared fixed geometric relationship.
//!
//! From IGES-5.3, From 1 says that at least one item is a Brep
//! else all are Primitives, Boolean Trees, Solid Instances or
//! other Assemblies
class IGESSolid_SolidAssembly : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESSolid_SolidAssembly();

  //! This method is used to set the fields of the class
  //! SolidAssembly
  //! - allItems    : the collection of items
  //! - allMatrices : transformation matrices corresponding to each
  //! item
  //! raises exception if the length of allItems & allMatrices
  //! do not match
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allItems,
    const occ::handle<NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>>&
      allMatrices);

  //! Tells if at least one item is a Brep, from FormNumber
  Standard_EXPORT bool HasBrep() const;

  //! Sets or Unsets the status "HasBrep" from FormNumber
  //! Default is False
  Standard_EXPORT void SetBrep(const bool hasbrep);

  //! returns the number of items in the collection
  Standard_EXPORT int NbItems() const;

  //! returns the Index'th item
  //! raises exception if Index <= 0 or Index > NbItems()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Item(const int Index) const;

  //! returns the transformation matrix of the Index'th item
  //! raises exception if Index <= 0 or Index > NbItems()
  Standard_EXPORT occ::handle<IGESGeom_TransformationMatrix> TransfMatrix(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESSolid_SolidAssembly, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>           theItems;
  occ::handle<NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>> theMatrices;
};

#endif // _IGESSolid_SolidAssembly_HeaderFile
