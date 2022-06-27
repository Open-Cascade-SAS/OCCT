// Created on: 2000-05-24
// Created by: data exchange team
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

#ifndef _XCAFApp_Application_HeaderFile
#define _XCAFApp_Application_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDocStd_Application.hxx>
#include <Standard_CString.hxx>


class XCAFApp_Application;
DEFINE_STANDARD_HANDLE(XCAFApp_Application, TDocStd_Application)

//! Implements an Application for the DECAF documents
class XCAFApp_Application : public TDocStd_Application
{

public:

  //! methods from TDocStd_Application
  //! ================================
  Standard_EXPORT virtual Standard_CString ResourcesName() Standard_OVERRIDE;
  
  //! Set XCAFDoc_DocumentTool attribute
  Standard_EXPORT virtual void InitDocument (const Handle(CDM_Document)& aDoc) const Standard_OVERRIDE;
  
  //! Initializes (for the first time) and returns the
  //! static object (XCAFApp_Application)
  //! This is the only valid method to get XCAFApp_Application
  //! object, and it should be called at least once before
  //! any actions with documents in order to init application
  Standard_EXPORT static Handle(XCAFApp_Application) GetApplication();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(XCAFApp_Application,TDocStd_Application)

protected:
  
  Standard_EXPORT XCAFApp_Application();
};

#endif // _XCAFApp_Application_HeaderFile
