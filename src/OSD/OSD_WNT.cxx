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

#include <wchar.h>
#include <stdlib.h>
/***/
static void Init ( void );
/***/
class Init_OSD_WNT {  // provides initialization

 public:

  Init_OSD_WNT () { Init (); }

}; // end Init_OSD_WNT

static Init_OSD_WNT initOsdWnt;
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
/***/
/******************************************************************************/
/* Function : LookupAccountSidEx                                            */
/* Purpose  : Looking for account corresponding to the given SID and returns*/
/*            a name of that account on success                             */
/* Returns  : TRUE if account was found in the security database            */
/*            FALSE otherwise                                               */
/* Warning  : If account was found then this function allocates memory      */
/*            needed to hold the name of that account and the name of the   */
/*            domain to which this account belongs. To free that memoty     */
/*            use 'FreeAccountNames' function                               */
/******************************************************************************/
/***/

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return retVal
#endif

BOOL LookupAccountSidEx ( PSID pSID, LPWSTR* name, LPWSTR* domain ) {

 DWORD        errVal;
 DWORD        dwSizeName   = 0;
 DWORD        dwSizeDomain = 0;
 BOOL         retVal       = FALSE;
 SID_NAME_USE eUse;

 __try {

  do {   

   errVal = ERROR_SUCCESS;

   if (  !LookupAccountSidW (
           NULL, pSID, *name, &dwSizeName, *domain, &dwSizeDomain, &eUse
          )
   ) {
   
    if (   (  errVal = GetLastError ()  ) != ERROR_INSUFFICIENT_BUFFER   ) __leave;

    if (   (  *name   = ( LPWSTR )HeapAlloc ( hHeap, 0, dwSizeName   )  ) == NULL ||
           (  *domain = ( LPWSTR )HeapAlloc ( hHeap, 0, dwSizeDomain )  ) == NULL
    ) __leave;
   
   }  /* end if */

  } while ( errVal != ERROR_SUCCESS );

  retVal = TRUE;
 
 }  /* end __try */

 __finally {

  if ( !retVal ) {
  
   if ( *name   == NULL ) HeapFree ( hHeap, 0, *name   );
   if ( *domain == NULL ) HeapFree ( hHeap, 0, *domain );
  
  }  /* end if */
 
 }  /* end __finally */

#ifdef VAC
leave: ;        // added for VisualAge
#endif

 return retVal;

}  /* end LookupAccountSidEx */

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

