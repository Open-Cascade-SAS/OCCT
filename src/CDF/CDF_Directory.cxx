// File:	CDF_Directory.cxx
// Created:	Thu Aug  7 17:05:01 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
