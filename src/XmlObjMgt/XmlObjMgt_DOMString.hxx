#ifndef _XmlObjMgt_DOMString_HeaderFile
#define _XmlObjMgt_DOMString_HeaderFile

#include <LDOMString.hxx>

typedef LDOMString XmlObjMgt_DOMString;

#define IMPLEMENT_DOMSTRING(FnName, String)                     \
static const XmlObjMgt_DOMString& FnName () {                   \
  static const XmlObjMgt_DOMString aString (String);            \
  return aString;                                               \
}

#endif
