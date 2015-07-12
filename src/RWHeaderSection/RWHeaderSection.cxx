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


#include <HeaderSection.hxx>
#include <HeaderSection_Protocol.hxx>
#include <RWHeaderSection.hxx>
#include <RWHeaderSection_GeneralModule.hxx>
#include <RWHeaderSection_ReadWriteModule.hxx>
#include <StepData.hxx>

/// #include <EuclidStandard.hxx>
static Handle(RWHeaderSection_ReadWriteModule) rwm;
static Handle(RWHeaderSection_GeneralModule) rwg;


void RWHeaderSection::Init()
{
///   EuclidStandard::Init();
  Handle(HeaderSection_Protocol) proto = HeaderSection::Protocol();
  StepData::AddHeaderProtocol(proto);
  if (rwm.IsNull()) rwm = new RWHeaderSection_ReadWriteModule;
  if (rwg.IsNull()) rwg = new RWHeaderSection_GeneralModule;
}


