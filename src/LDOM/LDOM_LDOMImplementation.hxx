// File:      LDOM_LDOMImplementation.hxx
// Created:   28.06.01 09:30:17
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_LDOMImplementation_HeaderFile
#define LDOM_LDOMImplementation_HeaderFile

#include <Standard_Type.hxx>
#include <LDOM_Document.hxx>

class LDOM_DocumentType;

//  Block of comments describing class LDOM_LDOMImplementation
//

class LDOM_LDOMImplementation 
{
 public:
  // ---------- PUBLIC METHODS ----------

  static Standard_EXPORT LDOM_Document  createDocument
                                       (const LDOMString&        aNamespaceURI,
                                        const LDOMString&        aQualifiedName,
                                        const LDOM_DocumentType& aDocType);

};

#endif
