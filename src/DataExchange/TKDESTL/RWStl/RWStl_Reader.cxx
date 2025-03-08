// Created: 2016-05-01
// Author: Andrey Betenev
// Copyright: Open CASCADE 2016
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

#include <RWStl_Reader.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <FSD_BinaryFile.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Timer.hxx>
#include <Poly_MergeNodesTool.hxx>
#include <Standard_CLocaleSentry.hxx>

#include <algorithm>
#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(RWStl_Reader, Standard_Transient)

namespace
{
// Binary STL sizes
static const size_t THE_STL_HEADER_SIZE   = 84;
static const size_t THE_STL_SIZEOF_FACET  = 50;
static const size_t THE_STL_MIN_FILE_SIZE = THE_STL_HEADER_SIZE + THE_STL_SIZEOF_FACET;

// The length of buffer to read (in bytes)
static const size_t THE_BUFFER_SIZE = 1024;

//! Auxiliary tool for merging nodes during STL reading.
class MergeNodeTool : public Poly_MergeNodesTool
{
public:
  //! Constructor
  MergeNodeTool(RWStl_Reader* theReader, const Standard_Integer theNbFacets = -1)
      : Poly_MergeNodesTool(theReader->MergeAngle(), 0.0, theNbFacets),
        myReader(theReader),
        myNodeIndexMap(1024, new NCollection_IncAllocator(1024 * 1024))
  {
    // avoid redundant allocations as final triangulation is managed by RWStl_Reader subclass
    ChangeOutput().Nullify();
  }

  //! Add new triangle
  void AddTriangle(const gp_XYZ theElemNodes[3])
  {
    Poly_MergeNodesTool::AddTriangle(theElemNodes);

    // remap node indices returned by RWStl_Reader::AddNode();
    // this is a waste of time for most cases of sequential index adding, but preserved for keeping
    // RWStl_Reader interface
    int aNodesSrc[3] = {ElementNodeIndex(0), ElementNodeIndex(1), ElementNodeIndex(2)};
    int aNodesRes[3] = {-1, -1, -1};
    for (int aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
    {
      // use existing node if found at the same point
      if (!myNodeIndexMap.Find(aNodesSrc[aNodeIter], aNodesRes[aNodeIter]))
      {
        aNodesRes[aNodeIter] = myReader->AddNode(theElemNodes[aNodeIter]);
        myNodeIndexMap.Bind(aNodesSrc[aNodeIter], aNodesRes[aNodeIter]);
      }
    }
    if (aNodesRes[0] != aNodesRes[1] && aNodesRes[1] != aNodesRes[2]
        && aNodesRes[2] != aNodesRes[0])
    {
      myReader->AddTriangle(aNodesRes[0], aNodesRes[1], aNodesRes[2]);
    }
  }

private:
  RWStl_Reader*                                           myReader;
  NCollection_DataMap<Standard_Integer, Standard_Integer> myNodeIndexMap;
};

//! Read a Little Endian 32 bits float
inline static float readStlFloat(const char* theData)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  // on big-endian platform, map values byte-per-byte
  union {
    uint32_t i;
    float    f;
  } bidargum;

  bidargum.i = theData[0] & 0xFF;
  bidargum.i |= (theData[1] & 0xFF) << 0x08;
  bidargum.i |= (theData[2] & 0xFF) << 0x10;
  bidargum.i |= (theData[3] & 0xFF) << 0x18;
  return bidargum.f;
#else
  // on little-endian platform, use plain cast
  return *reinterpret_cast<const float*>(theData);
#endif
}

//! Read a Little Endian 32 bits float
inline static gp_XYZ readStlFloatVec3(const char* theData)
{
  return gp_XYZ(readStlFloat(theData),
                readStlFloat(theData + sizeof(float)),
                readStlFloat(theData + sizeof(float) * 2));
}

} // namespace

//=================================================================================================

RWStl_Reader::RWStl_Reader()
    : myMergeAngle(M_PI / 2.0),
      myMergeTolearance(0.0)
{
  //
}

//=================================================================================================

