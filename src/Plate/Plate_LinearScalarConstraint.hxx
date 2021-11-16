// Created on: 1998-03-23
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Plate_LinearScalarConstraint_HeaderFile
#define _Plate_LinearScalarConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Plate_HArray1OfPinpointConstraint.hxx>
#include <TColgp_HArray2OfXYZ.hxx>
#include <Plate_Array1OfPinpointConstraint.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array2OfXYZ.hxx>
#include <Standard_Integer.hxx>
class Plate_PinpointConstraint;
class gp_XYZ;


//! define on or several constraints  as linear combination of
//! the X,Y and Z components of a set of PinPointConstraint
class Plate_LinearScalarConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_LinearScalarConstraint();
  
  Standard_EXPORT Plate_LinearScalarConstraint(const Plate_PinpointConstraint& thePPC1, const gp_XYZ& theCoeff);
  
  Standard_EXPORT Plate_LinearScalarConstraint(const Plate_Array1OfPinpointConstraint& thePPC, const TColgp_Array1OfXYZ& theCoeff);
  
  Standard_EXPORT Plate_LinearScalarConstraint(const Plate_Array1OfPinpointConstraint& thePPC, const TColgp_Array2OfXYZ& theCoeff);
  
  Standard_EXPORT Plate_LinearScalarConstraint(const Standard_Integer ColLen, const Standard_Integer RowLen);
  
    const Plate_Array1OfPinpointConstraint& GetPPC() const;
  
    const TColgp_Array2OfXYZ& Coeff() const;
  
  //! Sets   the PinPointConstraint of   index Index to
  //! Value raise if Index is greater than the length of
  //! PPC or the Row length of coeff or lower  than 1
  Standard_EXPORT void SetPPC (const Standard_Integer Index, const Plate_PinpointConstraint& Value);
  
  //! Sets the coeff  of index (Row,Col)  to Value
  //! raise if  Row (respectively Col)  is greater than the
  //! Row (respectively Column) length of coeff
  Standard_EXPORT void SetCoeff (const Standard_Integer Row, const Standard_Integer Col, const gp_XYZ& Value);




protected:





private:



  Handle(Plate_HArray1OfPinpointConstraint) myPPC;
  Handle(TColgp_HArray2OfXYZ) myCoef;


};


#include <Plate_LinearScalarConstraint.lxx>





#endif // _Plate_LinearScalarConstraint_HeaderFile
