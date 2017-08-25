// Created by: PLOTNIKOV Eugeny
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifdef _WIN32

/******************************************************************************/
/* File:      OSD_WNT.cxx                                                     */
/* Purpose:   Security management routines ( more convinient than WIN32       */
/*            ones ) and other convinient functions.                          */
/******************************************************************************/
/***/
#include <OSD_WNT_1.hxx>

#include <Strsafe.h>
#include <wchar.h>
#include <stdlib.h>

#include <Standard_Macro.hxx>

/***/
#ifndef OCCT_UWP
static void Init ( void );
/***/
class Init_OSD_WNT {  // provides initialization

 public:

  Init_OSD_WNT () { Init (); }

}; // end Init_OSD_WNT

static Init_OSD_WNT initOsdWnt;
#endif
/***/
static BOOL   fInit = FALSE;
static PSID*  predefinedSIDs;
static HANDLE hHeap;
/***/
static MOVE_DIR_PROC     _move_dir_proc;
static COPY_DIR_PROC     _copy_dir_proc;
static RESPONSE_DIR_PROC _response_dir_proc;
/***/
#define PREDEFINED_SIDs_COUNT           9
#define UNIVERSAL_PREDEFINED_SIDs_COUNT 5
/***/
#define SID_INTERACTIVE   0
#define SID_NETWORK       1
#define SID_LOCAL         2
#define SID_DIALUP        3
#define SID_BATCH         4
#define SID_CREATOR_OWNER 5
#define SID_ADMIN         6
#define SID_WORLD         7
#define SID_NULL          8
/***/
#ifndef OCCT_UWP
// None of the existing security APIs are supported in a UWP applications
/******************************************************************************/
/* Function : AllocSD                                                       */
/* Purpose  : Allocates and initializes security identifier                 */
/* Returns  : Pointer to allocated SID on success, NULL otherwise           */
/* Warning  : Allocated SID must be deallocated by 'FreeSD' function        */
/******************************************************************************/
/***/
PSECURITY_DESCRIPTOR AllocSD ( void ) {

 PSECURITY_DESCRIPTOR retVal =
  ( PSECURITY_DESCRIPTOR )HeapAlloc (
                           hHeap, 0, sizeof ( SECURITY_DESCRIPTOR )
                          );

 if ( retVal != NULL &&
      !InitializeSecurityDescriptor ( retVal, SECURITY_DESCRIPTOR_REVISION )
 ) {
 
  HeapFree (  hHeap, 0, ( PVOID )retVal  );
  retVal = NULL;
 
 }  /* end if */

 return retVal; 

}  /* end AllocSD */
/***/
/******************************************************************************/
/* Function : FreeSD                                                        */
/* Purpose  : Deallocates security identifier which was allocated by the    */
/*            'AllocSD' function                                            */
/******************************************************************************/
/***/
void FreeSD ( PSECURITY_DESCRIPTOR pSD ) {

 BOOL   fPresent;
 BOOL   fDaclDefaulted;
 PACL   pACL;
 
 if (  GetSecurityDescriptorDacl ( pSD, &fPresent, &pACL, &fDaclDefaulted ) &&
       fPresent
 )

  HeapFree (  hHeap, 0, ( PVOID )pACL  );

 HeapFree (  hHeap, 0, ( PVOID )pSD  );

}  /* end FreeSD */
/***/
/******************************************************************************/
/* Function : GetTokenInformationEx                                         */
/* Purpose  : Allocates and fills out access token                          */
/* Returns  : Pointer to the access token on success, NULL otherwise        */
/* Warning  : Allocated access token  must be deallocated by                */
/*            'FreeTokenInformation' function                               */
/******************************************************************************/
/***/

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return buffer
#endif

