// File:      LDOM_OSStream.cxx
// Created:   01.10.01 10:57:37
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <LDOM_OSStream.hxx>
#include <string.h>
#include <Standard_Integer.hxx>

// One element of sequence
class LDOM_StringElem
{
  char* buf;             // pointer on data string
  int len;               // quantity of really written data
  LDOM_StringElem* next; // pointer on the next element of a sequence

  LDOM_StringElem (int aLen)
  {
    buf = new char[aLen];
    len = 0;
    next = 0;
  }

  ~LDOM_StringElem ()
  {
    delete [] buf;
    if (next) delete next;
  }
friend class LDOM_SBuffer;
};

//=======================================================================
//function : LDOM_SBuffer()
//purpose  : 
//=======================================================================
LDOM_SBuffer::LDOM_SBuffer (const Standard_Integer theMaxBuf)
     : myMaxBuf (theMaxBuf), myLength(0)
{
  myFirstString = new LDOM_StringElem (theMaxBuf);
  myCurString = myFirstString;
}

//=======================================================================
//function : ~LDOM_SBuffer()
//purpose  : 
//=======================================================================
LDOM_SBuffer::~LDOM_SBuffer ()
{
  if (myFirstString) delete myFirstString;
}

//=======================================================================
//function : Clear()
//purpose  : 
//=======================================================================
void LDOM_SBuffer::Clear ()
{
  if (myFirstString->next) delete myFirstString->next;
  myFirstString->next = 0;
  myFirstString->len = 0;
  myLength = 0;
  myCurString = myFirstString;
}

//=======================================================================
//function : str()
//purpose  : 
//=======================================================================
Standard_CString LDOM_SBuffer::str () const
{
  char* aRetStr = new char [myLength + 1];

  LDOM_StringElem* aCurElem = myFirstString;
  int aCurLen = 0;
  while (aCurElem)
  {
    strncpy(aRetStr + aCurLen, aCurElem->buf, aCurElem->len);
    aCurLen += aCurElem->len;
    aCurElem = aCurElem->next;
  }
  *(aRetStr + myLength) = '\0';

  return aRetStr;
}

//=======================================================================
//function : overflow()
//purpose  : redefined virtual
//=======================================================================
int LDOM_SBuffer::overflow(int c)
{
  char cc = c;
  return xsputn(&cc,1);
}

//=======================================================================
//function : underflow
//purpose  : redefined virtual
//=======================================================================

int LDOM_SBuffer::underflow()
{
  return EOF;
}

//int LDOM_SBuffer::uflow()
//{ return EOF; }

//=======================================================================
//function : xsputn()
//purpose  : redefined virtual
//=======================================================================
int LDOM_SBuffer::xsputn(const char* aStr, int n)
{
  int aLen = n + 1;
  int freeLen = myMaxBuf - myCurString->len - 1;
  if (freeLen >= n)
  {
    strncpy(myCurString->buf + myCurString->len, aStr, aLen);
  }
  else if (freeLen <= 0)
  {
    LDOM_StringElem* aNextElem = new LDOM_StringElem(Max(aLen, myMaxBuf));
    myCurString->next = aNextElem;
    myCurString = aNextElem;
    strncpy(myCurString->buf + myCurString->len, aStr, aLen);
  }
  else // 0 < freeLen < n
  {
    // copy string by parts
    strncpy(myCurString->buf + myCurString->len, aStr, freeLen);
    myCurString->len += freeLen;
    *(myCurString->buf + myCurString->len) = '\0';
    aLen -= freeLen;
    LDOM_StringElem* aNextElem = new LDOM_StringElem(Max(aLen, myMaxBuf));
    myCurString->next = aNextElem;
    myCurString = aNextElem;
    strncpy(myCurString->buf + myCurString->len, aStr + freeLen, aLen);
  }
  myCurString->len += aLen - 1;
  *(myCurString->buf + myCurString->len) = '\0';

  myLength += n;
  return n;
}

//streamsize LDOM_SBuffer::xsgetn(char* s, streamsize n)
//{ return _IO_default_xsgetn(this, s, n); }

//=======================================================================
//function : LDOM_OSStream()
//purpose  : Constructor
//=======================================================================
LDOM_OSStream::LDOM_OSStream (const Standard_Integer theMaxBuf)
     : Standard_OStream (&myBuffer), myBuffer (theMaxBuf)
{
  init(&myBuffer);
}