/***/
/******************************************************************************/
/* Function : FreeAccountNames                                              */
/* Purpose  : Deallocates memory which was allocated by the                 */
/*            'LookupAccountSidEx' function                                 */
/******************************************************************************/
/***/
void FreeAccountNames ( LPWSTR lpszName, LPWSTR lpszDomain ) {

 HeapFree (  hHeap, 0, ( PVOID )lpszDomain  );
 HeapFree (  hHeap, 0, ( PVOID )lpszName    );

}  /* end FreeAccountNames */
/***/
/******************************************************************************/
/* Function : GetSecurityDescriptorOwnerEx                                  */
/* Purpose  : Returns owner SID in the specified security descriptor.       */
/*            If specified security descriptor does not have an owner field */
/*            then returns NULL                                             */
/******************************************************************************/
/***/
PSID GetSecurityDescriptorOwnerEx ( PSECURITY_DESCRIPTOR pSD ) {

 BOOL bDummy;
 PSID retVal;

 if (  !GetSecurityDescriptorOwner ( pSD, &retVal, &bDummy )  )
  
  retVal = NULL;

 return retVal;

}  /* end GetSecurityDescriptorOwnerEx */
/***/
/******************************************************************************/
/* Function : GetSecurityDescriptorGroupEx                                  */
/* Purpose  : Returns primary group SID in the specified security           */
/*            descriptor. If specified security descriptor does not have a  */
/*            primary group field then returns NULL                         */
/******************************************************************************/
/***/
PSID GetSecurityDescriptorGroupEx ( PSECURITY_DESCRIPTOR pSD ) {

 BOOL bDummy;
 PSID retVal;

 if (  !GetSecurityDescriptorGroup ( pSD, &retVal, &bDummy )  )
  
  retVal = NULL;

 return retVal;

}  /* end GetSecurityDescriptorGroupEx */
/***/
/******************************************************************************/
/* Function : GetSecurityDescriptorDaclEx                                   */
/* Purpose  : Returns a pointer to the discretionary access-control list in */
/*            the specified security descriptor. If specified security      */
/*            descriptor does not have a discretionary access-control list  */
/*            then returns NULL                                             */
/******************************************************************************/
/***/
PACL GetSecurityDescriptorDaclEx ( PSECURITY_DESCRIPTOR pSD ) {

 PACL retVal;
 BOOL bDummy;
 BOOL fPresent;

 if (  !GetSecurityDescriptorDacl ( pSD, &fPresent, &retVal, &bDummy ) ||
       !fPresent
 )

  retVal = NULL;

 return retVal;
 
}  /* end GetSecurityDescriptorDaclEx */
/***/
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
/***/
/******************************************************************************/
/* Function : CopySidEx                                                     */
/* Purpose  : Makes a copy of the SID                                       */
/* Returns  : Pointer to the copy of the specified SID on success,          */
/*            NULL otherwise                                                */
/* Warning  : Allocated SID must be deallocated by 'FreeSidEx' function     */
/******************************************************************************/
/***/
PSID CopySidEx ( PSID pSIDsrc ) {

 DWORD dwLen;
 PSID  retVal;

 dwLen  = GetLengthSid ( pSIDsrc );
 retVal = ( PSID )HeapAlloc ( hHeap, 0, dwLen );

 if ( retVal != NULL )

  CopySid ( dwLen, retVal, pSIDsrc );

 return retVal;

}  /* end CopySidEx */
/***/
/******************************************************************************/
/* Function : FreeSidEx                                                     */
/* Purpose  : Deallocates SID which was allocated by the 'CopySidEx'        */
/*            function                                                      */
/******************************************************************************/
/***/
void FreeSidEx ( PSID pSID ) {

 HeapFree ( hHeap, 0, pSID );

}  /* end FreeSidEx */
/***/
/******************************************************************************/
/* Function : AllocGroupSid                                                 */
/* Purpose  : Allocates a structure which holds SIDs of groups which are    */
/*            not predefined. These SIDs is taking from the DACL of the     */
/*            specified security descriptor                                 */
/* Returns  : Pointer the allocated structure on success,                   */
/*            NULL otherwise                                                */
/* Warning  : Allocated structure must be deallocated by 'FreeGroupSid'     */
/*            function                                                      */
/******************************************************************************/
/***/
PGROUP_SID AllocGroupSid ( PSECURITY_DESCRIPTOR pSD ) {

 int        i;
 PGROUP_SID retVal  = NULL;
 PSID*      pSID    = NULL;
 DWORD      dwLen;
 DWORD      dwCount = 0;
 LPVOID     pACE;
 PACL       pACL;
 PSID       pSIDowner;
 PSID       pSIDadmin;
 PSID       pSIDworld;
 BOOL       fPresent;
 BOOL       fDefaulted;

 if (  GetSecurityDescriptorDacl ( pSD, &fPresent, &pACL, &fDefaulted ) &&
       fPresent                                                         &&
       GetSecurityDescriptorOwner ( pSD, &pSIDowner, &fDefaulted )      &&
       pSIDowner != NULL                                                &&
       (   retVal = ( PGROUP_SID )HeapAlloc (  hHeap, 0, sizeof ( GROUP_SID )  )   ) !=
        NULL
 ) {

  pSIDadmin = AdminSid ();
  pSIDworld = WorldSid ();
 
  for ( i = 0; i < ( int )pACL -> AceCount; ++i )
    if ( GetAce ( pACL, i, &pACE )                  &&
         !EqualSid (  pSIDadmin, GET_SID( pACE )  ) &&
         !EqualSid (  pSIDworld, GET_SID( pACE )  ) &&
         !EqualSid (  pSIDowner, GET_SID( pACE )  ) )
      ++dwCount;

  pSID    = ( PSID* )HeapAlloc (  hHeap, 0, dwCount * sizeof ( PSID )  );
  dwCount = 0;

  if ( pSID != NULL ) {
  
   for ( i = 0; i < ( int )pACL -> AceCount; ++i )
  
    if (  GetAce ( pACL, i, &pACE )                  &&
          !EqualSid (  pSIDadmin, GET_SID( pACE )  ) &&
          !EqualSid (  pSIDworld, GET_SID( pACE )  ) &&
          !EqualSid (  pSIDowner, GET_SID( pACE )  )
    ) {
   
     pSID[ dwCount ] = ( PSID )HeapAlloc (
                                hHeap, 0, dwLen = GetLengthSid (  GET_SID( pACE )  )
                               );

     if ( pSID[ dwCount ] != NULL )

      CopySid (  dwLen, pSID[ dwCount++ ], GET_SID( pACE )  );
   
    }  /* end if */

  }  /* end if */
    
  retVal -> nCount = dwCount;
  retVal -> pSID   = pSID;

 }  /* end if */

 return retVal;

}  /* end AllocGroupSid */
/***/
/******************************************************************************/
/* Function : FreeGroupSid                                                  */
/* Purpose  : Deallocates a structure which was allocated by the            */
/*            'AllocGroupSid' function                                      */
/******************************************************************************/
/***/
void FreeGroupSid ( PGROUP_SID pGSID ) {

 int i;

 for ( i = 0; i < ( int )pGSID -> nCount; ++i )

  HeapFree ( hHeap, 0, pGSID -> pSID[ i ] );

 HeapFree ( hHeap, 0, pGSID -> pSID );
 HeapFree ( hHeap, 0, pGSID         );

}  /* end FreeGroupSid */
/***/
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

