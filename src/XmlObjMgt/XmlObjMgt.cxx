// Created on: 2001-07-18
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

//AGV 150202: Add parameter isClearText to SetStringValue()

#include <LDOM_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>

#include <errno.h>
#include <stdio.h>
static const char aRefPrefix [] = "/document/label";
static const char aRefElem1  [] = "/label[@tag=";
static const char aRefElem2  [] = "]";

//=======================================================================
//function : IdString
//purpose  : return name of ID attribute to be used everywhere
//=======================================================================

const XmlObjMgt_DOMString& XmlObjMgt::IdString ()
{
  static const LDOMString aString ("id");
  return aString;
}

//=======================================================================
//function : SetStringValue
//purpose  : Add theData as the last child text node to theElement
//remark   : Set isClearText to True if only you guarantee that the string
//           does not contain '&', '<', '>', '\"', '\'', etc.
//=======================================================================

void XmlObjMgt::SetStringValue (XmlObjMgt_Element&         theElement,
                                const XmlObjMgt_DOMString& theData,
                                const Standard_Boolean     isClearText)
{
  XmlObjMgt_Document aDocument = theElement.getOwnerDocument();
  LDOM_Text aText = aDocument.createTextNode (theData);
  if (isClearText) aText.SetValueClear();
  theElement.appendChild (aText);
}

//=======================================================================
//function : GetStringValue
//purpose  : returns the first child text node
//=======================================================================

XmlObjMgt_DOMString XmlObjMgt::GetStringValue
                                        (const XmlObjMgt_Element& theElement)
{
  XmlObjMgt_DOMString aString;
  for (LDOM_Node aNode = theElement.getFirstChild();
       aNode != NULL;
       aNode = aNode.getNextSibling())
  {
    if (aNode.getNodeType() == LDOM_Node::TEXT_NODE) {
      aString = ((const LDOM_Text&)aNode).getData();
      break;
    }
  }
  return aString;
}

//=======================================================================
//function : SprintfExtStr
//purpose  : Converts theString to hex printable representation and put it
//         : to the out buffer
//=======================================================================
void SprintfExtStr(char * out, const TCollection_ExtendedString& theString) {
  unsigned short * p = (unsigned short *)theString.ToExtString();
  int len = theString.Length();
  int i = 0;
  unsigned short mask[4] = {0xf000,0x0f00,0x00f0,0x000f};
  while (len) {
    for(int j = 0,k=3; j<4; j++,k--) {
      unsigned short v = *(p+i) & mask[j];//x000
      v = (unsigned short)(v >> (4*k));
      if(v < 10)
	v |= 0x30;
      else
	v += 87;
      out[4*i+j] = (char)v;
    }
    i++;len--;
  }
  out[4*theString.Length()] = 0x00;
}
//=======================================================================
//function : SetExtendedString
//purpose  : Add text node to element and initialize it with string
//=======================================================================

Standard_Boolean XmlObjMgt::SetExtendedString
                                  (XmlObjMgt_Element&                theElement,
                                   const TCollection_ExtendedString& theString)
{
  TCollection_AsciiString anAString;
  if (theString.IsAscii()) {
    anAString = TCollection_AsciiString (theString, '?');
    SetStringValue (theElement, anAString.ToCString());
  } else {
    const Standard_Integer aLen = theString.Length();
//    const Standard_ExtCharacter * aString = theString.ToExtString();
    char * buf0 = new char [4 * (aLen + 1) + 3];
    Sprintf (&buf0[0], "##%04x", 0xfeff);          // set UNICODE header
    char * buf = &buf0[6];
//     Standard_Integer i = 0;
//     while (i <= (aLen - 4)) {
//       Sprintf (&buf[i*4], "%04x%04x%04x%04x", aString[i], aString[i+1],
//                aString[i+2], aString[i+3]);
//         i += 4;
//     }
//     while (i < aLen) {
//       Sprintf (&buf[i*4], "%04x", aString[i]);
//       ++i;
//     }
//     buf[4*aLen] = '\0';

    SprintfExtStr(buf, theString);  
    SetStringValue (theElement, buf0);
    delete [] buf0;
  }
  return Standard_True;
}

//=======================================================================
//function : GetExtendedString
//purpose  : Get the first text node in theElement and convert to ExtendedStr
//=======================================================================

