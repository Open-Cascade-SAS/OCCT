#ifdef WNT
#include <OSD_WNT_1.hxx>

#include <windowsx.h>

static FILETIME ftKernelTimeStart, ftUserTimeStart, ftElapsedStart;
static TCHAR    timeBuffer[ 80 ];

static __int64 __fastcall FileTimeToQuadWord ( PFILETIME );
static void    __fastcall QuadWordToFileTime ( __int64, PFILETIME );

BOOL OSDAPI DirWalk (
             LPCTSTR dirName, LPCTSTR wildCard, BOOL ( *func ) ( LPCTSTR, BOOL, void* ),
             BOOL fRecurse,   void* lpClientData
            ) {

 int              len;
 PWIN32_FIND_DATA pFD;
 LPTSTR           pName = NULL;
 LPTSTR           pFullName = NULL;
 LPTSTR           pTmp;
 HANDLE           hFindFile = INVALID_HANDLE_VALUE;
 BOOL             fFind;
 BOOL             retVal = TRUE;
 HANDLE           hHeap = GetProcessHeap ();

 if (   (  pFD = ( PWIN32_FIND_DATA )HeapAlloc (
                                      hHeap, 0, sizeof ( WIN32_FIND_DATA )
                                     )
        ) != NULL &&
        (  pName = ( LPTSTR )HeapAlloc (
                              hHeap, 0, lstrlen ( dirName ) +
                              lstrlen ( wildCard ) + sizeof (  TEXT( '\x00' )  )
                             )
        ) != NULL
 ) {

  lstrcpy (  pName, dirName  );
  lstrcat (  pName, wildCard );

  fFind  = (  hFindFile = FindFirstFile ( pName, pFD )  ) != INVALID_HANDLE_VALUE;

  while ( fFind ) {

   if (  pFD -> cFileName[ 0 ] != TEXT( '.' ) ||
         pFD -> cFileName[ 0 ] != TEXT( '.' ) &&
         pFD -> cFileName[ 1 ] != TEXT( '.' )
   ) {

    if (   ( pFullName = ( LPTSTR )HeapAlloc (
                                    hHeap, 0,
                                    lstrlen ( dirName ) + lstrlen ( pFD -> cFileName ) +
                                    sizeof (  TEXT( '/' )  )                           +
                                    sizeof (  TEXT( '\x00' )  )
                                   )
           ) == NULL
    ) {
    
     retVal = FALSE;
     break;
    
    }  /* end if */

    lstrcpy (  pFullName, dirName          );
    lstrcat (  pFullName, pFD -> cFileName );
   
    if ( pFD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && fRecurse ) {

     lstrcat (  pFullName, TEXT( "/" )  );

     if (  !DirWalk ( pFullName,  wildCard, func, fRecurse, lpClientData )  ) retVal = FALSE;
   
    } else if (  !( *func ) (
                   pFullName, pFD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? TRUE :
                                                                                   FALSE,
                   lpClientData 
                  )
           ) retVal = FALSE;

    HeapFree ( hHeap, 0, pFullName );
    pFullName = NULL;

   }  /* end if */

   fFind = FindNextFile ( hFindFile, pFD );
  
  }  /* end while */

 } else
 
  retVal = FALSE;

 len = 1;

 if ( hFindFile != INVALID_HANDLE_VALUE ) FindClose ( hFindFile );

 if ( fRecurse ) {

  if (  dirName[ len = lstrlen ( dirName ) - 1 ] == TEXT( '/' )  ) {
 
   if (   (  pTmp = ( LPTSTR )HeapAlloc ( hHeap, 0, len + 2 )  ) != NULL   ) {
  
    lstrcpy ( pTmp, dirName );
    pTmp[ len ] = 0;
    fFind = TRUE;
  
   } else {

    retVal = FALSE;
    len    = 0;

   }  /* end else */
 
  }  /* end if */

  if ( len ) {

   retVal = ( *func ) ( fFind ? pTmp : dirName, TRUE, lpClientData );

   if ( fFind ) HeapFree ( hHeap, 0, pTmp );

  }  /* end if */

 }  /* end if */

 if ( pFullName != NULL ) HeapFree ( hHeap, 0, pFullName );
 if ( pName     != NULL ) HeapFree ( hHeap, 0, pName     );
 if ( pFD       != NULL ) HeapFree ( hHeap, 0, pFD       );

 return retVal;
            
}  // end DirWalk

void WNT_InitTimer ( void ) {

 FILETIME   ftDummy;
 SYSTEMTIME	stStart;

 GetProcessTimes (
  GetCurrentProcess (), &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart
 );

 GetSystemTime ( &stStart );
 SystemTimeToFileTime ( &stStart, &ftElapsedStart );

}  // end WNT_InitTimer

