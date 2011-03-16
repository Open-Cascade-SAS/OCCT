//
//-- File:        MFT_FontManager.cxx
//-- Created:     Fri Mar 7 11:10:20 1997
//-- Author:      Gerard GRAS 
//--              <gg@photox> 
//-- Update:	  gg 19-01-98
//--	reason :
//		Suppress global static class variables from this class
//		LOADER optimisation.
#define S3593	//gg 13-03-98
//--	reason :
//		OPTIMISATION MFT
//		-utilisation optimum de MMAP.
//		-rendre statique la declaration des types importes.
#define CTS20718 //syl 04-09-98
//              -meilleure precision sur la vectorisation des polices courier ...
#define PRO17604 //GG_210199
//		Avoid to crash at object creation time due to wrong
//		sprintf() call
// JR 10.02.100 : OSD_FileIterator files(pathdir,"*.mft");
//                g++(Linux) : no matching function(OSD_Path()(),char[6])

//		FMN 11-01-99
//
//---Copyright:    Matra Datavision 1993
//
//---Comments :
//			MFT File format
//			---------------
//
// An xxx.mft file contains an HEADER of size 512 bytes (first record),
//			    a CHAR ENTRIES VECTOR of size 512 or 65536 int
//						        (second record)
//			    a COMMAND SECTION of size 512 * N (next records)
//			    according to the MFT_COMMANDBUFFERSIZE parameter.
//
// The xxx.mft name of the file depends directly of the xxx alias name of the
// font (i.e: Helvetica-Bold.mft)
//
//			Header format			(Record 0)
//			#############
//
//  abs byte location	component	type		comments
//  -----------------	---------	----		--------
//  0			signature 	uint		MFT file signature.
//  4			iscomposite	bool		True for extended font.
//  8  			pentries	int 		Char entries section address 
//  12  		pcommands	int		Command section address
//  16  		freeSpace 	int		Free space address
//  20  		fontBox		int array	Font bounding box
//  36			paintType	int		Font paint type
//  40  		version		int		File creation ID
//  44  		fixedPitch	bool	 	False if proportionnal	
//  48  		fontMatrix	int array	Font matrix 
//  72			italicAngle	int		Font Italic Angle
//  76			dummy		char array	adjustable dummy
//  ??  		fontName	char array	Full font name.
//
//			Char entries vector format	(Record 1)
//			##########################
//
//  abs byte location	component	type		comments
//  -----------------	---------	----		--------
//  512			fcommand 	int array	First command descriptor vector address.
//
//			Command section format		(record 2 at ....)
//			######################
//
//  abs byte location	component	type		comments
//  -----------------	---------	----		--------
//  2100 or 262656 	command header 	bit field	First command descriptor
//     ...		
//  EOF			End of file			Free space address
//
//			Command format		
//			##############
//
//  rel byte location	component	type		comments
//  -----------------	---------	----		--------
//  0 			value1_type 	bit field:2	1:int,2:float,3:string	
//  0 			value2_type 	bit field:2	1:int,2:float,3:string	
//  0 			value3_type 	bit field:2	1:int,2:float,3:string	
//  0 			value4_type 	bit field:2	1:int,2:float,3:string	
//  1 			value5_type 	bit field:2	1:int,2:float,3:string	
//  1 			value6_type 	bit field:2	1:int,2:float,3:string	
//  1 			value7_type 	bit field:2	1:int,2:float,3:string	
//  1 			value8_type 	bit field:2	1:int,2:float,3:string	
//  2 			length 		bit field:8	max 8	
//  3 			type 		bit field:8	MFT_TypeOfCommand
//
//  4			value1		card32		int,float or string value
// ...
//  4+4*(N-1)  		valueN		card32
//

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <MFT_FontManager.ixx>
#include <MFT.hxx>
#include <MFT_ListOfFontName.hxx>
#include <MFT_ListOfFontHandle.hxx>
#include <MFT_ListOfFontReference.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_Environment.hxx>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if defined(HAVE_SYS_STAT_H) || defined(WNT)
# include <sys/stat.h>
#endif

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

//        --------------------------------
//        -- Category: Local constants 
//        --------------------------------

#define TRACE 0
#define TRACE_MMAP 0
//#define MFT_VERSION 970605
#define MFT_VERSION 970623
#define MFT_DUMMYSIZE 180
#define MFT_MAXFONTNAME 256
#define MFT_SIGNATURE 0x30c730a3
#define MFT_MAXASCIICHARENTRIES 256
#define MFT_MAXSEACCHARENTRIES 256
#define MFT_MAXEXTENDEDCHARENTRIES 65536
#define MFT_MAXVALUES 8
#define MFT_MAXSTACKVALUES 16

//        --------------------------------
//        -- Category: Local structures 
//        --------------------------------


#define MFT_COMMANDBUFFERSIZE 512
typedef struct _MFT_CommandBuffer {
    char command[MFT_COMMANDBUFFERSIZE];
} MFT_CommandBuffer;

typedef union _MFT_CommandValue {
    int i;
    float f;
    char s[4];
    MFT_CommandDescriptor d;
} MFT_CommandValue;

typedef struct _MFT_AsciiCharEntries {		//First command entries
    MFT_FilePosition fcommand[MFT_MAXASCIICHARENTRIES+MFT_MAXSEACCHARENTRIES];
} MFT_AsciiCharEntries;

typedef struct _MFT_ExtendedCharEntries {	//First command entries
    MFT_FilePosition fcommand[MFT_MAXEXTENDEDCHARENTRIES];
} MFT_ExtendedCharEntries;

typedef union _MFT_CharEntries {
      MFT_AsciiCharEntries ascii;
      MFT_ExtendedCharEntries extended;
      MFT_FilePosition fcommand[1];
} MFT_CharEntries;

typedef struct _MFT_FileHeader {
    unsigned int signature;
    Standard_Boolean iscomposite;	//True if this is an extended font.
    MFT_FilePosition pentries;		//Starting char entries section address 
    MFT_FilePosition pcommands;		//Starting command section address
    MFT_FilePosition freeSpace;		//Free space address
    Standard_Integer fontBox[4];	//Bounding box of the MAX character size
    Standard_Integer paintType;		//Font paint type
    Standard_Integer version;		//File creation ID
    Standard_Boolean fixedPitch;	//Font proportionnal flag
    Standard_Integer fontMatrix[6];	//Font matrix * FTOI 
    Standard_Integer italicAngle;	//Font italic angle
    Standard_Character dummy[MFT_DUMMYSIZE];
    Standard_Character fontName[MFT_MAXFONTNAME];
} MFT_FileHeader;


//        -----------------------------
//        -- Category: Static variables
//        -----------------------------

static MFT_ListOfFontName& _theListOfOpenFontName() {
static MFT_ListOfFontName theListOfOpenFontName; 
  return theListOfOpenFontName;
}
#define theListOfOpenFontName _theListOfOpenFontName()

static MFT_ListOfFontHandle& _theListOfOpenFontHandle() { 
static MFT_ListOfFontHandle theListOfOpenFontHandle; 
  return theListOfOpenFontHandle;
}
#define theListOfOpenFontHandle _theListOfOpenFontHandle()

static MFT_ListOfFontReference& _theListOfOpenFontReference() { 
static MFT_ListOfFontReference theListOfOpenFontReference;
  return theListOfOpenFontReference;
}
#define theListOfOpenFontReference _theListOfOpenFontReference()

static MFT_ListOfFontName& _theListOfFontName() {
static MFT_ListOfFontName theListOfFontName;
  return theListOfFontName;
}
#define theListOfFontName _theListOfFontName()

static Handle(MFT_TextManager)& _theTextManager() {
static Handle(MFT_TextManager) theTextManager = new MFT_TextManager();
  return theTextManager;
}
#define theTextManager _theTextManager()

static OSD_Environment& _CSF_MDTVFontDirectory() {
static OSD_Environment CSF_MDTVFontDirectory("CSF_MDTVFontDirectory");
  return CSF_MDTVFontDirectory;
}
#define CSF_MDTVFontDirectory _CSF_MDTVFontDirectory()

static OSD_Environment& _CSF_MDTVFontPrecision() {
static OSD_Environment CSF_MDTVFontPrecision("CSF_MDTVFontPrecision");
  return CSF_MDTVFontPrecision;
}
#define CSF_MDTVFontPrecision _CSF_MDTVFontPrecision()

static TCollection_AsciiString& _theAsciiString() {
static TCollection_AsciiString theAsciiString;
  return theAsciiString;
}
#define theAsciiString _theAsciiString()

static TCollection_ExtendedString& _theExtendedString() {
static TCollection_ExtendedString theExtendedString;
  return theExtendedString;
}
#define theExtendedString _theExtendedString()

static Standard_Integer theCharCount = 0;
static Standard_Integer theCharPosition = 0;
static Standard_Integer theStringLength = 0;
static Standard_Boolean theStringIsAscii = Standard_True;
static MFT_CommandDescriptor theCommand;
static MFT_TypeOfValue theValueType = MFT_TOV_UNKNOWN;
static MFT_TypeOfCommand theCommandType = MFT_TOC_UNKNOWN;
static Standard_Integer theCommandLength = 0;
static MFT_FilePosition theFirstCommandPosition,theNextCommandPosition,theCommandPosition;
static Standard_Integer theValueCount = 0;
static Standard_Integer theStackCount = 0;
static Standard_Integer theIStack[MFT_MAXSTACKVALUES];
static Standard_Real theRStack[MFT_MAXSTACKVALUES];
static Standard_Integer theIValues[MFT_MAXVALUES];
static Standard_Real theFValues[MFT_MAXVALUES];
static Standard_Character theSValues[MFT_MAXVALUES*sizeof(MFT_CommandValue)];
static Standard_Real theDeltaX,theCharX,theCharY,theOldCharX,theOldCharY;
static Standard_Real theLeftCharX,theLeftCharY,theCharWidth,theCharHeight;
static Standard_Real theStringX,theStringY,theFixedCharWidth;
static Standard_Integer theSeacBaseChar,theSeacAccentChar;
static Standard_Real theSeacAsbValue,theSeacAdxValue,theSeacAdyValue;
static Standard_Real theTanCharSlant;
static Standard_Real theSinCharAngle,theCosCharAngle;
static Standard_Real theFontXRatio,theFontYRatio;
static Standard_Real theCurveT,theCurveX0,theCurveAX,theCurveBX,theCurveCX;
static Standard_Real theCurveDT,theCurveY0,theCurveAY,theCurveBY,theCurveCY;
static Standard_Real theCurvePrecision = 10.0,theCurveFlatness = 1.0;
static Standard_Integer theCurveNT;
static Standard_Boolean theDotSection,theMDTVFontPrecision = Standard_False;
static Standard_Real theOrientation;

#ifdef MMAP
#ifndef WNT
static long int thePageSize = sysconf(_SC_PAGESIZE);
#else
////////////////////////////// WNT MMAP (MUNMAP) //////////////////////////
#define STRICT
#ifndef NODRAWTEXT
#define NODRAWTEXT
#endif
#include <io.h>
#include <windows.h>

DWORD WNT_PageSize ()         { SYSTEM_INFO si; GetSystemInfo (&si);
                                return 32*si.dwPageSize; }
DWORD WNT_AllocGranularity () { SYSTEM_INFO si; GetSystemInfo (&si);
                                return si.dwAllocationGranularity; }

static long thePageSize = WNT_PageSize ();
static long theAllocGranularity = WNT_AllocGranularity ();

/*====================================================*/
void errstr (char* str, DWORD errcode)
{
  LPVOID lpMsgBuf;
  FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, errcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR) &lpMsgBuf, 0,  NULL);
  cout << str << ":\n" << (char*)lpMsgBuf << endl;
  LocalFree (lpMsgBuf);
}

/*====================================================*/
static HANDLE hFileMapping = NULL;
LPVOID mmap(off_t file_addr, int mmap_size, int fd)
{
  register LPVOID lpMapAddress = NULL;
  register HANDLE hFile        = (HANDLE)_get_osfhandle (fd);
  register DWORD  dwMapStart   = (file_addr / theAllocGranularity) * theAllocGranularity;
  register DWORD  dwMapOffset  = (file_addr % theAllocGranularity);
  register DWORD  dwFileSize   = ((long)hFile != -1 ? GetFileSize(hFile,NULL) : 0xFFFFFFFF);
  register long   dwLength     = 0;
//  register char   strH[10];
//  sprintf (strH, "%05d", (int)hFile);
  ////////////////////////////////////
//  cout << ">> HANDLE: " << strH << ", " << fd << endl;
//  cout << "MMAP (OFF: " << file_addr << ", LEN: " << thePageSize << ", MSTR: "
//       << dwMapStart << ", MOFF: " << dwMapOffset << ", SIZE: " << dwFileSize << ");\n";
//  hFile = (HANDLE)_get_osfhandle (fd);
  if ((long)hFile != -1) {
    hFileMapping = CreateFileMapping (hFile, NULL, PAGE_WRITECOPY, 0, 0, NULL );
    if (hFileMapping == NULL) {
      errstr(">>>>> MMAP: CreateMap failed", GetLastError());
      return (LPVOID)MAP_FAILED;
    }
    if (hFileMapping != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
      errstr(">>>>> MMAP: Map already exist", GetLastError());
      CloseHandle(hFileMapping);
      return (LPVOID)MAP_FAILED;
    }
    dwLength = (file_addr + mmap_size < (long)dwFileSize ?
                  dwMapOffset + mmap_size : 0/*dwMapOffset+dwFileSize-file_addr*/);
//    cout << "LENGTH: " << dwLength << endl;
    lpMapAddress = MapViewOfFile (hFileMapping, FILE_MAP_COPY, 0, dwMapStart, dwLength);
    if (lpMapAddress == NULL) {
      errstr(">>>>> MMAP: MapView failed", GetLastError());
      CloseHandle (hFileMapping);
      return (LPVOID)MAP_FAILED;
    }
    return (LPVOID)((LPSTR)lpMapAddress+dwMapOffset);
  } else {
    return (LPVOID)MAP_FAILED;
  }
}

/*====================================================*/
int munmap(LPVOID addr)
{
  if (addr) {
    UnmapViewOfFile (addr);
    CloseHandle (hFileMapping);
    hFileMapping = NULL;
  }
  return (0);
}

////////////////////////////// WNT MMAP (MUNMAP) //////////////////////////
#endif // WNT
#endif

//        ------------------------------
//        -- Category: Inline procedures
//        ------------------------------
//

#define ROUND(v) Standard_Integer( ((v) > 0.) ? (v)+.5 : (v)-.5 )

// To calculate how many points to sample along a path in order to
// approximate it to the desired degree of flatness, we define
//      dist((x,y)) = abs(x) + abs(y);
// then the number of points we need is
//      N = 1 + sqrt(3/4 * D / flatness),
// where
//      D = max(dist((p0 - 2*p1 + p2), (p1 - 2*p2 + p3))).
//
#if 1
#define SETCURVE(dx10,dy10,dx21,dy21,dx32,dy32) \
	theCurveX0 = theOldCharX = theCharX; \
	theCurveY0 = theOldCharY = theCharY; \
        theCurveCX = 3.0 * dx10; \
        theCurveBX = 3.0 * (dx21 - dx10); \
        theCurveAX = dx32 - 2.0 * dx21 + dx10; \
        theCurveCY = 3.0 * dy10; \
        theCurveBY = 3.0 * (dy21 - dy10); \
        theCurveAY = dy32 - 2.0 * dy21 + dy10; \
        theCurveT = 0.0; \
	{ Standard_Real dd,dx0,dy0,dx1,dy1; \
	  dx0 = dx21-dx10; dy0 = dy21-dy10; \
	  dx1 = dx32-dx21; dy1 = dy32-dy21; \
	  dd = Max(Abs(dx0),Abs(dx1)) + Max(Abs(dy0),Abs(dy1)); \
	  theCurveNT = 4 + ROUND(Sqrt(3./4. * dd / theCurveFlatness)); \
	  theCurveDT = 1.0 / theCurveNT; \
	}
#else
#define SETCURVE(dx10,dy10,dx21,dy21,dx32,dy32) theCurveX0 = theOldCharX = theCharX; theCurveY0 = theOldCharY = theCharY; theCurveCX = 3.0 * dx10; theCurveBX = 3.0 * (dx21 - dx10); theCurveAX = dx32 - 2.0 * dx21 + dx10; theCurveCY = 3.0 * dy10; theCurveBY = 3.0 * (dy21 - dy10); theCurveAY = dy32 - 2.0 * dy21 + dy10; theCurveT = 0.0; { Standard_Real dd,dx0,dy0,dx1,dy1; dx0 = dx21-dx10; dy0 = dy21-dy10; dx1 = dx32-dx21; dy1 = dy32-dy21; dd = Max(Abs(dx0),Abs(dx1)) + Max(Abs(dy0),Abs(dy1)); theCurveNT = 4 + ROUND(Sqrt(3./4. * dd / theCurveFlatness)); theCurveDT = 1.0 / theCurveNT; }
#endif

#if 1
#define GETCURVE(T) \
        theCharX = ((theCurveAX * T + theCurveBX) * T + \
                                        theCurveCX) * T + theCurveX0; \
        theCharY = ((theCurveAY * T + theCurveBY) * T + \
                                        theCurveCY) * T + theCurveY0;
#else
#define GETCURVE(T) theCharX = ((theCurveAX * T + theCurveBX) * T + theCurveCX) * T + theCurveX0; theCharY = ((theCurveAY * T + theCurveBY) * T + theCurveCY) * T + theCurveY0;
#endif

#if 1
#define CHECKCOMMAND(np) \
	if( theCommandLength != np ) { \
	  cout << " WARNING on MFT_Vertex : BAD command length " << \
		  theCommandLength << "/" << np << " on " <<  \
		    MFT::Convert(theCommandType) << \
		      " from char position " << theCharPosition << endl; \
	  vazy = Standard_True; \
	  break; \
	}