Standard_Boolean RWStl_Reader::Read(const char* theFile, const Message_ProgressRange& theProgress)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream =
    aFileSystem->OpenIStream(theFile, std::ios::in | std::ios::binary);
  if (aStream.get() == NULL)
  {
    Message::SendFail(TCollection_AsciiString("Error: file '") + theFile + "' is not found");
    return Standard_False;
  }
  // get length of file to feed progress indicator in Ascii mode
  aStream->seekg(0, aStream->end);
  std::streampos theEnd = aStream->tellg();
  aStream->seekg(0, aStream->beg);

  // binary STL files cannot be shorter than 134 bytes
  // (80 bytes header + 4 bytes facet count + 50 bytes for one facet);
  // thus assume files shorter than 134 as Ascii without probing
  // (probing may bring stream to fail state if EOF is reached)
  bool isAscii = ((size_t)theEnd < THE_STL_MIN_FILE_SIZE || IsAscii(*aStream, true));

  Standard_ReadLineBuffer aBuffer(THE_BUFFER_SIZE);

  // Note: here we are trying to handle rare but realistic case of
  // STL files which are composed of several STL data blocks
  // running translation in cycle.
  // For this reason use infinite (logarithmic) progress scale,
  // but in special mode so that the first cycle will take ~ 70% of it
  Message_ProgressScope aPS(theProgress, NULL, 1, true);
  while (aStream->good())
  {
    if (isAscii)
    {
      if (!ReadAscii(*aStream, aBuffer, theEnd, aPS.Next(2)))
      {
        break;
      }
    }
    else
    {
      if (!ReadBinary(*aStream, aPS.Next(2)))
      {
        break;
      }
    }
    *aStream >> std::ws; // skip any white spaces
    AddSolid();
  }
  return !aStream->fail();
}

//=================================================================================================

Standard_Boolean RWStl_Reader::IsAscii(Standard_IStream& theStream, const bool isSeekgAvailable)
{
  // read first 134 bytes to detect file format
  char            aBuffer[THE_STL_MIN_FILE_SIZE];
  std::streamsize aNbRead = theStream.read(aBuffer, THE_STL_MIN_FILE_SIZE).gcount();
  if (!theStream)
  {
    Message::SendFail("Error: Cannot read file");
    return true;
  }

  if (isSeekgAvailable)
  {
    // get back to the beginning
    theStream.seekg(0, theStream.beg);
  }
  else
  {
    // put back the read symbols
    for (std::streamsize aByteIter = aNbRead; aByteIter > 0; --aByteIter)
    {
      theStream.unget();
    }
  }

  // if file is shorter than size of binary file with 1 facet, it must be ascii
  if (aNbRead < std::streamsize(THE_STL_MIN_FILE_SIZE))
  {
    return true;
  }

  // otherwise, detect binary format by presence of non-ascii symbols in first 128 bytes
  // (note that binary STL file may start with the same bytes "solid " as Ascii one)
  for (Standard_Integer aByteIter = 0; aByteIter < aNbRead; ++aByteIter)
  {
    if ((unsigned char)aBuffer[aByteIter] > (unsigned char)'~')
    {
      return false;
    }
  }
  return true;
}

// adapted from Standard_CString.cxx
#ifdef __APPLE__
  // There are a lot of *_l functions available on Mac OS X - we use them
  #define SAVE_TL()
#elif defined(_MSC_VER)
  // MSVCRT has equivalents with slightly different syntax
  #define SAVE_TL()
  #define sscanf_l(theBuffer, theLocale, theFormat, ...)                                           \
    _sscanf_s_l(theBuffer, theFormat, theLocale, __VA_ARGS__)
#else
  // glibc provides only limited xlocale implementation:
  // strtod_l/strtol_l/strtoll_l functions with explicitly specified locale
  // and newlocale/uselocale/freelocale to switch locale within current thread only.
  // So we switch to C locale temporarily
  #define SAVE_TL() Standard_CLocaleSentry aLocaleSentry;
  #define sscanf_l(theBuffer, theLocale, theFormat, ...) sscanf(theBuffer, theFormat, __VA_ARGS__)
#endif

// Macro to get 64-bit position of the file from std::streampos
#if defined(_MSC_VER) && _MSC_VER < 1700
  // In MSVC 2010, cast of std::streampos to 64-bit int is implemented incorrectly;
  // work-around (relevant for files larger than 4 GB) is to use internal function seekpos().
  // Since MSVC 15.8, seekpos() is deprecated and is said to always return 0.
  #define GETPOS(aPos) aPos.seekpos()