LPVOID GetTokenInformationEx ( HANDLE hToken, TOKEN_INFORMATION_CLASS tic ) {

 DWORD  errVal;
 DWORD  dwSize;
 DWORD  dwSizeNeeded = 0;
 LPVOID buffer       = NULL;
 BOOL   fOK          = FALSE;

 __try {

  do {

   dwSize = dwSizeNeeded;
   errVal = ERROR_SUCCESS;
 
   if (  !GetTokenInformation ( hToken, tic, buffer, dwSize, &dwSizeNeeded )  ) {

    if (   (  errVal = GetLastError ()  ) != ERROR_INSUFFICIENT_BUFFER   )
        
     __leave;

    if (  ( buffer = HeapAlloc (  hHeap, 0, dwSizeNeeded  ) ) == NULL  )

     __leave;

   }  /* end if */
 
  } while ( errVal != ERROR_SUCCESS );

  fOK = TRUE;

 }  /* end __try */

 __finally {
 
  if ( !fOK && buffer != NULL ) {
  
   HeapFree ( hHeap, 0, buffer );
   buffer = NULL;
  
  }  /* end if */
 
 }  /* end __finally */

#ifdef VAC
leave: ;     // added for VisualAge
#endif

 return buffer;

}  /* end GetTokenInformationEx */

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