#else
#define CHECKCOMMAND(np) if( theCommandLength != np ) { cout << " WARNING on MFT_Vertex : BAD command length " << theCommandLength << "/" << np << " on " <<  MFT::Convert(theCommandType) << " from char position " << theCharPosition << endl; vazy = Standard_True; break; }
#endif

#if 1
#define IPUSH(value) \
	if( theStackCount < MFT_MAXSTACKVALUES ) { \
	  theIStack[theStackCount] = value; theStackCount++; \
	} else { \
	  cout << " ERROR in MFT.STACK Overflow" << endl; \
	}
#else
#define IPUSH(value) if( theStackCount < MFT_MAXSTACKVALUES ) { theIStack[theStackCount] = value; theStackCount++; } else { cout << " ERROR in MFT.STACK Overflow" << endl; }
#endif

#if 1
#define IPOP(value) \
	if( theStackCount > 0 ) { \
	  --theStackCount; value = theIStack[theStackCount]; \
	} else { \
	  cout << " ERROR in MFT.STACK Underflow" << endl; \
	}
#else
#define IPOP(value) if( theStackCount > 0 ) { --theStackCount; value = theIStack[theStackCount]; } else { cout << " ERROR in MFT.STACK Underflow" << endl; }
#endif

#if 1
#define RPUSH(value) \
	if( theStackCount < MFT_MAXSTACKVALUES ) { \
	  theRStack[theStackCount] = value; theStackCount++; \
	} else { \
	  cout << " ERROR in MFT.STACK Overflow" << endl; \
	}
#else
#define RPUSH(value) if( theStackCount < MFT_MAXSTACKVALUES ) { theRStack[theStackCount] = value; theStackCount++; } else { cout << " ERROR in MFT.STACK Overflow" << endl; }
#endif

#if 1
#define RPOP(value) \
	if( theStackCount > 0 ) { \
	  --theStackCount; value = theRStack[theStackCount]; \
	} else { \
	  cout << " ERROR in MFT.STACK Underflow" << endl; \
	}
#else
#define RPOP(value) if( theStackCount > 0 ) { --theStackCount; value = theRStack[theStackCount]; } else { cout << " ERROR in MFT.STACK Underflow" << endl; }
#endif

#if 1
#define TRANSFORM(X,Y) \
	{ Standard_Real x = X * theFontXRatio,y = Y * theFontYRatio; \
    	  if( myCharSlant != 0. ) x = x + y*theTanCharSlant; \
    	  if( theOrientation != 0. ) { \
      	    Standard_Real xx = x,yy = y; \
            x = xx*theCosCharAngle - yy*theSinCharAngle; \
            y = xx*theSinCharAngle + yy*theCosCharAngle; \
    	  } \
    	  X = theStringX + x; Y = theStringY + y; \
	}
#else
#define TRANSFORM(X,Y) { Standard_Real x = X * theFontXRatio,y = Y * theFontYRatio; if( myCharSlant != 0. ) x = x + y*theTanCharSlant; if( theOrientation != 0. ) { Standard_Real xx = x,yy = y; x = xx*theCosCharAngle - yy*theSinCharAngle; y = xx*theSinCharAngle + yy*theCosCharAngle; } X = theStringX + x; Y = theStringY + y; }
#endif

#define FTOI(v) Standard_Integer((v)*1000000.)
#define ITOF(i) (Standard_ShortReal((i))/1000000.F)

#if 1
#define SWAPINT(v) \
    { \
      Standard_Character c; \
      Standard_PCharacter s = (Standard_PCharacter)&v; \
      c = s[0]; s[0] = s[3]; s[3] = c; \
      c = s[1]; s[1] = s[2]; s[2] = c; \
    }
#else
#define SWAPINT(v) { Standard_Character c; Standard_PCharacter s = (Standard_PCharacter)&v; c = s[0]; s[0] = s[3]; s[3] = c; c = s[1]; s[1] = s[2]; s[2] = c; }
#endif

#if 1
#define SWAPBOOL(v) \
    { \
      Standard_Character c; \
      Standard_PCharacter s = (Standard_PCharacter)&v; \
      c = s[0]; s[0] = s[3]; s[3] = c; \
      c = s[1]; s[1] = s[2]; s[2] = c; \
    }
#else
#define SWAPBOOL(v) { Standard_PCharacter c; Standard_PCharacter s = (Standard_PCharacter)&v; c = s[0]; s[0] = s[3]; s[3] = c; c = s[1]; s[1] = s[2]; s[2] = c; }
#endif

#if 1
#define SWAPFLOAT(v) \
    { \
      Standard_Character c; \
      Standard_PCharacter s = (Standard_PCharacter)&v; \
      c = s[0]; s[0] = s[3]; s[3] = c; \
      c = s[1]; s[1] = s[2]; s[2] = c; \
    }
#else
#define SWAPFLOAT(v) { Standard_Character c; Standard_PCharacter s = (Standard_PCharacter)&v; c = s[0]; s[0] = s[3]; s[3] = c; c = s[1]; s[1] = s[2]; s[2] = c; }
#endif

#if 1
#define SWAPPOSITION(v) \
    { \
      Standard_Character c; \
      Standard_PCharacter s = (Standard_PCharacter)&v; \
      c = s[0]; s[0] = s[3]; s[3] = c; \
      c = s[1]; s[1] = s[2]; s[2] = c; \
    }
#else
#define SWAPPOSITION(v) { Standard_Character c; Standard_PCharacter s = (Standard_PCharacter)&v; c = s[0]; s[0] = s[3]; s[3] = c; c = s[1]; s[1] = s[2]; s[2] = c; }
#endif

//        --------------------------------
//        -- Category: Fields explanations 
//        --------------------------------

//        myFileName:             AsciiString from TCollection;
//        myFileMode:             OpenMode from OSD;
//	  myFileHandle:           Normally >= 0 when the file is open.
//        myIsComposite:          TRUE if the font contains 
//				    extended chars.
//        myIsFixedPitch:         FALSE if the font is proportionnal. 
//	  myItalicAngle:	  angle d'inclinaison des caracteres.
//	  myPaintType:		  0 for filled fonts 2 for stroke fonts
//        myCharWidth:            Current text attribute
//				    for retrieve methods.
//        myCharHeight:           Current text attribute
//				    for retrieve methods.
//        myCharSlant:            Current text attribute
//				    for retrieve methods.
//        myCharPrecision:        Current text attribute
//				    for retrieve methods.
//        myCharCapsHeight:       Current text attribute
//				    for retrieve methods.
//        myFileHeader:           Current file header record
//				    to read or write from/to the file
//        myCharEntries:          Current char entries record
//				    to read or write from/to the file
//        myCommandBuffer:        Current command buffer record
//				    to read or write from/to the file


//
//        -------------------------
//        -- Category: Constructors
//        -------------------------
//
// =============================================================================
//        ---Purpose: Gives access to the font <anAliasName> associated
//        --          to the file $CSF_MDTVFontDirectory/<anAliasName>.mft
//        --          with the open mode <aFileMode>.
//        ---Warning: If the symbol CSF_MDTVFontDirectory is not defined
//        --          try to reads or writes in $PWD directory.
//        ---Example: myFontManager = new MFT_FontManager("Helvetica-Bold")
//        --          permits to access to the file
//        --          $CSF_MDTVFontDirectory/"helvetica-bold.mft"
//        raises FontManagerDefinitionError from MFT;
//        ---Trigger: If the font file don't exist or if the file
//        --          don't have an MFT signature.
// =============================================================================


MFT_FontManager::MFT_FontManager(const Standard_CString anAliasName) : 
myFileName(anAliasName),
myFileMode(OSD_ReadOnly),
myIsFixedPitch(Standard_False),
myItalicAngle(0.F),
myPaintType(0),
#ifdef CTS20718
myCharWidth(1.F),
myCharHeight(1.F),
myCharSlant(0.F),
myCharPrecision(0.002F),
#else
myCharWidth(1.F),
myCharHeight(1.F),
myCharSlant(0.F),
myCharPrecision(0.05F),
#endif
myCharCapsHeight(Standard_True)
{
    
#if TRACE > 0
    cout << " MFT_FontManager::MFT_FontManager('" << myFileName << "')" << endl;
#endif

    myFileHandle = Open(myFileName,myFileMode);
#ifdef PRO17604
    if( myFileHandle < 0 ) {
      char message[512];
      sprintf(message,"BAD MFT font name '%s'",myFileName.ToCString());
      MFT_FontManagerDefinitionError::Raise(message);
    }
#else
    char message[80];
    sprintf(message,"BAD MFT font name '%s'",myFileName);
    MFT_FontManagerDefinitionError_Raise_if(myFileHandle < 0,message);
#endif

				//Retrieves header section.
    myFileHeader.fileHandle = myFileHandle;
    myFileHeader.beginPosition = myFileHeader.recordPosition = 0;
    myFileHeader.recordSize = sizeof(MFT_FileHeader);
    myFileHeader.update = Standard_False;
    myFileHeader.precord = NULL;
    myFileHeader.precordMMAPAddress = NULL;
    myFileHeader.recordMMAPSize = 0;
    myFileHeader.swap = Standard_False;
    myCharEntries.fileHandle = myFileHandle;
    myCharEntries.update = Standard_False;
    myCharEntries.precord = NULL;
    myCharEntries.precordMMAPAddress = NULL;
    myCharEntries.recordMMAPSize = 0;
    myCharEntries.swap = Standard_False;
    myCommandBuffer.fileHandle = myFileHandle;
    myCommandBuffer.recordSize = sizeof(MFT_CommandBuffer);
    myCommandBuffer.update = Standard_False;
    myCommandBuffer.precord = NULL;
    myCommandBuffer.precordMMAPAddress = NULL;
#ifdef MMAP
    myCommandBuffer.recordMMAPSize = thePageSize;
#endif
    myCommandBuffer.swap = Standard_False;
    Standard_Boolean status = Read(myFileHeader);
    MFT_FontManagerDefinitionError_Raise_if(!status,"HEADER Read error");

    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    MFT_FontManagerDefinitionError_Raise_if(!pfileheader,"BAD MFT header");
    if( pfileheader->signature != MFT_SIGNATURE ) {
      SWAPINT(pfileheader->signature);
      SWAPBOOL(pfileheader->iscomposite);
      SWAPPOSITION(pfileheader->pentries);
      SWAPPOSITION(pfileheader->pcommands);
      SWAPPOSITION(pfileheader->freeSpace);
      SWAPINT(pfileheader->fontBox[0]);
      SWAPINT(pfileheader->fontBox[1]);
      SWAPINT(pfileheader->fontBox[2]);
      SWAPINT(pfileheader->fontBox[3]);
      SWAPINT(pfileheader->paintType);
      SWAPINT(pfileheader->version);
      SWAPBOOL(pfileheader->fixedPitch);
      SWAPINT(pfileheader->fontMatrix[0]);
      SWAPINT(pfileheader->fontMatrix[1]);
      SWAPINT(pfileheader->fontMatrix[2]);
      SWAPINT(pfileheader->fontMatrix[3]);
      SWAPINT(pfileheader->fontMatrix[4]);
      SWAPINT(pfileheader->fontMatrix[5]);
      SWAPINT(pfileheader->italicAngle);
      myFileHeader.swap = Standard_True;
      myCharEntries.swap = Standard_True;
      myCommandBuffer.swap = Standard_True;
    }
    MFT_FontManagerDefinitionError_Raise_if(
	    pfileheader->signature != MFT_SIGNATURE,"BAD MFT signature");

				//Gets char entries section 
    myCharEntries.beginPosition = 
	  myCharEntries.recordPosition = pfileheader->pentries;
    myIsComposite = pfileheader->iscomposite;
    if( myIsComposite ) {
      myCharEntries.recordSize = sizeof(MFT_ExtendedCharEntries);
    } else {
      myCharEntries.recordSize = sizeof(MFT_AsciiCharEntries);
    }
    myPaintType = pfileheader->paintType;
    myIsFixedPitch = pfileheader->fixedPitch;
    myItalicAngle = ITOF(pfileheader->italicAngle);
    status = Read(myCharEntries);
    MFT_FontManagerDefinitionError_Raise_if(!status,"CHAR ENTRIES Read error");

    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    MFT_FontManagerDefinitionError_Raise_if(
	    !pcharentries, "BAD char entries address");
    if( myCharEntries.swap ) {
      for( Standard_Integer i=0 ;
           i < (int)( myCharEntries.recordSize/sizeof(Standard_Integer) ); i++ ) {
        SWAPINT(pcharentries->fcommand[i]);
      }
    }

				//Fill empty command buffer.
    myCommandBuffer.beginPosition =
		myCommandBuffer.recordPosition = pfileheader->pcommands;
    MFT_FontManagerDefinitionError_Raise_if(
	    sizeof(MFT_CommandDescriptor) != 4, 
			"BAD command descriptor implementation");

    TCollection_AsciiString env(CSF_MDTVFontPrecision.Value());
    if( env.Length() > 0 ) {
      Standard_ShortReal flatness,precision;
      Standard_Integer n = sscanf(env.ToCString(),"%f %f",&flatness,&precision);
      if( n > 0 && flatness > 0. ) {
	  theMDTVFontPrecision = Standard_True;
	  theCurveFlatness = flatness;
          theCurvePrecision = flatness * 10.;
	  if( precision > 0. ) theCurvePrecision = precision;
      }
      if( theMDTVFontPrecision ) {
	cout << " CSF_MFTVFontPrecision is " << theCurveFlatness << "," 
						<< theCurvePrecision << endl;
      }
    }

#ifdef S3593
    myCommandBuffer.recordMMAPSize = pfileheader->freeSpace;
#endif

}

// =============================================================================
//        ---Purpose: Gives access to the font <aFont> associated
//        --          to the file $CSF_MDTVFontDirectory/<aFont.AliasName()>.mft
//        --          with the open mode <aFileMode>.
//        ---Warning: If the symbol CSF_MDTVFontDirectory is not defined
//        --          try to reads or writes in $PWD directory.
//        raises FontManagerDefinitionError from MFT;
//        ---Trigger: If <aFileMode> is ReadOnly or ReadWrite and
//        --          the font file don't exist or if the file
//        --          don't have an MFT signature.
// =============================================================================

