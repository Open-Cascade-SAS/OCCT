// Created on: 2004-06-29
// Created by: Eugeny NAPALKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

#include <AppStdL_Application.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AppStdL_Application, TDocStd_Application)

//=================================================================================================

Standard_CString AppStdL_Application::ResourcesName()
{
  const Standard_CString aRes = "StandardLite";
  return aRes;
}

//=================================================================================================

void AppStdL_Application::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDocStd_Application)
}
