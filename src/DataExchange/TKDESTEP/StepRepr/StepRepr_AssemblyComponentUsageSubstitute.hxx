// Created on: 1998-06-30
// Created by: Christian CAILLET
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

#ifndef _StepRepr_AssemblyComponentUsageSubstitute_HeaderFile
#define _StepRepr_AssemblyComponentUsageSubstitute_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_AssemblyComponentUsage;

class StepRepr_AssemblyComponentUsageSubstitute : public Standard_Transient
{

public:
  Standard_EXPORT StepRepr_AssemblyComponentUsageSubstitute();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&        aName,
                            const occ::handle<TCollection_HAsciiString>&        aDef,
                            const occ::handle<StepRepr_AssemblyComponentUsage>& aBase,
                            const occ::handle<StepRepr_AssemblyComponentUsage>& aSubs);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Definition() const;

  Standard_EXPORT void SetDefinition(const occ::handle<TCollection_HAsciiString>& aDef);

  Standard_EXPORT occ::handle<StepRepr_AssemblyComponentUsage> Base() const;

  Standard_EXPORT void SetBase(const occ::handle<StepRepr_AssemblyComponentUsage>& aBase);

  Standard_EXPORT occ::handle<StepRepr_AssemblyComponentUsage> Substitute() const;

  Standard_EXPORT void SetSubstitute(const occ::handle<StepRepr_AssemblyComponentUsage>& aSubstitute);

  DEFINE_STANDARD_RTTIEXT(StepRepr_AssemblyComponentUsageSubstitute, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>        theName;
  occ::handle<TCollection_HAsciiString>        theDef;
  occ::handle<StepRepr_AssemblyComponentUsage> theBase;
  occ::handle<StepRepr_AssemblyComponentUsage> theSubs;
};

#endif // _StepRepr_AssemblyComponentUsageSubstitute_HeaderFile
