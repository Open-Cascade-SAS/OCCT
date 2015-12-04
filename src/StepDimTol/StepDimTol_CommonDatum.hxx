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

#ifndef _StepDimTol_CommonDatum_HeaderFile
#define _StepDimTol_CommonDatum_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepData_Logical.hxx>
class StepDimTol_Datum;
class TCollection_HAsciiString;
class StepRepr_ProductDefinitionShape;


class StepDimTol_CommonDatum;
DEFINE_STANDARD_HANDLE(StepDimTol_CommonDatum, StepRepr_CompositeShapeAspect)

//! Representation of STEP entity CommonDatum
class StepDimTol_CommonDatum : public StepRepr_CompositeShapeAspect
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_CommonDatum();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theShapeAspect_Name,
                            const Handle(TCollection_HAsciiString)& theShapeAspect_Description,
                            const Handle(StepRepr_ProductDefinitionShape)& theShapeAspect_OfShape,
                            const StepData_Logical                  theShapeAspect_ProductDefinitional,
                            const Handle(TCollection_HAsciiString)& theDatum_Name,
                            const Handle(TCollection_HAsciiString)& theDatum_Description,
                            const Handle(StepRepr_ProductDefinitionShape)& theDatum_OfShape,
                            const StepData_Logical                  theDatum_ProductDefinitional,
                            const Handle(TCollection_HAsciiString)& theDatum_Identification);
  
  //! Returns data for supertype Datum
  Standard_EXPORT Handle(StepDimTol_Datum) Datum() const;
  
  //! Set data for supertype Datum
  Standard_EXPORT void SetDatum (const Handle(StepDimTol_Datum)& theDatum);




  DEFINE_STANDARD_RTTIEXT(StepDimTol_CommonDatum,StepRepr_CompositeShapeAspect)

protected:




private:


  Handle(StepDimTol_Datum) myDatum;


};







#endif // _StepDimTol_CommonDatum_HeaderFile
