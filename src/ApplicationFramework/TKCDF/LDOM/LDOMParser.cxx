// Created on: 2001-07-20
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

// AGV 060302: Input from std::istream
//             AGV 130302: Return error if there are data after the root element

// #define LDOM_PARSER_TRACE

#include <LDOMParser.hxx>
#include <LDOM_MemManager.hxx>
#include <LDOM_XmlReader.hxx>
#include <LDOM_BasicText.hxx>
#include <LDOM_CharReference.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_FileSystem.hxx>

#ifdef _MSC_VER
  #include <io.h>
#else
  #include <unistd.h>
#endif

//=================================================================================================

LDOMParser::~LDOMParser()
{
  delete myReader;
}

//=======================================================================
// function : ReadRecord
// purpose  : Take the next lexical element from XML stream
//=======================================================================

#ifdef LDOM_PARSER_TRACE
static
#else
inline
#endif
  LDOM_XmlReader::RecordType
  ReadRecord(LDOM_XmlReader&   aReader,
             Standard_IStream& theIStream,
             LDOM_OSStream&    aData,
             bool&             theDocStart)
{
#ifdef LDOM_PARSER_TRACE
  static aCounter = 0;
  ++aCounter;
#endif
  const LDOM_XmlReader::RecordType aType = aReader.ReadRecord(theIStream, aData, theDocStart);
#ifdef LDOM_PARSER_TRACE
  static FILE*            ff = NULL;
  TCollection_AsciiString aTraceFileName;
  #ifdef _WIN32
  aTraceFileName = TCollection_AsciiString(getenv("TEMP")) + "\\ldom.trace";
  #else
  aTraceFileName = "/tmp/ldom.trace";
  #endif
  ff = fopen(aTraceFileName.ToCString(), ff ? "at" : "wt");
  const char* aDataType;
  switch (aType)
  {
    case LDOM_XmlReader::XML_UNKNOWN:
      aDataType = "XML_UNKNOWN      ";
      break;
    case LDOM_XmlReader::XML_HEADER:
      aDataType = "XML_HEADER       ";
      break;
    case LDOM_XmlReader::XML_DOCTYPE:
      aDataType = "XML_DOCTYPE      ";
      break;
    case LDOM_XmlReader::XML_COMMENT:
      aDataType = "XML_COMMENT      ";
      break;
    case LDOM_XmlReader::XML_START_ELEMENT:
      aDataType = "XML_START_ELEMENT";
      break;
    case LDOM_XmlReader::XML_END_ELEMENT:
      aDataType = "XML_END_ELEMENT  ";
      break;
    case LDOM_XmlReader::XML_FULL_ELEMENT:
      aDataType = "XML_FULL_ELEMENT ";
      break;
    case LDOM_XmlReader::XML_TEXT:
      aDataType = "XML_TEXT         ";
      break;
    case LDOM_XmlReader::XML_CDATA:
      aDataType = "XML_CDATA        ";
      break;
    case LDOM_XmlReader::XML_EOF:
      aDataType = "XML_EOF          ";
  }
  char* aStr = aData.str();
  fprintf(ff, "%5d %s: %s\n", aCounter, aDataType, aStr);
  delete[] aStr;
  fclose(ff);
#endif
  return aType;
}

//=======================================================================
// function : GetError
// purpose  : Return text describing a parsing error
//=======================================================================

const TCollection_AsciiString& LDOMParser::GetError(TCollection_AsciiString& aData) const
{
  char* aStr = (char*)myCurrentData.str();
  aData      = aStr;
  delete[] aStr;
  return myError;
}

//=======================================================================
// function : GetBOM
// purpose  : Returns the byte order mask defined at the start of a stream
//=======================================================================

LDOM_OSStream::BOMType LDOMParser::GetBOM() const
{
  if (myReader)
    return myReader->GetBOM();
  return LDOM_OSStream::BOM_UNDEFINED;
}

//=================================================================================================