LPCTSTR WNT_StatTimer ( void ) {

 __int64    qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;
 FILETIME   ftKernelTimeEnd, ftUserTimeEnd, ftTotalTimeEnd, ftDummy;
 SYSTEMTIME	stKernel, stUser, stTotal;

 GetProcessTimes (
  GetCurrentProcess (), &ftDummy, &ftDummy, &ftKernelTimeEnd, &ftUserTimeEnd
 );

 GetSystemTime ( &stTotal );
 SystemTimeToFileTime ( &stTotal, &ftTotalTimeEnd );

 qwKernelTimeElapsed = FileTimeToQuadWord ( &ftKernelTimeEnd ) -
                       FileTimeToQuadWord ( &ftKernelTimeStart );

 qwUserTimeElapsed   = FileTimeToQuadWord ( &ftUserTimeEnd ) -
                       FileTimeToQuadWord ( &ftUserTimeStart ); 

 qwTotalTimeElapsed  = FileTimeToQuadWord ( &ftTotalTimeEnd ) -
					   FileTimeToQuadWord ( &ftElapsedStart );

 QuadWordToFileTime ( qwKernelTimeElapsed, &ftKernelTimeEnd );
 QuadWordToFileTime ( qwUserTimeElapsed,   &ftUserTimeEnd   );
 QuadWordToFileTime ( qwTotalTimeElapsed,  &ftTotalTimeEnd  );

 FileTimeToSystemTime ( &ftKernelTimeEnd, &stKernel );
 FileTimeToSystemTime ( &ftUserTimeEnd,   &stUser   );
 FileTimeToSystemTime ( &ftTotalTimeEnd,  &stTotal  );

 wsprintf (
  timeBuffer,
  TEXT( "Kernel=%02d:%02d:%02d:%03d User=%02d:%02d:%02d:%03d Elapsed=%02d:%02d:%02d:%03d" ),
  stKernel.wHour, stKernel.wMinute, stKernel.wSecond, stKernel.wMilliseconds,
  stUser.wHour,   stUser.wMinute,   stUser.wSecond,   stUser.wMilliseconds,
  stTotal.wHour,  stTotal.wMinute,  stTotal.wSecond,  stTotal.wMilliseconds
 );

 return timeBuffer;

}  // end WNT_StatTimer

static __int64 __fastcall FileTimeToQuadWord ( PFILETIME pFt ) {

 __int64 qw;

 qw   = pFt -> dwHighDateTime;
 qw <<= 32;
 qw  |=	pFt -> dwLowDateTime;

 return qw;

}  // end FileTimeToQuadWord

static void __fastcall QuadWordToFileTime ( __int64 qw, PFILETIME pFt ) {

 pFt -> dwHighDateTime = ( DWORD )( qw >> 32 );
 pFt -> dwLowDateTime  = ( DWORD ) ( qw & 0xFFFFFFFF );

}  // end QuadWordToFileTime

#if defined (_UNICODE)
#define CHR_SIZE sizeof(WORD)/2
#else
#define CHR_SIZE sizeof(WORD)
#endif
////////////////////////////////////////////////////////////////
LPWORD lpwAlign (LPWORD lpIn)
{
  ULONG ul;

  ul = (ULONG) lpIn;
  ul +=3;
  ul >>=2;
  ul <<=2;
  return (LPWORD) ul;
}

int CopyAnsiToWideChar (LPWORD lpWCStr, LPSTR lpAnsiIn)
{
  int nChar = 0;

  do {
    *lpWCStr++ = (WORD) *lpAnsiIn;
    nChar++;
  } while (*lpAnsiIn++);

  return nChar;
}

LPWORD SetClassParams (LPWORD p, DWORD style, short x, short y,
                       short cx, short cy, WORD id, WORD classId)
{
  LPWORD pp = p;
  *pp++ = LOWORD (style);
  *pp++ = HIWORD (style);
  *pp++ = 0;          // LOWORD (lExtendedStyle)
  *pp++ = 0;          // HIWORD (lExtendedStyle)
  *pp++ = x;          // x
  *pp++ = y;          // y
  *pp++ = cx;         // cx
  *pp++ = cy;         // cy
  *pp++ = id;         // id
  *pp++ = 0xFFFF;
  *pp++ = classId;
  return pp;
}

short MapX (LONG aX) {
  static LONG baseUnits = 0;
  if ( !baseUnits ) {
    baseUnits = GetDialogBaseUnits ();
  }
  return (short)(aX*4) / LOWORD (baseUnits);
}

short MapY (LONG aY) {
  static LONG baseUnits = 0;
  if ( !baseUnits ) {
    baseUnits = GetDialogBaseUnits ();
  }
  return (short)(aY*8) / HIWORD (baseUnits);
}

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return res
#endif

