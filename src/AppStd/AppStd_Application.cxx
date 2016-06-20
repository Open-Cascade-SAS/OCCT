// Created on: 2000-09-07
// Created by: TURIN Anatoliy
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

//AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

#include <AppStd_Application.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AppStd_Application,TDocStd_Application)

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString AppStd_Application::ResourcesName() {
  const Standard_CString aRes = "Standard";
  return aRes;
}