MFT_FontManager::MFT_FontManager(const Aspect_FontStyle& aFont, const OSD_OpenMode aFileMode,const Standard_Boolean isComposite) : 
myFileName(aFont.AliasName()),
myFileMode(aFileMode),
myIsFixedPitch(Standard_False),
myItalicAngle(0.F),
myPaintType(0),
#ifdef CTS20718
myCharWidth(1.F),
myCharHeight(1.F),
myCharSlant(0.F),
myCharPrecision(0.002F),
#else
myCharWidth(1.F),
myCharHeight(1.F),
myCharSlant(0.F),
myCharPrecision(0.05F),
#endif
myCharCapsHeight(Standard_True)
{
#if TRACE > 0
    cout << " MFT_FontManager::MFT_FontManager('" << aFont.AliasName() << 
		"'," << Standard_Integer(aFileMode) << ")" << endl;
#endif

    myFileHandle = Open(myFileName,myFileMode);
#ifdef PRO17604
    if( myFileHandle < 0 ) {
      char message[512];
      sprintf(message,"BAD MFT font name '%s'",myFileName.ToCString());
      MFT_FontManagerDefinitionError::Raise(message);
    }
#else
    char message[80];
    sprintf(message,"BAD MFT font name '%s'",myFileName);
    MFT_FontManagerDefinitionError_Raise_if( myFileHandle < 0,message);
#endif

    MFT_FileHeader *pfileheader = NULL; 
    MFT_CharEntries *pcharentries = NULL;

    myFileHeader.fileHandle = myFileHandle;
    myFileHeader.beginPosition = myFileHeader.recordPosition = 0;
    myFileHeader.recordSize = sizeof(MFT_FileHeader);
    myFileHeader.update = Standard_False;
    myFileHeader.precord = NULL;
    myFileHeader.swap = Standard_False;
    myFileHeader.precordMMAPAddress = NULL;
    myFileHeader.recordMMAPSize = 0;
    myCharEntries.fileHandle = myFileHandle;
    myCharEntries.update = Standard_False;
    myCharEntries.precord = NULL;
    myCharEntries.swap = Standard_False;
    myCharEntries.precordMMAPAddress = NULL;
    myCharEntries.recordMMAPSize = 0;
    myCommandBuffer.fileHandle = myFileHandle;
    myCommandBuffer.recordSize = sizeof(MFT_CommandBuffer);
    myCommandBuffer.update = Standard_False;
    myCommandBuffer.precord = NULL;
    myCommandBuffer.swap = Standard_False;
    myCommandBuffer.precordMMAPAddress = NULL;
    myCommandBuffer.recordMMAPSize = 0;

    switch (myFileMode) {
      case OSD_ReadOnly:
#ifdef MMAP
        myCommandBuffer.recordMMAPSize = thePageSize;
#endif
      case OSD_ReadWrite:
	{
				//Retrieves header section.
	Standard_Boolean status = Read(myFileHeader);
        MFT_FontManagerDefinitionError_Raise_if(!status,"HEADER Read error");

        pfileheader = (MFT_FileHeader*) myFileHeader.precord;
        MFT_FontManagerDefinitionError_Raise_if(!pfileheader,"BAD MFT header");
        if( pfileheader->signature != MFT_SIGNATURE ) {
          SWAPINT(pfileheader->signature);
          SWAPBOOL(pfileheader->iscomposite);
          SWAPPOSITION(pfileheader->pentries);
          SWAPPOSITION(pfileheader->pcommands);
          SWAPPOSITION(pfileheader->freeSpace);
          SWAPINT(pfileheader->fontBox[0]);
          SWAPINT(pfileheader->fontBox[1]);
          SWAPINT(pfileheader->fontBox[2]);
          SWAPINT(pfileheader->fontBox[3]);
          SWAPINT(pfileheader->paintType);
          SWAPINT(pfileheader->version);
          SWAPBOOL(pfileheader->fixedPitch);
          SWAPINT(pfileheader->fontMatrix[0]);
          SWAPINT(pfileheader->fontMatrix[1]);
          SWAPINT(pfileheader->fontMatrix[2]);
          SWAPINT(pfileheader->fontMatrix[3]);
          SWAPINT(pfileheader->fontMatrix[4]);
          SWAPINT(pfileheader->fontMatrix[5]);
          SWAPINT(pfileheader->italicAngle);
          myFileHeader.swap = Standard_True;
          myCharEntries.swap = Standard_True;
          myCommandBuffer.swap = Standard_True;
        }
        MFT_FontManagerDefinitionError_Raise_if(
	    pfileheader->signature != MFT_SIGNATURE,"BAD MFT signature");

        myIsComposite = pfileheader->iscomposite;
				//Retrieves char entries section 
        if( myIsComposite ) {
          myCharEntries.recordSize = sizeof(MFT_ExtendedCharEntries);
        } else {
          myCharEntries.recordSize = sizeof(MFT_AsciiCharEntries);
        }
        myPaintType = pfileheader->paintType;
        myIsFixedPitch = pfileheader->fixedPitch;
	      myItalicAngle = ITOF(pfileheader->italicAngle);
        myCharEntries.beginPosition =
		myCharEntries.recordPosition = pfileheader->pentries;
	status = Read(myCharEntries);
    	MFT_FontManagerDefinitionError_Raise_if(!status,"CHAR ENTRIES Read error");

    	pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    	MFT_FontManagerDefinitionError_Raise_if(
	    !pcharentries, "BAD char entries address");
        if( myCharEntries.swap ) {
          for( Standard_Integer i=0 ;
               i < (int)( myCharEntries.recordSize/sizeof(Standard_Integer) ); i++ ) {
            SWAPINT(pcharentries->fcommand[i]);
          }
        }

				//Retrieves command buffer section 
        myCommandBuffer.beginPosition =
		myCommandBuffer.recordPosition = pfileheader->pcommands;
#ifdef S3593
        if( myCommandBuffer.recordMMAPSize > 0 )
			myCommandBuffer.recordMMAPSize = pfileheader->freeSpace;
#endif
	}
      	break;
      case OSD_WriteOnly:
        {
	          //Builds file header section 
          myFileHeader.precord = (Standard_CString) malloc(myFileHeader.recordSize); 
          pfileheader = (MFT_FileHeader*)myFileHeader.precord; 
            Standard_OutOfMemory_Raise_if(
                    !pfileheader,"MEMORY allocation failed");
            Standard_CString pfontname = aFont.FullName();
            Standard_Integer length = Min(MFT_MAXFONTNAME-1,strlen(pfontname));

            myIsComposite = isComposite;
	          //Retrieves char entries section 
          pfileheader->signature = MFT_SIGNATURE;
          pfileheader->version = MFT_VERSION;
          pfileheader->iscomposite = myIsComposite;
          pfileheader->paintType = myPaintType;
          pfileheader->fixedPitch = myIsFixedPitch;
          pfileheader->fontBox[0] = 0;		//XMin font
          pfileheader->fontBox[1] = 0;		//YMin font
          pfileheader->fontBox[2] = 1000;		//XMax font
          pfileheader->fontBox[3] = 1000;		//YMax font
          pfileheader->fontMatrix[0] = FTOI(0.001);		
          pfileheader->fontMatrix[1] = 0;		
          pfileheader->fontMatrix[2] = 0;	
          pfileheader->fontMatrix[3] = FTOI(0.001);	
          pfileheader->fontMatrix[4] = 0;	
          pfileheader->fontMatrix[5] = 0;	
          pfileheader->italicAngle = FTOI(myItalicAngle);
          for( Standard_Integer i=0 ; i < MFT_DUMMYSIZE ; i++ ) 
            pfileheader->dummy[i] = (unsigned char)0xFF;
          strncpy(pfileheader->fontName,pfontname,length);
          pfileheader->fontName[length] = '\0';
          pfileheader->pentries = myFileHeader.recordSize;

          //Builds char entries section 
          if( myIsComposite ) {
            myCharEntries.recordSize = sizeof(MFT_ExtendedCharEntries);
          } else {
            myCharEntries.recordSize = sizeof(MFT_AsciiCharEntries);
          } 
          myCharEntries.precord = (Standard_CString) calloc(myCharEntries.recordSize,1);
          pcharentries = (MFT_CharEntries*) myCharEntries.precord;
            Standard_OutOfMemory_Raise_if(
                    !pcharentries,"MEMORY allocation failed");
            myCharEntries.beginPosition =
          myCharEntries.recordPosition = pfileheader->pentries;
          myCharEntries.update = Standard_True;
          pfileheader->pcommands = 
          pfileheader->pentries + myCharEntries.recordSize;
          pfileheader->freeSpace = pfileheader->pcommands;
          myFileHeader.update = Standard_True;

	          //Builds command buffer section 
            myCommandBuffer.beginPosition =
          myCommandBuffer.recordPosition = pfileheader->pcommands;
          myCommandBuffer.precord = (Standard_CString) malloc(myCommandBuffer.recordSize);
        }
        break;
    }

    TCollection_AsciiString env(CSF_MDTVFontPrecision.Value());
    if( env.Length() > 0 ) {
      Standard_ShortReal flatness,precision;
      Standard_Integer n = sscanf(env.ToCString(),"%f %f",&flatness,&precision);
      if( n > 0 && flatness > 0. ) {
	  theMDTVFontPrecision = Standard_True;
	  theCurveFlatness = flatness;
          theCurvePrecision = flatness * 10.;
	  if( precision > 0. ) theCurvePrecision = precision;
      }
      if( theMDTVFontPrecision ) {
	cout << " CSF_MFTVFontPrecision is " << theCurveFlatness << "," 
						<< theCurvePrecision << endl;
      }
    }
}

//        -------------------------
//        -- Category: Destructors
//        -------------------------

// =============================================================================
//        ---Purpose: Save the font file when the open mode is
//        --         Write or ReadWrite and Close it in all the case.
//        ---C++: alias ~
// =============================================================================

void MFT_FontManager::Destroy()
{
#if TRACE > 0
    cout << " MFT_FontManager::Destroy()" << endl;
#endif
    this->Close();
}


//        ---------------------------------------------
//        -- Category: Methods to updates the .mft files
//        ---------------------------------------------

// =============================================================================
//        ---Purpose: Updates the font name.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
// =============================================================================

