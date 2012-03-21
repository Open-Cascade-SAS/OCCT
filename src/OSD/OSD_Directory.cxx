// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef WNT

#include <OSD_Directory.ixx>
#include <OSD_WhoAmI.hxx>
#include <OSD_Protection.hxx>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>   // For getwd()
#endif

#include <errno.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>

#ifdef HAVE_OSFCN_H
# include <osfcn.h>
#endif

const OSD_WhoAmI Iam = OSD_WDirectory;


OSD_Directory::OSD_Directory():OSD_FileNode() {
}

OSD_Directory::OSD_Directory(const OSD_Path& Name):OSD_FileNode(Name){
}

// Create physically a directory

void OSD_Directory::Build(const OSD_Protection& Protect){
int status;
Standard_Integer internal_prot;
TCollection_AsciiString aBuffer;

 internal_prot = Protect.Internal();
 myPath.SystemName(aBuffer);
 umask ( 0 );
 status = mkdir (aBuffer.ToCString(), (mode_t)internal_prot);
 if (status == -1) 
  if (errno != EEXIST) {
   Standard_PCharacter err_message;

   err_message = new Standard_Character [255];
   sprintf (err_message,
	    "OSD_Directory::Build Directory \"%s\"",
	    aBuffer.ToCString());
   
   myError.SetValue(errno, Iam, err_message);
   delete err_message;
  }
}

OSD_Directory OSD_Directory::BuildTemporary(){
OSD_Protection          Protect;
OSD_Directory           aDirectoryToReturn;
Standard_Integer        internal_prot;
Standard_CString        name = tmpnam(NULL);
TCollection_AsciiString aString (name);

 internal_prot = Protect.Internal();

 umask ( 0 );
 mkdir (name, (mode_t)internal_prot);
 unlink(name);//Destroys link but directory still exists while 
              //current process lives.

 aDirectoryToReturn.SetPath ( aString );
 return aDirectoryToReturn;

}

#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Diretory -----------------------
//------------------------------------------------------------------------

#include <OSD_Directory.hxx>
#include <OSD_Protection.hxx>
#include <Standard_ProgramError.hxx>

#include <OSD_WNT_1.hxx>

#include <stdio.h>

#ifndef _INC_TCHAR
# include <tchar.h>
#endif  // _INC_TCHAR

#ifdef _UNICODE
# define tctmpnam _wtmpnam
#else
# define tctmpnam tmpnam
#endif  // _UNICODE

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

OSD_Directory :: OSD_Directory () {

}  // end constructor ( 1 )

OSD_Directory :: OSD_Directory ( const OSD_Path& Name ) :
                 OSD_FileNode ( Name ) {

}  // end constructor ( 2 )

void OSD_Directory :: Build (const OSD_Protection& Protect ) {

 TCollection_AsciiString dirName;

 myPath.SystemName ( dirName );

 if (  dirName.IsEmpty ()  )

  Standard_ProgramError :: Raise (
                            TEXT( "OSD_Directory :: Build (): incorrect call - no directory name" )
                           );

 if (  Exists () || CreateDirectory ( dirName.ToCString (), NULL )  )

  SetProtection ( Protect );

 else

  _osd_wnt_set_error ( myError, OSD_WDirectory );

}  // end OSD_Directory :: Build

OSD_Directory OSD_Directory :: BuildTemporary () {

 OSD_Directory           retVal;
 OSD_Protection          prt;
 OSD_Path                dirPath (  tctmpnam ( NULL )  );

 retVal.SetPath ( dirPath );
 retVal.Build ( prt );                            

 return retVal;

}  // end OSD_Directory :: BuildTemporary

#endif
