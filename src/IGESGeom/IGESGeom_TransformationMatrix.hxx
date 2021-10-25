// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_TransformationMatrix_HeaderFile
#define _IGESGeom_TransformationMatrix_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray2OfReal.hxx>
#include <IGESData_TransfEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class gp_GTrsf;


class IGESGeom_TransformationMatrix;
DEFINE_STANDARD_HANDLE(IGESGeom_TransformationMatrix, IGESData_TransfEntity)

//! defines IGESTransformationMatrix, Type <124> Form <0>
//! in package IGESGeom
//! The transformation matrix entity transforms three-row column
//! vectors by means of matrix multiplication and then a vector
//! addition. This entity can be considered as an "operator"
//! entity in that it starts with the input vector, operates on
//! it as described above, and produces the output vector.
class IGESGeom_TransformationMatrix : public IGESData_TransfEntity
{

public:

  
  Standard_EXPORT IGESGeom_TransformationMatrix();
  
  //! This method is used to set the fields of the class
  //! TransformationMatrix
  //! - aMatrix : 3 x 4 array containing elements of the
  //! transformation matrix
  //! raises exception if aMatrix is not 3 x 4 array
  Standard_EXPORT void Init (const Handle(TColStd_HArray2OfReal)& aMatrix);
  
  //! Changes FormNumber (indicates the Type of Transf :
  //! Transformation 0-1 or Coordinate System 10-11-12)
  //! Error if not in ranges [0-1] or [10-12]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns individual Data
  //! Error if I not in [1-3] or J not in [1-4]
  Standard_EXPORT Standard_Real Data (const Standard_Integer I, const Standard_Integer J) const;
  
  //! returns the transformation matrix
  //! 4th row elements of GTrsf will always be 0, 0, 0, 1 (not defined)
  Standard_EXPORT gp_GTrsf Value() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_TransformationMatrix,IGESData_TransfEntity)

protected:




private:


  Handle(TColStd_HArray2OfReal) theData;


};







#endif // _IGESGeom_TransformationMatrix_HeaderFile
