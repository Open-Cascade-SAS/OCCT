// File:	PCDM_Reference.cxx
// Created:	Tue Dec  9 09:22:54 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <PCDM_Reference.ixx>

PCDM_Reference::PCDM_Reference(){}
PCDM_Reference::PCDM_Reference(const Standard_Integer aReferenceIdentifier, const TCollection_ExtendedString& aFileName, const Standard_Integer aDocumentVersion):myReferenceIdentifier(aReferenceIdentifier),myFileName(aFileName),myDocumentVersion(aDocumentVersion) {}


Standard_Integer PCDM_Reference::ReferenceIdentifier() const {
  return myReferenceIdentifier;
}

TCollection_ExtendedString PCDM_Reference::FileName() const {
  return myFileName;
}

Standard_Integer PCDM_Reference::DocumentVersion() const {
  return myDocumentVersion;
}
