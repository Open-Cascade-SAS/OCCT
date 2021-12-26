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

#ifndef _AppStdL_Application_HeaderFile
#define _AppStdL_Application_HeaderFile

#include <Standard.hxx>

#include <TDocStd_Application.hxx>

class AppStdL_Application;
DEFINE_STANDARD_HANDLE(AppStdL_Application, TDocStd_Application)

//! Legacy class defining resources name for lite OCAF documents
class AppStdL_Application : public TDocStd_Application
{
public:
  //! returns   the file  name  which  contains  application
  //! resources
  Standard_EXPORT Standard_CString ResourcesName() Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(AppStdL_Application,TDocStd_Application)
};

#endif // _AppStdL_Application_HeaderFile
