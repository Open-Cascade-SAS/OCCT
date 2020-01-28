// Created on: 2000-09-07
// Created by: TURIN  Anatoliy <ati@nnov.matra-dtv.fr>
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

#ifndef _AppStd_Application_HeaderFile
#define _AppStd_Application_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDocStd_Application.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Standard_CString.hxx>


class AppStd_Application;
DEFINE_STANDARD_HANDLE(AppStd_Application, TDocStd_Application)

//! Legacy class defining resources name for standard OCAF documents
class AppStd_Application : public TDocStd_Application
{
public:  
  //! returns   the file  name  which  contains  application
  //! resources
  Standard_EXPORT Standard_CString ResourcesName() Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(AppStd_Application,TDocStd_Application)
};

#endif // _AppStd_Application_HeaderFile