/***/
/******************************************************************************/
/* Function : FreeTokenInformation                                          */
/* Purpose  : Deallocates access token which was allocated by the           */
/*            'GetTokenInformationEx' function                              */
/******************************************************************************/
/***/
void FreeTokenInformation ( LPVOID lpvTkInfo ) {

 HeapFree (  hHeap, 0, lpvTkInfo  );

}  /* end FreeTokenInformation */
/***/
/******************************************************************************/
/* Function : Init                                                          */
/* Purpose  : Allocates and initializes predefined security identifiers     */
/* Warning  : Generates 'STATUS_NO_MEMORY' software exception if there are  */
/*            insufficient of memory. This exception can be caught by using */
/*            software exception handling ( SEH ) mechanism                 */
/*            ( __try / __except )                                          */
/******************************************************************************/
/***/
static void Init ( void ) {

 SID_IDENTIFIER_AUTHORITY sidIDAnull    = SECURITY_NULL_SID_AUTHORITY;
 SID_IDENTIFIER_AUTHORITY sidIDAworld   = SECURITY_WORLD_SID_AUTHORITY;
 SID_IDENTIFIER_AUTHORITY sidIDANT      = SECURITY_NT_AUTHORITY;
 SID_IDENTIFIER_AUTHORITY sidIDAlocal   = SECURITY_LOCAL_SID_AUTHORITY;
 SID_IDENTIFIER_AUTHORITY sidIDAcreator = SECURITY_CREATOR_SID_AUTHORITY;

 if ( !fInit ) {

  predefinedSIDs = ( PSID* )HeapAlloc (
                             hHeap = GetProcessHeap (),
                             HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
                             PREDEFINED_SIDs_COUNT * sizeof ( PSID* )
                            );

  AllocateAndInitializeSid (
   &sidIDANT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
   0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_ADMIN ]
  );

  AllocateAndInitializeSid (
   &sidIDAworld, 1, SECURITY_WORLD_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_WORLD ]
  );

  AllocateAndInitializeSid (
   &sidIDANT, 1, SECURITY_INTERACTIVE_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_INTERACTIVE ]
  );

  AllocateAndInitializeSid (
   &sidIDANT, 1, SECURITY_NETWORK_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_NETWORK ]
  );

  AllocateAndInitializeSid (
   &sidIDAlocal, 1, SECURITY_LOCAL_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_LOCAL ]
  );

  AllocateAndInitializeSid (
   &sidIDANT, 1, SECURITY_DIALUP_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_DIALUP ]
  );

  AllocateAndInitializeSid (
   &sidIDANT, 1, SECURITY_BATCH_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_BATCH ]
  );

  AllocateAndInitializeSid (
   &sidIDAcreator, 1, SECURITY_CREATOR_OWNER_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_CREATOR_OWNER ]
  );

  AllocateAndInitializeSid (
   &sidIDAnull, 1, SECURITY_NULL_RID,
   0, 0, 0, 0, 0, 0, 0, &predefinedSIDs[ SID_NULL ]
  );

  fInit = TRUE;

 }  /* end if */

}  /* end init */
/***/
/******************************************************************************/
/* Function : PredefinedSid                                                 */
/* Purpose  : Checks whether specified SID predefined or not                */
/* Returns  : TRUE if specified SID is predefined, FALSE otherwise          */
/******************************************************************************/
/***/
BOOL PredefinedSid ( PSID pSID ) {

 int i;

 for ( i = 0; i < PREDEFINED_SIDs_COUNT; ++i )
 
  if (  EqualSid ( pSID, predefinedSIDs[ i ] )  )

   return TRUE;
 
 return FALSE;

}  /* end PredefinedSid */
/***/
/******************************************************************************/
/* Function : NtPredefinedSid                                               */
/* Purpose  : Checks whether specified SID universal or not                 */
/* Returns  : TRUE if specified SID is NOT universal, FALSE otherwise       */
/******************************************************************************/
/***/
BOOL NtPredefinedSid ( PSID pSID ) {

 int                       i;
 PSID_IDENTIFIER_AUTHORITY pTestIDA;
 SID_IDENTIFIER_AUTHORITY  ntIDA = SECURITY_NT_AUTHORITY;
 PDWORD                    pdwTestSA;

 for ( i = 0; i < UNIVERSAL_PREDEFINED_SIDs_COUNT; ++i )

  if (  EqualSid ( pSID, predefinedSIDs[ i ] )  )

   return TRUE;

 pTestIDA = GetSidIdentifierAuthority ( pSID );

 if (   memcmp (  pTestIDA, &ntIDA, sizeof ( SID_IDENTIFIER_AUTHORITY )  ) == 0   ) {
 
  pdwTestSA = GetSidSubAuthority ( pSID, 0 );

  if ( *pdwTestSA == SECURITY_LOGON_IDS_RID )

   return TRUE;
     
 }  /* end if */

 return FALSE;

}  /* end NtPredefinedSid */
/***/
/******************************************************************************/
/* Function : AdminSid                                                      */
/* Purpose  : Returns SID of the administrative user account                */
/******************************************************************************/
/***/
PSID AdminSid ( void ) {

 return predefinedSIDs[ SID_ADMIN ];

}  /* end AdminSid */
/***/
/******************************************************************************/
/* Function : WorldSid                                                      */
/* Purpose  : Returns SID of group that includes all users                  */
/******************************************************************************/
/***/
PSID WorldSid ( void ) {

 return predefinedSIDs[ SID_WORLD ];

}  /* end WorldSid */
/***/
/******************************************************************************/
/* Function : InteractiveSid                                                */
/* Purpose  : Returns SID of group that includes all users logged on for    */
/*            interactive operation                                         */
/******************************************************************************/
/***/
PSID InteractiveSid ( void ) {

 return predefinedSIDs[ SID_INTERACTIVE ];

}  /* end InteractiveSID */
/***/
/******************************************************************************/
/* Function : NetworkSid                                                    */
/* Purpose  : Returns SID of group that includes all users logged on across */
/*            a network                                                     */
/******************************************************************************/
/***/
PSID NetworkSid ( void ) {

 return predefinedSIDs[ SID_NETWORK ];

}  /* end NetworkSid */
/***/
/******************************************************************************/
/* Function : LocalSid                                                      */
/* Purpose  : Returns SID of group that includes all users logged on locally*/
/******************************************************************************/
/***/
PSID LocalSid ( void ) {

 return predefinedSIDs[ SID_LOCAL ];

}  /* end LocalSid */
/***/
/******************************************************************************/
/* Function : DialupSid                                                     */
/* Purpose  : Returns SID of group that includes all users logged on to     */
/*            terminals using a dialup modem                                */
/******************************************************************************/
/***/
PSID DialupSid ( void ) {

 return predefinedSIDs[ SID_DIALUP ];
          
}  /* end DialupSid */
/***/
/******************************************************************************/
/* Function : BatchSid                                                      */
/* Purpose  : Returns SID of group that includes all users logged on using  */
/*            a batch queue facility                                        */
/******************************************************************************/
/***/
PSID BatchSid ( void ) {

 return predefinedSIDs[ SID_BATCH ];

}  /* end BatchSid */
/***/
/******************************************************************************/
/* Function : CreatorOwnerSid                                               */
/* Purpose  : Returns SID of 'CREATOR OWNER' special group                  */
/******************************************************************************/
/***/
PSID CreatorOwnerSid ( void ) {

 return predefinedSIDs[ SID_CREATOR_OWNER ];

}  /* end CreatorOwnerSid */
/***/
/******************************************************************************/
/* Function : NullSid                                                       */
/* Purpose  : Returns null SID                                              */
/******************************************************************************/
/***/
PSID NullSid ( void ) {

 return predefinedSIDs[ SID_NULL ];

}  /* end NullSid */
/***/
/******************************************************************************/
/* Function : GetFileSecurityEx                                             */
/* Purpose  : Allocates a security descriptor and fills it out by security  */
/*            information which belongs to the specified file               */
/* Returns  : Pointer to the allocated security descriptor on success       */
/*            NULL otherwise                                                */
/* Warning  : Allocated security descriptor must be deallocated by          */
/*            'FreeFileSecurity' function                                   */
/******************************************************************************/
/***/


