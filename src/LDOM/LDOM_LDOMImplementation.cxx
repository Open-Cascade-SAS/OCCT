// File:      LDOM_LDOMImplementation.cxx
// Created:   28.06.01 09:36:21
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001
// History:

#include <LDOM_LDOMImplementation.hxx>

//=======================================================================
//function : createDocument
//purpose  : 
//=======================================================================

LDOM_Document LDOM_LDOMImplementation::createDocument
                                      (const LDOMString&      /*aNamespaceURI*/,
                                       const LDOMString&        aQualifiedName,
                                       const LDOM_DocumentType& /*aDocType*/)
{
  return LDOM_Document::createDocument (aQualifiedName);
}

