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
#include <Standard_Type.hxx>

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESGeom_HArray1OfTransformationMatrix.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_DimensionMismatch;
class Standard_OutOfRange;
class IGESData_IGESEntity;
class IGESGeom_TransformationMatrix;


class IGESSolid_SolidAssembly;
DEFINE_STANDARD_HANDLE(IGESSolid_SolidAssembly, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Handle(IGESData_HArray1OfIGESEntity)& allItems, const Handle(IGESGeom_HArray1OfTransformationMatrix)& allMatrices);
  
  //! Tells if at least one item is a Brep, from FormNumber
  Standard_EXPORT Standard_Boolean HasBrep() const;
  
  //! Sets or Unsets the status "HasBrep" from FormNumber
  //! Default is False
  Standard_EXPORT void SetBrep (const Standard_Boolean hasbrep);
  
  //! returns the number of items in the collection
  Standard_EXPORT Standard_Integer NbItems() const;
  
  //! returns the Index'th item
  //! raises exception if Index <= 0 or Index > NbItems()
  Standard_EXPORT Handle(IGESData_IGESEntity) Item (const Standard_Integer Index) const;
  
  //! returns the transformation matrix of the Index'th item
  //! raises exception if Index <= 0 or Index > NbItems()
  Standard_EXPORT Handle(IGESGeom_TransformationMatrix) TransfMatrix (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SolidAssembly,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_HArray1OfIGESEntity) theItems;
  Handle(IGESGeom_HArray1OfTransformationMatrix) theMatrices;


};







#endif // _IGESSolid_SolidAssembly_HeaderFile
