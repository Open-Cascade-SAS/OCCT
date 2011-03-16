// File:      Standard_Stream.hxx
// Copyright: Open Cascade 2007
// Purpose:   Includes standard header files containing definition of streams;
//            defines macro USE_STL_STREAM if C++ standard STL streams are used
//            as opposed to obsolete non-standard streams.
//
//            Macro USE_OLD_STREAMS may be defined externally to command using 
//            old streams on WNT; otherwise new streams are used whenever available.
//
//            Macro NO_USING_STD may be defined externally to avoid "using"
//            declaratiions for types from std namespace.

#ifndef _Standard_Stream_HeaderFile
#define _Standard_Stream_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifdef USE_STL_STREAM
#undef USE_STL_STREAM
#endif

// Unix variant
#ifndef WNT

#ifdef HAVE_IOSTREAM
  #include <iostream>
  #define USE_STL_STREAM
#elif defined (HAVE_IOSTREAM_H)
  #include <iostream.h>
#else
  #error "check config.h file or compilation options: either HAVE_IOSTREAM or HAVE_IOSTREAM_H should be defined"
#endif

#ifdef HAVE_IOMANIP
  #include <iomanip>
#elif defined (HAVE_IOMANIP_H)
  #include <iomanip.h>
#endif

#ifdef HAVE_FSTREAM
  #include <fstream>
#elif defined (HAVE_FSTREAM_H)
  #include <fstream.h>
#endif

// Windows variant
#else /* WNT */

// Macro USE_OLD_STREAMS may be defined externally to command 
// using old streams on Windows NT; otherwise new streams are used
#ifndef USE_OLD_STREAMS
  #include <iostream>
  #include <iomanip>
  #include <fstream>
  #define USE_STL_STREAM
#else
  #include <iostream.h>
  #include <iomanip.h>
  #include <fstream.h>
#endif /* USE_OLD_STREAMS */

#endif /* WNT */

// "using" declaration for STL types is still necessary
// as OCCT code contains too much of this staff without std: prefix
#if defined(USE_STL_STREAM) && ! defined(NO_USING_STD)
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::fstream;
using std::filebuf;
using std::streambuf;
using std::streampos;
using std::ios;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ends;
using std::flush;
using std::setw;
using std::setprecision;
using std::hex;
using std::dec;
#endif

#endif /* _Standard_Stream_HeaderFile */
