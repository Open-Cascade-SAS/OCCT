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

#ifndef _IGESDraw_Planar_HeaderFile
#define _IGESDraw_Planar_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESGeom_TransformationMatrix;


class IGESDraw_Planar;
DEFINE_STANDARD_HANDLE(IGESDraw_Planar, IGESData_IGESEntity)

//! defines IGESPlanar, Type <402> Form <16>
//! in package IGESDraw
//!
//! Indicates that a collection of entities is coplanar.The
//! entities may be geometric, annotative, and/or structural.
class IGESDraw_Planar : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDraw_Planar();
  
  //! This method is used to set the fields of the class Planar
  //! - nbMats                : Number of Transformation matrices
  //! - aTransformationMatrix : Pointer to the Transformation matrix
  //! - allEntities           : Pointers to the entities specified
  Standard_EXPORT void Init (const Standard_Integer nbMats, const Handle(IGESGeom_TransformationMatrix)& aTransformationMatrix, const Handle(IGESData_HArray1OfIGESEntity)& allEntities);
  
  //! returns the number of Transformation matrices in <me>
  Standard_EXPORT Standard_Integer NbMatrices() const;
  
  //! returns the number of Entities in the plane pointed to by this
  //! associativity
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! returns True if TransformationMatrix is Identity Matrix,
  //! i.e:- No Matrix defined.
  Standard_EXPORT Standard_Boolean IsIdentityMatrix() const;
  
  //! returns the Transformation matrix moving data from the XY plane
  //! into space or zero
  Standard_EXPORT Handle(IGESGeom_TransformationMatrix) TransformMatrix() const;
  
  //! returns the Entity on the specified plane, indicated by EntityIndex
  //! raises an exception if EntityIndex <= 0 or
  //! EntityIndex > NbEntities()
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity (const Standard_Integer EntityIndex) const;




  DEFINE_STANDARD_RTTIEXT(IGESDraw_Planar,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbMatrices;
  Handle(IGESGeom_TransformationMatrix) theTransformationMatrix;
  Handle(IGESData_HArray1OfIGESEntity) theEntities;


};







#endif // _IGESDraw_Planar_HeaderFile
