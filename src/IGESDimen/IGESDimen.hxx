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
class IGESDimen_CenterLine;
class IGESDimen_Section;
class IGESDimen_WitnessLine;
class IGESDimen_AngularDimension;
class IGESDimen_CurveDimension;
class IGESDimen_DiameterDimension;
class IGESDimen_FlagNote;
class IGESDimen_GeneralLabel;
class IGESDimen_GeneralNote;
class IGESDimen_NewGeneralNote;
class IGESDimen_LeaderArrow;
class IGESDimen_LinearDimension;
class IGESDimen_OrdinateDimension;
class IGESDimen_PointDimension;
class IGESDimen_RadiusDimension;
class IGESDimen_GeneralSymbol;
class IGESDimen_SectionedArea;
class IGESDimen_DimensionedGeometry;
class IGESDimen_NewDimensionedGeometry;
class IGESDimen_DimensionUnits;
class IGESDimen_DimensionTolerance;
class IGESDimen_DimensionDisplayData;
class IGESDimen_BasicDimension;
class IGESDimen_ToolCenterLine;
class IGESDimen_ToolSection;
class IGESDimen_ToolWitnessLine;
class IGESDimen_ToolAngularDimension;
class IGESDimen_ToolCurveDimension;
class IGESDimen_ToolDiameterDimension;
class IGESDimen_ToolFlagNote;
class IGESDimen_ToolGeneralLabel;
class IGESDimen_ToolGeneralNote;
class IGESDimen_ToolNewGeneralNote;
class IGESDimen_ToolLeaderArrow;
class IGESDimen_ToolLinearDimension;
class IGESDimen_ToolOrdinateDimension;
class IGESDimen_ToolPointDimension;
class IGESDimen_ToolRadiusDimension;
class IGESDimen_ToolGeneralSymbol;
class IGESDimen_ToolSectionedArea;
class IGESDimen_ToolDimensionedGeometry;
class IGESDimen_ToolNewDimensionedGeometry;
class IGESDimen_ToolDimensionUnits;
class IGESDimen_ToolDimensionTolerance;
class IGESDimen_ToolDimensionDisplayData;
class IGESDimen_ToolBasicDimension;
class IGESDimen_Protocol;
class IGESDimen_ReadWriteModule;
class IGESDimen_GeneralModule;
class IGESDimen_SpecificModule;


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