bool LDOMParser::parse(std::istream& anInput, const bool theTagPerStep, const bool theWithoutRoot)
{
  // Open the DOM Document
  myDocument = new LDOM_MemManager(20000);
  myError.Clear();

  // Create the Reader instance
  delete myReader;
  myReader = new LDOM_XmlReader(myDocument, myError, theTagPerStep);

  // Parse
  return ParseDocument(anInput, theWithoutRoot);
}

//=================================================================================================

bool LDOMParser::parse(const char* const aFileName)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFileStream = aFileSystem->OpenIStream(aFileName, std::ios::in);

  if (aFileStream.get() != nullptr && aFileStream->good())
  {
    return parse(*aFileStream);
  }
  else
  {
    myError = "Fatal XML error: Cannot open XML file";
    return true;
  }
}

//=======================================================================
// function : ParseDocument
// purpose  : parse the whole document (abstracted from the XML source)
//=======================================================================

bool LDOMParser::ParseDocument(std::istream& theIStream, const bool theWithoutRoot)
{
  bool isError   = false;
  bool isElement = false;
  bool isDoctype = false;

  bool isInsertFictRootElement = false;
  bool aDocStart               = true;

  for (;;)
  {
    LDOM_XmlReader::RecordType aType =
      (theWithoutRoot && !isInsertFictRootElement
         ? LDOM_XmlReader::XML_START_ELEMENT
         : ReadRecord(*myReader, theIStream, myCurrentData, aDocStart));
    switch (aType)
    {
      case LDOM_XmlReader::XML_HEADER:
        if (isDoctype || isElement)
        {
          myError = "Unexpected XML declaration";
          isError = true;
          break;
        }
        continue;
      case LDOM_XmlReader::XML_DOCTYPE:
        if (isElement)
        {
          myError = "Unexpected DOCTYPE declaration";
          isError = true;
          break;
        }
        isDoctype = true;
        continue;
      case LDOM_XmlReader::XML_COMMENT:
        continue;
      case LDOM_XmlReader::XML_FULL_ELEMENT:
        if (!isElement)
        {
          isElement                 = true;
          myDocument->myRootElement = &myReader->GetElement();
          if (startElement())
          {
            isError = true;
            myError = "User abort at startElement()";
            break;
          }
          if (endElement())
          {
            isError = true;
            myError = "User abort at endElement()";
            break;
          }
          continue;
        }
        isError = true;
        myError = "Expected comment or end-of-file";
        break;
      case LDOM_XmlReader::XML_START_ELEMENT:
        if (!isElement)
        {
          isElement = true;

          if (theWithoutRoot && !isInsertFictRootElement)
          {
            isInsertFictRootElement = true;

            // create fiction root element
            TCollection_AsciiString aFicName("document");
            myReader->CreateElement(aFicName.ToCString(), aFicName.Length());
          }

          myDocument->myRootElement = &myReader->GetElement();

          if (startElement())
          {
            isError = true;
            myError = "User abort at startElement()";
            break;
          }
          isError = ParseElement(theIStream, aDocStart);
          if (isError)
            break;
          continue;
        }
        isError = true;
        myError = "Expected comment or end-of-file";
        break;
      case LDOM_XmlReader::XML_END_ELEMENT:
        if (endElement())
        {
          isError = true;
          myError = "User abort at endElement()";
        }
        break;
      case LDOM_XmlReader::XML_EOF:
        break;
      case LDOM_XmlReader::XML_UNKNOWN:
        if (isElement)
        {
          default:
            myError = "Unexpected data beyond the Document Element";
        }
        isError = true;
    }
    break;
  }
  return isError;
}

//=======================================================================
// function : ParseElement
// purpose  : parse one element, given the type of its XML presentation
//=======================================================================

