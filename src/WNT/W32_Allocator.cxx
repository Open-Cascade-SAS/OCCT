// File:        W32_Allocator.cxx
// Created:     Feb 1998
// Author:      PLOTNIKOV Eugeny & CHABROVSKY Dmitry
// Copyright:   Matra Datavision 1998

#include <W32_Allocator.hxx>
#include <W95_Allocator.hxx>
#include <WNT_Allocator.hxx>

#include <windowsx.h>

#pragma comment( lib, "gdi32.lib"  )
#pragma comment( lib, "user32.lib" )

////////////////////////////////////////////////////////////////////////////////
//                       I N I T I A L I Z A T I O N                          //
////////////////////////////////////////////////////////////////////////////////
#define DEF_BLOCK_SIZE ( 2 * s_dwPageSize )

static DWORD s_dwPageSize;

PW32_Allocator ( *W32_GetAllocator ) ( int, PW32_Allocator );
double W32_TextFactor;

static PW32_Allocator W95_GetAllocator ( int anID, PW32_Allocator head ) {

 return new W95_Allocator ( anID, head );

}  // end W95_GetAllocator

static PW32_Allocator WNT_GetAllocator ( int anID, PW32_Allocator head ) {

 return new WNT_Allocator ( anID, head );

}  // end WNT_GetAllocator

class _initAllocator {

 public:

  _initAllocator ();

};

_initAllocator :: _initAllocator () {

 OSVERSIONINFO os;
 SYSTEM_INFO   si;

 GetSystemInfo ( &si );

 s_dwPageSize = si.dwPageSize;

 os.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );
 GetVersionEx ( &os );

 if ( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
doWin95:
  W32_GetAllocator = &W95_GetAllocator;
  W32_TextFactor = 5.0;

 } else {

  GetEnvironmentVariable (  TEXT( "CSF_WNT_FORCE_WIN95" ), NULL, 0  );

  if (  GetLastError () != ERROR_ENVVAR_NOT_FOUND  ) goto doWin95;

  W32_GetAllocator = &WNT_GetAllocator;
  W32_TextFactor = 1.0;

 }  // end else

}  // end constructor

static _initAllocator s_InitAllocator;
////////////////////////////////////////////////////////////////////////////////
//                 W 3 2 _ A L L O C A T O R  S T U F F                       //
////////////////////////////////////////////////////////////////////////////////
W32_Allocator :: W32_Allocator ( int anID, PW32_Allocator head ) :
                  myID ( anID ), myFlags ( W32F_EMPTY ), myEnd ( NULL ),
                  myNext ( NULL ) {

 LOGFONT lf;
 HDC     hdc = GetDC ( NULL );

  GetObject (  hdc, sizeof ( LOGFONT ), &lf  );

 ReleaseDC ( NULL, hdc );

 lf.lfOutPrecision |= OUT_TT_ONLY_PRECIS;

 if ( head != NULL ) {

  while ( head -> myNext != NULL ) head = head -> myNext;

  head -> myNext = this;

 }  // end if

 myStart = MakeBlock ( DEF_BLOCK_SIZE );

 myTextFont   = CreateFontIndirect ( &lf );
 myTextSlant  = 0.0;
 myTextHScale = 
 myTextVScale = 1.0 / W32_TextFactor;

 myScaleX  = myScaleY = 1.0;
 myAngle   = 0.0;
 myPivot.x = myMove.x =
 myPivot.y = myMove.y = 0;

 myFlags     = ( W32F_EMPTY | W32F_POUTL | W32F_DFONT );
 myPrimitive = zzNone;

 myPointColor       =
 myMarkerPointColor = RGB( 255, 255, 255 );

}  // end constructor

W32_Allocator :: ~W32_Allocator () {

 KillBlocks ( myStart );

 if ( myFlags & W32F_DFONT ) DeleteFont( myTextFont );

}  // end destructor

PW32_Block W32_Allocator :: MakeBlock ( int aBlockSize ) {

 PW32_Block retVal;

 aBlockSize = ( s_dwPageSize / aBlockSize + 1 ) * s_dwPageSize;

 retVal = ( PW32_Block )VirtualAlloc (
                         NULL, aBlockSize, MEM_RESERVE | MEM_COMMIT,
                         PAGE_READWRITE
                        );

 if ( retVal == NULL ) RaiseException ( STATUS_NO_MEMORY, 0, 0, NULL );

 if ( myEnd != NULL ) myEnd -> next = retVal;
  
 myEnd = retVal;

 retVal -> next = NULL;
 retVal -> size = aBlockSize / sizeof ( int ) - sizeof ( W32_Block ) / sizeof ( int );
 retVal -> free = 0;
  
 return retVal;

}  // end  W32_Allocator :: MakeBlock

