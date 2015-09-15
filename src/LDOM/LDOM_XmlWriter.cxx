// Created on: 2001-06-28
// Created by: Alexander GRIGORIEV
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

#include <LDOM_XmlWriter.hxx>
#include <LDOM_Document.hxx>
#include <LDOM_CharReference.hxx>

#define chOpenAngle     '<'
#define chCloseAngle    '>'
#define chOpenSquare    '['
#define chCloseSquare   ']'
#define chQuestion      '?'
#define chForwardSlash  '/'
#define chLF            '\n'
#define chNull          '\0'
#define chEqual         '='
#define chDash          '-'
#define chBang          '!'
#define chSpace         ' '
#define chDoubleQuote   '\"'
#define chZero          '0'
#define chOne           '1'
#define chTwo           '2'
#define chThree         '3'
#define chFour          '4'
#define chFive          '5'
#define chSix           '6'
#define chSeven         '7'
#define chEight         '8'
#define chNine          '9'
#define chLatin_a       'a'
#define chLatin_b       'b'
#define chLatin_c       'c'
#define chLatin_d       'd'
#define chLatin_e       'e'
#define chLatin_f       'f'
#define chLatin_g       'g'
#define chLatin_h       'h'
#define chLatin_i       'i'
#define chLatin_j       'j'
#define chLatin_k       'k'
#define chLatin_l       'l'
#define chLatin_m       'm'
#define chLatin_n       'n'
#define chLatin_o       'o'
#define chLatin_p       'p'
#define chLatin_q       'q'
#define chLatin_r       'r'
#define chLatin_s       's'
#define chLatin_t       't'
#define chLatin_u       'u'
#define chLatin_v       'v'
#define chLatin_w       'w'
#define chLatin_x       'x'
#define chLatin_y       'y'
#define chLatin_z       'z'
#define chLatin_A       'A'
#define chLatin_B       'B'
#define chLatin_C       'C'
#define chLatin_D       'D'
#define chLatin_E       'E'
#define chLatin_F       'F'
#define chLatin_G       'G'
#define chLatin_H       'H'
#define chLatin_I       'I'
#define chLatin_J       'J'
#define chLatin_K       'K'
#define chLatin_L       'L'
#define chLatin_M       'M'
#define chLatin_N       'N'
#define chLatin_O       'O'
#define chLatin_P       'P'
#define chLatin_Q       'Q'
#define chLatin_R       'R'
#define chLatin_S       'S'
#define chLatin_T       'T'
#define chLatin_U       'U'
#define chLatin_V       'V'
#define chLatin_W       'W'
#define chLatin_X       'X'
#define chLatin_Y       'Y'
#define chLatin_Z       'Z'

