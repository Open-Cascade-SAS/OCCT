// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Standard_ReadLineBuffer_HeaderFile
#define _Standard_ReadLineBuffer_HeaderFile

#include <iostream>
#include <vector>

//! Auxiliary tool for buffered reading of lines from input stream.
class Standard_ReadLineBuffer
{
public:
  //! Constructor with initialization.
  //! @param theMaxBufferSizeBytes the length of buffer to read (in bytes)
  Standard_ReadLineBuffer(size_t theMaxBufferSizeBytes)
      : myUseReadBufferLastStr(false),
        myIsMultilineMode(false),
        myToPutGapInMultiline(true),
        myBufferPos(0),
        myBytesLastRead(0)
  {
    // allocate read buffer
    myReadBuffer.resize(theMaxBufferSizeBytes);
  }

  //! Destructor.
  virtual ~Standard_ReadLineBuffer() {}

  //! Clear buffer and cached values.
  void Clear()
  {
    myReadBufferLastStr.clear();
    myUseReadBufferLastStr = false;
    myIsMultilineMode      = false;
    myToPutGapInMultiline  = true;
    myBufferPos            = 0;
    myBytesLastRead        = 0;
  }

  //! Read next line from the stream.
  //! @return pointer to the line or NULL on error / end of reading buffer
  //!         (in case of NULL result theStream should be checked externally to identify the
  //!         presence of errors).
  //!          Empty lines will be returned also with zero length.
  //! @param theStream [inout] - the stream to read from.
  //! @param[out] theLineLength  - output parameter defined length of returned line.
  template <typename Stream_T>
  const char* ReadLine(Stream_T& theStream, size_t& theLineLength)
  {
    int64_t aReadData = 0;
    return ReadLine(theStream, theLineLength, aReadData);
  }

  //! Read next line from the stream.
  //! @return pointer to the line or NULL on error / end of reading buffer
  //!         (in case of NULL result theStream should be checked externally to identify the
  //!         presence of errors).
  //!          Empty lines will be returned also with zero length.
  //! @param theStream [inout] - the stream to read from.
  //! @param[out] theLineLength  - output parameter defined length of returned line.
  //! @param[out] theReadData    - output parameter defined the number of elements successfully read
  //! from the stream during this call,
  //!                              it can be zero if no data was read and the line is taken from the
  //!                              buffer.
  template <typename Stream_T>
  const char* ReadLine(Stream_T& theStream, size_t& theLineLength, int64_t& theReadData)
  {
    char* aResultLine = NULL;
    bool  isMultiline = false;
    theLineLength     = 0;
    theReadData       = 0;

    while (aResultLine == NULL)
    {
      if (myBufferPos == 0 || myBufferPos >= (myBytesLastRead))
      {
        // read new chunk from the stream
        if (!readStream(theStream, myReadBuffer.size(), myBytesLastRead))
        {
          // error during file reading
          break;
        }

        theReadData = myBytesLastRead;

        if (myBytesLastRead > 0)
        {
          myBufferPos = 0;
        }
        else
        {
          // end of the stream
          if (myUseReadBufferLastStr)
          {
            theLineLength          = myReadBufferLastStr.size();
            aResultLine            = &myReadBufferLastStr.front();
            myUseReadBufferLastStr = false;
          }
          break;
        }
      }

      size_t aStartLinePos  = myBufferPos;
      bool   isEndLineFound = false;

      // read next line from myReadBuffer
      while (myBufferPos < myBytesLastRead)
      {
        if (myIsMultilineMode && myReadBuffer[myBufferPos] == '\\')
        {
          // multi-line syntax
          if (myBufferPos + 1 == myBytesLastRead
              || (myBufferPos + 2 == myBytesLastRead && myReadBuffer[myBufferPos + 1] == '\r'))
          {
            isMultiline = true;
            if (myToPutGapInMultiline)
            {
              myReadBuffer[myBufferPos] = ' ';
              if (myBufferPos + 1 != myBytesLastRead)
              {
                myReadBuffer[myBufferPos + 1] = ' ';
              }
            }
          }
          else if (myReadBuffer[myBufferPos + 1] == '\n'
                   || (myReadBuffer[myBufferPos + 1] == '\r'
                       && myReadBuffer[myBufferPos + 2] == '\n'))
          {
            size_t aBufferPos = myBufferPos;
            myBufferPos       = aBufferPos + (myReadBuffer[aBufferPos + 1] == '\r' ? 2 : 1);
            if (myToPutGapInMultiline)
            {
              myReadBuffer[aBufferPos] = ' ';
              ++aBufferPos;
            }

            if (myUseReadBufferLastStr)
            {
              myReadBufferLastStr.insert(myReadBufferLastStr.end(),
                                         myReadBuffer.begin() + aStartLinePos,
                                         myReadBuffer.begin() + aBufferPos);
            }
            else
            {
              myReadBufferLastStr    = std::vector<char>(myReadBuffer.begin() + aStartLinePos,
                                                      myReadBuffer.begin() + aBufferPos);
              myUseReadBufferLastStr = true;
            }

            aStartLinePos = myBufferPos + 1;
          }
        }
        else if (myReadBuffer[myBufferPos] == '\n')
        {
          if (!isMultiline)
          {
            isEndLineFound = true;
          }
          else if (myBufferPos == 1 && myReadBuffer[0] == '\r')
          {
            myReadBufferLastStr.erase(myReadBufferLastStr.end() - 1);
            aStartLinePos += 2;
            isMultiline = false;
          }
          else if (myBufferPos == 0)
          {
            aStartLinePos += 1;
            if (myReadBufferLastStr[myReadBufferLastStr.size() - 1] == '\\')
            {
              myReadBufferLastStr.erase(myReadBufferLastStr.end() - 1);
            }
            else
            {
              myReadBufferLastStr.erase(myReadBufferLastStr.end() - 2, myReadBufferLastStr.end());
            }
            isMultiline = false;
          }
        }

        ++myBufferPos;

        if (isEndLineFound)
          break;
      }

      if (isEndLineFound)
      {
        if (myUseReadBufferLastStr)
        {
          // append current string to the last "unfinished" string of the previous chunk
          myReadBufferLastStr.insert(myReadBufferLastStr.end(),
                                     myReadBuffer.begin() + aStartLinePos,
                                     myReadBuffer.begin() + myBufferPos);
          myUseReadBufferLastStr = false;
          theLineLength          = myReadBufferLastStr.size();
          aResultLine            = &myReadBufferLastStr.front();
        }
        else
        {
          if (myReadBufferLastStr.size() > 0)
          {
            myReadBufferLastStr.clear();
          }
          theLineLength = myBufferPos - aStartLinePos;
          aResultLine   = &myReadBuffer.front() + aStartLinePos;
        }
        // make string null terminated by replacing '\n' or '\r' (before '\n') symbol to null
        // character.
        if (theLineLength > 1 && aResultLine[theLineLength - 2] == '\r')
        {
          aResultLine[theLineLength - 2] = '\0';
          theLineLength -= 2;
        }
        else
        {
          aResultLine[theLineLength - 1] = '\0';
          theLineLength -= 1;
        }
      }
      else
      {
        // save "unfinished" part of string to additional buffer
        if (aStartLinePos != myBufferPos)
        {
          if (myUseReadBufferLastStr)
          {
            myReadBufferLastStr.insert(myReadBufferLastStr.end(),
                                       myReadBuffer.begin() + aStartLinePos,
                                       myReadBuffer.begin() + myBufferPos);
          }
          else
          {
            myReadBufferLastStr    = std::vector<char>(myReadBuffer.begin() + aStartLinePos,
                                                    myReadBuffer.begin() + myBufferPos);
            myUseReadBufferLastStr = true;
          }
        }
      }
    }
    return aResultLine;
  }

