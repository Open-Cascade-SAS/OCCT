// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepDimTol_DatumReference_HeaderFile
#define _StepDimTol_DatumReference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class StepDimTol_Datum;

class StepDimTol_DatumReference;
DEFINE_STANDARD_HANDLE(StepDimTol_DatumReference, Standard_Transient)

//! Representation of STEP entity DatumReference
class StepDimTol_DatumReference : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumReference();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Standard_Integer          thePrecedence,
                            const Handle(StepDimTol_Datum)& theReferencedDatum);

  //! Returns field Precedence
  Standard_EXPORT Standard_Integer Precedence() const;

  //! Set field Precedence
  Standard_EXPORT void SetPrecedence(const Standard_Integer thePrecedence);

  //! Returns field ReferencedDatum
  Standard_EXPORT Handle(StepDimTol_Datum) ReferencedDatum() const;

  //! Set field ReferencedDatum
  Standard_EXPORT void SetReferencedDatum(const Handle(StepDimTol_Datum)& theReferencedDatum);

  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumReference, Standard_Transient)

protected:
private:
  Standard_Integer         myPrecedence;
  Handle(StepDimTol_Datum) myReferencedDatum;
};

#endif // _StepDimTol_DatumReference_HeaderFile