#else
  #define GETPOS(aPos) ((int64_t)aPos)
#endif

static inline bool str_starts_with(const char* theStr, const char* theWord, int theN)
{
  while (isspace(*theStr) && *theStr != '\0')
    theStr++;
  return !strncasecmp(theStr, theWord, theN);
}

static bool ReadVertex(const char* theStr, double& theX, double& theY, double& theZ)
{
  const char* aStr = theStr;

  // skip 'vertex'
  while (isspace((unsigned char)*aStr) || isalpha((unsigned char)*aStr))
    ++aStr;

  // read values
  char* aEnd;
  theX = Strtod(aStr, &aEnd);
  theY = Strtod(aStr = aEnd, &aEnd);
  theZ = Strtod(aStr = aEnd, &aEnd);

  return aEnd != aStr;
}

//=================================================================================================

Standard_Boolean RWStl_Reader::ReadAscii(Standard_IStream&            theStream,
                                         Standard_ReadLineBuffer&     theBuffer,
                                         const std::streampos         theUntilPos,
                                         const Message_ProgressRange& theProgress)
{
  // use method seekpos() to get true 64-bit offset to enable
  // handling of large files (VS 2010 64-bit)
  const int64_t aStartPos = GETPOS(theStream.tellg());
  size_t        aLineLen  = 0;
  const char*   aLine;

  // skip header "solid ..."
  aLine = theBuffer.ReadLine(theStream, aLineLen);
  // skip empty lines
  while (aLine && !*aLine)
  {
    aLine = theBuffer.ReadLine(theStream, aLineLen);
  }
  if (aLine == NULL)
  {
    Message::SendFail("Error: premature end of file");
    return false;
  }

  MergeNodeTool aMergeTool(this);
  aMergeTool.SetMergeAngle(myMergeAngle);
  aMergeTool.SetMergeTolerance(myMergeTolearance);

  Standard_CLocaleSentry::clocale_t aLocale = Standard_CLocaleSentry::GetCLocale();
  (void)aLocale; // to avoid warning on GCC where it is actually not used
  SAVE_TL()      // for GCC only, set C locale globally

  // report progress every 1 MiB of read data
  const int              aStepB = 1024 * 1024;
  const Standard_Integer aNbSteps =
    1 + Standard_Integer((GETPOS(theUntilPos) - aStartPos) / aStepB);
  Message_ProgressScope aPS(theProgress, "Reading text STL file", aNbSteps);
  int64_t               aProgressPos = aStartPos + aStepB;
  int                   aNbLine      = 1;

  while (aPS.More())
  {
    if (GETPOS(theStream.tellg()) > aProgressPos)
    {
      aPS.Next();
      aProgressPos += aStepB;
    }

    aLine = theBuffer.ReadLine(theStream, aLineLen); // "facet normal nx ny nz"
    if (aLine == NULL)
    {
      Message::SendFail("Error: premature end of file");
      return false;
    }
    if (str_starts_with(aLine, "endsolid", 8))
    {
      // end of STL code
      break;
    }
    if (!str_starts_with(aLine, "facet", 5))
    {
      Message::SendFail(TCollection_AsciiString("Error: unexpected format of facet at line ")
                        + (aNbLine + 1));
      return false;
    }

    aLine = theBuffer.ReadLine(theStream, aLineLen); // "outer loop"
    if (aLine == NULL || !str_starts_with(aLine, "outer", 5))
    {
      Message::SendFail(TCollection_AsciiString("Error: unexpected format of facet at line ")
                        + (aNbLine + 1));
      return false;
    }

    gp_XYZ           aVertex[3];
    Standard_Boolean isEOF = false;
    for (Standard_Integer i = 0; i < 3; i++)
    {
      aLine = theBuffer.ReadLine(theStream, aLineLen);
      if (aLine == NULL)
      {
        isEOF = true;
        break;
      }
      gp_XYZ aReadVertex;
      if (!ReadVertex(aLine,
                      aReadVertex.ChangeCoord(1),
                      aReadVertex.ChangeCoord(2),
                      aReadVertex.ChangeCoord(3)))
      {
        Message::SendFail(TCollection_AsciiString("Error: cannot read vertex coordinates at line ")
                          + aNbLine);
        return false;
      }
      aVertex[i] = aReadVertex;
    }

    // stop reading if end of file is reached;
    // note that well-formatted file never ends by the vertex line
    if (isEOF)
    {
      break;
    }

    aNbLine += 5;

    // add triangle
    aMergeTool.AddTriangle(aVertex);

    theBuffer.ReadLine(theStream, aLineLen); // skip "endloop"
    theBuffer.ReadLine(theStream, aLineLen); // skip "endfacet"

    aNbLine += 2;
  }

  return aPS.More();
}

