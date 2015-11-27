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

static const char  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const char  gEndElement1[]= { chForwardSlash, chNull };

static const char  gXMLDecl1[] =
{       chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const char  gXMLDecl2[] =
{       chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};

static const char  gXMLDecl4[] =
{       chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};
static const char  gStartCDATA[] =
{       chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};
static const char  gEndCDATA[] =
{    chCloseSquare, chCloseSquare, chCloseAngle, chNull };
static const char  gStartComment[] =
{    chOpenAngle, chBang, chDash, chDash, chNull };
static const char  gEndComment[] =
{    chDash, chDash, chCloseAngle, chNull };

static char* getEncodingName (const char* theEncodingName)
{
  const char* anEncoding = theEncodingName;
  if (theEncodingName == NULL)
  {
    static const char anUTFEncoding [] =  {chLatin_U, chLatin_T, chLatin_F, chDash, chEight, chNull};
    anEncoding = anUTFEncoding;
  }

  Standard_Integer aLen = 0;
  while (anEncoding[aLen++] != chNull);

  char * aResult = new char [aLen];
  memcpy (aResult, anEncoding, aLen * sizeof (char));
  
  return aResult;
}

//=======================================================================
//function : LDOM_XmlWriter
//purpose  : 
//=======================================================================
LDOM_XmlWriter::LDOM_XmlWriter (const char * theEncoding)
 : myEncodingName (::getEncodingName (theEncoding)),
   myIndent       (0),
   myCurIndent    (0),
   myABuffer      (NULL),
   myABufferLen   (0)
{
  ;
}

//=======================================================================
//function : ~LDOM_XmlWriter
//purpose  : Destructor
//=======================================================================
LDOM_XmlWriter::~LDOM_XmlWriter ()
{
  delete [] myEncodingName;

  if (myABuffer != NULL)
  {
    delete [] myABuffer;
  }
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
void LDOM_XmlWriter::Write (Standard_OStream& theOStream, const LDOM_Document& aDoc)
{
  Write (theOStream, gXMLDecl1);

  const char * anXMLversion = "1.0"; 
  Write (theOStream, anXMLversion);

  Write (theOStream, gXMLDecl2);
  Write (theOStream, myEncodingName);
  Write (theOStream, gXMLDecl4);

  Write (theOStream, aDoc.getDocumentElement());
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
void LDOM_XmlWriter::Write (Standard_OStream& theOStream, const LDOM_Node& theNode)
{
  // Get the name and value out for convenience
  LDOMString aNodeName  = theNode.getNodeName();
  LDOMString aNodeValue = theNode.getNodeValue();

  switch (theNode.getNodeType()) 
  {
    case LDOM_Node::TEXT_NODE : 
    Write (theOStream, aNodeValue);
    break;
    case LDOM_Node::ELEMENT_NODE : 
    {
      const int aMaxNSpaces    = 40;
      static char aSpaces [] = {
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace, chSpace,
        chOpenAngle, chNull };
      const char * anIndentString = &aSpaces [aMaxNSpaces -  myCurIndent];
      
      if (anIndentString < &aSpaces[0])
      {
        anIndentString = &aSpaces[0];
      }

      // Output the element start tag.
      Write (theOStream, anIndentString);
      Write (theOStream, aNodeName.GetString());

      // Output any attributes of this element
      const LDOM_Element& anElemToWrite = (const LDOM_Element&)theNode;
      LDOM_NodeList aListAtt = anElemToWrite.GetAttributesList();
      Standard_Integer aListInd = aListAtt.getLength();
      
      while (aListInd--)
      {
        LDOM_Node aChild = aListAtt.item (aListInd);
        WriteAttribute (theOStream, aChild);
      }

      //  Test for the presence of children
      LDOM_Node aChild = theNode.getFirstChild();
      if (aChild != 0) 
      {
        // There are children. Close start-tag, and output children.
        Write (theOStream, chCloseAngle);
        if (aChild.getNodeType() == LDOM_Node::ELEMENT_NODE && myIndent > 0)
        {
          Write(theOStream, chLF);
        }

        Standard_Boolean isChildElem = Standard_False;
        while( aChild != 0) 
        {
          isChildElem = (aChild.getNodeType() == LDOM_Node::ELEMENT_NODE);
          if (isChildElem)
          {
            myCurIndent += myIndent;
          }

          Write(theOStream, aChild);
          
          if (isChildElem)
          {
            myCurIndent -= myIndent;
          }

          do 
          {
            aChild = aChild.getNextSibling();
          } while (aChild.getNodeType() == LDOM_Node::ATTRIBUTE_NODE);
        }

        // Done with children.  Output the end tag.
        if (isChildElem)
        {
          Write (theOStream, anIndentString);
          Write (theOStream, gEndElement1);
          Write (theOStream, aNodeName.GetString());
          Write (theOStream, chCloseAngle);
        }
        else
        {
          Write (theOStream, gEndElement);
          Write (theOStream, aNodeName.GetString());
          Write (theOStream, chCloseAngle);
        }
      }
      else
      {
        //  There were no children. Output the short form close of
        //  the element start tag, making it an empty-element tag.
        Write (theOStream, chForwardSlash);
        Write (theOStream, chCloseAngle);
      }

      if (myIndent > 0)
      {
        Write (theOStream, chLF);
      }
      break;
    }
    case LDOM_Node::CDATA_SECTION_NODE: 
    {
      Write (theOStream, gStartCDATA);
      Write (theOStream, aNodeValue);
      Write (theOStream, gEndCDATA);
      break;
    }
    case LDOM_Node::COMMENT_NODE: 
    {
      Write (theOStream, gStartComment);
      Write (theOStream, aNodeValue);
      Write (theOStream, gEndComment);
      break;
    }
  default:
#ifndef _MSC_VER
      cerr << "Unrecognized node type = "
        << (long)theNode.getNodeType() << endl
#endif
  ; }
}

//=======================================================================
//function : 
//purpose  : Stream out an LDOMString
//=======================================================================
void LDOM_XmlWriter::Write (Standard_OStream& theOStream, const LDOMBasicString& theString)
{
  switch (theString.Type())
  {
    case LDOMBasicString::LDOM_Integer:
    {
      Standard_Integer aValue;
      theString.GetInteger (aValue);

      TCollection_AsciiString aStrValue (aValue);
      theOStream.write(aStrValue.ToCString(), strlen (aStrValue.ToCString()));

      break;
    }
    case LDOMBasicString::LDOM_AsciiHashed:       // attr names and element tags
    case LDOMBasicString::LDOM_AsciiDocClear:
    {
      const char* aStr = theString.GetString();
      if (aStr)
      {
        const Standard_Size aLen = strlen (aStr);
        if (aLen > 0) 
        {
          theOStream.write(aStr, aLen);
        }
      }
    }
    break;
    case LDOMBasicString::LDOM_AsciiFree:
    case LDOMBasicString::LDOM_AsciiDoc:
    {
      const char* aStr = theString.GetString();
      if (aStr)
      {
        Standard_Integer aLen;
        char* encStr = LDOM_CharReference::Encode (aStr, aLen, Standard_False);
        if (aLen > 0)
        {
          theOStream.write(encStr, aLen);
        }

        if (encStr != aStr)
        {
          delete [] encStr;
        }
      }
    }
  default: ;
  }
}

//=======================================================================
//function : Write
//purpose  : Stream out a char
//=======================================================================
void LDOM_XmlWriter::Write (Standard_OStream& theOStream, const char theChar)
{
  theOStream.write (&theChar, sizeof(char));
}

//=======================================================================
//function : Write
//purpose  : Stream out a char *
//=======================================================================
void LDOM_XmlWriter::Write (Standard_OStream& theOStream, const char * theString)
{
  Standard_Size aLength = strlen (theString);
  if (aLength > 0)
  {
    theOStream.write (theString, aLength);
  }
}

//=======================================================================
//function : WriteAttribute()
//purpose  : Stream out an XML attribute.
//=======================================================================
void LDOM_XmlWriter::WriteAttribute (Standard_OStream& theOStream, const LDOM_Node& theAtt)
{
  const char* aName = theAtt.getNodeName().GetString();
  const LDOMString aValueStr = theAtt.getNodeValue();

  int aLength = 0;

  // Integer attribute value
  if (aValueStr.Type() == LDOMBasicString::LDOM_Integer)
  {
    Standard_Integer anIntValue;
    aValueStr.GetInteger (anIntValue);

    aLength = (Standard_Integer)(20 + strlen (aName));
    if (aLength > myABufferLen)
    {
      if (myABuffer != NULL)
      {
        delete [] myABuffer;
      }
      
      myABuffer    = new char [aLength+1];
      myABufferLen = aLength;
    }
    sprintf (myABuffer, "%c%s%c%c%d%c", chSpace, aName, chEqual, chDoubleQuote, anIntValue, chDoubleQuote);
    aLength = (Standard_Integer)strlen (myABuffer);

  
  }
  else // String attribute value
  {
    char* encStr;
    const char* aValue = aValueStr.GetString();
    if (aValueStr.Type() == LDOMBasicString::LDOM_AsciiDocClear)
    {
      encStr  = (char *) aValue;
      aLength = (Standard_Integer) (4 + strlen (aValue) + strlen (aName));
    }
    else
    {
      encStr = LDOM_CharReference::Encode (aValue, aLength, Standard_True);
      aLength += (Standard_Integer) (4 + strlen (aName));
    }

    if (aLength > myABufferLen)
    {
      if (myABuffer != NULL) 
      {
        delete [] myABuffer;
      }
      
      myABuffer    = new char [aLength+1];
      myABufferLen = aLength;
    }

    sprintf (myABuffer, "%c%s%c%c%s%c", chSpace, aName, chEqual, chDoubleQuote, encStr, chDoubleQuote);
    
    if (encStr != aValue)
    {
      delete [] encStr;
    }
  }

  theOStream.write (myABuffer, aLength);
}