  //! Returns TRUE when the Multiline Mode is on; FALSE by default.
  //! Multiline modes joins several lines in file having \ at the end of line:
  //! @code
  //!   Line starts here, \ // line continuation character without this comment
  //!   continues \         // line continuation character without this comment
  //!   and ends.
  //! @endcode
  bool IsMultilineMode() const { return myIsMultilineMode; }

  //! Put gap space while merging lines within multiline syntax, so that the following sample:
  //! @code
  //! 1/2/3\      // line continuation character without this comment
  //! 4/5/6
  //! @endcode
  //! Will become "1/2/3 4/5/6" when flag is TRUE, and "1/2/35/5/6" otherwise.
  bool ToPutGapInMultiline() const { return myToPutGapInMultiline; }

  //! Sets or unsets the multi-line mode.
  //! @param[in] theMultilineMode  multiline mode flag
  //! @param[in] theToPutGap       put gap space while connecting lines (no gap otherwise)
  void SetMultilineMode(bool theMultilineMode, bool theToPutGap = true)
  {
    myIsMultilineMode     = theMultilineMode;
    myToPutGapInMultiline = theToPutGap;
  }

protected:
  //! Read from stl stream.
  //! @return true if reading was finished without errors.
  bool readStream(std::istream& theStream, size_t theLen, size_t& theReadLen)
  {
    theReadLen = (size_t)theStream.read(&myReadBuffer.front(), theLen).gcount();
    return !theStream.bad();
  }

  //! Read from FILE stream.
  //! @return true if reading was finished without errors.
  bool readStream(FILE* theStream, size_t theLen, size_t& theReadLen)
  {
    theReadLen = ::fread(&myReadBuffer.front(), 1, theLen, theStream);
    return ::ferror(theStream) == 0;
  }

protected:
  std::vector<char> myReadBuffer;        //!< Temp read buffer
  std::vector<char> myReadBufferLastStr; //!< Part of last string of myReadBuffer
  // clang-format off
  bool              myUseReadBufferLastStr; //!< Flag to use myReadBufferLastStr during next line reading
  bool              myIsMultilineMode;      //!< Flag to process of the special multi-line case at the end of the line
  bool              myToPutGapInMultiline;  //!< Flag to put gap space while joining lines in multi-line syntax
  size_t            myBufferPos;            //!< Current position in myReadBuffer
  size_t            myBytesLastRead;        //!< The number of characters that were read last time from myReadBuffer.
  // clang-format on
};

#endif // _Standard_ReadLineBuffer_HeaderFile
