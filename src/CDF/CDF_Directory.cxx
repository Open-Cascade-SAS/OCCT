// Created on: 1997-08-07
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


#include <CDF_Directory.hxx>
#include <CDF_DirectoryIterator.hxx>
#include <CDM_Document.hxx>
#include <CDM_ListIteratorOfListOfDocument.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_Directory,Standard_Transient)

CDF_Directory::CDF_Directory () {}

void CDF_Directory::Add(const Handle(CDM_Document)& aDocument) {
  if(!Contains(aDocument)) myDocuments.Append(aDocument);
}

void CDF_Directory::Remove(const Handle(CDM_Document)& aDocument) {

  CDM_ListIteratorOfListOfDocument it(myDocuments);
  
  Standard_Boolean found = Standard_False;
  for (; it.More() && !found;) {
    found = aDocument == it.Value();
    if(found) 
      myDocuments.Remove(it);
    else
      it.Next();
  }
}


Standard_Boolean CDF_Directory::Contains(const Handle(CDM_Document)& aDocument) const {

  CDM_ListIteratorOfListOfDocument it(myDocuments);
  Standard_Boolean found = Standard_False;
  for (; it.More() && !found; it.Next()) {
    found = aDocument == it.Value();
  }
  return found;
}


Standard_Integer CDF_Directory::Length() const {
  return myDocuments.Extent();
}

const CDM_ListOfDocument& CDF_Directory::List() const {

  return myDocuments;

}

Standard_Boolean CDF_Directory::IsEmpty() const {
  return myDocuments.IsEmpty();
}
Handle(CDM_Document) CDF_Directory::Last() {
  Standard_NoSuchObject_Raise_if(IsEmpty(),"CDF_Directory::Last: the directory does not contain any document");
  return myDocuments.Last();
}
