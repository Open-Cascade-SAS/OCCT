// File:	CDM_ReferenceIterator.cxx
// Created:	Mon Aug  4 17:11:25 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <CDM_ReferenceIterator.ixx>
#include <CDM_Reference.hxx>
#include <CDM_Document.hxx>

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