#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return retVal
#endif

PSECURITY_DESCRIPTOR GetFileSecurityEx ( LPCWSTR fileName, SECURITY_INFORMATION si ) {

 DWORD                errVal;
 DWORD                dwSize;
 DWORD                dwSizeNeeded = 0;
 PSECURITY_DESCRIPTOR retVal = NULL;
 BOOL                 fOK    = FALSE;

 __try {

  do {

   dwSize = dwSizeNeeded;
   errVal = ERROR_SUCCESS;

   if (  !GetFileSecurityW (
           fileName, si,
           retVal, dwSize, &dwSizeNeeded
          )
   ) {
 
    if (   (  errVal = GetLastError ()  ) != ERROR_INSUFFICIENT_BUFFER   ) __leave;

    if (   (  retVal = ( PSECURITY_DESCRIPTOR )HeapAlloc ( hHeap, 0, dwSizeNeeded )
           ) == NULL
    ) __leave;

   }  /* end if */
 
  } while ( errVal != ERROR_SUCCESS );

  fOK = TRUE;

 }  /* end __try */

 __finally {
 
  if ( !fOK && retVal != NULL ) {
  
   HeapFree ( hHeap, 0, retVal );
   retVal = NULL;
  
  }  /* end if */
 
 }  /* end __finally */

#ifdef VAC
leave: ;        // added for VisualAge
#endif

 return retVal;

}  /* end GetFileSecurityEx */

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

/***/
/******************************************************************************/
/* Function : FreeFileSecurity                                              */
/* Purpose  : Deallocates security descriptor which was allocated by the    */
/*            'GetFileSecurityEx' function                                  */
/******************************************************************************/
/***/
void FreeFileSecurity ( PSECURITY_DESCRIPTOR pSD ) {

 HeapFree (  hHeap, 0, ( LPVOID )pSD  );

}  /* end FreeFileSecurity */


/******************************************************************************/
/* Function : CreateAcl                                                     */
/* Purpose  : Allocates and initializes access-control list                 */
/* Returns  : Pointer to the allocated and initialized ACL on success,      */
/*            NULL otherwise                                                */
/* Warning  : Allocated ACL must be deallocated by 'FreeAcl' function       */
/******************************************************************************/
/***/
PACL CreateAcl ( DWORD dwAclSize ) {

 PACL retVal;

 retVal = ( PACL )HeapAlloc ( hHeap, 0, dwAclSize );

 if ( retVal != NULL )

  InitializeAcl ( retVal, dwAclSize, ACL_REVISION );

 return retVal;

}  /* end CreateAcl */
/***/
/******************************************************************************/
/* Function : FreeAcl                                                       */
/* Purpose  : Deallocates access-control list which was allocated by the    */
/*            'CreateAcl' function                                          */
/******************************************************************************/
/***/
void FreeAcl ( PACL pACL ) {

 HeapFree (  hHeap, 0, ( PVOID )pACL  );

}  /* end FreeAcl */