bool LDOMParser::ParseElement(Standard_IStream& theIStream, bool& theDocStart)
{
  bool                     isError    = false;
  const LDOM_BasicElement* aParent    = &myReader->GetElement();
  const LDOM_BasicNode*    aLastChild = nullptr;
  for (;;)
  {
    LDOM_Node::NodeType        aLocType;
    LDOMBasicString            aTextValue;
    char*                      aTextStr;
    LDOM_XmlReader::RecordType aType =
      ReadRecord(*myReader, theIStream, myCurrentData, theDocStart);
    switch (aType)
    {
      case LDOM_XmlReader::XML_UNKNOWN:
        isError = true;
        break;
      case LDOM_XmlReader::XML_FULL_ELEMENT:
        aParent->AppendChild(&myReader->GetElement(), aLastChild);
        if (startElement())
        {
          isError = true;
          myError = "User abort at startElement()";
          break;
        }
        if (endElement())
        {
          isError = true;
          myError = "User abort at endElement()";
          break;
        }
        break;
      case LDOM_XmlReader::XML_START_ELEMENT:
        aParent->AppendChild(&myReader->GetElement(), aLastChild);
        if (startElement())
        {
          isError = true;
          myError = "User abort at startElement()";
          break;
        }
        isError = ParseElement(theIStream, theDocStart);
        break;
      case LDOM_XmlReader::XML_END_ELEMENT: {
        const char* aParentName = static_cast<const char*>(aParent->GetTagName());
        aTextStr                = (char*)myCurrentData.str();
        if (strcmp(aTextStr, aParentName) != 0)
        {
          myError = "Expected end tag \'";
          myError += aParentName;
          myError += "\'";
          isError = true;
        }
        else if (endElement())
        {
          isError = true;
          myError = "User abort at endElement()";
        }
        delete[] aTextStr;
      }
        return isError;
      case LDOM_XmlReader::XML_TEXT:
        aLocType = LDOM_Node::TEXT_NODE;
        {
          int aTextLen;
          aTextStr = LDOM_CharReference::Decode((char*)myCurrentData.str(), aTextLen);
          // try to convert to integer
          if (IsDigit(aTextStr[0]))
          {
            if (LDOM_XmlReader::getInteger(aTextValue, aTextStr, aTextStr + aTextLen))
              aTextValue = LDOMBasicString(aTextStr, aTextLen, myDocument);
          }
          else
            aTextValue = LDOMBasicString(aTextStr, aTextLen, myDocument);
        }
        goto create_text_node;
      case LDOM_XmlReader::XML_COMMENT:
        aLocType = LDOM_Node::COMMENT_NODE;
        {
          int aTextLen;
          aTextStr   = LDOM_CharReference::Decode((char*)myCurrentData.str(), aTextLen);
          aTextValue = LDOMBasicString(aTextStr, aTextLen, myDocument);
        }
        goto create_text_node;
      case LDOM_XmlReader::XML_CDATA:
        aLocType   = LDOM_Node::CDATA_SECTION_NODE;
        aTextStr   = (char*)myCurrentData.str();
        aTextValue = LDOMBasicString(aTextStr, myCurrentData.Length(), myDocument);
      create_text_node: {
        LDOM_BasicNode& aTextNode = LDOM_BasicText::Create(aLocType, aTextValue, myDocument);
        aParent->AppendChild(&aTextNode, aLastChild);
      }
        delete[] aTextStr;
        break;
      case LDOM_XmlReader::XML_EOF:
        myError = "Inexpected end of file";
        isError = true;
        break;
      default:;
    }
    if (isError)
      break;
  }
  return isError;
}

//=======================================================================
// function : startElement
// purpose  : virtual hook on 'StartElement' event for descendant classes
//=======================================================================

bool LDOMParser::startElement()
{
  return false;
}

//=======================================================================
// function : endElement
// purpose  : virtual hook on 'EndElement' event for descendant classes
//=======================================================================

bool LDOMParser::endElement()
{
  return false;
}

//=================================================================================================

LDOM_Element LDOMParser::getCurrentElement() const
{
  return LDOM_Element(myReader->GetElement(), myDocument);
}

//=================================================================================================

LDOM_Document LDOMParser::getDocument()
{
  return myDocument->Self();
}
