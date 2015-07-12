// Created on: 1997-08-04
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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


#include <CDM_Document.hxx>
#include <CDM_Reference.hxx>
#include <CDM_ReferenceIterator.hxx>

CDM_ReferenceIterator::CDM_ReferenceIterator(const Handle(CDM_Document)& aDocument):myIterator(aDocument->myToReferences){}

Standard_Boolean CDM_ReferenceIterator::More() const {
  return myIterator.More();
}

void CDM_ReferenceIterator::Next() {
  myIterator.Next();
}

Standard_Integer CDM_ReferenceIterator::ReferenceIdentifier() const{
  return myIterator.Value()->ReferenceIdentifier();
}

Handle(CDM_Document) CDM_ReferenceIterator::Document() const {
  return myIterator.Value()->ToDocument();
}


Standard_Integer CDM_ReferenceIterator::DocumentVersion() const {
  return myIterator.Value()->DocumentVersion();
}
