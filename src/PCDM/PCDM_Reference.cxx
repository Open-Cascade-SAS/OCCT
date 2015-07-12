// Created on: 1997-12-09
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


#include <PCDM_Reference.hxx>
#include <TCollection_ExtendedString.hxx>

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
