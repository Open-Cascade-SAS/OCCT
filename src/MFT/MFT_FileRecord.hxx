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

#ifndef _MFT_FileRecord_HeaderFile
#define _MFT_FileRecord_HeaderFile
#include <OSD.hxx>
#include <MFT_FilePosition.hxx>
#include <MFT_FileHandle.hxx>

#define MFT_BLOCKSIZE 512		//Physical record size

#ifdef WNT
#include <sys/types.h>
#define MAP_FAILED -1
#define MMAP(file_addr,mmap_size,fildes)\
            mmap ((off_t)file_addr, (int)mmap_size, (int)fildes)
#define MUNMAP(mmap_addr,mmap_size)\
            munmap ((void*)mmap_addr)
/*
#undef MMAP
#undef MUNMAP
*/
#endif	//WNT

#if defined ( __hpux ) || defined ( HPUX )
#include <sys/mman.h>
#define MAP_FAILED -1
#define MMAP(file_addr,mmap_size,fildes) \
        mmap((caddr_t) 0x80000000,(size_t) mmap_size,PROT_READ, \
                MAP_FILE | MAP_PRIVATE | MAP_VARIABLE,fildes,(off_t) file_addr)
#define MUNMAP(mmap_addr,mmap_size) \
        munmap((caddr_t) mmap_addr,(size_t) mmap_size)
#endif 	// __hpux

#if defined ( sun ) || defined ( SOLARIS )
#include <sys/mman.h>
#define MMAP(file_addr,mmap_size,fildes) \
        mmap((caddr_t) 0x60000000,(size_t) mmap_size,PROT_READ, \
                MAP_PRIVATE,fildes,(off_t) file_addr)
#define MUNMAP(mmap_addr,mmap_size) \
        munmap((caddr_t) mmap_addr,(size_t) mmap_size)
#endif 	// sun

#if defined ( sgi ) || defined ( IRIX )
#include <sys/mman.h>
#define MMAP(file_addr,mmap_size,fildes) \
        mmap((caddr_t) 0x60000000,(size_t) mmap_size,PROT_READ, \
                MAP_PRIVATE,fildes,(off_t) file_addr)
#define MUNMAP(mmap_addr,mmap_size) \
        munmap((caddr_t) mmap_addr,(size_t) mmap_size)
#endif // sgi

#if defined ( __alpha) || defined ( DECOSF1 )
#include <sys/mman.h>
#define MMAP(file_addr,mmap_size,fildes) \
        mmap((caddr_t) 0x1000000000,(size_t) mmap_size,PROT_READ, \
                MAP_FILE | MAP_PRIVATE | MAP_VARIABLE,fildes,(off_t) file_addr)
#define MUNMAP(mmap_addr,mmap_size) \
        munmap((caddr_t) mmap_addr,(size_t) mmap_size)
#endif 	// __alpha

#ifndef MMAP
// #error  *** MMAP is not used on your system by MFT fonts ***
#endif

typedef struct _MFT_FileRecord {
    MFT_FileHandle  fileHandle;
    MFT_FilePosition beginPosition; 	//First record position of the section
    MFT_FilePosition recordPosition;	//Current record position 
    unsigned int recordSize;		//Logical record size 
    Standard_Boolean update;		//TRUE if something have change. 
    Standard_CString precord;		//Process record address.
    Standard_CString precordMMAPAddress;//MMAP physical record address
    MFT_FilePosition recordMMAPPosition;//MMAP logical record position 
    long int         recordMMAPSize;	//MMAP physical record size 
    Standard_Boolean swap;		//TRUE if this record must be swapped 
} MFT_FileRecord;

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif
const Handle(Standard_Type)& STANDARD_TYPE(MFT_FileRecord);

#endif