static const LXMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const LXMLCh  gEndElement1[]= { chForwardSlash, chNull };
//static const LXMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull };
//static const LXMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const LXMLCh  gXMLDecl1[] =
{       chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const LXMLCh  gXMLDecl2[] =
{       chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
/*
static const LXMLCh  gXMLDecl3[] =
{       chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
*/
static const LXMLCh  gXMLDecl4[] =
{       chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};
static const LXMLCh  gStartCDATA[] =
{       chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};
static const LXMLCh  gEndCDATA[] =
{    chCloseSquare, chCloseSquare, chCloseAngle, chNull };
static const LXMLCh  gStartComment[] =
{    chOpenAngle, chBang, chDash, chDash, chNull };
static const LXMLCh  gEndComment[] =
{    chDash, chDash, chCloseAngle, chNull };
/*
static const LXMLCh  gStartDoctype[] =
{   chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const LXMLCh  gPublic[] =
{   chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const LXMLCh  gSystem[] =
{   chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const LXMLCh  gStartEntity[] =
{   chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const LXMLCh  gNotation[] =
{   chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};
*/

static LXMLCh * getEncodingName (const LXMLCh * theEncodingName)
{
  const LXMLCh * anEncoding = theEncodingName;
  if (theEncodingName == NULL)
  {
//  anEncoding =           // US-ASCII
//  { chLatin_U, chLatin_S, chDash, chLatin_A, chLatin_S, chLatin_C, chLatin_I,
//      chLatin_I, chNull };
    static const LXMLCh anUTFEncoding [] =   // UTF-8
      { chLatin_U, chLatin_T, chLatin_F, chDash, chEight, chNull };
    anEncoding = anUTFEncoding;
  }
  Standard_Integer aLen = 0;
  while (anEncoding[aLen++] != chNull);
  LXMLCh * aResult = new LXMLCh [aLen];
  memcpy (aResult, anEncoding, aLen * sizeof (LXMLCh));
  return aResult;
}

//=======================================================================
//function : LH3D_LXMLWriter()
//purpose  : Constructor
//=======================================================================
LDOM_XmlWriter::LDOM_XmlWriter (FILE            * aFile,
                                const LXMLCh    * theEncoding)
     : myFile         (aFile),
       myEncodingName (::getEncodingName (theEncoding)),
       myIndent       (0),
       myCurIndent    (0),
       myABuffer      (NULL),
       myABufferLen   (0)
{}

//=======================================================================
//function : ~LDOM_XmlWriter
//purpose  : Destructor
//=======================================================================

LDOM_XmlWriter::~LDOM_XmlWriter ()
{
  delete [] myEncodingName;
  if (myABuffer != NULL) delete [] myABuffer;
}

//=======================================================================
//function : operator <<
//purpose  : 
//=======================================================================

LDOM_XmlWriter& LDOM_XmlWriter::operator <<     (const LDOM_Document& aDoc)
{
  const char * anXMLversion = "1.0";
  * this << gXMLDecl1 << anXMLversion
    << gXMLDecl2 << myEncodingName << gXMLDecl4;

  return (* this << aDoc.getDocumentElement());
}

//=======================================================================
//function : operator <<
//purpose  : Stream out an LDOMString
//=======================================================================

inline LDOM_XmlWriter& LDOM_XmlWriter::operator <<
                                        (const LDOMBasicString& aString)
{
  switch (aString.Type()) {
  case LDOMBasicString::LDOM_Integer:
    {
      Standard_Integer aValue;
      aString.GetInteger (aValue);
      fprintf (myFile, "%d", aValue);
      break;
    }
  case LDOMBasicString::LDOM_AsciiHashed:       // attr names and element tags
  case LDOMBasicString::LDOM_AsciiDocClear:
    {
      const char * str = aString.GetString();
      if (str) {
        const Standard_Size aLen = strlen (str);
        if (aLen > 0) fwrite (str, aLen, 1, myFile);
      }
    }
    break;
  case LDOMBasicString::LDOM_AsciiFree:
  case LDOMBasicString::LDOM_AsciiDoc:
    {
      const char * str = aString.GetString();
      if (str) {
        Standard_Integer aLen;
        char * encStr = LDOM_CharReference::Encode(str, aLen, Standard_False);
        if (aLen > 0) fwrite (encStr, aLen, 1, myFile);
        if (encStr != str) delete [] encStr;
      }
    }
  default: ;
  }
  return * this;
}

//=======================================================================
//function : operator<<()
//purpose  : Stream out a char *.
//=======================================================================
inline LDOM_XmlWriter& LDOM_XmlWriter::operator << (const LXMLCh * aString)
{
  Standard_Size aLength = strlen (aString);
  if (aLength > 0) fwrite ((void *) aString, aLength, 1, myFile);
  return * this;
}

//=======================================================================
//function : operator<<()
//purpose  : Stream out a character.
//=======================================================================
inline LDOM_XmlWriter& LDOM_XmlWriter::operator << (const LXMLCh aChar)
{
  fputc (aChar, myFile);
  return * this;
}

//=======================================================================
//function : WriteAttribute()
//purpose  : Stream out an XML attribute.
//=======================================================================
void LDOM_XmlWriter::WriteAttribute (const LDOM_Node& theAtt)
{
  int        aLength;
  const char * aName = theAtt.getNodeName().GetString();
  const LDOMString aValueStr = theAtt.getNodeValue();

  //    Integer attribute value
  if (aValueStr.Type() == LDOMBasicString::LDOM_Integer) {
    Standard_Integer anIntValue;
    aValueStr.GetInteger (anIntValue);
    aLength = (Standard_Integer) (20 + strlen (aName));
    if (aLength > myABufferLen) {
      if (myABuffer != NULL) delete [] myABuffer;
      myABuffer    = new char [aLength+1];
      myABufferLen = aLength;
    }
    sprintf (myABuffer, "%c%s%c%c%d%c", chSpace, aName,
             chEqual, chDoubleQuote, anIntValue, chDoubleQuote);
    aLength = (Standard_Integer) strlen (myABuffer);

  //    String attribute value
  } else {
    const char  * aValue = aValueStr.GetString();
    char        * encStr;
    if (aValueStr.Type() == LDOMBasicString::LDOM_AsciiDocClear) {
      encStr  = (char *) aValue;
      aLength = (Standard_Integer) (4 + strlen (aValue) + strlen (aName));
    } else {
      encStr = LDOM_CharReference::Encode (aValue, aLength, Standard_True);
      aLength += (Standard_Integer) (4 + strlen (aName));
    }
    if (aLength > myABufferLen) {
      if (myABuffer != NULL) delete [] myABuffer;
      myABuffer    = new char [aLength+1];
      myABufferLen = aLength;
    }
    sprintf (myABuffer, "%c%s%c%c%s%c", chSpace, aName,
             chEqual, chDoubleQuote, encStr, chDoubleQuote);
    if (encStr != aValue) delete [] encStr;
  }
  fwrite ((void *) myABuffer, aLength, 1, myFile);
}

//=======================================================================
//function : operator<<()
//purpose  : Stream out a DOM node, and, recursively, all of its children.
//           This function is the heart of writing a DOM tree out as XML source.
//           Give it a document node and it will do the whole thing.
//=======================================================================
LDOM_XmlWriter& LDOM_XmlWriter::operator<<     (const LDOM_Node& theNodeToWrite)
{
  // Get the name and value out for convenience
  LDOMString   aNodeName  = theNodeToWrite.getNodeName();
  LDOMString   aNodeValue = theNodeToWrite.getNodeValue();
//  unsigned long dwLent = aNodeValue.length();

  switch (theNodeToWrite.getNodeType()) 
  {
  case LDOM_Node::TEXT_NODE : 
    * this << aNodeValue;
    break;
  case LDOM_Node::ELEMENT_NODE : 
    {
      const int aMaxNSpaces    = 40;
      static LXMLCh aSpaces [] = {
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chOpenAngle, chNull };
      const LXMLCh * anIndentString = &aSpaces [aMaxNSpaces -  myCurIndent];
      if (anIndentString < &aSpaces[0]) anIndentString = &aSpaces[0];

      // Output the element start tag.
      * this << anIndentString << aNodeName.GetString();

        // Output any attributes of this element
      const LDOM_Element& anElemToWrite = (const LDOM_Element&) theNodeToWrite;
      LDOM_NodeList aListAtt = anElemToWrite.GetAttributesList ();
      Standard_Integer aListInd = aListAtt.getLength();
      while (aListInd--) {
        LDOM_Node aChild = aListAtt.item (aListInd);
        WriteAttribute (aChild);
      }

      //  Test for the presence of children
      LDOM_Node aChild = theNodeToWrite.getFirstChild();
      if (aChild != 0) 
      {
        // There are children. Close start-tag, and output children.
        * this << chCloseAngle;
        if (aChild.getNodeType() == LDOM_Node::ELEMENT_NODE && myIndent > 0)
          * this << chLF;
        Standard_Boolean isChildElem = Standard_False;
        while( aChild != 0) 
        {
          isChildElem = (aChild.getNodeType() == LDOM_Node::ELEMENT_NODE);
          if (isChildElem)  myCurIndent += myIndent;
          *this << aChild;
          if (isChildElem)  myCurIndent -= myIndent;
          do aChild = aChild.getNextSibling();
          while (aChild.getNodeType() == LDOM_Node::ATTRIBUTE_NODE);
        }
        // Done with children.  Output the end tag.
        //
        if (isChildElem)
          * this << anIndentString
            << gEndElement1 << aNodeName.GetString() << chCloseAngle;
        else
          * this << gEndElement << aNodeName.GetString() << chCloseAngle;
      }
      else
      {
        //  There were no children. Output the short form close of
        //  the element start tag, making it an empty-element tag.
        * this << chForwardSlash << chCloseAngle;
      }
      if (myIndent > 0)
        * this << chLF;
      break;
    }
    case LDOM_Node::CDATA_SECTION_NODE: 
    {
      * this << gStartCDATA << aNodeValue << gEndCDATA;
      break;
    }
    case LDOM_Node::COMMENT_NODE: 
    {
      * this << gStartComment << aNodeValue << gEndComment;
      break;
    }
  default:
#ifndef _MSC_VER
      cerr << "Unrecognized node type = "
        << (long)theNodeToWrite.getNodeType() << endl
#endif
  ; }
  return *this;
}