#define WILD_CARD     L"/*.*"
#define WILD_CARD_LEN (  sizeof ( WILD_CARD )  )

/***/
/******************************************************************************/
/* Function : MoveDirectory                                                 */
/* Purpose  : Moves specified directory tree to the new location            */
/* Returns  : TRUE on success, FALSE otherwise                              */
/******************************************************************************/
/***/
static BOOL MoveDirectory ( LPCWSTR oldDir, LPCWSTR newDir, DWORD& theRecurseLevel ) {

 PWIN32_FIND_DATAW    pFD;
 LPWSTR               pName;
 LPWSTR               pFullNameSrc;
 LPWSTR               pFullNameDst;
 LPWSTR               driveSrc, driveDst;
 LPWSTR               pathSrc,  pathDst;
 HANDLE               hFindFile;
 BOOL                 fFind;
 BOOL                 retVal = FALSE;
 DIR_RESPONSE         response;

 if (theRecurseLevel == 0) {

  ++theRecurseLevel;

  fFind = FALSE;
  driveSrc = driveDst = pathSrc = pathDst = NULL;
 
  if (   (  driveSrc = ( LPWSTR )HeapAlloc ( hHeap, 0, _MAX_DRIVE * sizeof(WCHAR) )  ) != NULL &&
         (  driveDst = ( LPWSTR )HeapAlloc ( hHeap, 0, _MAX_DRIVE * sizeof(WCHAR) )  ) != NULL &&
         (  pathSrc  = ( LPWSTR )HeapAlloc ( hHeap, 0, _MAX_DIR * sizeof(WCHAR)   )  ) != NULL &&
         (  pathDst  = ( LPWSTR )HeapAlloc ( hHeap, 0, _MAX_DIR * sizeof(WCHAR)   )  ) != NULL
  ) {
  
   _wsplitpath ( oldDir, driveSrc, pathSrc, NULL, NULL );
   _wsplitpath ( newDir, driveDst, pathDst, NULL, NULL );

   if (  wcscmp ( driveSrc, driveDst ) == 0 &&
         wcscmp ( pathSrc,  pathDst  ) == 0
   ) {
retry:   
    retVal = MoveFileExW (
              oldDir, newDir, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED
             );
    fFind  = TRUE;

    if ( !retVal ) {
    
     if ( _response_dir_proc != NULL ) {
     
      response = ( *_response_dir_proc ) ( oldDir );

      if ( response == DIR_RETRY )

       goto retry;

      else if ( response == DIR_IGNORE )

       retVal = TRUE;

     }  /* end if */
    
    } else if ( _move_dir_proc != NULL )

     ( *_move_dir_proc ) ( oldDir, newDir );
    
   }  /* end if */
  
  }  /* end if */

  if ( pathDst  != NULL ) HeapFree ( hHeap, 0, pathDst  );
  if ( pathSrc  != NULL ) HeapFree ( hHeap, 0, pathSrc  );
  if ( driveDst != NULL ) HeapFree ( hHeap, 0, driveDst );
  if ( driveSrc != NULL ) HeapFree ( hHeap, 0, driveSrc );

  if ( fFind ) {
    
   --theRecurseLevel;
   return retVal;

  }  // end if
 
 } else {
 
  ++theRecurseLevel;
 
 }  // end else

 pFD          = NULL;
 pName        = NULL;
 pFullNameSrc = pFullNameDst = NULL;
 hFindFile    = INVALID_HANDLE_VALUE;
 retVal       = FALSE;

 retVal = CreateDirectoryW ( newDir, NULL );

 if (   retVal || (  !retVal && GetLastError () == ERROR_ALREADY_EXISTS  )   ) {

  if (   (  pFD = ( PWIN32_FIND_DATAW )HeapAlloc (
                                       hHeap, 0, sizeof ( WIN32_FIND_DATAW )
                                      )
         ) != NULL &&
         (  pName = ( LPWSTR )HeapAlloc (
                               hHeap, 0, lstrlenW ( oldDir ) + WILD_CARD_LEN +
                               sizeof (  L'\x00'  )
                              )
         ) != NULL
  ) {

   lstrcpyW ( pName, oldDir    );
   lstrcatW ( pName, WILD_CARD );

   retVal = TRUE;
   fFind  = (  hFindFile = FindFirstFileW ( pName, pFD )  ) != INVALID_HANDLE_VALUE;

   while ( fFind ) {
  
    if (  pFD -> cFileName[ 0 ] != L'.' ||
          pFD -> cFileName[ 0 ] != L'.' &&
          pFD -> cFileName[ 1 ] != L'.'
    ) {
  
     if (   ( pFullNameSrc = ( LPWSTR )HeapAlloc (
                                        hHeap, 0,
                                        lstrlenW ( oldDir ) + lstrlenW ( pFD -> cFileName ) +
                                        sizeof (  L'/'  ) + sizeof ( L'\x00'  )
                                       )
            ) == NULL ||
            ( pFullNameDst = ( LPWSTR )HeapAlloc (
                                        hHeap, 0,
                                        lstrlenW ( newDir ) + lstrlenW ( pFD -> cFileName ) +
                                        sizeof (  L'/'  ) + sizeof (  L'\x00'  )
                                       )
            ) == NULL
     ) break;
  
     lstrcpyW ( pFullNameSrc, oldDir );
     lstrcatW ( pFullNameSrc, L"/"  );
     lstrcatW ( pFullNameSrc, pFD -> cFileName );

     lstrcpyW ( pFullNameDst, newDir );
     lstrcatW ( pFullNameDst, L"/"  );
     lstrcatW ( pFullNameDst, pFD -> cFileName );

     if ( pFD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {

       retVal = MoveDirectory ( pFullNameSrc, pFullNameDst, theRecurseLevel );
       if (!retVal) break;
   
     } else {
retry_1:   
      retVal = MoveFileExW (pFullNameSrc, pFullNameDst,
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
      if (! retVal) {
      
       if ( _response_dir_proc != NULL ) {
      
        response = ( *_response_dir_proc ) ( pFullNameSrc );

        if ( response == DIR_ABORT )

         break;

        else if ( response == DIR_RETRY )

         goto retry_1;

        else if ( response == DIR_IGNORE )

         retVal = TRUE;

        else

         break;

       }  /* end if */
      
      } else if ( _move_dir_proc != NULL )

       ( *_move_dir_proc ) ( pFullNameSrc, pFullNameDst );

     }  /* end else */

     HeapFree ( hHeap, 0, pFullNameDst );
     HeapFree ( hHeap, 0, pFullNameSrc );
     pFullNameSrc = pFullNameDst = NULL;

    }  /* end if */

    fFind = FindNextFileW ( hFindFile, pFD );

   }  /* end while */

  }  /* end if */

 }  /* end if ( error creating directory ) */

 if ( hFindFile != INVALID_HANDLE_VALUE ) FindClose ( hFindFile );

 if ( pFullNameSrc != NULL ) HeapFree ( hHeap, 0, pFullNameSrc );
 if ( pFullNameDst != NULL ) HeapFree ( hHeap, 0, pFullNameDst );
 if ( pName        != NULL ) HeapFree ( hHeap, 0, pName        );
 if ( pFD          != NULL ) HeapFree ( hHeap, 0, pFD          );

 if ( retVal ) {
retry_2:  
  retVal = RemoveDirectoryW ( oldDir );

  if ( !retVal ) {
  
   if ( _response_dir_proc != NULL ) {
      
    response = ( *_response_dir_proc ) ( oldDir );

    if ( response == DIR_RETRY )

     goto retry_2;

    else if ( response == DIR_IGNORE )

     retVal = TRUE;

   }  /* end if */

  }  /* end if */
  
 }  /* end if */

 --theRecurseLevel;

 return retVal;

}  /* end MoveDirectory */

BOOL MoveDirectory (LPCWSTR oldDir, LPCWSTR newDir)
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
BOOL CopyDirectory ( LPCWSTR dirSrc, LPCWSTR dirDst ) {

 PWIN32_FIND_DATAW    pFD = NULL;
 LPWSTR               pName = NULL;
 LPWSTR               pFullNameSrc = NULL;
 LPWSTR               pFullNameDst = NULL;
 HANDLE               hFindFile = INVALID_HANDLE_VALUE;
 BOOL                 fFind;
 BOOL                 retVal = FALSE;
 DIR_RESPONSE         response;

 retVal = CreateDirectoryW ( dirDst, NULL );

 if (   retVal || (  !retVal && GetLastError () == ERROR_ALREADY_EXISTS  )   ) {

  if (   (  pFD = ( PWIN32_FIND_DATAW )HeapAlloc (
                                       hHeap, 0, sizeof ( WIN32_FIND_DATAW )
                                      )
         ) != NULL &&
         (  pName = ( LPWSTR )HeapAlloc (
                               hHeap, 0, lstrlenW ( dirSrc ) + WILD_CARD_LEN +
                               sizeof (  L'\x00'  )
                              )
         ) != NULL
  ) {

   lstrcpyW ( pName, dirSrc    );
   lstrcatW ( pName, WILD_CARD );

   retVal = TRUE;
   fFind  = (  hFindFile = FindFirstFileW ( pName, pFD )  ) != INVALID_HANDLE_VALUE;

   while ( fFind ) {
  
    if (  pFD -> cFileName[ 0 ] != L'.' ||
          pFD -> cFileName[ 0 ] != L'.' &&
          pFD -> cFileName[ 1 ] != L'.'
    ) {
  
     if (   ( pFullNameSrc = ( LPWSTR )HeapAlloc (
                                        hHeap, 0,
                                        lstrlenW ( dirSrc ) + lstrlenW ( pFD -> cFileName ) +
                                        sizeof (  L'/'  ) + sizeof (  L'\x00'  )
                                       )
            ) == NULL ||
            ( pFullNameDst = ( LPWSTR )HeapAlloc (
                                        hHeap, 0,
                                        lstrlenW ( dirDst ) + lstrlenW ( pFD -> cFileName ) +
                                        sizeof (  L'/'  ) + sizeof (  L'\x00'  )
                                       )
            ) == NULL
     ) break;
  
     lstrcpyW ( pFullNameSrc, dirSrc );
     lstrcatW ( pFullNameSrc, L"/"  );
     lstrcatW ( pFullNameSrc, pFD -> cFileName );

     lstrcpyW ( pFullNameDst, dirDst );
     lstrcatW ( pFullNameDst, L"/"  );
     lstrcatW ( pFullNameDst, pFD -> cFileName );

     if ( pFD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {

       retVal = CopyDirectory ( pFullNameSrc, pFullNameDst );
       if ( ! retVal ) break;
   
     } else {
retry:   
      retVal = CopyFileW ( pFullNameSrc, pFullNameDst, FALSE );
      if ( ! retVal ) {
      
       if ( _response_dir_proc != NULL ) {
      
        response = ( *_response_dir_proc ) ( pFullNameSrc );

        if ( response == DIR_ABORT )

         break;

        else if ( response == DIR_RETRY )

         goto retry;

        else if ( response == DIR_IGNORE )

         retVal = TRUE;

        else

         break;

       }  /* end if */
      
      } else if ( _copy_dir_proc != NULL )

       ( *_copy_dir_proc ) ( pFullNameSrc, pFullNameDst );

     }  /* end else */

     HeapFree ( hHeap, 0, pFullNameDst );
     HeapFree ( hHeap, 0, pFullNameSrc );
     pFullNameSrc = pFullNameDst = NULL;

    }  /* end if */

    fFind = FindNextFileW ( hFindFile, pFD );

   }  /* end while */

  }  /* end if */

 }  /* end if ( error creating directory ) */

 if ( hFindFile != INVALID_HANDLE_VALUE ) FindClose ( hFindFile );

 if ( pFullNameSrc != NULL ) HeapFree ( hHeap, 0, pFullNameSrc );
 if ( pFullNameDst != NULL ) HeapFree ( hHeap, 0, pFullNameDst );
 if ( pName        != NULL ) HeapFree ( hHeap, 0, pName        );
 if ( pFD          != NULL ) HeapFree ( hHeap, 0, pFD          );

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
