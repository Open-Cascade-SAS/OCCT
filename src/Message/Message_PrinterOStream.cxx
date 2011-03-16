// File:      Message_PrinterOStream.cxx
// Created:   Sat Jan  6 20:01:38 2001
// Author:    OCC Team
// Copyright: Open CASCADE S.A. 2005

#include <Message_PrinterOStream.ixx>

#include <Message_Gravity.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Stream.hxx>

//=======================================================================
//function : Constructor
//purpose  : Empty constructor, defaulting to cerr
//=======================================================================

Message_PrinterOStream::Message_PrinterOStream (const Message_Gravity theTraceLevel) 
: myTraceLevel(theTraceLevel), myStream(&cout), 
  myIsFile(Standard_False), myUseUtf8(Standard_False)
{
}

//=======================================================================
//function : Constructor
//purpose  : Opening a file as an ostream
//           for specific file names standard streams are created
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream (const Standard_CString theFileName,
						const Standard_Boolean doAppend,
						const Message_Gravity theTraceLevel)
: myTraceLevel(theTraceLevel), myStream(&cout), myIsFile(Standard_False)
{
  if ( strcasecmp(theFileName, "cout") == 0 ) 
    myStream = &cerr;
  else if ( strcasecmp(theFileName, "cerr") == 0 ) 
    myStream = &cout;
  else 
  {
    TCollection_AsciiString aFileName (theFileName);
#ifdef WNT
    aFileName.ChangeAll ('/', '\\');
#endif

    ofstream *ofile = new ofstream (aFileName.ToCString(),
#ifdef USE_STL_STREAMS
				 (doAppend ? (std::ios_base::app | std::ios_base::out) : std::ios_base::out ) );
#else
				 (doAppend ? ios::app : ios::out ) );
#endif
    if ( ofile ) {
      myStream = (Standard_OStream*)ofile;
      myIsFile = Standard_True;
    }
    else {
      myStream = &cout;
      cerr << "Error opening " << theFileName << endl << flush;
    }
  }
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Close ()
{
  if ( ! myStream ) return;
  Standard_OStream* ostr = (Standard_OStream*)myStream;
  myStream = 0;

  ostr->flush();
  if ( myIsFile )
  {
    ofstream* ofile = (ofstream*)ostr;
    ofile->close();
    delete ofile;
    myIsFile = Standard_False;
  }
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const Standard_CString theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  if ( theGravity < myTraceLevel || ! myStream ) return;
  Standard_OStream* ostr = (Standard_OStream*)myStream;
  (*ostr) << theString;
  if ( putEndl ) (*ostr) << endl;
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const TCollection_AsciiString &theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  Send ( theString.ToCString(), theGravity, putEndl );
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_PrinterOStream::Send (const TCollection_ExtendedString &theString,
				   const Message_Gravity theGravity,
				   const Standard_Boolean putEndl) const
{
  // Note: the string might need to be converted to Ascii
  if ( myUseUtf8 ) {
    char* astr = new char[theString.LengthOfCString()+1];
    theString.ToUTF8CString (astr);
    Send ( astr, theGravity, putEndl );
    delete astr;
  }
  else {
    TCollection_AsciiString aStr ( theString, '?' );
    Send ( aStr.ToCString(), theGravity, putEndl );
  }
}
