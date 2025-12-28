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

#include <Plate_PinpointConstraint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
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

  Standard_EXPORT Plate_LinearXYZConstraint(
    const NCollection_Array1<Plate_PinpointConstraint>& thePPC,
    const NCollection_Array1<double>&                   theCoeff);

  Standard_EXPORT Plate_LinearXYZConstraint(
    const NCollection_Array1<Plate_PinpointConstraint>& thePPC,
    const NCollection_Array2<double>&                   theCoeff);

  Standard_EXPORT Plate_LinearXYZConstraint(const int ColLen, const int RowLen);

  const NCollection_Array1<Plate_PinpointConstraint>& GetPPC() const;

  const NCollection_Array2<double>& Coeff() const;

  //! Sets the PinPointConstraint of index Index to
  //! Value raise if Index is greater than the length of
  //! PPC or the Row length of coeff or lower than 1
  Standard_EXPORT void SetPPC(const int Index, const Plate_PinpointConstraint& Value);

  //! Sets the coeff of index (Row,Col) to Value
  //! raise if Row (respectively Col) is greater than the
  //! Row (respectively Column) length of coeff
  Standard_EXPORT void SetCoeff(const int Row, const int Col, const double Value);

private:
  occ::handle<NCollection_HArray1<Plate_PinpointConstraint>> myPPC;
  occ::handle<NCollection_HArray2<double>>                   myCoef;
};

#include <Plate_LinearXYZConstraint.lxx>

#endif // _Plate_LinearXYZConstraint_HeaderFile