/******************************************************************************/
/* Function : AllocAccessAllowedAce                                         */
/* Purpose  : Allocates and initializes access-control entry                */
/* Returns  : Pointer to the ACE on success, NULL othrwise                  */
/* Warning  : Allocated ACE must be deallocated by the 'FreeAce' function   */
/******************************************************************************/
/***/
PVOID AllocAccessAllowedAce ( DWORD dwMask, BYTE flags, PSID pSID ) {

 PFILE_ACE retVal;
 WORD      wSize;

 wSize = (WORD)( sizeof ( ACE_HEADER ) + sizeof ( DWORD ) + GetLengthSid ( pSID ) );

 retVal = ( PFILE_ACE )HeapAlloc ( hHeap, 0, wSize );

 if ( retVal != NULL ) {
 
  retVal -> header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
  retVal -> header.AceFlags = flags;
  retVal -> header.AceSize  = wSize;

  retVal -> dwMask = dwMask;

  CopySid (  GetLengthSid ( pSID ), &retVal -> pSID, pSID  );
 
 }  /* end if */

 return retVal;

}  /* end AllocAccessAllowedAce */
/***/
/******************************************************************************/
/* Function : FreeAce                                                       */
/* Purpose  : Deallocates an ACE which was allocated by the                 */
/*            'AllocAccessAllowedAce ' function                             */
/******************************************************************************/
/***/
void FreeAce ( PVOID pACE ) {

 HeapFree ( hHeap, 0, pACE );

}  /* end FreeAce */
#endif
#define WILD_CARD     L"/*.*"
#define WILD_CARD_LEN (  sizeof ( WILD_CARD )  )

