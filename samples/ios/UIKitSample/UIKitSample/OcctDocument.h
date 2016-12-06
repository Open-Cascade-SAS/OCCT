// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef OcctDocument_h
#define OcctDocument_h

#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>

//! The document
class OcctDocument : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OcctDocument, Standard_Transient)
  
public:
  Standard_EXPORT OcctDocument();
  
  Standard_EXPORT virtual ~OcctDocument();
  
  Standard_EXPORT void InitDoc();
  
  Standard_EXPORT Handle(TDocStd_Document)& ChangeDocument() { return myOcafDoc; }
  Standard_EXPORT const Handle(TDocStd_Document)& Document() const { return myOcafDoc; }
  
private:
  Handle(XCAFApp_Application) myApp;
  Handle(TDocStd_Document) myOcafDoc;
};

DEFINE_STANDARD_HANDLE(OcctDocument, Standard_Transient)

#endif // OcctDocument_h