void MFT_FontManager::SetFont(const Aspect_FontStyle& aFont)
{
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    Standard_CString pfontname = aFont.FullName();
    Standard_Integer length = Min(MFT_MAXFONTNAME-1,strlen(pfontname));
    strncpy(pfileheader->fontName,pfontname,length);
    pfileheader->fontName[length] = '\0';
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Defines and Enable the char <aChar> for writing.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or the char is already defined.
// =============================================================================

void MFT_FontManager::SetChar(const Standard_Character aChar)
{
#if TRACE > 0
    cout << " MFT_FontManager::SetChar('" << aChar << "')" << endl;
#endif

    unsigned int position = aChar & 0xFF;
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    if( IsDefinedChar(position) ) {
      cout << "TRY to updates the existing character" << position << endl;
    }
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    theCharPosition = position;
    theFirstCommandPosition = theNextCommandPosition = pfileheader->freeSpace;
}

// =============================================================================
//        ---Purpose: Defines and Enable the accent char <aChar> for writing.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or the char is already defined.
// =============================================================================

void MFT_FontManager::SetAccentChar(const Standard_Character aChar)
{
#if TRACE > 0
    cout << " MFT_FontManager::SetAccentChar('" << aChar << "')" << endl;
#endif

    unsigned int position = MFT_MAXASCIICHARENTRIES + (aChar & 0xFF);
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    if( IsDefinedChar(position) ) {
      cout << "TRY to updates the existing character" << position << endl;
    }
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    theCharPosition = position;
    theFirstCommandPosition = theNextCommandPosition = pfileheader->freeSpace;
}

// =============================================================================
//        ---Purpose: Defines and Enable the char <aChar> for writing.
//        --          This char being current for adding command.
//        ---Warning: The char must have an UNICODE UCS2 encoding.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if the font is not a composite font.
// =============================================================================

void MFT_FontManager::SetChar(const Standard_ExtCharacter aChar)
{
#if TRACE > 0
    cout << " MFT_FontManager::SetChar('" << aChar << "')" << endl;
#endif

    unsigned int position = aChar & 0xFFFF;
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if( !myIsComposite && 
		(position >= (MFT_MAXASCIICHARENTRIES+MFT_MAXSEACCHARENTRIES)),
						"FONT is not composite");
    if( IsDefinedChar(position) ) {
      cout << "TRY to updates the existing character" << position << endl;
    }
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    theCharPosition = position;
    theFirstCommandPosition = theNextCommandPosition = pfileheader->freeSpace;
}


// =============================================================================
//        ---Purpose: Changes the default encoding of the char position
//        --         <aPosition>.
//        ---Example: SetEncoding(233,"eacute")
//        --         change the default encoding of the char position 233
//        --         from "Oslash" to "eacute".
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode.
//        --          or if the char encoding is not defined.
// =============================================================================

void MFT_FontManager::SetEncoding(const Standard_Integer aPosition, const Standard_CString anEncoding)
{
#if TRACE > 0
    cout << " MFT_FontManager::SetEncoding(" << aPosition << ",'" <<
		anEncoding << "')" << endl;
#endif

    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if(
		aPosition > this->MaxCharPosition(),
				"BAD encoding position");

    Standard_Integer anEncodingPosition = this->Encoding(anEncoding);

    if( !anEncodingPosition ) {
      char message[80];
      sprintf(message," BAD encoding symbol '%s'",anEncoding); 
      MFT_FontManagerError::Raise(message);
    }

    if( aPosition != anEncodingPosition  && 
			anEncodingPosition > MFT_MAXASCIICHARENTRIES ) {
      MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
      pcharentries->fcommand[aPosition] = 
			pcharentries->fcommand[anEncodingPosition];
      myCharEntries.update = Standard_True;
    }

}

// =============================================================================
//        ---Purpose: Remove the definition of the char <aChar>
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode.
// =============================================================================

void MFT_FontManager::DelChar(const Standard_Character aChar)
{
#if TRACE > 0
    cout << " MFT_FontManager::DelChar('" << aChar << "')" << endl;
#endif

    unsigned int position = aChar & 0xFFFF;
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if( !myIsComposite && 
		(position >= (MFT_MAXASCIICHARENTRIES+MFT_MAXSEACCHARENTRIES)),
						"FONT is not composite");
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    pcharentries->fcommand[position] = -Abs(pcharentries->fcommand[position]);
    myCharEntries.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Remove the definition of the char <aChar>
//        --          This char being current for adding command.
//        ---Warning: The char must have an UNICODE UCS2 encoding.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if the font is not a composite font.
// =============================================================================

void MFT_FontManager::DelChar(const Standard_ExtCharacter aChar)
{
#if TRACE > 0
    cout << " MFT_FontManager::DelChar('" << aChar << "')" << endl;
#endif

    unsigned int position = aChar & 0xFFFF;
    MFT_FontManagerError_Raise_if(
		myFileMode == OSD_ReadOnly,
				"TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if( !myIsComposite, "FONT is not composite");
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    pcharentries->fcommand[position] = -Abs(pcharentries->fcommand[position]);
    myCharEntries.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Adds the command <aCommand>
//        --          to describe the current char.
//        --          This command being current for adding parameters if any.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if a current char don't have being defined.
//        ---Warning: the last command of the char descriptor
//        --         must have a type MFC_TOC_ENDCHAR.
// =============================================================================

void MFT_FontManager::AddCommand(const MFT_TypeOfCommand aCommandType)
{
    if( theNextCommandPosition == theFirstCommandPosition &&
				aCommandType != MFT_TOC_MINMAX) {
			// Add a MINMAX command first.
      this->AddCommand(MFT_TOC_MINMAX);
      this->AddValue(0);
      this->AddValue(0);
      this->AddValue(0);
      this->AddValue(0);
    }

#if TRACE > 0
    cout << " MFT_FontManager::AddCommand(" << MFT::Convert(aCommandType) << ")" << endl;
#endif

    theCommandType = aCommandType;
    theCommandPosition = theNextCommandPosition;
    Standard_Integer *pcommand = 
	(Standard_Integer*) Locate(myCommandBuffer,theCommandPosition);
    MFT_CommandDescriptor *pdescr = (MFT_CommandDescriptor*) pcommand;
    *pcommand = 0;
    SET_COMMAND_TYPE(*pdescr,theCommandType);
    myCommandBuffer.update = Standard_True;
    theNextCommandPosition += sizeof(MFT_CommandDescriptor);

    if( theCommandType == MFT_TOC_ENDCHAR ) { 
							//Saves char
      MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
      pcharentries->fcommand[theCharPosition] = theFirstCommandPosition;
      myCharEntries.update = Standard_True;
      MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
      pfileheader->freeSpace = theNextCommandPosition; 
      myFileHeader.update = Standard_True;
      this->ComputeBoundingBox(theCharPosition);
      theFirstCommandPosition = 0;
    }
}

// =============================================================================
//        ---Purpose: Adds the integer parameter <aValue> to fill
//        --          the current command.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if a current command don't have being defined.
//	  --          or if the number of values is > MaxCommandValues()
// =============================================================================

void MFT_FontManager::AddValue(const Standard_Integer aValue)
{
#if TRACE > 0
    cout << " MFT_FontManager::AddValue(" << aValue << ")" << endl;
#endif

    MFT_CommandDescriptor *pdescr = 
	(MFT_CommandDescriptor*) Locate(myCommandBuffer,theCommandPosition);
    Standard_Integer length = COMMAND_LENGTH(*pdescr);
    if( length < 8 ) {
      length++;
      SET_COMMAND_LENGTH(*pdescr,length);
      SET_COMMAND_VALUETYPE(*pdescr,length,MFT_TOV_INTEGER);
    } else {
      Standard_OutOfRange::Raise("TOO many command INTEGER values");
    }
    myCommandBuffer.update = Standard_True;
    Standard_Integer *pvalue = 
	(Standard_Integer*) Locate(myCommandBuffer,theNextCommandPosition);
    if( theCommandType == MFT_TOC_SEAC ) {
      if( length > 3 && length < 6 ) {
      	MFT_CharEntries *pcharentries = 
		(MFT_CharEntries*) myCharEntries.precord;
  	        	//Gets the first command position
  	        	// of the base char and Accented Char
        if( pcharentries->fcommand[aValue] ) {
	  *pvalue = -pcharentries->fcommand[aValue];
	} else {
          *pvalue = aValue;
	}
      } else {
        *pvalue = aValue;
      }
    } else {
      *pvalue = aValue;
    }
    myCommandBuffer.update = Standard_True;
    theNextCommandPosition += sizeof(MFT_CommandValue);
}

// =============================================================================
//        ---Purpose: Adds the float parameter <aValue> to fill
//        --          the current command.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if a current command don't have being defined.
//	  --          or if the number of values is > MaxCommandValues()
// =============================================================================

void MFT_FontManager::AddValue(const Standard_Real aValue)
{
#if TRACE > 0
    cout << " MFT_FontManager::AddValue(" << aValue << ")" << endl;
#endif

    MFT_CommandDescriptor *pdescr = 
	(MFT_CommandDescriptor*) Locate(myCommandBuffer,theCommandPosition);
    Standard_Integer length = COMMAND_LENGTH(*pdescr);
    if( length < 8 ) {
      length++;
      SET_COMMAND_LENGTH(*pdescr,length);
      SET_COMMAND_VALUETYPE(*pdescr,length,MFT_TOV_FLOAT);
    } else {
      Standard_OutOfRange::Raise("TOO many command FLOAT values");
    }
    myCommandBuffer.update = Standard_True;
    Standard_ShortReal *pvalue = 
	(Standard_ShortReal*) Locate(myCommandBuffer,theNextCommandPosition);
    *pvalue = Standard_ShortReal(aValue);
    myCommandBuffer.update = Standard_True;
    theNextCommandPosition += sizeof(MFT_CommandValue);
}

// =============================================================================
//        ---Purpose: Adds the string parameter <aValue> to fill
//        --          the current command.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if a current command don't have being defined.
//	  --          or if the number of values is > MaxCommandValues()
// =============================================================================

void MFT_FontManager::AddValue(const Standard_CString aValue)
{
    MFT_FontManagerError_Raise_if(!aValue,"BAD string value");

#if TRACE > 0
    cout << " MFT_FontManager::AddValue('" << aValue << "')" << endl;
#endif

    Standard_Integer lvalue = strlen(aValue);
    Standard_Integer i,nvalue =  (lvalue > 0) ? 
			1 + (lvalue-1)/sizeof(MFT_CommandValue) : 0;
    MFT_CommandDescriptor *pdescr = 
	(MFT_CommandDescriptor*) Locate(myCommandBuffer,theCommandPosition);
    Standard_Integer length = COMMAND_LENGTH(*pdescr);
    Standard_OutOfRange_Raise_if( length + nvalue + 1 > MaxCommandValues(),
					"TOO many command STRING values");
    for( i=0 ; i<nvalue ; i++ ) {
      length++;
      SET_COMMAND_VALUETYPE(*pdescr,length,MFT_TOV_STRING);
    }
    SET_COMMAND_LENGTH(*pdescr,length);
    myCommandBuffer.update = Standard_True;
    Standard_PCharacter pvalue,pstring;
    pstring = (Standard_PCharacter)aValue;
    for( i=0 ; i<nvalue ; i++ ) {
      pvalue = (Standard_PCharacter) Locate(myCommandBuffer,theNextCommandPosition);
      strncpy(pvalue,pstring,4); 
      myCommandBuffer.update = Standard_True;
      pstring += sizeof(MFT_CommandValue);
      theNextCommandPosition += sizeof(MFT_CommandValue);
    }
}

// =============================================================================
//        ---Purpose: Sets the bounding box of the font.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if the bounding box has a wrong value.
//        ---Warning: The bounding box coordinates
//        --          must be given in the space 1000. corresponding to the
//        --          MAX of all character width of the font.
// =============================================================================

void MFT_FontManager::SetBoundingBox(const Standard_Integer aMinX, const Standard_Integer aMinY, const Standard_Integer aMaxX, const Standard_Integer aMaxY) {
#if TRACE > 0
    cout << " MFT_FontManager::SetBoundingBox(" << aMinX << "," <<
	aMinY << "," << aMaxX << "," << aMaxY << ")" << endl;
#endif

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if(
                aMinX >= aMaxX || aMinY >= aMaxY,
                                "BAD font bounding box definition");
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->fontBox[0] = aMinX; 
    pfileheader->fontBox[1] = aMinY; 
    pfileheader->fontBox[2] = aMaxX; 
    pfileheader->fontBox[3] = aMaxY; 
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Sets the font matrix.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        ---Warning: The font matrix default are :
//        --          0.001,0,0,0.001,0,0
// =============================================================================

void MFT_FontManager::SetFontMatrix(const Standard_Real M1, const Standard_Real M2, const Standard_Real M3, const Standard_Real M4, const Standard_Real M5, const Standard_Real M6) {
#if TRACE > 0
    cout << " MFT_FontManager::SetFontMatrix(" << M1 << "," <<
	M2 << "," << M3 << "," << M4 << "," << M5 << "," << M6 << ")" << endl;
#endif

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->fontMatrix[0] = FTOI(M1); 
    pfileheader->fontMatrix[1] = FTOI(M2); 
    pfileheader->fontMatrix[2] = FTOI(M3); 
    pfileheader->fontMatrix[3] = FTOI(M4); 
    pfileheader->fontMatrix[4] = FTOI(M5); 
    pfileheader->fontMatrix[5] = FTOI(M6); 
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Sets the paint type of the font.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
//        --          or if the value is not a Type 1 font value.
//        ---Warning: The paint type <aValue> must be one of Type 1 font
//        --         value 0 for FILL or 2 for OUTLINE.
// =============================================================================

void MFT_FontManager::SetPaintType(const Standard_Integer aValue) {
#if TRACE > 0
    cout << " MFT_FontManager::SetPaintType(" << aValue << ")" << endl;
#endif

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");
    MFT_FontManagerError_Raise_if(
                aValue < 0 || aValue > 2,
                                "BAD font paint type value");
    myPaintType = aValue;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->paintType = myPaintType; 
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Sets the fixed pitch flag of the font.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
// =============================================================================

void MFT_FontManager::SetFixedPitch(const Standard_Boolean aFlag) {
#if TRACE > 0
    cout << " MFT_FontManager::SetFixedPitch(" << aFlag << ")" << endl;
#endif

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");
    myIsFixedPitch = aFlag;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->fixedPitch = myIsFixedPitch; 
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Level: Advanced
//        ---Purpose: Sets the italic angle of the font
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode
// =============================================================================

void MFT_FontManager::SetItalicAngle(const Quantity_PlaneAngle anAngle) {
#if TRACE > 0
    cout << " MFT_FontManager::SetItalicAngle(" << anAngle << ")" << endl;
#endif

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");
    myItalicAngle = Standard_ShortReal( anAngle );
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->italicAngle = FTOI(myItalicAngle); 
    myFileHeader.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Computes the bounding box of the font from the
//        --          min-max of all chars of the font. 
//        ---Warning: this must be call after all characters has been defined.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font file is not opened
//        --          in WriteOnly or ReadWrite mode.
// =============================================================================

void MFT_FontManager::ComputeBoundingBox ()
{

    MFT_FontManagerError_Raise_if(
                myFileMode == OSD_ReadOnly,
                                "TRY to updates a ReadOnly file");

    theTanCharSlant = 0.;
    theFontXRatio = theFontYRatio = 1.;
    theOrientation = 0.;
    theSinCharAngle = 0.;
    theCosCharAngle = 1.;
    Standard_Boolean fixedPitch = myIsFixedPitch;
    myIsFixedPitch = Standard_False;

    Quantity_Length Fxmin = 0.,Fymin = 0.,Fxmax = 0.,Fymax = 0.;
    Quantity_Length Cxmin,Cymin,Cxmax,Cymax;
    Standard_Integer i;
    for( i=0 ; i<this->MaxCharPosition() ; i++ ) {
      if( this->IsDefinedChar(i) ) {
        theDeltaX = 0.;
        this->DrawChar(theTextManager,i);
        theTextManager->MinMax(Cxmin,Cymin,Cxmax,Cymax);
	Fxmin = Min(Fxmin,Cxmin); Fymin = Min(Fymin,Cymin);
	Fxmax = Max(Fxmax,Cxmax); Fymax = Max(Fymax,Cymax);
      }
    }
    myIsFixedPitch = fixedPitch;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    pfileheader->fontBox[0] = Standard_Integer(Fxmin);
    pfileheader->fontBox[1] = Standard_Integer(Fymin);
    pfileheader->fontBox[2] = Standard_Integer(Fxmax);
    pfileheader->fontBox[3] = Standard_Integer(Fymax);
    myFileHeader.update = Standard_True;

#if TRACE > 1
    cout << " MFT_FontManager::ComputeBoundingBox()" << endl;
    cout << " 	is " << Fxmin << "," << Fymin << "," << Fxmax << "," << Fymax << endl;
#endif
}

//        ---------------------------------------------------------------

//        ---------------------------------------------------------------
//        -- Category: Methods to sets the current interpretor attributes
//        ---------------------------------------------------------------

// =============================================================================
//        ---Purpose: Sets the current font attributes.
//        --  <aWidth> : the maximum width of one character.
//        --  <aHeight> : the maximum height of one character
//        --              (ascent + descent components of the character).
//        --  <aSlant> : the slant of one character given in RAD
//        --              from vertical.
//        --  <aPrecision> : the absolute interpolator precision
//        --              (the maximum deflection for the curves).
// =============================================================================

void MFT_FontManager::SetFontAttribs(const Quantity_Length aWidth, const Quantity_Length aHeight, const Quantity_PlaneAngle aSlant, const Quantity_Factor aPrecision, const Standard_Boolean aCapsHeight) 
{
#if TRACE > 0
    cout << " MFT_FontManager::SetFontAttribs(" << aWidth << "," <<
		aHeight << "," << aSlant << "," << aPrecision << ")" << endl;
#endif

    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    myCharWidth  = Standard_ShortReal( aWidth );
    myCharHeight = Standard_ShortReal( aHeight );
    myCharSlant  = Standard_ShortReal( aSlant );
    if( pfileheader->version > 280597 ) {
      Standard_ShortReal slant = ITOF(pfileheader->italicAngle);
      myCharSlant += slant;
    }
    if( aPrecision > 0. ) myCharPrecision = Standard_ShortReal( aPrecision );
    myCharCapsHeight = aCapsHeight;
}

// =============================================================================
//        ---Purpose: Sets the text attributes.
//        --  <aString> : the string to interpret
//        --  <anOrientation> : the orientation angle in RAD from horizontal.
// =============================================================================

void MFT_FontManager::SetTextAttribs(const Standard_CString aString, const Quantity_PlaneAngle anOrientation)
{
#if TRACE > 0
    cout << " MFT_FontManager::SetTextAttribs('" << aString << "'," <<
		anOrientation << ")" << endl;
#endif

    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    theTanCharSlant = Tan(myCharSlant);
    theFixedCharWidth = 0.;
    if( !theMDTVFontPrecision ) {
      theCurveFlatness = myCharPrecision *
                pfileheader->fontBox[2]/Max(myCharWidth,myCharHeight);
      theCurvePrecision = theCurveFlatness * 10.;
#if TRACE > 0
      cout << " SetTextAttribs(aString,anOrientation): Flatness =" << theCurveFlatness
	<< " FontBox =" << pfileheader->fontBox[2] 
	  << " Precision ="   << theCurvePrecision << endl;
#endif
    }
    if( myCharCapsHeight ) {
      Standard_Integer xmin,ymin,xmax,ymax;
      this->CharBoundingBox(Standard_Integer('H'),xmin,ymin,xmax,ymax);
      if( ymax > 0 ) {
        theFontXRatio = myCharWidth/ymax;
        theFontYRatio = myCharHeight/ymax;
        if( myIsFixedPitch ) {
	  theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/ymax;
        }
      } else {
        theFontXRatio = myCharWidth/pfileheader->fontBox[3];
        theFontYRatio = myCharHeight/pfileheader->fontBox[3];
        if( myIsFixedPitch ) {
	  theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/
						pfileheader->fontBox[3];
        }
      }
    } else {
      if( myIsFixedPitch ) {
        Standard_Integer hmax = Max(pfileheader->fontBox[2],
			pfileheader->fontBox[3]-pfileheader->fontBox[1]);
	hmax -= 50;	// Ajustement manuel !!
        theFontXRatio = myCharWidth/hmax;
        theFontYRatio = myCharHeight/hmax;
	theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/hmax;
      } else {
        theFontXRatio = myCharWidth/pfileheader->fontBox[2];
        theFontYRatio = myCharHeight/pfileheader->fontBox[2];
      }
    }

    theAsciiString = aString;
    theOrientation = anOrientation;
    theCharCount = 0;
    theStringLength = theAsciiString.Length();
    theStringIsAscii = Standard_True;
    theDeltaX = 0.;
    theSinCharAngle = Sin(theOrientation);
    theCosCharAngle = Cos(theOrientation);
}

// =============================================================================
//        ---Purpose: Sets the extended text attributes.
//        --  <aString> : the string to interpret
//        --  <anOrientation> : the orientation angle in RAD from horizontal.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font is not a composite font and
//        --         the string is not ASCII.
// =============================================================================

void MFT_FontManager::SetTextAttribs(const Standard_ExtString aString, const Quantity_PlaneAngle anOrientation)
{
TCollection_ExtendedString estring(aString);

    if( estring.IsAscii() ) {
      TCollection_AsciiString astring(estring,'?');
      this->SetTextAttribs(astring.ToCString(),anOrientation);
    } else {
#if TRACE > 0
    cout << " MFT_FontManager::SetTextAttribs('" << estring << "'," <<
		anOrientation << ")" << endl;
#endif
      MFT_FontManagerError_Raise_if(
			!myIsComposite,"FONT is not composite");
      MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
      theTanCharSlant = Tan(myCharSlant);
      theFixedCharWidth = 0.;
      if( !theMDTVFontPrecision ) {
        theCurveFlatness = myCharPrecision *
                pfileheader->fontBox[2]/Max(myCharWidth,myCharHeight);
        theCurvePrecision = theCurveFlatness * 10.;
      }
      if( myCharCapsHeight ) {
        Standard_Integer xmin,ymin,xmax,ymax;
        this->CharBoundingBox(Standard_Integer('H')+0xFEE0,xmin,ymin,xmax,ymax);
        if( ymax > 0 ) {
          theFontXRatio = myCharWidth/ymax;
          theFontYRatio = myCharHeight/ymax;
          if( myIsFixedPitch ) {
	    theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/ymax;
          }
        } else {
          theFontXRatio = myCharWidth/pfileheader->fontBox[3];
          theFontYRatio = myCharHeight/pfileheader->fontBox[3];
          if( myIsFixedPitch ) {
	    theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/
					pfileheader->fontBox[3];
          }
        }
      } else {
        if( myIsFixedPitch ) {
          Standard_Integer hmax = Max(pfileheader->fontBox[2],
                        pfileheader->fontBox[3]-pfileheader->fontBox[1]);
          theFontXRatio = myCharWidth/hmax;
          theFontYRatio = myCharHeight/hmax;
	  theFixedCharWidth = (myCharWidth*
		(pfileheader->fontBox[2]+pfileheader->fontBox[0]))/hmax;
        } else {
          theFontXRatio = myCharWidth/pfileheader->fontBox[2];
          theFontYRatio = myCharHeight/pfileheader->fontBox[2];
        }
      }

      theExtendedString = aString;
      theOrientation = anOrientation;
      theCharCount = 0;
      theStringLength = theExtendedString.Length();
      theStringIsAscii = Standard_False;
      theDeltaX = 0;
      theSinCharAngle = Sin(theOrientation);
      theCosCharAngle = Cos(theOrientation);
    }
}

//        ----------------------------------------------------------------
//        -- Category: Methods to retrieve current attributes informations.
//        ----------------------------------------------------------------

// =============================================================================
//        ---Purpose: Drawn an ANSI text
//        --  <aTextManager> : the draw manager to call for each vertex of the string.
//        --  <aString> : the string to interpret
//        --  <anOrientation> : the orientation angle in RAD from horizontal.
// =============================================================================

void MFT_FontManager::DrawText(const Handle(MFT_TextManager)& aTextManager, const Standard_CString aString, const Quantity_Length anX, const Quantity_Length anY, const Quantity_PlaneAngle anOrientation) {
Standard_Boolean vazy;

    this->SetTextAttribs(aString,anOrientation);

    Standard_Integer aPosition;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    Quantity_Length Cwidth = theFontXRatio *
		(pfileheader->fontBox[2] - pfileheader->fontBox[0]);
    Quantity_Length Cheight = theFontYRatio * 
		(pfileheader->fontBox[3] - pfileheader->fontBox[1]);
    Quantity_PlaneAngle Cslant = myCharSlant;
    aTextManager->BeginString(anX,anY,anOrientation,
				Cwidth,Cheight,Cslant,myPaintType);
    while ( theCharCount < theStringLength ) {
      aPosition = Standard_Integer((unsigned char)theAsciiString.Value(theCharCount+1));
      vazy = this->DrawChar(aTextManager,aPosition);
      theCharCount++;
    }
    aTextManager->EndString();
}

// =============================================================================
//        ---Purpose: Drawn an EXTENDED text
//        --  <aTextManager> : the draw manager to call for each vertex of the string.
//        --  <aString> : the string to interpret
//        --  <anOrientation> : the orientation angle in RAD from horizontal.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font is not a composite font and
//        --         the string is not ASCII.
// =============================================================================

void MFT_FontManager::DrawText(const Handle(MFT_TextManager)& aTextManager, const Standard_ExtString aString, const Quantity_Length anX, const Quantity_Length anY, const Quantity_PlaneAngle anOrientation) {
Standard_Boolean vazy;

    this->SetTextAttribs(aString,anOrientation);

    Standard_Integer aPosition;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    Quantity_Length Cwidth = theFontXRatio *
		(pfileheader->fontBox[2] - pfileheader->fontBox[0]);
    Quantity_Length Cheight = theFontYRatio * 
		(pfileheader->fontBox[3] - pfileheader->fontBox[1]);
    Quantity_PlaneAngle Cslant = myCharSlant;
    aTextManager->BeginString(anX,anY,anOrientation,
				Cwidth,Cheight,Cslant,myPaintType);
    while ( theCharCount < theStringLength ) {
      if( theStringIsAscii )
        aPosition = Standard_Integer((unsigned char)theAsciiString.Value(theCharCount+1));
      else
        aPosition = Standard_Integer((unsigned short)theExtendedString.Value(theCharCount+1));
      vazy = this->DrawChar(aTextManager,aPosition);
      theCharCount++;
    }
    aTextManager->EndString();
}

// =============================================================================
//        ---Purpose: Retrieves the bounding box of the font.
// =============================================================================

void MFT_FontManager::BoundingBox(Standard_Integer& aMinX, Standard_Integer& aMinY, Standard_Integer& aMaxX, Standard_Integer& aMaxY) const
{
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    aMinX = pfileheader->fontBox[0];
    aMinY = pfileheader->fontBox[1];
    aMaxX = pfileheader->fontBox[2];
    aMaxY = pfileheader->fontBox[3];
}

// =============================================================================
//        ---Purpose: Retrieves the bounding box of a character.
//        raises FontManagerError from MFT;
//        ---Trigger: If the position <aPosition> is < 0 or > MaxCharPosition().
// =============================================================================

void MFT_FontManager::CharBoundingBox(const Standard_Integer aPosition,Standard_Integer& aMinX, Standard_Integer& aMinY, Standard_Integer& aMaxX, Standard_Integer& aMaxY)
{
    aMinX = aMaxX = aMinY = aMaxY = 0;
    if( this->IsDefinedChar(aPosition) ) {
      this->SetChar(aPosition);
      theCommand = NextCommand(myCommandBuffer);
      if( COMMAND_TYPE(theCommand) == MFT_TOC_MINMAX ) {
        aMinX = IValue(myCommandBuffer,1);
        aMinY = IValue(myCommandBuffer,2);
        aMaxX = IValue(myCommandBuffer,3);
        aMaxY = IValue(myCommandBuffer,4);
      }
    }
}

// =============================================================================
//        ---Purpose: Retrieves the paint type of the font.
// =============================================================================

Standard_Integer MFT_FontManager::PaintType() const
{
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    return pfileheader->paintType;
}

// =============================================================================
//        ---Purpose: Retrieves the fixed pitch flag of the font.
// =============================================================================

Standard_Boolean MFT_FontManager::FixedPitch() const
{
    return myIsFixedPitch;
}

// =============================================================================
//        ---Purpose: Retrieves the fixed italic angle of the font.
// =============================================================================

Quantity_PlaneAngle MFT_FontManager::ItalicAngle() const
{
    return Quantity_PlaneAngle(myItalicAngle);
}

// =============================================================================
//        ---Purpose: Runs the interpretor with the current attributes setting
//        --         (font and text attributes)
//        --         and returns :
//        --         The max char width of the font CharSet <aWidth>.
//        --         The max char height of the font CharSet <aHeight>.
//        --         The max char descent value below the baseline <aDescent>
//        --         The slant angle of the font <aSlant>
//        --         The interpolator precision of the font <aPrecision>
//        --         The caps height flag <aCapsHeight>
// =============================================================================

Standard_CString MFT_FontManager::FontAttribs(Quantity_Length& aWidth, Quantity_Length& aHeight, Quantity_Length& aDescent, Quantity_PlaneAngle& aSlant, Quantity_Factor& aPrecision, Standard_Boolean &aCapsHeight)
{
    static TCollection_AsciiString fontname;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    fontname = pfileheader->fontName;
    this->SetTextAttribs("",0.);
    aWidth = theFontXRatio *
		(pfileheader->fontBox[2] - pfileheader->fontBox[0]);
    aHeight = theFontYRatio * 
		(pfileheader->fontBox[3] - pfileheader->fontBox[1]);
    aDescent = theFontYRatio * pfileheader->fontBox[1];
    aSlant = myCharSlant;
    aPrecision = myCharPrecision;
    aCapsHeight = myCharCapsHeight;

#if TRACE > 0
    cout << "'" << fontname << "' = MFT_FontManager::FontAttribs(" << 
	aWidth << "," << aHeight << "," << aDescent << "," << aSlant << "," << 
		aPrecision << "," << aCapsHeight << ")" << endl;
#endif
    return fontname.ToCString();
}

// =============================================================================
//        ---Purpose: Runs the interpretor on the ASCII text <aString>
//        --         with the current font attributes setting and returns :
//        --         The string width <aWidth>.
//        --         The string ascent <anAscent>.
//        --         The string left bearing value from the origine <aLbearing>
//        --         The string descent value below the baseline <aDescent>
// =============================================================================

void MFT_FontManager::TextSize(const Standard_CString aString, Quantity_Length& aWidth, Quantity_Length& anAscent, Quantity_Length& aLbearing, Quantity_Length& aDescent) 
{
    this->DrawText(theTextManager,aString,0.,0.,0.);

    Quantity_Length Xmin,Ymin,Xmax,Ymax;
    theTextManager->MinMax(Xmin,Ymin,Xmax,Ymax);
    aWidth = Xmax - Xmin; anAscent = Ymax; 
    aLbearing = Xmin; aDescent = -Ymin;

#if TRACE > 0
    cout << " MFT_FontManager::TextSize(" << aString << ","  << aWidth << ","
	 << anAscent << "," << aLbearing << "," << aDescent << ")" << endl;
#endif
}

// =============================================================================
//        ---Purpose: Runs the interpretor on the EXTENDED text <aString>
//        --         with the current font attributes setting and returns :
//        --         The string width <aWidth>.
//        --         The string ascent <anAscent>.
//        --         The string left bearing value from the origine <aLbearing>
//        --         The string descent value below the baseline <aDescent>
//        raises FontManagerError from MFT;
//        ---Trigger: If the font is not a composite font and
//        --         the string is not ASCII.
// =============================================================================

void MFT_FontManager::TextSize(const Standard_ExtString aString, Quantity_Length& aWidth, Quantity_Length& anAscent, Quantity_Length& aLbearing, Quantity_Length& aDescent) 
{
    this->DrawText(theTextManager,aString,0.,0.,0.);

    Quantity_Length Xmin,Ymin,Xmax,Ymax;
    theTextManager->MinMax(Xmin,Ymin,Xmax,Ymax);
    aWidth = Xmax - Xmin; anAscent = Ymax; 
    aLbearing = Xmin; aDescent = -Ymin;

#if TRACE > 0
    cout << " MFT_FontManager::TextSize(" << aString << ","  << aWidth << ","
	 << anAscent << "," << aLbearing << "," << aDescent << ")" << endl;
#endif

}

// =============================================================================
//        ---Purpose: Runs the interpretor with the current attributes setting
//        --         (font attributes)
//        --         and returns :
//        --         The char width <aWidth>.
//        --         The char left bearing <aLbearing>.
//        --         The char right bearing <aRbearing>.
//        --         The char ascent <anAscent>.
//        --         The char descent value below the baseline <aDescent>
//        --      And returns TRUE if the character is defined.
// =============================================================================

Standard_Boolean MFT_FontManager::CharSize(const Standard_Character aChar, Quantity_Length& aWidth, Quantity_Length& aLbearing, Quantity_Length& aRbearing, Quantity_Length& anAscent, Quantity_Length& aDescent) 
{
    Standard_Integer aPosition = Standard_Integer(aChar);
    Standard_Boolean theStatus = Standard_False;

    if( this->IsDefinedChar(aPosition) ) {
      this->SetTextAttribs("",0.);
      MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
      Quantity_Length Cwidth = theFontXRatio *
		(pfileheader->fontBox[2] - pfileheader->fontBox[0]);
      Quantity_Length Cheight = theFontYRatio * 
		(pfileheader->fontBox[3] - pfileheader->fontBox[1]);
      Quantity_PlaneAngle Cslant = myCharSlant;
      theTextManager->BeginString(0.,0.,0.,
				Cwidth,Cheight,Cslant,myPaintType);
      this->DrawChar(theTextManager,aPosition);
      theTextManager->EndString();
      Quantity_Length Xmin,Ymin,Xmax,Ymax;
      theTextManager->MinMax(Xmin,Ymin,Xmax,Ymax);
      aWidth = Xmax - Xmin; anAscent = Ymax;
      aLbearing = Xmin; aDescent = -Ymin;
      aRbearing = Xmax;
      theStatus = Standard_True;
    }
#if TRACE > 0
    cout << theStatus << " = MFT_FontManager::CharSize('" << aChar <<
	"'," << aWidth << "," << aLbearing << "," << aRbearing <<
	"," <<anAscent << "," << aDescent << ")" << endl;
#endif
    return theStatus;
}

// =============================================================================
//        ---Purpose: Runs the interpretor with the current attributes setting
//        --         (font attributes)
//        --         and returns :
//        --         The extended char width <aWidth>.
//        --         The extended char left bearing <aLbearing>.
//        --         The extended char right bearing <aRbearing>.
//        --         The extended char ascent <anAscent>.
//        --         The extended char descent value below the baseline <aDescent>
//        --      And returns TRUE if the character is defined.
//        raises FontManagerError from MFT;
//        ---Trigger: If the font is not a composite font and
//        --         the char is not ASCII.
// =============================================================================

Standard_Boolean MFT_FontManager::CharSize(const Standard_ExtCharacter aChar, Quantity_Length& aWidth, Quantity_Length& aLbearing, Quantity_Length& aRbearing, Quantity_Length& anAscent, Quantity_Length& aDescent) 
{
    Standard_Integer aPosition = Standard_Integer(aChar);
    MFT_FontManagerError_Raise_if(
			!myIsComposite && aPosition > 255,
						"FONT is not composite");

    Standard_Boolean theStatus = Standard_False;
    if( this->IsDefinedChar(aPosition) ) {
      this->SetTextAttribs("",0.);
      MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
      Quantity_Length Cwidth = theFontXRatio *
		(pfileheader->fontBox[2] - pfileheader->fontBox[0]);
      Quantity_Length Cheight = theFontYRatio * 
		(pfileheader->fontBox[3] - pfileheader->fontBox[1]);
      Quantity_PlaneAngle Cslant = myCharSlant;
      theTextManager->BeginString(0.,0.,0.,
				Cwidth,Cheight,Cslant,myPaintType);
      this->DrawChar(theTextManager,aPosition);
      theTextManager->EndString();
      Quantity_Length Xmin,Ymin,Xmax,Ymax;
      theTextManager->MinMax(Xmin,Ymin,Xmax,Ymax);
      aWidth = Xmax - Xmin; anAscent = Ymax;
      aLbearing = Xmin; aDescent = -Ymin;
      aRbearing = Xmax;
      theStatus = Standard_True;
    }
#if TRACE > 0
    cout << theStatus << " = MFT_FontManager::CharSize('" << aChar <<
	"'," << aWidth << "," << aLbearing << "," << aRbearing <<
	"," <<anAscent << "," << aDescent << ")" << endl;
#endif
    return theStatus;
}


//        ----------------------------
//        -- Category: Inquire methods
//        ----------------------------


// =============================================================================
//        ---Purpose: Retrieves the font descriptor
// =============================================================================

Aspect_FontStyle MFT_FontManager::Font() const
{
MFT_FileHeader *pfileheader = (MFT_FileHeader*)myFileHeader.precord;
Aspect_FontStyle aFont((Standard_CString)pfileheader->fontName);

    return aFont;
}

// =============================================================================
//        ---Purpose: Returns TRUE if the font is composite.
//        ---Example: KANJI fonts returns TRUE.
// =============================================================================

Standard_Boolean MFT_FontManager::IsComposite() const
{
    return myIsComposite;
}

// =============================================================================
//        ---Purpose: Returns TRUE if the font
//        --          $CSF_MDTVFontDirectory/<anAliasName>.mft exist.
// =============================================================================

Standard_Boolean MFT_FontManager::IsKnown(const Standard_CString anAliasName)
{
Standard_CString path = Path(anAliasName);

   if( access(path,0) != -1 ) return Standard_True;
   else return Standard_False;
}

// =============================================================================
//        ---Purpose: Retrieves the font descriptor from an existing font
//        raises FontManagerError from MFT;
//        ---Trigger: If the font does not exist.
// =============================================================================

Aspect_FontStyle MFT_FontManager::Font(const Standard_CString anAliasName)
{
    OSD_OpenMode aFileMode = OSD_ReadOnly;
    MFT_FileHandle handle = Open(anAliasName,aFileMode);
#ifdef PRO17604
    if( handle < 0 ) {
      char message[512];
      sprintf(message,"BAD MFT font name '%s'",anAliasName);
      MFT_FontManagerError::Raise(message);
    }
#else
    char message[80];
    sprintf(message,"BAD MFT font name '%s'",anAliasName);
    MFT_FontManagerError_Raise_if(handle < 0,message);
#endif

    MFT_FileRecord header;
    header.fileHandle = handle;
    header.beginPosition = header.recordPosition = 0;
    header.recordSize = sizeof(MFT_FileHeader);
    header.update = Standard_False;
    header.precord = NULL;
    header.precordMMAPAddress = NULL;
    header.recordMMAPSize = 0;
#ifndef No_Exception
    Standard_Boolean status =
#endif
    				Read(header);
    MFT_FontManagerError_Raise_if(!status,"HEADER Read error");

    MFT_FileHeader *pfileheader = (MFT_FileHeader*) header.precord;
    MFT_FontManagerDefinitionError_Raise_if(!pfileheader,"BAD MFT header");
    if( pfileheader->signature != MFT_SIGNATURE ) {
      SWAPINT(pfileheader->signature);
    }
    MFT_FontManagerDefinitionError_Raise_if(
	    pfileheader->signature != MFT_SIGNATURE,"BAD MFT signature");
    Aspect_FontStyle aFont((Standard_PCharacter)pfileheader->fontName);
    free((Standard_Address)header.precord);
    Close(handle);

    return aFont;
} 

// =============================================================================
//        ---Purpose: Returns the MFT font number available
//        --          in the directory $CSF_MDTVFontDirectory
//        --          according to the filter
//        ---Examples: number = MFT_FontMAnager::FontNumber("-euclid3");
//        --          returns only the euclid3 fonts.
//        --          number = MFT_FontMAnager::FontNumber(
//                                "-*-*-*-*-*-*-*-*-*-*-*-*-japanese");
//        --          returns only the japanese fonts.
// =============================================================================

Standard_Integer MFT_FontManager::FontNumber(const Standard_CString aFilter)
{
static TCollection_AsciiString astar("*");
//
// Retrieves all xxx.fmt files
//
//GG090200 BUG    OSD_Path pathfile,pathdir(TCollection_AsciiString(""));
//JR    OSD_Path pathfile,pathdir(TCollection_AsciiString(Path("")));
    OSD_Path pathfile;
    TCollection_AsciiString apathmft = TCollection_AsciiString(Path("")) ;
    OSD_Path pathdir = OSD_Path(apathmft);
    OSD_FileIterator files(pathdir,"*.mft");
    OSD_File file;
    theListOfFontName.Clear();
    if( aFilter && strlen(aFilter) > 0 && strcmp(aFilter,"*") ) {
      Aspect_FontStyle filter(aFilter);
      TCollection_AsciiString atoken1,atoken2,afilter = filter.FullName();
      Standard_Boolean found;
      Standard_Integer i;
      while (files.More()) {
        file = files.Values();
        file.Path(pathfile);
        Aspect_FontStyle font = MFT_FontManager::Font(pathfile.Name().ToCString());
	TCollection_AsciiString afont = font.FullName();
	found = Standard_True;
	for( i=2 ; i<=14 ; i++ ) {
	  atoken1 = afilter.Token("-",i);
	  if( atoken1 != astar ) {
	    atoken2 = afont.Token("-",i);
	    if( atoken1 != atoken2 ) {
	      found = Standard_False; break;
	    }
	  }
	}
	if( found ) theListOfFontName.Append(pathfile.Name());
        files.Next();
      }
    } else {
      while (files.More()) {
        file = files.Values();
        file.Path(pathfile);
        theListOfFontName.Append(pathfile.Name());
        files.Next();
      }
    }

    return theListOfFontName.Length();
}

// =============================================================================
//        ---Purpose: Retrieves the font descriptor of index <aRank>
//        --         from the directory $CSF_MDTVFontDirectory
//        raises OutOfRange from Standard;
//        ---Trigger: If the font rank <aRank> is < 1 or > FontNumber().
// =============================================================================

Aspect_FontStyle MFT_FontManager::Font(const Standard_Integer aRank)
{
    Standard_OutOfRange_Raise_if(
        aRank < 1 || aRank > theListOfFontName.Length(),"BAD font rank");

    TCollection_AsciiString filename = theListOfFontName.Value(aRank); 
    OSD_OpenMode aFileMode = OSD_ReadOnly;
    MFT_FileHandle handle = Open(filename,aFileMode);
#ifdef PRO17604
    if( handle < 0 ) {
      char message[512];
      sprintf(message,"BAD MFT font name '%s'",filename.ToCString());
      MFT_FontManagerError::Raise(message);
    }
#else
    char message[80];
    sprintf(message,"BAD MFT font name '%s'",filename);
    MFT_FontManagerError_Raise_if(handle < 0,message);
#endif

    MFT_FileRecord header;
    header.fileHandle = handle;
    header.beginPosition = header.recordPosition = 0;
    header.recordSize = sizeof(MFT_FileHeader);
    header.update = Standard_False;
    header.precord = NULL;
    header.precordMMAPAddress = NULL;
    header.recordMMAPSize = 0;
#ifndef No_Exception
    Standard_Boolean status = 
#endif
				Read(header);
    MFT_FontManagerError_Raise_if(!status,"HEADER Read error");

    MFT_FileHeader *pfileheader = (MFT_FileHeader*) header.precord;
    MFT_FontManagerDefinitionError_Raise_if(!pfileheader,"BAD MFT header");
    if( pfileheader->signature != MFT_SIGNATURE ) {
      SWAPINT(pfileheader->signature);
    }
    MFT_FontManagerDefinitionError_Raise_if(
	    pfileheader->signature != MFT_SIGNATURE,"BAD MFT signature");
    Aspect_FontStyle aFont((Standard_PCharacter)pfileheader->fontName);
    free((Standard_Address)header.precord);
    Close(handle);

    return aFont;
}

// =============================================================================
//	  ---Purpose: Returns the max values of any defined command
// =============================================================================

Standard_Integer MFT_FontManager::MaxCommandValues()
{
    return MFT_MAXVALUES;
}

// =============================================================================
//        ---Purpose: Returns the encoding of the char position
//        --         <aPosition>.
//        raises FontManagerError from MFT;
//        ---Trigger: If the char is not defined
// =============================================================================

Standard_CString MFT_FontManager::Encoding(const Standard_Integer aPosition)
{
    MFT_FontManagerError_Raise_if(
			!this->IsDefinedChar(aPosition),"CHAR is not defined");
    this->DrawChar(theTextManager,aPosition);
    Standard_CString pencoding = theTextManager->Encoding();

    return pencoding;
}

// =============================================================================
//        ---Purpose: Returns the char position
//        --         from the encoding <anEncoding>.
//        raises FontManagerError from MFT;
//        ---Trigger: If the char is not defined
// =============================================================================

Standard_Integer MFT_FontManager::Encoding(const Standard_CString anEncoding)
{
Standard_Integer i;
Standard_CString pencoding;

    for( i=0 ; i<MaxCharPosition() ; i++ ) {
      if( this->IsDefinedChar(i) ) {
        this->DrawChar(theTextManager,i);
        pencoding = theTextManager->Encoding();
	if( !strcmp(anEncoding,pencoding) ) return i;
      }
    }

    return 0;
}

// =============================================================================
//        ---Purpose: Dumps the font descriptor off all characters.
// =============================================================================

void MFT_FontManager::Dump()
{
MFT_FileHeader *pfileheader = (MFT_FileHeader*)myFileHeader.precord;
Standard_Integer i;

    cout << endl << endl;
    cout << ".MFT_FontManager::Dump() :" << endl;
    cout << " -----------------------" << endl << endl;
    cout << "    File version is : " << pfileheader->version  << endl;
    cout << "    File name is : '" << Path(myFileName) << "'" << endl;
    cout << "    XLFD Font name is : '" << pfileheader->fontName << "'" << endl;
    cout << "    Char entries section begin at : " << pfileheader->pentries << endl;
    cout << "    Commands section begin at : " << pfileheader->pcommands << endl;
    cout << "    Free space address is : " << pfileheader->freeSpace << endl;
    cout << "    Font bounding box is : (" << pfileheader->fontBox[0] << "," <<
	pfileheader->fontBox[1] << "," << pfileheader->fontBox[2] << "," <<
	pfileheader->fontBox[3] << ")" << endl; 
    if( myIsComposite )
      cout << "    The font is COMPOSITE " << endl;
    else 
      cout << "    The font is ASCII " << endl;
    if( myPaintType > 0 )
      cout << "    The font paint type is STROKE" << endl;
    else
      cout << "    The font paint type is FILL" << endl;
    if( myIsFixedPitch )
      cout << "    The font has a fixed char width" << endl;
    else
      cout << "    The font has a proportionnal char width" << endl;
    if( pfileheader->version > 280597 ) {
      cout << "    The font italic angle is " << myItalicAngle << " rad" << endl;
    }

    for( i=0 ; i<this->MaxCharPosition() ; i++ ) {
      if( this->IsDefinedChar(i) ) {
	if( myIsComposite ) this->Dump(Standard_ExtCharacter(i));
	else if( i < MFT_MAXASCIICHARENTRIES ) this->Dump(Standard_Character(i));
	else {
          cout << endl;
          cout << "  .MFT_FontManager::DumpAccent(" << i << ") :" << endl;
          cout << "   -----------------------" << endl << endl;

          this->Dump(i);
	}
      }
    }
}

// =============================================================================
//        ---Purpose: Dumps the font descriptor off the character <aChar>.
// =============================================================================

void MFT_FontManager::Dump(const Standard_Character aChar)
{
Standard_Integer aPosition((unsigned char)aChar);

    cout << endl;
    cout << "  .MFT_FontManager::Dump('" << aChar << "/" << 
					aPosition << "') :" << endl;
    cout << "   -----------------------" << endl << endl;

    this->Dump(aPosition);
}

// =============================================================================
//        ---Purpose: Dumps the font descriptor off the extended character <aChar> .
//        raises FontManagerError from MFT;
//        ---Trigger: If the font is not a composite font.
// =============================================================================

void MFT_FontManager::Dump(const Standard_ExtCharacter aChar)
{
Standard_Integer aPosition((unsigned short)aChar);
    cout << endl;
    cout << "  .MFT_FontManager::Dump(" << aChar << "/" << 
					aPosition << ") :" << endl;
    cout << "   -----------------------" << endl << endl;

    MFT_FontManagerError_Raise_if(
			!myIsComposite,"FONT is not composite");
    this->Dump(aPosition);
}

// =============================================================================
//        ---Purpose: Saves the file in CSF_MDTVFontDirectory/xxxx.dat
// =============================================================================

#define MAXENTRIESBYLINE 8
Standard_Boolean MFT_FontManager::Save()
{
MFT_FileHeader *pfileheader = (MFT_FileHeader*)myFileHeader.precord;
MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
MFT_CommandValue value;
Standard_CString path = Path(myFileName,".dat");
Standard_Integer i,j,n,rp;
Standard_Boolean skip = Standard_False;

    FILE *aWFileHandle = fopen(path,"w");
    if( !aWFileHandle ) {
      cout << "*MFT_FontManager::Save().cann't open the file : '" <<
	path << "',returns with errno " << errno << endl;
      return Standard_False;
    }

    fprintf(aWFileHandle," MFT font comes from '%s'\n",myFileName.ToCString());
    fprintf(aWFileHandle," %d %d %d %d %d %d %d %d %d %d %d\n %s\n",
		pfileheader->iscomposite, pfileheader->pentries,
		pfileheader->pcommands, pfileheader->freeSpace,
		pfileheader->fontBox[0], pfileheader->fontBox[1],
		pfileheader->fontBox[2], pfileheader->fontBox[3],
		pfileheader->paintType, pfileheader->version,
		pfileheader->fixedPitch, pfileheader->fontName);
    if( pfileheader->version > 280597 ) {
      float A;
      A = ITOF(pfileheader->italicAngle);
      fprintf(aWFileHandle," %f\n",A);
    }
    fprintf(aWFileHandle,"#MFT_char_entries_begin_at_position %d\n",
		pfileheader->pentries);
    for( i=n=0 ; i<MaxCharPosition() ; i += MAXENTRIESBYLINE ) {
      for( j=0 ; j<MAXENTRIESBYLINE ; j++ ) {
	if( pcharentries->fcommand[n+j] != 0 ) break;
      }
      if( j < MAXENTRIESBYLINE ) {
	if( skip ) fprintf(aWFileHandle,"*0x%04X/",n);
	else fprintf(aWFileHandle," 0x%04X/",n);
	skip = Standard_False;
        for( j=0 ; j<MAXENTRIESBYLINE ; j++ ) {
          rp = pcharentries->fcommand[n];
          fprintf(aWFileHandle," %8d", rp); n++;
        }
        fprintf(aWFileHandle,"\n");
      } else {
	n += MAXENTRIESBYLINE; skip = Standard_True;
      }
    }
    fprintf(aWFileHandle,"#MFT_commands_begin_at_position %d\n",
		pfileheader->pcommands);
    MFT_TypeOfValue aValueType;
    theNextCommandPosition = pfileheader->pcommands;
    do {
      theCommand = NextCommand(myCommandBuffer);
      rp = theCommandPosition - pfileheader->pcommands;
      value.d = theCommand; 
      fprintf(aWFileHandle," %8d/ '%s' 0x%08x",	rp,
              MFT::Convert(MFT_TypeOfCommand(COMMAND_TYPE(theCommand))),value.i);
      if( COMMAND_LENGTH(theCommand) > 0 ) {
        for( i=1 ; i <= Standard_Integer( COMMAND_LENGTH(theCommand) ); i++ ) {
          aValueType = Value(theCommand,i);
          switch( aValueType ) {
            case MFT_TOV_UNKNOWN:
              fprintf(aWFileHandle," ????????");
              break;
            case MFT_TOV_INTEGER:
              fprintf(aWFileHandle," %d",IValue(myCommandBuffer,i));
              break;
            case MFT_TOV_FLOAT:
              fprintf(aWFileHandle," %f",FValue(myCommandBuffer,i));
              break;
            case MFT_TOV_STRING:
              fprintf(aWFileHandle," '%s'",SValue(myCommandBuffer,i));		
              break;
          }
        }
      }
      fprintf(aWFileHandle,"\n"); 
    } while( theNextCommandPosition < pfileheader->freeSpace );
    fclose(aWFileHandle);
    return Standard_True;
}

// =============================================================================
//        ---Purpose: Restores the file from CSF_MDTVFontDirectory/xxxx.dat
// =============================================================================

Standard_Boolean MFT_FontManager::Restore(const Standard_CString anAliasName)
{
static TCollection_AsciiString svalue;
Standard_CString path = Path(anAliasName,".dat");
Standard_Integer i,j,n,rp,status;
char skip,astring[128];

    FILE *aRFileHandle = fopen(path,"r");
    if( !aRFileHandle ) {
      cout << "*MFT_FontManager::Restore().cann't open the file : '" <<
	path << "',returns with errno " << errno << endl;
      return Standard_False;
    }
    OSD_OpenMode aFileMode = OSD_WriteOnly;
    MFT_FileHandle aWFileHandle = Open(anAliasName,aFileMode);
    if( aWFileHandle < 0 ) {
      cout << "*MFT_FontManager::Restore().cann't open the file : '" <<
      Path(anAliasName) << "',returns with errno " << errno << endl;
      fclose(aRFileHandle);
      return Standard_False;
    }

    MFT_FileRecord aWFileHeader;
    aWFileHeader.fileHandle = aWFileHandle;
    aWFileHeader.beginPosition = aWFileHeader.recordPosition = 0;
    aWFileHeader.recordSize = sizeof(MFT_FileHeader);
    aWFileHeader.update = Standard_True;
    aWFileHeader.precord = (Standard_CString) malloc(aWFileHeader.recordSize);
    aWFileHeader.precordMMAPAddress = NULL;
    aWFileHeader.recordMMAPSize = 0;
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) aWFileHeader.precord;
    if( !pfileheader ) {
      cout << "*MFT_FontManager::Restore().BAD header allocation" << endl;
      fclose(aRFileHandle);
      Close(aWFileHandle);
      return Standard_False;
    }

    pfileheader->signature = MFT_SIGNATURE;
    fscanf(aRFileHandle," MFT font comes from '%s'\n",astring);
    fscanf(aRFileHandle," %d %d %d %d %d %d %d %d %d %d %d\n %s\n",
		&pfileheader->iscomposite, &pfileheader->pentries,
		&pfileheader->pcommands, &pfileheader->freeSpace,
		&pfileheader->fontBox[0], &pfileheader->fontBox[1],
		&pfileheader->fontBox[2], &pfileheader->fontBox[3],
		&pfileheader->paintType, &pfileheader->version,
		&pfileheader->fixedPitch, pfileheader->fontName);
    if( pfileheader->version > 280597 ) {
      float A;
      fscanf(aRFileHandle," %f\n",&A);
      pfileheader->italicAngle = FTOI(A); 
    }
    Write(aWFileHeader); 

    fscanf(aRFileHandle,"#MFT_char_entries_begin_at_position %d\n",&j);
    MFT_FileRecord aWCharEntries;
    aWCharEntries.fileHandle = aWFileHandle;
    aWCharEntries.beginPosition = 
  	aWCharEntries.recordPosition = pfileheader->pentries;
    if( pfileheader->iscomposite ) {
      aWCharEntries.recordSize = sizeof(MFT_ExtendedCharEntries);
    } else {
      aWCharEntries.recordSize = sizeof(MFT_AsciiCharEntries);
    }
    aWCharEntries.update = Standard_True;
    aWCharEntries.precord = (Standard_CString) calloc(aWCharEntries.recordSize,1);
    aWCharEntries.precordMMAPAddress = NULL;
    aWCharEntries.recordMMAPSize = 0;
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) aWCharEntries.precord;
    if( !pcharentries ) {
      cout << "*MFT_FontManager::Restore().BAD char entries allocation" << endl;
      fclose(aRFileHandle);
      Close(aWFileHandle);
      return Standard_False;
    }

    for( i=0 ; i < (int)( aWCharEntries.recordSize/sizeof(MFT_FilePosition) ); i++ ) {
      pcharentries->fcommand[i] = 0;
    }
    for( i=n=0 ; i < (int)( aWCharEntries.recordSize/sizeof(MFT_FilePosition) ); i += MAXENTRIESBYLINE ) {
      fscanf(aRFileHandle,"%c%s\n",&skip,astring);
      if( skip != '#' ) {
        sscanf(astring,"0x%X/",&n);
        for( j=0 ; j<MAXENTRIESBYLINE ; j++ ) {
          fscanf(aRFileHandle,"%d", &rp);
	  pcharentries->fcommand[n] = rp; n++;
        }
        fscanf(aRFileHandle,"\n");
      } else break;
    }
    Write(aWCharEntries); 

    if( skip != '#' )
      fscanf(aRFileHandle,"#MFT_commands_begin_at_position %d\n", &j);
    else
      fscanf(aRFileHandle,"%d\n", &j);
    MFT_FileRecord aWCommandBuffer;
    aWCommandBuffer.fileHandle = aWFileHandle;
    aWCommandBuffer.beginPosition =
                aWCommandBuffer.recordPosition = pfileheader->pcommands;
    aWCommandBuffer.recordSize = sizeof(MFT_CommandBuffer);
    aWCommandBuffer.update = Standard_True;
    aWCommandBuffer.precord = (Standard_CString) malloc(aWCommandBuffer.recordSize);
    aWCommandBuffer.precordMMAPAddress = NULL;
    aWCommandBuffer.recordMMAPSize = 0;
    MFT_TypeOfValue aValueType;
    MFT_CommandValue value;
    Standard_Integer *pcommand;
    theNextCommandPosition = pfileheader->pcommands;
    while ( 
      (status = fscanf(aRFileHandle," %d/ '%s 0x%x",&rp,astring,&value.i)) != EOF ) {
      theCommand = value.d;
      theCommandPosition = pfileheader->pcommands + rp;
      pcommand = (Standard_Integer*) Locate(aWCommandBuffer,theCommandPosition);
      *pcommand = value.i; aWCommandBuffer.update = Standard_True;
      if( COMMAND_LENGTH(theCommand) > 0 ) {
        for( i=1 ; i <= (int)( COMMAND_LENGTH(theCommand) ); i++ ) {
          aValueType = Value(theCommand,i);
          switch( aValueType ) {
            case MFT_TOV_UNKNOWN:
      	      fscanf(aRFileHandle," %s",astring);
	      value.i = 0;
              break;
            case MFT_TOV_INTEGER:
              fscanf(aRFileHandle," %d",&value.i);
              break;
            case MFT_TOV_FLOAT:
              fscanf(aRFileHandle," %f",&value.f);
              break;
            case MFT_TOV_STRING:
              fscanf(aRFileHandle," '%s",astring);		
	      astring[strlen(astring)-1] = '\0';
	      strncpy(value.s,astring,4);
              break;
          }
	  theCommandPosition += sizeof(MFT_CommandValue);
          pcommand = (Standard_Integer*) Locate(aWCommandBuffer,theCommandPosition);
	  *pcommand = value.i; aWCommandBuffer.update = Standard_True;
        }
      }
      fscanf(aRFileHandle,"\n"); 
    }
    fclose(aRFileHandle);
    free((Standard_Address)aWFileHeader.precord);
    free((Standard_Address)aWCharEntries.precord);
    if( aWCommandBuffer.update ) Write(aWCommandBuffer); 
    free((Standard_Address)aWCommandBuffer.precord);
    Close(aWFileHandle);

    return Standard_True;
}

// =============================================================================
//        ---Purpose: Returns the Underline descent position from the origin 
//        --         of the text according to the current font attributes.
// =============================================================================

Quantity_Length MFT_FontManager::UnderlinePosition() const
{
    MFT_FileHeader *pfileheader = (MFT_FileHeader*) myFileHeader.precord;
    Quantity_Length value = 0.;

    if( pfileheader->fontBox[1] < 0 ) {
      if( myCharCapsHeight ) {
        value = -0.6 * myCharHeight * pfileheader->fontBox[1] / pfileheader->fontBox[3];
      } else {
        value = -0.6 * myCharHeight * pfileheader->fontBox[1] / pfileheader->fontBox[2];
      }
    } else {
        value = 0.2 * myCharHeight;
    }

    return value;
}

//        ----------------------------
//        -- Category: Private methods
//        ----------------------------

// =============================================================================
//        ---Purpose: Open the file ,load and verify the header
//        --         and returns the file handle & file header address 
//        --         when the file is opened correctly.
//        raises OutOfMemory from Standard is private;
//        ---Trigger: If header allocation fails 
// =============================================================================
 

MFT_FileHandle MFT_FontManager::Open(const TCollection_AsciiString& aFileName, const OSD_OpenMode aFileMode)
{
MFT_FileHandle aFileHandle = -1;
TCollection_AsciiString path(Path(aFileName));

    if( path.Length() > 0 ) {
      Standard_Integer i,nfont = theListOfOpenFontName.Length();
      if( nfont > 0 ) {
	for( i=1 ; i<=nfont ; i++ ) {
	  if( path == theListOfOpenFontName.Value(i) ) {
		// USE an already opened font
	    aFileHandle = theListOfOpenFontHandle.Value(i);		
	    Standard_Integer ref = theListOfOpenFontReference.Value(i);
	    theListOfOpenFontReference.SetValue(i,ref+1);
	    break;
	  }
	}
      }
      if( aFileHandle < 0 ) {
      	switch (aFileMode) {
          case OSD_ReadOnly:
	          aFileHandle = open(path.ToCString(), O_RDONLY
#ifndef WNT
            );
#else
            | O_BINARY );
#endif
      	    break;

          case OSD_WriteOnly:
	          aFileHandle = open(path.ToCString(), O_CREAT | O_RDWR | O_EXCL 
#ifndef WNT
              ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#else
              | O_BINARY, S_IREAD | S_IWRITE);
#endif
      	    break;

          case OSD_ReadWrite:
            aFileHandle = open(path.ToCString(),O_RDWR
#ifndef WNT
            );
#else
            | O_BINARY);
#endif
	      }

        if( aFileHandle < 0 ) {
          cout << "*IO Error status " << errno << 
	  " in MFT_FontManager::Open('" << path << "'," << 
			Standard_Integer(aFileMode) << ")" << endl;
	} else {
		// SAVE the just opened font
          theListOfOpenFontName.Append(path);
          theListOfOpenFontHandle.Append(aFileHandle);
          theListOfOpenFontReference.Append(1);
        }
      }
    }

#if TRACE > 0
    cout << aFileHandle << " = MFT_FontManager::Open('" << path <<
	 "'," << Standard_Integer(aFileMode) << ")" << endl;
#endif

    return aFileHandle;
}

// =============================================================================
//        ---Purpose: Close the file 
//        --         and returns TRUE if the file was successfully closed.
// =============================================================================

Standard_Boolean MFT_FontManager::Close(const MFT_FileHandle& aFileHandle)
{
   Standard_Integer i,nfont = theListOfOpenFontHandle.Length();
   Standard_Integer status = 0;
   if( nfont > 0 ) {
     for( i=1 ; i<=nfont ; i++ ) {
       if( aFileHandle == theListOfOpenFontHandle.Value(i) ) { 
	 Standard_Integer ref = theListOfOpenFontReference.Value(i);
	 if( ref > 1 ) {
	   theListOfOpenFontReference.SetValue(i,ref-1);
	   status = -1;
	 } else {
	   theListOfOpenFontName.Remove(i);
	   theListOfOpenFontHandle.Remove(i);
	   theListOfOpenFontReference.Remove(i);
         }
	 break;
	}
      }
    }

    if( status >= 0 ) {
      status = close(aFileHandle);
    }

#if TRACE > 0
    cout << status << " = MFT_FontManager::Close(" << aFileHandle << ")" << endl;
#endif

    return (status >= 0) ? Standard_True : Standard_False;
}

// =============================================================================
//        ---Purpose: Saves updated records and Close this file 
//        --         and returns TRUE if the file was successfully closed.
// =============================================================================

Standard_Boolean MFT_FontManager::Close()
{
				//Save file
    if( myFileHeader.update ) Write(myFileHeader);
    if( myCharEntries.update ) Write(myCharEntries);
    if( myCommandBuffer.update ) Write(myCommandBuffer);

				//Free memories
#ifdef MUNMAP
    if( myFileHeader.recordMMAPSize && myFileHeader.precordMMAPAddress ) {
#if TRACE_MMAP > 0
	printf(" FH_MUNMAP(%lx,%ld)\n",
		myFileHeader.precordMMAPAddress,myFileHeader.recordMMAPSize);
#endif
    	MUNMAP(myFileHeader.precordMMAPAddress,myFileHeader.recordMMAPSize);
    } else 
#endif
      if( myFileHeader.precord ) free((Standard_Address)myFileHeader.precord);
    myFileHeader.precord = NULL;
    myFileHeader.precordMMAPAddress = NULL;

#ifdef MUNMAP
    if( myCharEntries.recordMMAPSize && myCharEntries.precordMMAPAddress )  {
#if TRACE_MMAP > 0
	printf(" CE_MUNMAP(%lx,%ld)\n",
		myCharEntries.precordMMAPAddress,myCharEntries.recordMMAPSize);
#endif
	MUNMAP(myCharEntries.precordMMAPAddress,myCharEntries.recordMMAPSize);
    } else 
#endif
      if( myCharEntries.precord ) free((Standard_Address)myCharEntries.precord);
    myCharEntries.precord = NULL;
    myCharEntries.precordMMAPAddress = NULL;

#ifdef MUNMAP
    if( myCommandBuffer.recordMMAPSize && myCommandBuffer.precordMMAPAddress ) {
#if TRACE_MMAP > 0
	printf(" CB_MUNMAP(%lx,%ld)\n",
	myCommandBuffer.precordMMAPAddress,myCommandBuffer.recordMMAPSize);
#endif
	MUNMAP(myCommandBuffer.precordMMAPAddress,myCommandBuffer.recordMMAPSize);
    } else 
#endif
      if( myCommandBuffer.precord ) free((Standard_Address)myCommandBuffer.precord);
    myCommandBuffer.precord = NULL;
    myCommandBuffer.precordMMAPAddress = NULL;

    return Close(myFileHandle);
}

// =============================================================================
//        ---Purpose: Reads a record from the file.
//        --         Returns TRUE if the file was successfully read.
// =============================================================================

Standard_Boolean MFT_FontManager::Read(MFT_FileRecord& aRecord) {

#if TRACE > 1
    cout << "MFT_FontManager::Read(Handle(" << aRecord.fileHandle <<
		"),Position(" << aRecord.recordPosition << 
		"),Size(" << aRecord.recordSize << "))" << endl;
#endif

#ifdef S3593
# ifdef MMAP
    static MFT_FilePosition position,offset;
# else
    static MFT_FilePosition position;
# endif
#else
# ifdef MMAP
    MFT_FilePosition position,offset;
# else
    MFT_FilePosition position;
# endif
#endif

#ifdef MMAP
    if( aRecord.recordMMAPSize ) {
      position = (MFT_FilePosition)
		(aRecord.recordMMAPSize * (aRecord.recordPosition/
						aRecord.recordMMAPSize)); 
      offset = aRecord.recordPosition - position;

      if( !aRecord.precordMMAPAddress || 
		(position != aRecord.recordMMAPPosition) ) {
	aRecord.recordMMAPPosition = position;
# ifdef MUNMAP
        if( aRecord.precordMMAPAddress ) {
#  if TRACE_MMAP > 0
	  printf(" MUNMAP(%lx,%ld)\n",
		aRecord.precordMMAPAddress,aRecord.recordMMAPSize);
#  endif
	  MUNMAP(aRecord.precordMMAPAddress,aRecord.recordMMAPSize);
	}
# endif
        aRecord.precordMMAPAddress = 
	  (Standard_CString) MMAP(position,aRecord.recordMMAPSize,aRecord.fileHandle);
# if TRACE_MMAP > 0
	printf(" %lx = MMAP(%d,%ld,%d)\n", aRecord.precordMMAPAddress,
			position,aRecord.recordMMAPSize,aRecord.fileHandle);
# endif
        if( aRecord.precordMMAPAddress == (Standard_CString)MAP_FAILED ) { 
	  char message[64];
	  sprintf(message,"MEMORY mapping failed with errno = %d\n",errno);
          Standard_OutOfMemory::Raise(message);
        }
      }
      aRecord.precord = aRecord.precordMMAPAddress + offset;
    } else {
#endif
      position = (MFT_FilePosition)
	lseek(aRecord.fileHandle, (long)aRecord.recordPosition, SEEK_SET);
      if( position < 0 ) {
        cout << "*IO Error status " << errno << 
		" in MFT_FontManager::Read(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "))" << endl;
        return Standard_False;
      }
      if( aRecord.precord == NULL ) {
        aRecord.precord = (Standard_CString) malloc(aRecord.recordSize);
        Standard_OutOfMemory_Raise_if(
                !aRecord.precord,"MEMORY allocation failed");
      }
      Standard_Integer i,length; 
      Standard_CString precord = aRecord.precord;
      for( i=0 ; i < (int) aRecord.recordSize ;
			i += MFT_BLOCKSIZE,precord += MFT_BLOCKSIZE) {
	length = read(aRecord.fileHandle, (Standard_Address)precord, MFT_BLOCKSIZE);

        if( length == -1 ) {
          cout << "*IO Error status " << errno << 
		" in MFT_FontManager::Read(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "),Size(" <<
		aRecord.recordSize << "))" << endl;
          return Standard_False;
        } else if( length > 0 && length != MFT_BLOCKSIZE ) {
          cout << "*IO warning " << length << 
		" in MFT_FontManager::Read(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "),Size(" <<
		aRecord.recordSize << "))" << endl;
        }
      }
#ifdef MMAP
    }
#endif

    return Standard_True;
}

// =============================================================================
//        ---Purpose: Writes a record to the file.
//        --         Returns TRUE if the file was successfully written.
// =============================================================================

Standard_Boolean MFT_FontManager::Write(MFT_FileRecord& aRecord) {

#if TRACE > 1
    cout << "MFT_FontManager::Write(Handle(" << aRecord.fileHandle <<
		"),Position(" << aRecord.recordPosition << 
		"),Size(" << aRecord.recordSize << "))" << endl;
#endif

#ifdef S3593
    static MFT_FilePosition position;
#else
    MFT_FilePosition position;
#endif

    position = (MFT_FilePosition)
	lseek(aRecord.fileHandle,(long)aRecord.recordPosition,SEEK_SET);
    if( position < 0 ) {
      cout << "*IO Error status " << errno << 
		" in MFT_FontManager::Write(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "))" << endl;
      return Standard_False;
    }
    if( aRecord.precord == NULL ) {
      Standard_OutOfMemory_Raise_if(
                !aRecord.precord,"UNALLOCATED memory");
    }
    Standard_Integer i,length; 
    Standard_CString precord = aRecord.precord;
    for( i=0 ; i < (int) aRecord.recordSize ; 
			i += MFT_BLOCKSIZE,precord += MFT_BLOCKSIZE ) {
      length = write(aRecord.fileHandle,precord,MFT_BLOCKSIZE);

      if( length == -1 ) {
        cout << "*IO Error status " << errno << 
		" in MFT_FontManager::Write(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "),Size(" <<
		aRecord.recordSize << "))" << endl;
        return Standard_False;
      } else if( length != MFT_BLOCKSIZE ) {
        cout << "*IO warning " << length << 
		" in MFT_FontManager::Write(Handle(" << aRecord.fileHandle << 
		"),Position(" << aRecord.recordPosition << "),Size(" <<
		aRecord.recordSize << "))" << endl;
      }
    }
    aRecord.update = Standard_False;

    return Standard_True;
}

// =============================================================================
//        ---Purpose: Locates an information from the file record.
//        --         and returns the info address.
//        ---Warning: May read or write the current record
//        --         from/to the file and reload an other record
//        --         according the file position info.
// =============================================================================

Standard_Address MFT_FontManager::Locate(MFT_FileRecord& aRecord, const MFT_FilePosition& aFilePosition) {
Standard_Address aRecordAddress;
#ifdef S3593
#ifdef MMAP
    if( aRecord.precordMMAPAddress &&
		(aFilePosition < aRecord.recordMMAPSize) ) { 
      aRecordAddress = (Standard_Address)(aRecord.precordMMAPAddress + aFilePosition);

      return aRecordAddress;
    }
#endif
#endif

#ifdef S3593
    static MFT_FilePosition aRecordNumber; 
#else
    MFT_FilePosition aRecordNumber; 
#endif

    aRecordNumber = (aFilePosition - aRecord.beginPosition)/aRecord.recordSize;

    if( aRecordNumber < 0 ) {
      cout << "*MAPPING Error in MFT_FontManager::Locate(Handle(" <<
		aRecord.fileHandle << "),BeginPosition(" << 
		aRecord.beginPosition << "),Position(" << 
		aRecord.recordPosition << "),Size(" << 
		aRecord.recordSize << ")," << aFilePosition << ")" << endl;
      MFT_FontManagerError::Raise("BAD File position");
    }
#ifdef S3593
    static MFT_FilePosition aRecordPosition;
#else
    MFT_FilePosition aRecordPosition;
#endif
    aRecordPosition = aRecord.beginPosition + 
				aRecordNumber * aRecord.recordSize;

    if( !aRecord.precord || (aRecordPosition != aRecord.recordPosition) ) {
      if( aRecord.update ) Write(aRecord);
      aRecord.recordPosition = aRecordPosition;
      aRecord.update = Standard_False;
      Read(aRecord);
    }

    aRecordAddress = (Standard_Address)(aRecord.precord + (aFilePosition - aRecordPosition));

    return aRecordAddress;
}

// =============================================================================
//        ---Purpose: Computes the full path of the font file name
//        --          $CSF_MDTVFontDirectory/<anAliasName><anExtension>
// =============================================================================

Standard_CString MFT_FontManager::Path(const TCollection_AsciiString& aFileName,const Standard_CString anExtension)
{
static TCollection_AsciiString pathname;
TCollection_AsciiString dir(CSF_MDTVFontDirectory.Value());
  if( dir.Length() > 0 ) {pathname = dir; pathname += "/";}
  else pathname.Clear();
  if( aFileName.Length() > 0 ) {
    pathname += aFileName;
    if( pathname.SearchFromEnd(anExtension) < 0 ) {
      pathname += anExtension;
    }
  }

  return pathname.ToCString();
}

// =============================================================================
//        ---Purpose: Returns the max position of any defined char
//        --         in the header of the file according with the
//        --         type of the font.
// =============================================================================

Standard_Integer MFT_FontManager::MaxCharPosition() const
{
    return myCharEntries.recordSize/sizeof(MFT_FilePosition) - 1;
}

// =============================================================================
//        ---Purpose: Returns TRUE if the char at position <aPosition> is defined
//        --          in the header of the file.
//        raises FontManagerError from MFT is private;
//        ---Trigger: If the position <aPosition> is < 0 or > MaxCharPosition().
// =============================================================================

Standard_Boolean MFT_FontManager::IsDefinedChar(const Standard_Integer aPosition) const
{
    unsigned int position = aPosition & 0xFFFF; 
    Standard_OutOfRange_Raise_if(
        position > (unsigned int) MaxCharPosition(),"BAD char rank");

    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    if( pcharentries->fcommand[position] > 0 ) return Standard_True;

    return Standard_False;
}

// =============================================================================
//        ---Purpose: Returns the first defined char position
//        --          in the header of the file..
// =============================================================================

Standard_Integer MFT_FontManager::FirstDefinedChar() const
{
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    Standard_Integer i,n = (myIsComposite) ? 
		MFT_MAXEXTENDEDCHARENTRIES : MFT_MAXASCIICHARENTRIES;
    for( i=0 ; i<n ; i++ ) {
      if( pcharentries->fcommand[i] > 0 ) return i;
    }
    return 0;
}

// =============================================================================
//        ---Purpose: Returns the last defined char position
//        --          in the header of the file..
// =============================================================================

Standard_Integer MFT_FontManager::LastDefinedChar() const
{
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    Standard_Integer i,n = (myIsComposite) ? 
		MFT_MAXEXTENDEDCHARENTRIES : MFT_MAXASCIICHARENTRIES;
    for( i=n-1 ; i>0 ; --i ) {
      if( pcharentries->fcommand[i] > 0 ) return i;
    }
    return 0;
}

// =============================================================================
//        ---Purpose: Enable the char of position <aPosition> for reading.
// =============================================================================

void MFT_FontManager::SetChar (const Standard_Integer aPosition)
{
    unsigned int position;
#if TRACE > 1
    cout << " MFT_FontManager::SetChar(" << position << ")" << endl;
#endif
    MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
    if( aPosition < 0 ) {
      position = 0;
      theFirstCommandPosition = theNextCommandPosition = -aPosition;
    } else {
      position = aPosition & 0xFFFF;
      theFirstCommandPosition = theNextCommandPosition = 
				pcharentries->fcommand[position];
    }
    theCharPosition = position;
    if( theFirstCommandPosition > 0 ) return;

    if( myIsComposite ) {
      if( position > 0x20 && position <= 0x7F ) {
		position += 0xFEE0;	// Sets UNICODE ascii char
        if( pcharentries->fcommand[position] == 0 ) position = 0x3000;
      } else position = 0x3000; 		// Sets UNICODE BLANK char
    } else {
      position = 0;			// Sets DEFAULT char
    }

    theCharPosition = position;
    theFirstCommandPosition = theNextCommandPosition = // Sets UNDEF char
				pcharentries->fcommand[position];
}

// =============================================================================
//        ---Purpose: Drawn the char at position <aPosition>
//	   --   with the draw manager <aTextManager>.
// =============================================================================

Standard_Boolean MFT_FontManager::DrawChar (const Handle(MFT_TextManager)& aTextManager, const Standard_Integer aPosition)
{
#if TRACE > 1
    cout << " MFT_FontManager::DrawChar(" << aPosition << ")" << endl;
#endif

    Standard_Boolean vazy = Standard_True;
    Standard_Real X,Y,XX,YY,X1,Y1,X2,Y2,X3,Y3,X4,Y4;
    Standard_Integer position = aPosition;

    theDotSection = Standard_False;

    this->SetChar(position);
    if( theFirstCommandPosition > 0 ) {
      if( !theStackCount ) { 
        theCharX = theCharY = 0;
        theLeftCharX = theLeftCharY = 0;
        theCharWidth = theCharHeight = 0;
	if( myIsFixedPitch ) {
          theStringX = theFixedCharWidth*theCharCount*theCosCharAngle;
          theStringY = theFixedCharWidth*theCharCount*theSinCharAngle;
	} else {
          theStringX = theFontXRatio*theDeltaX*theCosCharAngle;
          theStringY = theFontXRatio*theDeltaX*theSinCharAngle;
	}
        vazy = aTextManager->BeginChar(position,theStringX,theStringY);
      }
      if( vazy ) do {
        theCommand = NextCommand(myCommandBuffer);
        theCommandType = COMMAND_TYPE(theCommand);
        theCommandLength = COMMAND_LENGTH(theCommand);
#if TRACE > 2
    cout << " MFT_FontManager::DrawChar.'" << 
			MFT::Convert(theCommandType) << "'(";
#endif
        theValueCount = 0;
					// GET command values
        while ( theValueCount < theCommandLength ) {
	  theValueType = Value(theCommand,theValueCount+1);
#if TRACE > 2
    cout << "[" << MFT::Convert(theValueType) << " = ";
#endif
          switch( theValueType ) {
            case MFT_TOV_UNKNOWN:
              break;
            case MFT_TOV_INTEGER:
              theIValues[theValueCount] = IValue(myCommandBuffer,theValueCount+1);
              theFValues[theValueCount] =
                        Standard_Real(theIValues[theValueCount]);
#if TRACE > 2
    cout << theIValues[theValueCount];
#endif
              break;
            case MFT_TOV_FLOAT:
              theFValues[theValueCount] = FValue(myCommandBuffer,theValueCount+1);
              theIValues[theValueCount] =
                        Standard_Integer(theFValues[theValueCount]);
#if TRACE > 2
    cout << theFValues[theValueCount];
#endif
              break;
            case MFT_TOV_STRING:
              strcpy(&theSValues[theValueCount*sizeof(MFT_CommandValue)],
                                                SValue(myCommandBuffer,theValueCount+1))
;
#if TRACE > 2
    cout << theSValues[theValueCount*sizeof(MFT_CommandValue)];
#endif
              break;
          }
#if TRACE > 2
    cout << "]";
#endif
          theValueCount++;
        }
#if TRACE > 2
    cout << ")" << endl;
#endif
					// EXECUTE command
        switch (theCommandType) {
          case MFT_TOC_UNKNOWN:
            break;
          case MFT_TOC_HSTEM:                 //UNIMPLEMENTED
            CHECKCOMMAND(2);
            break;
          case MFT_TOC_VSTEM:                 //UNIMPLEMENTED
            CHECKCOMMAND(2);
            break;
          case MFT_TOC_VMOVETO:
            CHECKCOMMAND(1);
            theCharY += theFValues[0];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Moveto(X,Y);
            break;
          case MFT_TOC_RLINETO:
            CHECKCOMMAND(2);
            theCharX += theFValues[0];
            theCharY += theFValues[1];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Lineto(X,Y);
            break;
          case MFT_TOC_HLINETO:
            CHECKCOMMAND(1);
            theCharX += theFValues[0];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Lineto(X,Y);
            break;
          case MFT_TOC_VLINETO:
            CHECKCOMMAND(1);
            theCharY += theFValues[0];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Lineto(X,Y);
            break;
          case MFT_TOC_RRCURVETO:
            CHECKCOMMAND(6);
	    X1 = theCharX; Y1 = theCharY;
	    X2 = X1 + theFValues[0]; Y2 = Y1 + theFValues[1]; 
	    X3 = X2 + theFValues[2]; Y3 = Y2 + theFValues[3]; 
	    XX = X4 = X3 + theFValues[4]; YY = Y4 = Y3 + theFValues[5]; 
	    TRANSFORM(X1,Y1); TRANSFORM(X2,Y2);
	    TRANSFORM(X3,Y3); TRANSFORM(X4,Y4);
	    if( !aTextManager->Curveto(X1,Y1,X2,Y2,X3,Y3,X4,Y4) ) {
              SETCURVE( theFValues[0],theFValues[1],
                        theFValues[2],theFValues[3],
                        theFValues[4],theFValues[5]);
	      if( theCurveNT < 5 ) {
	        vazy = aTextManager->Lineto(X2,Y2);
	        vazy = aTextManager->Lineto(X3,Y3);
	      } else {
	        for( theCurveT=theCurveDT ; theCurveT<1. ; 
						theCurveT+=theCurveDT) {
	          GETCURVE(theCurveT);
                  if( ((Abs(theCharX - theOldCharX) +
                    Abs(theCharY - theOldCharY)) > theCurvePrecision) ) {
	            X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	            vazy = aTextManager->Lineto(X,Y);
                    theOldCharX = theCharX; theOldCharY = theCharY;
	          }
	        }
	      }
	      vazy = aTextManager->Lineto(X4,Y4);
	    }
	    theCharX = XX; theCharY = YY;
            break;
          case MFT_TOC_CLOSEPATH:
	    aTextManager->ClosePath();
            break;
          case MFT_TOC_HSBW:
            CHECKCOMMAND(2);
            theLeftCharX = theFValues[0];
            theLeftCharY = 0;
            theCharWidth = theFValues[1];
            theCharHeight = 0;
    	    if( !theStackCount ) { 
              theCharX = theLeftCharX;
              theCharY = theLeftCharY;
	      X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	      vazy = aTextManager->Moveto(X,Y);
	    }
            break;
          case MFT_TOC_ENDCHAR:
            break;
          case MFT_TOC_RMOVETO:
            CHECKCOMMAND(2);
            theCharX += theFValues[0];
            theCharY += theFValues[1];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Moveto(X,Y);
            break;
          case MFT_TOC_HMOVETO:
            CHECKCOMMAND(1);
            theCharX += theFValues[0];
	    X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	    vazy = aTextManager->Moveto(X,Y);
            break;
          case MFT_TOC_VHCURVETO:
            CHECKCOMMAND(4);
	    X1 = theCharX; Y1 = theCharY;
	    X2 = X1; Y2 = Y1 + theFValues[0]; 
	    X3 = X2 + theFValues[1]; Y3 = Y2 + theFValues[2]; 
	    XX = X4 = X3 + theFValues[3]; YY = Y4 = Y3; 
	    TRANSFORM(X1,Y1); TRANSFORM(X2,Y2);
	    TRANSFORM(X3,Y3); TRANSFORM(X4,Y4);
	    if( !aTextManager->Curveto(X1,Y1,X2,Y2,X3,Y3,X4,Y4) ) {
              SETCURVE( 0,theFValues[0],
                        theFValues[1],theFValues[2],
                        theFValues[3],0);
	      if( theCurveNT < 5 ) {
	        vazy = aTextManager->Lineto(X2,Y2);
	        vazy = aTextManager->Lineto(X3,Y3);
	      } else {
	        for( theCurveT=theCurveDT ; theCurveT<1. ; 
						theCurveT+=theCurveDT) {
	          GETCURVE(theCurveT);
                  if( ((Abs(theCharX - theOldCharX) +
                    Abs(theCharY - theOldCharY)) > theCurvePrecision) ) {
	            X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	            vazy = aTextManager->Lineto(X,Y);
                    theOldCharX = theCharX; theOldCharY = theCharY;
	          }
	        }
	      }
	      vazy = aTextManager->Lineto(X4,Y4);
	    }
	    theCharX = XX; theCharY = YY;
            break;
          case MFT_TOC_HVCURVETO:
            CHECKCOMMAND(4);
	    X1 = theCharX; Y1 = theCharY;
	    X2 = X1 + theFValues[0]; Y2 = Y1; 
	    X3 = X2 + theFValues[1]; Y3 = Y2 + theFValues[2]; 
	    XX = X4 = X3; YY = Y4 = Y3 + theFValues[3]; 
	    TRANSFORM(X1,Y1); TRANSFORM(X2,Y2);
	    TRANSFORM(X3,Y3); TRANSFORM(X4,Y4);
	    if( !aTextManager->Curveto(X1,Y1,X2,Y2,X3,Y3,X4,Y4) ) {
              SETCURVE( theFValues[0],0,
                        theFValues[1],theFValues[2],
                        0,theFValues[3]);
	      if( theCurveNT < 5 ) {
	        vazy = aTextManager->Lineto(X2,Y2);
	        vazy = aTextManager->Lineto(X3,Y3);
	      } else {
	        for( theCurveT=theCurveDT ; theCurveT<1. ; 
						theCurveT+=theCurveDT) {
	          GETCURVE(theCurveT);
                  if( ((Abs(theCharX - theOldCharX) +
                    Abs(theCharY - theOldCharY)) > theCurvePrecision) ) {
	            X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	            vazy = aTextManager->Lineto(X,Y);
                    theOldCharX = theCharX; theOldCharY = theCharY;
	          }
	 	}
	      }
	      vazy = aTextManager->Lineto(X4,Y4);
	    }
	    theCharX = XX; theCharY = YY;
            break;
          case MFT_TOC_DOTSECTION:                    //UNIMPLEMENTED
            theDotSection = !theDotSection;
            break;
          case MFT_TOC_VSTEM3:                        //UNIMPLEMENTED
            CHECKCOMMAND(6);
            break;
          case MFT_TOC_HSTEM3:                        //UNIMPLEMENTED
            CHECKCOMMAND(6);
            break;
          case MFT_TOC_SEAC:
            CHECKCOMMAND(5);
            theSeacAsbValue = theFValues[0];
            theSeacAdxValue = theFValues[1];
            theSeacAdyValue = theFValues[2];
            theSeacBaseChar = theIValues[3];
            theSeacAccentChar = theIValues[4];
	    IPUSH(theFirstCommandPosition);
            IPUSH(theNextCommandPosition);
            RPUSH(theCharWidth);
	    this->DrawChar(aTextManager,theSeacBaseChar);
            theCharX = theLeftCharX + theSeacAdxValue;
            theCharY = theLeftCharY + theSeacAdyValue;
	    this->DrawChar(aTextManager,theSeacAccentChar);
            RPOP(theCharWidth);
            IPOP(theNextCommandPosition);
            IPOP(theFirstCommandPosition);
            break;
          case MFT_TOC_SBW:
            CHECKCOMMAND(4);
            theLeftCharX = theFValues[0];
            theLeftCharY = theFValues[1];
            theCharWidth = theFValues[2];
            theCharHeight = theFValues[3];
    	    if( !theStackCount ) { 
              theCharX = theLeftCharX;
              theCharY = theLeftCharY;
	      X = theCharX; Y = theCharY; TRANSFORM(X,Y);
	      vazy = aTextManager->Moveto(X,Y);
	    }
            break;
          case MFT_TOC_SETCURRENTPOINT:
            CHECKCOMMAND(2);
            theCharX = theFValues[0];
            theCharY = theFValues[1];
            break;
          case MFT_TOC_CHARSTRING:
	    if( theSValues[0] == '/' )
	    	vazy = aTextManager->SetCharEncoding(&theSValues[1]);
            break;
          case MFT_TOC_NUMERIC:                       //UNIMPLEMENTED
            break;
          case MFT_TOC_MINMAX:
            CHECKCOMMAND(4);
	    theCharWidth = theFValues[2];
            X1 = theFValues[0]; Y1 = theFValues[1];
            X3 = theFValues[2]; Y3 = theFValues[3];
	    X2 = X3; Y2 = Y1;
	    X4 = X1; Y4 = Y3;
	    TRANSFORM(X1,Y1); TRANSFORM(X2,Y2); 
	    TRANSFORM(X3,Y3); TRANSFORM(X4,Y4);
	    vazy = aTextManager->SetCharBoundingBox(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
            break;
          }

      } while ( vazy && (theCommandType != MFT_TOC_ENDCHAR) );
    }
    if( !theStackCount ) { 
      if( theCharWidth > 0. ) theDeltaX += theCharWidth;
      else theDeltaX += myCharWidth/theFontXRatio;
      theStringX = theFontXRatio*theDeltaX*theCosCharAngle;
      theStringY = theFontXRatio*theDeltaX*theSinCharAngle;
      vazy = aTextManager->EndChar(theStringX,theStringY);
    }

    return vazy;
}

// =============================================================================
//        ---Purpose: Computes the bounding box of the char position <aPosition>
// =============================================================================

void MFT_FontManager::ComputeBoundingBox (const Standard_Integer aPosition)
{
#if TRACE > 1
    cout << " MFT_FontManager::ComputeBoundingBox(" << aPosition << ")" << endl;
#endif

    theTanCharSlant = 0.;
    theFontXRatio = theFontYRatio = 1.;
    theOrientation = 0.;
    theDeltaX = 0.;
    theSinCharAngle = 0.;
    theCosCharAngle = 1.;
    Standard_Boolean fixedPitch = myIsFixedPitch;
    myIsFixedPitch = Standard_False;

    this->DrawChar(theTextManager,aPosition);
    myIsFixedPitch = fixedPitch;

    Quantity_Length Xmin,Ymin,Xmax,Ymax;
    theTextManager->MinMax(Xmin,Ymin,Xmax,Ymax);
    TCollection_AsciiString encoding = theTextManager->Encoding();
    if( encoding == ".notdef" ) {
      MFT_CharEntries *pcharentries = (MFT_CharEntries*) myCharEntries.precord;
      pcharentries->fcommand[0] = theFirstCommandPosition;
    }

    Standard_Integer *pvalue;
    theCommandPosition = theFirstCommandPosition + sizeof(MFT_CommandDescriptor);
    pvalue = (Standard_Integer*) Locate(myCommandBuffer,theCommandPosition);
    *pvalue = Standard_Integer(Xmin);
    myCommandBuffer.update = Standard_True;

    theCommandPosition += sizeof(MFT_CommandValue);
    pvalue = (Standard_Integer*) Locate(myCommandBuffer,theCommandPosition);
    *pvalue = Standard_Integer(Ymin);
    myCommandBuffer.update = Standard_True;

    theCommandPosition += sizeof(MFT_CommandValue);
    pvalue = (Standard_Integer*) Locate(myCommandBuffer,theCommandPosition);
    *pvalue = Standard_Integer(Xmax);
    myCommandBuffer.update = Standard_True;

    theCommandPosition += sizeof(MFT_CommandValue);
    pvalue = (Standard_Integer*) Locate(myCommandBuffer,theCommandPosition);
    *pvalue = Standard_Integer(Ymax);
    myCommandBuffer.update = Standard_True;
}

// =============================================================================
//        ---Purpose: Returns the next command type and length
//        --          from the current char (see SetChar()).
//        ---Warning: the last command of the char descriptor
//        --         has a type MFC_TOC_ENDCHAR.
// =============================================================================

MFT_CommandDescriptor MFT_FontManager::NextCommand(MFT_FileRecord& aRecord)
{
#ifdef S3593
static MFT_CommandValue value;
#else
MFT_CommandValue value;
#endif

    theCommandPosition = theNextCommandPosition;
    value.i = *(Standard_Integer*) Locate(aRecord,theCommandPosition);
    if( aRecord.swap ) SWAPINT(value.i);
    theNextCommandPosition += sizeof(MFT_CommandDescriptor) +
                        COMMAND_LENGTH(value.d) * sizeof(MFT_CommandValue);
    return value.d;
}

// =============================================================================
//        ---Purpose: Returns the value type at position <aRank>
//        --          of the current command.
//        raises OutOfRange from Standard is private;
//        ---Trigger: If the position <aRank> is < 1 or > the command length.
// =============================================================================

MFT_TypeOfValue MFT_FontManager::Value(const MFT_CommandDescriptor& aDescriptor, const Standard_Integer aRank)
{

#ifdef S3593
static MFT_CommandDescriptor d;

    d = aDescriptor;
    if( aRank < 1 || aRank > (int)COMMAND_LENGTH(d) )
	Standard_OutOfRange::Raise("BAD command value rank");

    unsigned int type = COMMAND_VALUETYPE(d,aRank);
#else
    Standard_OutOfRange_Raise_if(
		aRank < 1 || aRank > COMMAND_LENGTH(aDescriptor),
					"BAD command value rank");

    unsigned int type = COMMAND_VALUETYPE(aDescriptor,aRank);
#endif

    MFT_TypeOfValue aValueType = MFT_TypeOfValue(type);

#if TRACE > 3
    cout << MFT::Convert(aValueType) << " = MFT_FontManager::Value(" 
						<< aRank << ")" << endl;
#endif

    return aValueType;
}

// =============================================================================
//        ---Purpose: Returns the integer parameter at position <aRank>
//        --          of the current command.
//        raises FontManagerError from MFT is private;
//        ---Trigger: If the position <aRank> is < 1 or > the command length.
//        --          or if the value is not an INT value.
// =============================================================================

Standard_Integer MFT_FontManager::IValue(MFT_FileRecord& aRecord, const Standard_Integer aRank)
{
#ifdef S3593
static MFT_CommandValue value;

    if( Value(theCommand,aRank) != MFT_TOV_INTEGER ) 
	Standard_TypeMismatch::Raise("BAD command value type");
#else
MFT_CommandValue value;

    Standard_TypeMismatch_Raise_if(
		Value(theCommand,aRank) != MFT_TOV_INTEGER,
					"BAD command value type");
#endif

    value.i = *(Standard_Integer*)
	Locate(aRecord,theCommandPosition + 
		sizeof(MFT_CommandDescriptor) + (aRank-1) * 
					sizeof(MFT_CommandValue));
    if( aRecord.swap ) SWAPINT(value.i);
    return value.i;
}

// =============================================================================
//        ---Purpose: Returns the float parameter at position <aRank>
//        --          of the current command.
//        raises FontManagerError from MFT is private;
//        ---Trigger: If the position <aRank> is < 1 or > the command length.
//        --          or if the value is not a FLOAT value.
// =============================================================================

Standard_ShortReal MFT_FontManager::FValue(MFT_FileRecord& aRecord,const Standard_Integer aRank)
{
#ifdef S3593
static MFT_CommandValue value;

    if( Value(theCommand,aRank) != MFT_TOV_FLOAT ) 
	Standard_TypeMismatch::Raise("BAD command value type");
#else
MFT_CommandValue value;

    Standard_TypeMismatch_Raise_if(
		Value(theCommand,aRank) != MFT_TOV_FLOAT,
					"BAD command value type");
#endif

    value.i = *(Standard_Integer*)
	Locate(aRecord,theCommandPosition + 
		sizeof(MFT_CommandDescriptor) + (aRank-1) * 
					sizeof(MFT_CommandValue));
    if( aRecord.swap ) SWAPFLOAT(value.f);
    return value.f;
}

// =============================================================================
//        ---Purpose: Returns the string parameter at position <aRank>
//        --          of the current command.
//        raises FontManagerError from MFT is private;
//        ---Trigger: If the position <aRank> is < 1 or > the command length.
//        --          or if the value is not a STRING value.
// =============================================================================

Standard_CString MFT_FontManager::SValue(MFT_FileRecord& aRecord,const Standard_Integer aRank)
{
static char svalue[5];

#ifdef S3593
static MFT_CommandValue value;

    if( Value(theCommand,aRank) != MFT_TOV_STRING ) 
	Standard_TypeMismatch::Raise("BAD command value type");
#else
MFT_CommandValue value;

    Standard_TypeMismatch_Raise_if(
		Value(theCommand,aRank) != MFT_TOV_STRING,
					"BAD command value type");
#endif

    value.i = *(Standard_Integer*)
	Locate(aRecord,theCommandPosition + 
		sizeof(MFT_CommandDescriptor) + (aRank-1) * 
					sizeof(MFT_CommandValue));
    strncpy(svalue,value.s,4);
    svalue[4] = '\0';
    return svalue;
}

// =============================================================================
//        ---Purpose: Dumps the font descriptor off the character at
//	  --	      position <aPosition> 
// =============================================================================

void MFT_FontManager::Dump(const Standard_Integer aPosition)
{
    if( this->IsDefinedChar(aPosition) ) {
      Standard_Integer i;
      MFT_TypeOfValue aValueType;
      this->SetChar(aPosition);
      do {
        theCommand = this->NextCommand(myCommandBuffer);
        cout << "   >> " << MFT::Convert(MFT_TypeOfCommand(COMMAND_TYPE(theCommand)));
	if( COMMAND_LENGTH(theCommand) > 0 ) {
	  cout << " = ";
	  for( i=1 ; i <= (int) COMMAND_LENGTH(theCommand) ; i++ ) {
	    aValueType = Value(theCommand,i);
	    cout << "[" << MFT::Convert(aValueType) << " : ";
	    switch( aValueType ) {
	      case MFT_TOV_UNKNOWN:
		cout << "??????";
	        break;
	      case MFT_TOV_INTEGER:
	        cout << IValue(myCommandBuffer,i);
	        break;
	      case MFT_TOV_FLOAT:
	        cout << FValue(myCommandBuffer,i);
	        break;
	      case MFT_TOV_STRING:
	        cout << "'" << SValue(myCommandBuffer,i) << "'";
	        break;
	    }
	    if( i < (int)COMMAND_LENGTH(theCommand) ) cout << "],";
	    else cout << "]" << endl;
	  }
	} else cout << endl;
      } while( COMMAND_TYPE(theCommand) != MFT_TOC_ENDCHAR );
    } else {
      cout << "    This character is not DEFINED" << endl;
    }
}