/***/
/******************************************************************************/
/* Function : MoveDirectory                                                 */
/* Purpose  : Moves specified directory tree to the new location            */
/* Returns  : TRUE on success, FALSE otherwise                              */
/******************************************************************************/
/***/
static BOOL MoveDirectory (const wchar_t* oldDir, const wchar_t* newDir, DWORD& theRecurseLevel)
{
  wchar_t* driveSrc = NULL;
  wchar_t* driveDst = NULL;
  wchar_t* pathSrc = NULL;
  wchar_t* pathDst = NULL;
  BOOL     retVal = FALSE;
  if (theRecurseLevel == 0)
  {
    ++theRecurseLevel;
    BOOL fFind = FALSE;
    if ((driveSrc = (wchar_t* )HeapAlloc (hHeap, 0, _MAX_DRIVE * sizeof(wchar_t))) != NULL
     && (driveDst = (wchar_t* )HeapAlloc (hHeap, 0, _MAX_DRIVE * sizeof(wchar_t))) != NULL
     && (pathSrc  = (wchar_t* )HeapAlloc (hHeap, 0, _MAX_DIR   * sizeof(wchar_t))) != NULL
     && (pathDst  = (wchar_t* )HeapAlloc (hHeap, 0, _MAX_DIR   * sizeof(wchar_t))) != NULL)
    {
      _wsplitpath (oldDir, driveSrc, pathSrc, NULL, NULL);
      _wsplitpath (newDir, driveDst, pathDst, NULL, NULL);
      if (wcscmp (driveSrc, driveDst) == 0
       && wcscmp (pathSrc,  pathDst ) == 0)
      {
retry:
        retVal = MoveFileExW (oldDir, newDir, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
        fFind  = TRUE;
        if (!retVal)
        {
          if (_response_dir_proc != NULL)
          {
            const DIR_RESPONSE response = _response_dir_proc (oldDir);
            if (response == DIR_RETRY)
            {
              goto retry;
            }
            else if (response == DIR_IGNORE)
            {
              retVal = TRUE;
            }
          }
        }
        else if (_move_dir_proc != NULL)
        {
          _move_dir_proc (oldDir, newDir);
        }
      }
    }

    if (pathDst  != NULL)
    {
      HeapFree (hHeap, 0, pathDst);
    }
    if (pathSrc  != NULL)
    {
      HeapFree (hHeap, 0, pathSrc);
    }
    if (driveDst != NULL)
    {
      HeapFree (hHeap, 0, driveDst);
    }
    if (driveSrc != NULL)
    {
      HeapFree (hHeap, 0, driveSrc);
    }

    if (fFind)
    {
      --theRecurseLevel;
      return retVal;
    }
  }
  else
  {
    ++theRecurseLevel;
  }

  WIN32_FIND_DATAW* pFD = NULL;
  wchar_t* pName = NULL;
  wchar_t* pFullNameSrc = NULL;
  wchar_t* pFullNameDst = NULL;
  HANDLE hFindFile = INVALID_HANDLE_VALUE;
  retVal = CreateDirectoryW (newDir, NULL);
  if (retVal || (!retVal && GetLastError() == ERROR_ALREADY_EXISTS))
  {
    size_t anOldDirLength;
    StringCchLengthW (oldDir, sizeof(oldDir) / sizeof(oldDir[0]), &anOldDirLength);
    if ((pFD = (WIN32_FIND_DATAW* )HeapAlloc (hHeap, 0, sizeof(WIN32_FIND_DATAW))) != NULL
     && (pName =        (wchar_t* )HeapAlloc (hHeap, 0, anOldDirLength + WILD_CARD_LEN + sizeof(L'\x00'))) != NULL)
    {
      StringCchCopyW (pName, sizeof(pName) / sizeof(pName[0]), oldDir);
      StringCchCatW  (pName, sizeof(pName), WILD_CARD);
      retVal = TRUE;
      hFindFile = FindFirstFileExW (pName, FindExInfoStandard, pFD, FindExSearchNameMatch, NULL, 0);
      for (BOOL fFind = hFindFile != INVALID_HANDLE_VALUE; fFind; fFind = FindNextFileW (hFindFile, pFD))
      {
        if ((pFD->cFileName[0] == L'.' && pFD->cFileName[1] == L'\0')
         || (pFD->cFileName[0] == L'.' && pFD->cFileName[1] == L'.' && pFD->cFileName[2] == L'\0'))
        {
          continue;
        }

        size_t anOldDirLength2 = 0, aNewDirLength = 0, aFileNameLength = 0;
        StringCchLengthW (oldDir, sizeof(oldDir) / sizeof(oldDir[0]), &anOldDirLength2);
        StringCchLengthW (newDir, sizeof(newDir) / sizeof(newDir[0]), &aNewDirLength);
        StringCchLengthW (pFD->cFileName, sizeof(pFD->cFileName) / sizeof(pFD->cFileName[0]), &aFileNameLength);
        if ((pFullNameSrc = (wchar_t* )HeapAlloc (hHeap, 0, anOldDirLength2 + aFileNameLength + sizeof(L'/') + sizeof(L'\x00'))) == NULL
          || (pFullNameDst = (wchar_t* )HeapAlloc (hHeap, 0, aNewDirLength   + aFileNameLength + sizeof(L'/') + sizeof(L'\x00'))) == NULL)
        {
          break;
        }

        StringCchCopyW (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), oldDir);
        StringCchCatW  (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), L"/");
        StringCchCatW  (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), pFD->cFileName);

        StringCchCopyW (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), newDir);
        StringCchCatW  (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), L"/");
        StringCchCatW  (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), pFD->cFileName);

        if ((pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
          retVal = MoveDirectory (pFullNameSrc, pFullNameDst, theRecurseLevel);
          if (!retVal)
          {
            break;
          }
        }
        else
        {
retry_1:
          retVal = MoveFileExW (pFullNameSrc, pFullNameDst, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
          if (!retVal)
          {
            if (_response_dir_proc != NULL)
            {
              const DIR_RESPONSE response = _response_dir_proc (pFullNameSrc);
              if (response == DIR_ABORT)
              {
                break;
              }
              else if (response == DIR_RETRY)
              {
                goto retry_1;
              }
              else if (response == DIR_IGNORE)
              {
                retVal = TRUE;
              }
              else
              {
                break;
              }
            }
          }
          else if (_move_dir_proc != NULL)
          {
            _move_dir_proc (pFullNameSrc, pFullNameDst);
          }
        }

        HeapFree (hHeap, 0, pFullNameDst);
        HeapFree (hHeap, 0, pFullNameSrc);
        pFullNameSrc = pFullNameDst = NULL;
      }
    }
  }

  if (hFindFile != INVALID_HANDLE_VALUE)
  {
    FindClose (hFindFile);
  }

  if (pFullNameSrc != NULL)
  {
    HeapFree (hHeap, 0, pFullNameSrc);
  }
  if (pFullNameDst != NULL)
  {
    HeapFree (hHeap, 0, pFullNameDst);
  }
  if (pName != NULL)
  {
    HeapFree (hHeap, 0, pName);
  }
  if (pFD != NULL)
  {
    HeapFree (hHeap, 0, pFD);
  }

  if (retVal)
  {
retry_2:
    retVal = RemoveDirectoryW (oldDir);
    if (!retVal)
    {
      if (_response_dir_proc != NULL)
      {
        const DIR_RESPONSE response = _response_dir_proc (oldDir);
        if (response == DIR_RETRY)
        {
          goto retry_2;
        }
        else if (response == DIR_IGNORE)
        {
          retVal = TRUE;
        }
      }
    }
  }

  --theRecurseLevel;
  return retVal;
}