//=================================================================================================

Standard_Boolean RWStl_Reader::ReadBinary(Standard_IStream&            theStream,
                                          const Message_ProgressRange& theProgress)
{
  /*
    // the size of the file (minus the header size)
    // must be a multiple of SIZEOF_STL_FACET
    if ((theFileLen - THE_STL_HEADER_SIZE) % THE_STL_SIZEOF_FACET != 0
     || (theFileLen < THE_STL_MIN_FILE_SIZE))
    {
      Message::SendFail ("Error: Corrupted binary STL file (inconsistent file size)");
      return Standard_False;
    }
    const Standard_Integer  aNbFacets = Standard_Integer((theFileLen - THE_STL_HEADER_SIZE) /
    THE_STL_SIZEOF_FACET);
  */

  // read file header at first
  char aHeader[THE_STL_HEADER_SIZE + 1];
  if (theStream.read(aHeader, THE_STL_HEADER_SIZE).gcount() != std::streamsize(THE_STL_HEADER_SIZE))
  {
    Message::SendFail("Error: Corrupted binary STL file");
    return false;
  }

  // number of facets is stored as 32-bit integer at position 80
  const Standard_Integer aNbFacets = *(int32_t*)(aHeader + 80);

  MergeNodeTool aMergeTool(this, aNbFacets);
  aMergeTool.SetMergeAngle(myMergeAngle);
  aMergeTool.SetMergeTolerance(myMergeTolearance);

  // don't trust the number of triangles which is coded in the file
  // sometimes it is wrong, and with this technique we don't need to swap endians for integer
  Message_ProgressScope aPS(theProgress, "Reading binary STL file", aNbFacets);
  Standard_Integer      aNbRead = 0;

  // allocate buffer for 80 triangles
  const int THE_CHUNK_NBFACETS = 80;
  char      aBuffer[THE_STL_SIZEOF_FACET * THE_CHUNK_NBFACETS];

  // normal + 3 nodes + 2 extra bytes
  const size_t     aVec3Size        = sizeof(float) * 3;
  const size_t     aFaceDataLen     = aVec3Size * 4 + 2;
  const char*      aBufferPtr       = aBuffer;
  Standard_Integer aNbFacesInBuffer = 0;
  for (Standard_Integer aNbFacetRead = 0; aNbFacetRead < aNbFacets && aPS.More();
       ++aNbFacetRead, ++aNbRead, --aNbFacesInBuffer, aBufferPtr += aFaceDataLen, aPS.Next())
  {
    // read more data
    if (aNbFacesInBuffer <= 0)
    {
      aNbFacesInBuffer                  = Min(THE_CHUNK_NBFACETS, aNbFacets - aNbFacetRead);
      const std::streamsize aDataToRead = aNbFacesInBuffer * aFaceDataLen;
      if (theStream.read(aBuffer, aDataToRead).gcount() != aDataToRead)
      {
        Message::SendFail("Error: binary STL read failed");
        return false;
      }
      aBufferPtr = aBuffer;
    }

    // get points from buffer
    //    readStlFloatVec3 (aBufferPtr); // skip normal
    gp_XYZ aTriNodes[3] = {readStlFloatVec3(aBufferPtr + aVec3Size),
                           readStlFloatVec3(aBufferPtr + aVec3Size * 2),
                           readStlFloatVec3(aBufferPtr + aVec3Size * 3)};
    aMergeTool.AddTriangle(aTriNodes);
  }

  return aPS.More();
}