LONG GetTextParams ( HDC hdc, LPCTSTR lpText ) {
  LONG res = 0;
  SIZE size;
  _TCHAR* dummy;
  __try {
    dummy = _tcsdup (lpText);
    if ( !dummy ) __leave;
    short txtW = 0, txtH = 0;
    _TCHAR* tok = _tcstok ( dummy, TEXT( "\n" ) );
    while ( tok ) {
      if ( !GetTextExtentPoint32 (hdc, tok, _tcslen(tok), (LPSIZE)&size) ) __leave;
      txtH += (short)size.cy;
      if ( txtW < size.cx )
        txtW = (short)size.cx;
      tok = _tcstok ( NULL, TEXT( "\n" ) );
    }
    res = MAKELONG (MapX(txtW), MapY(txtH));
  }

  __finally {
    free (dummy);
  }

#ifdef VAC
leave: ;       // added for VisualAge
#endif

  return res;
}

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

/////////////////ON_COMMAND/////////////////////////////////////
void MsgBox_OnCommand (HWND hwnd, _TINT id, HWND hCtl, UINT codeNotify) {
  EndDialog (hwnd, id);
}

/////////////////ON_INIT/////////////////////////////////////
BOOL MsgBox_OnInit ( HWND hwnd, HWND hwndFocus, LPARAM lParam ) {

 SetForegroundWindow ( hwnd );

 return TRUE;

}  // end MsgBox_OnInit
/////////////////DISPATCH_MESSAGES////////////////////////////////////
BOOL CALLBACK MsgBoxProc (HWND hdlg,
                          UINT uMsg,
                          WPARAM wParam,
                          LPARAM lParam)
{
  BOOL fProcessed = TRUE;
  switch (uMsg) {
    HANDLE_MSG (hdlg, WM_INITDIALOG, MsgBox_OnInit);
    HANDLE_MSG (hdlg, WM_COMMAND, MsgBox_OnCommand);

    // Default actions
    default:
      fProcessed = FALSE;
      break;
  }
  return (fProcessed);
}

/////////////////////////////////////////////////////////////////

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return -1
#endif