BOOL MoveDirectory (const wchar_t* oldDir, const wchar_t* newDir)
{
  DWORD aRecurseLevel = 0;
  return MoveDirectory (oldDir, newDir, aRecurseLevel);
}

/***/
/******************************************************************************/
/* Function : CopyDirectory                                                 */
/* Purpose  : Copies specified directory tree to the new location           */
/* Returns  : TRUE on success, FALSE otherwise                              */
/******************************************************************************/
/***/
BOOL CopyDirectory (const wchar_t* dirSrc, const wchar_t* dirDst)
{
  WIN32_FIND_DATAW* pFD = NULL;
  wchar_t* pName = NULL;
  wchar_t* pFullNameSrc = NULL;
  wchar_t* pFullNameDst = NULL;
  HANDLE   hFindFile = INVALID_HANDLE_VALUE;

  BOOL retVal = CreateDirectoryW (dirDst, NULL);
  if (retVal || (!retVal && GetLastError() == ERROR_ALREADY_EXISTS))
  {
    size_t aDirSrcLength = 0;
    StringCchLengthW (dirSrc, sizeof(dirSrc) / sizeof(dirSrc[0]), &aDirSrcLength);
    if ((pFD = (WIN32_FIND_DATAW* )HeapAlloc (hHeap, 0, sizeof(WIN32_FIND_DATAW))) != NULL
     && (pName = (wchar_t* )HeapAlloc (hHeap, 0, aDirSrcLength + WILD_CARD_LEN + sizeof(L'\x00'))) != NULL)
    {
      StringCchCopyW(pName, sizeof(pName) / sizeof(pName[0]), dirSrc);
      StringCchCatW (pName, sizeof(pName) / sizeof(pName[0]), WILD_CARD);

      retVal = TRUE;
      hFindFile = FindFirstFileExW (pName, FindExInfoStandard, pFD, FindExSearchNameMatch, NULL, 0);
      for (BOOL fFind = hFindFile != INVALID_HANDLE_VALUE; fFind; fFind = FindNextFileW (hFindFile, pFD))
      {
        if ((pFD->cFileName[0] == L'.' && pFD->cFileName[1] == L'\0')
         || (pFD->cFileName[0] == L'.' && pFD->cFileName[1] == L'.' && pFD->cFileName[2] == L'\0'))
        {
          continue;
        }

        size_t aDirSrcLength2 = 0, aDirDstLength = 0, aFileNameLength = 0;
        StringCchLengthW (dirSrc, sizeof(dirSrc) / sizeof(dirSrc[0]), &aDirSrcLength2);
        StringCchLengthW (dirDst, sizeof(dirDst) / sizeof(dirDst[0]), &aDirDstLength);
        StringCchLengthW (pFD->cFileName, sizeof(pFD->cFileName) / sizeof(pFD->cFileName[0]), &aFileNameLength);
        if ((pFullNameSrc = (wchar_t* )HeapAlloc (hHeap, 0, aDirSrcLength2 + aFileNameLength + sizeof(L'/') + sizeof(L'\x00'))) == NULL
         || (pFullNameDst = (wchar_t* )HeapAlloc (hHeap, 0, aDirDstLength  + aFileNameLength + sizeof(L'/') + sizeof(L'\x00'))) == NULL)
        {
          break;
        }

        StringCchCopyW (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), dirSrc);
        StringCchCatW  (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), L"/");
        StringCchCatW  (pFullNameSrc, sizeof(pFullNameSrc) / sizeof(pFullNameSrc[0]), pFD->cFileName);

        StringCchCopyW (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), dirDst);
        StringCchCatW  (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), L"/");
        StringCchCatW  (pFullNameDst, sizeof(pFullNameDst) / sizeof(pFullNameDst[0]), pFD->cFileName);
        if ((pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
          retVal = CopyDirectory (pFullNameSrc, pFullNameDst);
          if (!retVal)
          {
            break;
          }
        }
        else
        {
retry:
        #ifndef OCCT_UWP
          retVal = CopyFileW (pFullNameSrc, pFullNameDst, FALSE);
        #else
          retVal = (CopyFile2 (pFullNameSrc, pFullNameDst, FALSE) == S_OK) ? TRUE : FALSE;
        #endif
          if (!retVal)
          {
            if (_response_dir_proc != NULL)
            {
              const DIR_RESPONSE response = _response_dir_proc (pFullNameSrc);
              if (response == DIR_ABORT)
              {
                break;
              }
              else if (response == DIR_RETRY)
              {
                goto retry;
              }
              else if (response == DIR_IGNORE)
              {
                retVal = TRUE;
              }
              else
              {
                break;
              }
            }
          }
          else if (_copy_dir_proc != NULL)
          {
            _copy_dir_proc (pFullNameSrc, pFullNameDst);
          }
        }

        HeapFree (hHeap, 0, pFullNameDst);
        HeapFree (hHeap, 0, pFullNameSrc);
        pFullNameSrc = pFullNameDst = NULL;
      }
    }
  }

  if (hFindFile != INVALID_HANDLE_VALUE)
  {
    FindClose (hFindFile);
  }

  if (pFullNameSrc != NULL)
  {
    HeapFree (hHeap, 0, pFullNameSrc);
  }
  if (pFullNameDst != NULL)
  {
    HeapFree (hHeap, 0, pFullNameDst);
  }
  if (pName != NULL)
  {
    HeapFree (hHeap, 0, pName);
  }
  if (pFD != NULL)
  {
    HeapFree (hHeap, 0, pFD);
  }

  return retVal;
}  /* end CopyDirectory */
/***/
/******************************************************************************/
/* Function : SetMoveDirectoryProc                                          */
/* Purpose  : Sets callback procedure which is calling by the               */ 
/*            'MoveDirectory' after moving of each item in the              */
/*            directory. To unregister this callback function supply NULL   */
/*            pointer                                                       */
/******************************************************************************/
/***/
void SetMoveDirectoryProc ( MOVE_DIR_PROC proc ) {

 _move_dir_proc = proc;

}  /* end SetMoveDirectoryProc */
/***/
/******************************************************************************/
/* Function : SetCopyDirectoryProc                                          */
/* Purpose  : Sets callback procedure which is calling by the               */ 
/*            'CopyDirectory' after copying of each item in the             */
/*            directory. To unregister this callback function supply NULL   */
/*            pointer                                                       */
/******************************************************************************/
/***/
void SetCopyDirectoryProc ( COPY_DIR_PROC proc ) {

 _copy_dir_proc = proc;

}  /* end SetCopyDirectoryProc */
/***/
/******************************************************************************/
/* Function : SetResponseDirectoryProc                                      */
/* Purpose  : Sets callback procedure which is calling by the               */ 
/*            directoy processing function if an error was occur.           */
/*            The return value of that callback procedure determines        */
/*            behaviour of directoy processing functions in case of error.  */
/*            To unregister this callback function supply NULL pointer      */
/******************************************************************************/
/***/
void SetResponseDirectoryProc ( RESPONSE_DIR_PROC proc ) {

 _response_dir_proc = proc;

}  /* end SetResponseDirectoryProc */
/***/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#endif