Standard_Boolean XmlObjMgt::GetExtendedString
                                (const XmlObjMgt_Element&       theElement,
                                 TCollection_ExtendedString&    theString)
{
  theString = GetStringValue (theElement);
  return Standard_True;
}

//=======================================================================
//function : GetTagEntryString
//purpose  : Convert XPath expression (DOMString) into TagEntry string
//           Returns False on error
//=======================================================================

Standard_Boolean XmlObjMgt::GetTagEntryString
                                        (const XmlObjMgt_DOMString& theSource,
                                         TCollection_AsciiString&   theTagEntry)
{
  //    Check the prefix
  const size_t aPrefixSize = sizeof(aRefPrefix) - 1;
  const char * aSource = theSource.GetString();
  if (strncmp (aSource, aRefPrefix, aPrefixSize))
    return Standard_False;

  //    Beging aTagEntry string
  char * aTagEntry =
    (char *) Standard::Allocate (strlen(aSource)/2);  // quite enough to hold it
  char * aTagEntryPtr = aTagEntry + 1;
  * aTagEntry = '0';
  aSource += aPrefixSize;

  //    Find all individual tags in a loop
  const size_t anElem1Size = sizeof(aRefElem1) - 1;
  const size_t anElem2Size = sizeof(aRefElem2) - 1;
  while (aSource[0] != '\0') {
    //  Check the first part of individual tag: "/label[@tag="
    if (strncmp (aSource, aRefElem1, anElem1Size))
      return Standard_False;
    aSource += anElem1Size;
    const char aQuote = aSource[0];
    if (aQuote != '\'' && aQuote != '\"')
      return Standard_False;

    //  Check the integer value of the tag
    errno = 0;
    char * aPtr;
    long aTagValue = strtol (&aSource[1], &aPtr, 10);
    Standard_Integer aLen = (Standard_Integer)(aPtr - &aSource[1]);
    if (aTagValue < 0 || aLen == 0 || aPtr[0] != aQuote ||
        errno == ERANGE || errno == EINVAL)
      return Standard_False;
    aTagEntryPtr[0] = ':';
    memcpy (&aTagEntryPtr[1], &aSource[1], aLen);
    aTagEntryPtr += (aLen + 1);

    //  Check the final part of individual tag : "]"
    if (strncmp (aPtr + 1, aRefElem2, anElem2Size))
      return Standard_False;
    aSource = aPtr + 1 + anElem2Size;
  }
  aTagEntryPtr[0] = '\0';
  theTagEntry = aTagEntry;
  Standard::Free (aTagEntry);
  return Standard_True;
}

//=======================================================================
//function : SetTagEntryString
//purpose  : Form an XPath string corresponding to the input TagEntry
//=======================================================================

void XmlObjMgt::SetTagEntryString (XmlObjMgt_DOMString&           theTarget,
                                   const TCollection_AsciiString& theTagEntry)
{
  //    Begin parsing theTagEntry
  const char * aTagEntry = (const char*) theTagEntry.ToCString() + 1;
  if (aTagEntry[-1] != '0')
    return;

  //    Count the number of tags in the label entry string
  const char * aPtr = aTagEntry;
  Standard_Integer aTagCount = 0;
  while (* aPtr) if (* aPtr++ == ':') aTagCount ++;

  //    Create a buffer to accumulate the XPath reference
  const size_t anElem1Size = sizeof(aRefElem1) - 1;
  const size_t anElem2Size = sizeof(aRefElem2) - 1;
  char * aTarget =
    (char *) Standard::Allocate (sizeof(aRefPrefix) + aTagCount *
                                 (anElem1Size + anElem2Size + 12));
  memcpy (aTarget, aRefPrefix, sizeof (aRefPrefix) - 1);
  char * aTargetPtr = aTarget + (sizeof (aRefPrefix) - 1);

  for(;;) {
    //  Check for the end-of-string; find the delimeter ':'
    aPtr = strchr (aTagEntry, ':');
    if (aPtr == NULL) break;
    aTagEntry = aPtr + 1;

    //  Find the range of characters for an integer number
    errno = 0;
    char * ptr;
    long aTagValue = strtol (aTagEntry, &ptr, 10);
    Standard_Integer aTagSize = (Standard_Integer)(ptr - aTagEntry);
    if (aTagValue < 0 || aTagSize == 0 ||
        errno == ERANGE || errno == EINVAL)
      return;           // error

    //  Add one XPath level to the expression in aTarget
    memcpy (&aTargetPtr[0],                     aRefElem1, anElem1Size);
    aTargetPtr[anElem1Size] = '\"';
    memcpy (&aTargetPtr[anElem1Size+1],         aTagEntry, aTagSize);
    aTargetPtr[anElem1Size+aTagSize+1] = '\"';
    memcpy (&aTargetPtr[anElem1Size+aTagSize+2],aRefElem2, anElem2Size);
    aTargetPtr += (anElem1Size + aTagSize + anElem2Size + 2);
  }
  * aTargetPtr = '\0';
  theTarget = aTarget;
  Standard::Free (aTarget);
}

