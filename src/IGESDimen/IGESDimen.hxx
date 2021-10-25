// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDimen_HeaderFile
#define _IGESDimen_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESDimen_Protocol;


//! This package represents Entities applied to Dimensions
//! ie. Annotation Entities and attached Properties and
//! Associativities.
class IGESDimen 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESDimen_Protocol) Protocol();




protected:





private:




friend class IGESDimen_CenterLine;
friend class IGESDimen_Section;
friend class IGESDimen_WitnessLine;
friend class IGESDimen_AngularDimension;
friend class IGESDimen_CurveDimension;
friend class IGESDimen_DiameterDimension;
friend class IGESDimen_FlagNote;
friend class IGESDimen_GeneralLabel;
friend class IGESDimen_GeneralNote;
friend class IGESDimen_NewGeneralNote;
friend class IGESDimen_LeaderArrow;
friend class IGESDimen_LinearDimension;
friend class IGESDimen_OrdinateDimension;
friend class IGESDimen_PointDimension;
friend class IGESDimen_RadiusDimension;
friend class IGESDimen_GeneralSymbol;
friend class IGESDimen_SectionedArea;
friend class IGESDimen_DimensionedGeometry;
friend class IGESDimen_NewDimensionedGeometry;
friend class IGESDimen_DimensionUnits;
friend class IGESDimen_DimensionTolerance;
friend class IGESDimen_DimensionDisplayData;
friend class IGESDimen_BasicDimension;
friend class IGESDimen_ToolCenterLine;
friend class IGESDimen_ToolSection;
friend class IGESDimen_ToolWitnessLine;
friend class IGESDimen_ToolAngularDimension;
friend class IGESDimen_ToolCurveDimension;
friend class IGESDimen_ToolDiameterDimension;
friend class IGESDimen_ToolFlagNote;
friend class IGESDimen_ToolGeneralLabel;
friend class IGESDimen_ToolGeneralNote;
friend class IGESDimen_ToolNewGeneralNote;
friend class IGESDimen_ToolLeaderArrow;
friend class IGESDimen_ToolLinearDimension;
friend class IGESDimen_ToolOrdinateDimension;
friend class IGESDimen_ToolPointDimension;
friend class IGESDimen_ToolRadiusDimension;
friend class IGESDimen_ToolGeneralSymbol;
friend class IGESDimen_ToolSectionedArea;
friend class IGESDimen_ToolDimensionedGeometry;
friend class IGESDimen_ToolNewDimensionedGeometry;
friend class IGESDimen_ToolDimensionUnits;
friend class IGESDimen_ToolDimensionTolerance;
friend class IGESDimen_ToolDimensionDisplayData;
friend class IGESDimen_ToolBasicDimension;
friend class IGESDimen_Protocol;
friend class IGESDimen_ReadWriteModule;
friend class IGESDimen_GeneralModule;
friend class IGESDimen_SpecificModule;

};







#endif // _IGESDimen_HeaderFile
