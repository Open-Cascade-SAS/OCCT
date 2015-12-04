// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_FeaModel_HeaderFile
#define _StepFEA_FeaModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfAsciiString.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;


class StepFEA_FeaModel;
DEFINE_STANDARD_HANDLE(StepFEA_FeaModel, StepRepr_Representation)

//! Representation of STEP entity FeaModel
class StepFEA_FeaModel : public StepRepr_Representation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaModel();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentation_Name, const Handle(StepRepr_HArray1OfRepresentationItem)& aRepresentation_Items, const Handle(StepRepr_RepresentationContext)& aRepresentation_ContextOfItems, const Handle(TCollection_HAsciiString)& aCreatingSoftware, const Handle(TColStd_HArray1OfAsciiString)& aIntendedAnalysisCode, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& aAnalysisType);
  
  //! Returns field CreatingSoftware
  Standard_EXPORT Handle(TCollection_HAsciiString) CreatingSoftware() const;
  
  //! Set field CreatingSoftware
  Standard_EXPORT void SetCreatingSoftware (const Handle(TCollection_HAsciiString)& CreatingSoftware);
  
  //! Returns field IntendedAnalysisCode
  Standard_EXPORT Handle(TColStd_HArray1OfAsciiString) IntendedAnalysisCode() const;
  
  //! Set field IntendedAnalysisCode
  Standard_EXPORT void SetIntendedAnalysisCode (const Handle(TColStd_HArray1OfAsciiString)& IntendedAnalysisCode);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns field AnalysisType
  Standard_EXPORT Handle(TCollection_HAsciiString) AnalysisType() const;
  
  //! Set field AnalysisType
  Standard_EXPORT void SetAnalysisType (const Handle(TCollection_HAsciiString)& AnalysisType);




  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaModel,StepRepr_Representation)

protected:




private:


  Handle(TCollection_HAsciiString) theCreatingSoftware;
  Handle(TColStd_HArray1OfAsciiString) theIntendedAnalysisCode;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(TCollection_HAsciiString) theAnalysisType;


};







#endif // _StepFEA_FeaModel_HeaderFile