_TINT MsgBox ( HWND hParent,
               LPTSTR  lpText,
               LPCTSTR lpCaption,
               _TINT childCount,
               LPMB_DESC lpChildren
             )
{
  WORD            *p = NULL, *pDlgTemplate = NULL, *pX, *pY,
                  *pWidth, *pHeight, *pItemsCount;
  int             nchar;
  DWORD           lStyle;
  _TINT           scrW = 0, scrH = 0;
  UINT            bufSize = 0;
  HDC             hDisp = NULL;

  short           iconWidth = 0, iconHeight = 0;
  short           butCount  = 0, butMaxWidth = 0, butMaxHeight = 0;
  short           butSpace = 8, butOffset = 0;

  __try {
    /* Get dialog base units & map it to dialog units*/
    hDisp = CreateDC ( TEXT( "DISPLAY" ), NULL, NULL, NULL );
    if ( !hDisp ) __leave;
    scrW = GetDeviceCaps ( hDisp, HORZRES );
    scrH = GetDeviceCaps ( hDisp, VERTRES );
    scrW = MapX (scrW);
    scrH = MapY (scrH);
    LONG txtParam = GetTextParams ( hDisp, lpText );
    if ( !txtParam ) __leave;
 
    // Calculate needed size of buffer
    bufSize = 14*sizeof(WORD)  + _tcslen(lpCaption)*CHR_SIZE;
    bufSize += 14*sizeof(WORD) + _tcslen(lpText)*CHR_SIZE;

    _TINT      i;
    BOOL       isIcon    = FALSE;
    for ( i=0; i<childCount; i++) {
      if ( (lpChildren[i].itemType == MBT_ICON) && !isIcon ) {
        isIcon = TRUE;
        bufSize += 16*sizeof(WORD);
      }
      if ( lpChildren[i].itemType == MBT_BUTTON ) {
        bufSize += ( 14*sizeof(WORD)
                     +_tcslen(lpChildren[i].buttonLabel)*CHR_SIZE );
      }
    }

    pDlgTemplate = p = (PWORD) LocalAlloc (LPTR, bufSize);
    if ( !pDlgTemplate )  __leave;

    lStyle = DS_MODALFRAME | DS_SYSMODAL | WS_POPUP | WS_CAPTION;
    *p++ = LOWORD (lStyle);
    *p++ = HIWORD (lStyle);
    *p++ = 0;          // LOWORD (lExtendedStyle)
    *p++ = 0;          // HIWORD (lExtendedStyle)
    pItemsCount = p;
    *p++ = 0;          // NumberOfItems
    pX = p;
    *p++ = 0;          // x
    pY = p;
    *p++ = 0;          // y
    pWidth = p;
    *p++ = 0;          // cx
    pHeight = p;
    *p++ = 0;          // cy
    *p++ = 0;          // Menu
    *p++ = 0;          // Class
    /* copy the title of the dialog */
    nchar = CopyAnsiToWideChar (p, (LPTSTR)lpCaption);
    p += nchar;
    /* make sure the first item starts on a DWORD boundary */
    p = lpwAlign (p);

    /* Add children to dialog */
    // First build icon
    isIcon    = FALSE;
    for ( i=0; i<childCount; i++) {
      if ( (lpChildren[i].itemType == MBT_ICON) && !isIcon ) {
        isIcon = TRUE;
        p = SetClassParams ( p,
              WS_CHILD | WS_VISIBLE | SS_ICON,
              8, 8, 0, 0, 0, 0x0082
            );
        *p++ = (WORD)0xffff;
        *p++ = (WORD)lpChildren[i].itemId;
        *p++ = 0;  // advance pointer over nExtraStuff WORD
        p = lpwAlign (p);
        *pItemsCount += 1;
        // Icon sizes LoadIcon GetIconInfo ICONINFO 
        iconWidth   =  MapX (  GetSystemMetrics ( SM_CXICON )  );
        iconHeight  =  MapY (  GetSystemMetrics ( SM_CYICON )  );
      }
      if ( lpChildren[i].itemType == MBT_BUTTON ) {
        _TINT len = _tcslen (lpChildren[i].buttonLabel);
        SIZE aTxtSize;
        GetTextExtentPoint32 ( hDisp, lpChildren[i].buttonLabel,
                               len, (LPSIZE)&aTxtSize );
        if ( butMaxWidth < aTxtSize.cx )
          butMaxWidth = (short)aTxtSize.cx;
        if ( butMaxHeight < aTxtSize.cy )
          butMaxHeight = (short)aTxtSize.cy;
        butCount++;
      }
    }  // Find and add icon and calculate button sizes, etc.

    // Map button sizes
    butMaxWidth   = MapX (butMaxWidth) + 8;
    butMaxHeight  = MapY (butMaxHeight) + 6;
    if ( butMaxWidth < 28 ) butMaxWidth = 28;
    // Update MsgBox width & height
    *pWidth  = iconWidth + LOWORD(txtParam) + 20;
    *pHeight = max ( iconHeight, HIWORD(txtParam) ) + 20;
  
    // Text itself

    lStyle = WS_CHILD | WS_VISIBLE;

    if (  *lpText == TEXT( '\x03' )  ) {
    
     ++lpText;
     lStyle |= SS_CENTER;
    
    } else

     lStyle |= SS_LEFT;

    p = SetClassParams ( p, lStyle,
          16 + iconWidth, 8, *pWidth - 8, *pHeight, 0, 0x0082
        );
    nchar = CopyAnsiToWideChar (p, (LPTSTR)lpText);
    p += nchar;
    *p++ = 0;  // advance pointer over nExtraStuff WORD
    p = lpwAlign (p);
    *pItemsCount += 1;

    // Update width & height of a dialog
    _TINT nW = (butSpace+3+butMaxWidth)*butCount;
    if ( nW > *pWidth ) {
      *pWidth   = nW;
    } else {
      butOffset = (*pWidth - butCount*(butMaxWidth+butSpace) - butSpace) / 2;
    }
    *pHeight = *pHeight + butMaxHeight + 4;
    // Update position of a dialog
    *pX = ( scrW - (*pWidth) ) / 2;
    *pY = ( scrH - (*pHeight) ) / 2;

    // Childs  ============================================================ //
    short butNum = 0;
    for ( i=0; i<childCount; i++) {
      if ( lpChildren[i].itemType == MBT_BUTTON ) {
		lStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		lStyle |= (  ( i == 0 ) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON  );
        p = SetClassParams ( p,
              lStyle,
              8 + butOffset + butNum*(butMaxWidth+butSpace),
              *pHeight - butMaxHeight - 4,
              butMaxWidth, butMaxHeight, lpChildren[i].itemId, 0x0080
            );
        nchar = CopyAnsiToWideChar (p, (LPTSTR)lpChildren[i].buttonLabel);
        p += nchar;
        *p++ = 0;  // advance pointer over nExtraStuff WORD
        p = lpwAlign (p);
        butNum++;
        *pItemsCount += 1;
      }
    } // Childs
  }  // __try
  /*----------------------------------------------------------------------*/
  __finally {
    int res = -1;
    if ( pDlgTemplate )
      res = DialogBoxIndirect (
              GetModuleHandle(NULL),
              (LPDLGTEMPLATE)pDlgTemplate,
              hParent,
              (DLGPROC)MsgBoxProc
            );

    if ( hDisp ) DeleteDC ( hDisp );
    if ( pDlgTemplate ) LocalFree (LocalHandle (pDlgTemplate));
    return ( res );
  }

#ifdef VAC
leave: ;         // added for VisualAge
#endif

}

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

#endif