//=======================================================================
//function : FindChildElement
//purpose  : 
//=======================================================================
XmlObjMgt_Element XmlObjMgt::FindChildElement
                                         (const XmlObjMgt_Element& theSource,
                                          const Standard_Integer   theId)
{
  LDOM_Node aNode = theSource.getFirstChild();
  Standard_Integer anId;
  while ( !aNode.isNull() )
  {
    if ( aNode.getNodeType() == LDOM_Node::ELEMENT_NODE )
    {
      LDOM_Element anElem = (LDOM_Element &) aNode;
      if (anElem.getAttribute (IdString()).GetInteger(anId))
        if (anId == theId) return anElem;
    }
    aNode = aNode.getNextSibling();
  }

  // find in all the document // to be done
//  LDOM_Document aDoc = theSource.getOwnerDocument();

  return LDOM_Element();
}

//=======================================================================
//function : FindChildByRef
//purpose  : 
//=======================================================================

XmlObjMgt_Element XmlObjMgt::FindChildByRef
                                       (const XmlObjMgt_Element&   theSource,
                                        const XmlObjMgt_DOMString& theRefName)
{
  Standard_Integer anID;
  if (theSource.getAttribute (theRefName).GetInteger (anID))
    return FindChildElement (theSource, anID);
  return LDOM_Element();
}


//=======================================================================
//function : FindChildByName
//purpose  : 
//=======================================================================
XmlObjMgt_Element XmlObjMgt::FindChildByName
                                         (const XmlObjMgt_Element&   theSource,
                                          const XmlObjMgt_DOMString& theName)
{
  return theSource.GetChildByTagName(theName);
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================
Standard_Boolean XmlObjMgt::GetInteger (Standard_CString& theString,
                                        Standard_Integer& theValue)
{
  char * ptr;
  errno = 0;
  long aValue = strtol (theString, &ptr, 10);
  if (ptr == theString || errno == ERANGE || errno == EINVAL)
    return Standard_False;
  theValue = Standard_Integer (aValue);
  theString = ptr;
  return Standard_True;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================
Standard_Boolean XmlObjMgt::GetReal (Standard_CString& theString,
                                     Standard_Real&    theValue)
{
  char * ptr;
  errno = 0;
  double aValue = Strtod (theString, &ptr);
  if (ptr == theString || errno == ERANGE || errno == EINVAL)
    return Standard_False;
  theValue = Standard_Real (aValue);
  theString = ptr;
  return Standard_True;
}

//=======================================================================
//function : GetReal
//purpose  : Convert LDOMString to Real
//=======================================================================
Standard_Boolean XmlObjMgt::GetReal (const XmlObjMgt_DOMString& theString,
                                     Standard_Real&             theValue)
{
  switch (theString.Type()) {
  case LDOMBasicString::LDOM_NULL:
    return Standard_False;
  case LDOMBasicString::LDOM_Integer:
    {
      Standard_Integer anIntValue;
      theString.GetInteger(anIntValue);
      theValue = Standard_Real(anIntValue);
      break;
    }
  default:      // LDOM_Ascii*
    {
      char       * ptr;
      const char * aString = theString.GetString();
      errno = 0;
      double aValue = Strtod (aString, &ptr);
      if (ptr == aString || errno == ERANGE || errno == EINVAL)
        return Standard_False;
      theValue = Standard_Real (aValue);
    }
  }
  return Standard_True;
}