PW32_Block W32_Allocator :: KillBlock ( PW32_Block aVictim ) {
  
 PW32_Block next = aVictim -> next;

 ReleaseBlock ( aVictim );

 VirtualFree (  ( LPVOID )aVictim, 0, MEM_RELEASE );
 
 return next;

}  // end W32_Allocator :: KillBlock

void W32_Allocator :: KillBlocks ( PW32_Block aBegin ) {
 
 while (   (  aBegin = KillBlock ( aBegin )  ) != NULL   );
 
}  // end W32_Allocator :: KillBlocks

void W32_Allocator :: ClearBlocks ( void ) {

 if ( myStart -> next != NULL ) KillBlocks ( myStart -> next );

 ReleaseBlock ( myStart );

 myStart -> next = NULL;
 myStart -> free = 0;
 myEnd           = myStart;

 myFlags &= ~W32F_DRAWN;
 myFlags |=  W32F_EMPTY;

}  // end W32_Allocator :: ClearBlocks

void W32_Allocator :: ReleaseBlock ( PW32_Block pb ) {

 for ( int i = 0; i < pb -> free; i += pb -> data[ i ] )
	    
  if ( pb -> data[ i + 1 ] != __W32_DATA ) {                                         

   W32_Note* pNote = ( W32_Note* )&( pb -> data[ i + 1 ] );
   pNote -> ~W32_Note ();

  }  // end if

}  // end W32_Allocator :: ReleaseBlock

BOOL W32_Allocator :: FreeSpace ( PW32_Block aBlock, int aQuerySize ) {

 return (  aBlock -> size >= ( aBlock -> free + aQuerySize + 1 )  );

}  // end W32_Allocator :: FreeSpace

PW32_Block W32_Allocator :: ReserveData ( unsigned int iSize ) {

 if (  !FreeSpace ( myEnd, iSize )  ) return NULL;

 return myEnd;

}  // end W32_Allocator :: ReserveData

PW32_Block W32_Allocator :: ReserveFind ( unsigned int iSize ) {

  PW32_Block aBlock = myStart;
  for ( ; aBlock != NULL; aBlock = aBlock -> next )

    if (  FreeSpace ( aBlock, iSize )  ) break;

 return aBlock;

}  // end W32_Allocator :: ReserveFind

void* W32_Allocator :: ReserveBlock ( PW32_Block aBlock, int aQuerySize, BOOL fData ) {

 void* retVal;

 ++aQuerySize;

 retVal = fData ? &( aBlock -> data[ aBlock -> free + 2 ] ) :
                  &( aBlock -> data[ aBlock -> free + 1 ] );

 aBlock -> data[ aBlock -> free     ] = aQuerySize;
 aBlock -> data[ aBlock -> free + 1 ] = __W32_DATA;
 aBlock -> free += aQuerySize;
 
 return retVal; 

}  // end W32_Allocator :: ReserveBlock

void* W32_Allocator :: NewClass ( unsigned int nBytes ) {

 PW32_Block aBlock = myEnd;

 nBytes = (  ( nBytes + 3  ) / sizeof ( int )  );

 if (  !FreeSpace ( aBlock, nBytes )  ) aBlock = MakeBlock ( nBytes );

 myFlags &= ~W32F_EMPTY;

 return ReserveBlock ( aBlock, nBytes );

}  // end W32_Allocator :: NewClass

void* W32_Allocator :: NewData ( unsigned int nBytes, BOOL fFind ) {
  
 PW32_Block aBlock;

 nBytes = (  ( nBytes + 3 ) / sizeof ( int )  ) + 1;

 aBlock = fFind ? ReserveFind ( nBytes ) : ReserveData ( nBytes );

 if ( aBlock == NULL ) aBlock = MakeBlock ( nBytes );

 return ReserveBlock ( myEnd, nBytes, TRUE ); 

}  // end W32_Allocator :: NewData

void* W32_Note :: operator new ( size_t cSize, PW32_Allocator anAllocator ) {

 W32_Note* note = ( W32_Note* )anAllocator -> NewClass ( cSize );

 note -> myAllocator = anAllocator;

 return ( void* )note;

}  // end W32_Note :: operator new
