// Copyright (c) 2017-2019 OPEN CASCADE SAS
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
// commercial license or contractual agreement..

#ifndef _Standard_ReadBuffer_HeaderFile
#define _Standard_ReadBuffer_HeaderFile

#include <Standard_ProgramError.hxx>

#include <iostream>

//! Auxiliary tool for buffered reading from input stream within chunks of constant size.
class Standard_ReadBuffer
{
public:

  //! Constructor with initialization.
  Standard_ReadBuffer (int64_t theDataLen,
                       size_t  theChunkLen,
                       bool theIsPartialPayload = false)
  : myBufferPtr(NULL),
    myBufferEnd(NULL),
    myDataLen  (0),
    myDataRead (0),
    myChunkLen (0),
    myNbChunks (0),
    myBufferLen(0)
  {
    Init (theDataLen, theChunkLen, theIsPartialPayload);
  }

  //! Initialize the buffer.
  //! @param theDataLen  [in] the full length of input data to read from stream.
  //! @param theChunkLen [in] the length of single chunk to read
  //! @param theIsPartialPayload [in] when FALSE, theDataLen will be automatically aligned to the multiple of theChunkLen;
  //!                                 when TRUE, last chunk will be read from stream exactly till theDataLen
  //!                                 allowing portion of chunk to be uninitialized (useful for interleaved data)
  void Init (int64_t theDataLen,
             size_t  theChunkLen,
             bool theIsPartialPayload = false)
  {
    myDataRead  = 0;
    if (theIsPartialPayload)
    {
      myDataLen = theDataLen;
    }
    else
    {
      myDataLen = theDataLen - theDataLen % int64_t(theChunkLen);
    }
    myChunkLen  = theChunkLen;
    myNbChunks  = sizeof(myBuffer) / theChunkLen;
    myBufferLen = theChunkLen * myNbChunks;

    myBufferEnd = myBuffer + sizeof(myBuffer);
    myBufferPtr = myBuffer + sizeof(myBuffer);
    memset (myBuffer, 0, sizeof(myBuffer));

    if (theChunkLen > sizeof(myBuffer))
    {
      Standard_ProgramError::Raise ("Internal error - chunk size is greater then preallocated buffer");
    }
  }

  //! Return TRUE if amount of read bytes is equal to requested length of entire data.
  bool IsDone() const
  {
    return myDataRead == myDataLen;
  }

  //! Read next chunk.
  //! @return pointer to the chunk or NULL on error / end of reading buffer
  template<typename Chunk_T, typename Stream_T>
  Chunk_T* ReadChunk (Stream_T& theStream)
  {
    return reinterpret_cast<Chunk_T*> (readRawDataChunk (theStream));
  }

  //! Read next chunk.
  //! @return pointer to the chunk or NULL on error / end of reading buffer
  template<typename Stream_T>
  char* ReadDataChunk (Stream_T& theStream)
  {
    return readRawDataChunk (theStream);
  }

private:

  //! Read next chunk.
  //! @return pointer to the chunk or NULL on error / end of reading buffer
  template<typename Stream_T>
  char* readRawDataChunk (Stream_T& theStream)
  {
    if (myBufferPtr == NULL)
    {
      return NULL;
    }

    myBufferPtr += myChunkLen;
    if (myBufferPtr < myBufferEnd)
    {
      return myBufferPtr;
    }

    const int64_t aDataLeft = myDataLen - myDataRead;
    if (aDataLeft <= 0) // myDataLen is normally multiple of myChunkLen, but can be smaller in interleaved data
    {
      myBufferPtr = NULL;
      return NULL;
    }

    const size_t aDataToRead = int64_t(myBufferLen) > aDataLeft ? size_t(aDataLeft) : myBufferLen;
    if (!readStream (theStream, aDataToRead))
    {
      myBufferPtr = NULL;
      return NULL;
    }

    myBufferPtr = myBuffer;
    myBufferEnd = myBuffer + aDataToRead;
    myDataRead += aDataToRead;
    return myBufferPtr;
  }

  //! Read from stl stream.
  bool readStream (std::istream& theStream,
                   size_t theLen)
  {
    theStream.read (myBuffer, theLen);
    return theStream.good();
  }

  //! Read from FILE stream.
  bool readStream (FILE* theStream,
                   size_t theLen)
  {
    return ::fread (myBuffer, 1, theLen, theStream) == theLen;
  }

private:

  char        myBuffer[4096]; //!< data cache
  char*       myBufferPtr;    //!< current position within the buffer
  const char* myBufferEnd;    //!< end of the buffer
  int64_t     myDataLen;      //!< length of entire data to read
  int64_t     myDataRead;     //!< amount of data already processed
  size_t      myChunkLen;     //!< length of single chunk that caller would like to read (e.g. iterator increment)
  size_t      myNbChunks;     //!< number of cached chunks
  size_t      myBufferLen;    //!< effective length of the buffer to be read at once (multiple of chunk length)

};

#endif // _Standard_ReadBuffer_HeaderFile
