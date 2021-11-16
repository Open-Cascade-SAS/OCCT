// Created on: 1998-03-24
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

#ifndef _Plate_LinearXYZConstraint_HeaderFile
#define _Plate_LinearXYZConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Plate_HArray1OfPinpointConstraint.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Plate_Array1OfPinpointConstraint.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Plate_PinpointConstraint;


//! define on or several constraints as linear combination of
//! PinPointConstraint unlike the LinearScalarConstraint, usage
//! of this kind of constraint preserve the X,Y and Z uncoupling.
class Plate_LinearXYZConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_LinearXYZConstraint();
  
  Standard_EXPORT Plate_LinearXYZConstraint(const Plate_Array1OfPinpointConstraint& thePPC, const TColStd_Array1OfReal& theCoeff);
  
  Standard_EXPORT Plate_LinearXYZConstraint(const Plate_Array1OfPinpointConstraint& thePPC, const TColStd_Array2OfReal& theCoeff);
  
  Standard_EXPORT Plate_LinearXYZConstraint(const Standard_Integer ColLen, const Standard_Integer RowLen);
  
    const Plate_Array1OfPinpointConstraint& GetPPC() const;
  
    const TColStd_Array2OfReal& Coeff() const;
  
  //! Sets   the PinPointConstraint of   index Index to
  //! Value raise if Index is greater than the length of
  //! PPC or the Row length of coeff or lower  than 1
  Standard_EXPORT void SetPPC (const Standard_Integer Index, const Plate_PinpointConstraint& Value);
  
  //! Sets the coeff  of index (Row,Col)  to Value
  //! raise if  Row (respectively Col)  is greater than the
  //! Row (respectively Column) length of coeff
  Standard_EXPORT void SetCoeff (const Standard_Integer Row, const Standard_Integer Col, const Standard_Real Value);




protected:





private:



  Handle(Plate_HArray1OfPinpointConstraint) myPPC;
  Handle(TColStd_HArray2OfReal) myCoef;


};


#include <Plate_LinearXYZConstraint.lxx>





#endif // _Plate_LinearXYZConstraint_HeaderFile
