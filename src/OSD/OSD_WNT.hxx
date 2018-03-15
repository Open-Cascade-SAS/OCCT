// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef __OSD_WNT_H
# define __OSD_WNT_H

# ifndef STRICT
#  define STRICT
# endif  /* STRICT */

# ifndef _INC_WINDOWS
#  include <windows.h>
# endif  /* _INC_WINDOWS */

# ifdef __cplusplus
extern "C" {
# endif  /* __cplusplus */

enum DIR_RESPONSE { DIR_ABORT, DIR_RETRY, DIR_IGNORE };

#define FLAG_READ_PIPE  0x00000001
#define FLAG_EOF        0x00000002
#define FLAG_FILE       0x00000004
#define FLAG_DIRECTORY  0x00000008
#define FLAG_PIPE       0x00000010
#define FLAG_SOCKET     0x00000020
#define FLAG_NAMED_PIPE 0x00000040
#define FLAG_DEVICE     0x00000080
#define FLAG_TYPE       0x0000007C

// 2 macros modified for VisualAge
//#define LODWORD( a ) ( DWORD )(   ( ( DWORDLONG )( a ) ) & 0x00000000FFFFFFFF   )
//#define HIDWORD( a ) ( DWORD )(   ( ( DWORDLONG )( a ) ) >> 32                  )

#define LODWORD( a ) ( DWORD )(   ( ( _int64 )( a ) ) & 0x00000000FFFFFFFF   )
#define HIDWORD( a ) ( DWORD )(   ( ( _int64 )( a ) ) >> 32                  )

typedef struct _file_ace {

                ACE_HEADER header;
                DWORD      dwMask;
                PSID       pSID;

               } FILE_ACE, *PFILE_ACE;

typedef void ( *MOVE_DIR_PROC   ) ( LPCWSTR, LPCWSTR );
typedef void ( *COPY_DIR_PROC   ) ( LPCWSTR, LPCWSTR );
typedef void ( *DELETE_DIR_PROC ) ( LPCWSTR          );

typedef DIR_RESPONSE ( *RESPONSE_DIR_PROC ) ( LPCWSTR );

#define GET_SID( pACE ) (  ( PSID )(   (  ( PBYTE )pACE    ) + \
                                       sizeof ( ACE_HEADER ) + \
                                       sizeof ( DWORD      )   \
                                   )                           \
                        )

#define GET_MSK( pACE ) (  ( PDWORD )(   (  ( PBYTE )pACE    ) + \
                                         sizeof ( ACE_HEADER )   \
                                     )                           \
                        )

PSECURITY_DESCRIPTOR AllocSD ( void                 );
void                 FreeSD  ( PSECURITY_DESCRIPTOR );

LPVOID GetTokenInformationEx ( HANDLE, TOKEN_INFORMATION_CLASS );
void   FreeTokenInformation  ( LPVOID                          );

PSECURITY_DESCRIPTOR GetFileSecurityEx ( LPCWSTR, SECURITY_INFORMATION );
void                 FreeFileSecurity  ( PSECURITY_DESCRIPTOR          );

PACL CreateAcl ( DWORD );
void FreeAcl   ( PACL  );

BOOL PredefinedSid   ( PSID );
BOOL NtPredefinedSid ( PSID );
PSID AdminSid        ( void );
PSID WorldSid        ( void );
PSID InteractiveSid  ( void );
PSID NetworkSid      ( void );
PSID LocalSid        ( void );
PSID DialupSid       ( void );
PSID BatchSid        ( void );
PSID CreatorOwnerSid ( void );
PSID NullSid         ( void );
PSID NtSid           ( void );

PVOID AllocAccessAllowedAce ( DWORD, BYTE, PSID );
void  FreeAce ( PVOID );

BOOL MoveDirectory   ( LPCWSTR, LPCWSTR );
BOOL CopyDirectory   ( LPCWSTR, LPCWSTR );

void SetMoveDirectoryProc     ( MOVE_DIR_PROC     );
void SetCopyDirectoryProc     ( COPY_DIR_PROC     );
void SetResponseDirectoryProc ( RESPONSE_DIR_PROC );

# ifdef __cplusplus
}
# endif  /* __cplusplus */

#endif  /* __OSD_WNT_H */
