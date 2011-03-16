// File:      LDOM_OSStream.hxx
// Created:   01.10.01 10:56:06
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001

#ifndef LDOM_OSStream_HeaderFile
#define LDOM_OSStream_HeaderFile

//  This implementation allows to increase performance
//        of outputting data into a string
//        avoiding reallocation of buffer.
//
//  class LDOM_OSStream implements output into a sequence of
//             strings and getting the result as a string.
//        It inherits Standard_OStream (ostream).
//        Beside methods of ostream, it also has additional
//        useful methods: str(), Length() and Clear().
//
//  struct LDOM_StringElem is one element of internal sequence
//
//  class LDOM_SBuffer inherits streambuf and
//             redefines some virtual methods of it
//             (overflow() and xsputn())
//        This class contains pointers on first
//          and current element of sequence,
//          also it has methods for the sequence management.

#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>

#include <stdlib.h>
#include <stdio.h> /* EOF */

class LDOM_StringElem; // defined in cxx file

class LDOM_SBuffer : public streambuf
{
 public:
  Standard_EXPORT LDOM_SBuffer (const Standard_Integer theMaxBuf);
  // Constructor. Sets a default value for the
  //              length of each sequence element.

  Standard_EXPORT Standard_CString str () const;
  // Concatenates strings of all sequence elements
  // into one string. Space for output string is allocated
  // with operator new.
  // Caller of this function is responsible
  // for memory release after the string usage.

  Standard_Integer Length () const {return myLength;};
  // Returns full length of data contained

  Standard_EXPORT void Clear ();
  // Clears first element of sequence and removes all others

    // Methods of streambuf

    Standard_EXPORT virtual int overflow(int c = EOF);
    Standard_EXPORT virtual int underflow();
    //virtual int uflow();

    Standard_EXPORT virtual int xsputn(const char* s, int n);
    //virtual int xsgetn(char* s, int n);
    //virtual int sync();

  Standard_EXPORT ~LDOM_SBuffer ();
  // Destructor

 private:
  Standard_Integer      myMaxBuf; // default length of one element
  Standard_Integer      myLength; // full length of contained data
  LDOM_StringElem* myFirstString; // the head of the sequence
  LDOM_StringElem* myCurString;   // current element of the sequence
};

class LDOM_OSStream : public Standard_OStream
{
 public:
  Standard_EXPORT LDOM_OSStream (const Standard_Integer theMaxBuf);
  // Constructor

  Standard_CString str () const {return myBuffer.str();};

  Standard_Integer Length () const {return myBuffer.Length();};

  void Clear () { myBuffer.Clear(); };

 private:
  LDOM_SBuffer myBuffer;
};

#endif
