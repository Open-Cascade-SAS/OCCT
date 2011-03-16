// File:	CDF_DirectoryIterator.cxx
// Created:	Wed Aug 13 13:55:19 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <CDF_DirectoryIterator.ixx>
#include <CDF_Session.hxx>
CDF_DirectoryIterator::CDF_DirectoryIterator():myIterator(CDF_Session::CurrentSession()->Directory()->List()) {}



CDF_DirectoryIterator::CDF_DirectoryIterator(const Handle(CDF_Directory)& aDirectory):myIterator(aDirectory->List()) {}




Standard_Boolean CDF_DirectoryIterator::MoreDocument() {
  return myIterator.More() ;
}
void CDF_DirectoryIterator::NextDocument() {
  myIterator.Next();
}

Handle(CDM_Document) CDF_DirectoryIterator::Document() {

  return myIterator.Value();
}

