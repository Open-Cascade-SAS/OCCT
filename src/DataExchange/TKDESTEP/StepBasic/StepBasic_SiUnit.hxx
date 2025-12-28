// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_SiUnit_HeaderFile
#define _StepBasic_SiUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnitName.hxx>
#include <StepBasic_NamedUnit.hxx>
class StepBasic_DimensionalExponents;

class StepBasic_SiUnit : public StepBasic_NamedUnit
{

public:
  //! Returns a SiUnit
  Standard_EXPORT StepBasic_SiUnit();

  Standard_EXPORT void Init(const bool                 hasAprefix,
                            const StepBasic_SiPrefix   aPrefix,
                            const StepBasic_SiUnitName aName);

  Standard_EXPORT void SetPrefix(const StepBasic_SiPrefix aPrefix);

  Standard_EXPORT void UnSetPrefix();

  Standard_EXPORT StepBasic_SiPrefix Prefix() const;

  Standard_EXPORT bool HasPrefix() const;

  Standard_EXPORT void SetName(const StepBasic_SiUnitName aName);

  Standard_EXPORT StepBasic_SiUnitName Name() const;

  Standard_EXPORT virtual void SetDimensions(
    const occ::handle<StepBasic_DimensionalExponents>& aDimensions) override;

  Standard_EXPORT virtual occ::handle<StepBasic_DimensionalExponents> Dimensions() const override;

  DEFINE_STANDARD_RTTIEXT(StepBasic_SiUnit, StepBasic_NamedUnit)

private:
  StepBasic_SiPrefix   prefix;
  StepBasic_SiUnitName name;
  bool                 hasPrefix;
};

#endif // _StepBasic_SiUnit_HeaderFile
