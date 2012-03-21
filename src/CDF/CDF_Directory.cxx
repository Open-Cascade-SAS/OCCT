// Created on: 1997-08-07
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <CDF_Directory.ixx>
#include <CDM_ListIteratorOfListOfDocument.hxx>
#include <Standard_NoSuchObject.hxx>
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
