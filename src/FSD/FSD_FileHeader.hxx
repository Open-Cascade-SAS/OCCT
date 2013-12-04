// Created on: 2008-06-20
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _FSD_FileHeader_HeaderFile
#define _FSD_FileHeader_HeaderFile

struct FSD_FileHeader {
  Standard_Integer testindian;
  Standard_Integer binfo;
  Standard_Integer einfo;
  Standard_Integer bcomment;
  Standard_Integer ecomment;
  Standard_Integer btype;
  Standard_Integer etype;
  Standard_Integer broot;
  Standard_Integer eroot;
  Standard_Integer bref;
  Standard_Integer eref;
  Standard_Integer bdata;
  Standard_Integer edata;  
};

#ifndef DO_INVERSE
#if defined ( SOLARIS ) || defined ( IRIX )
#define DO_INVERSE 1
#else
#define DO_INVERSE 0
#endif
#endif

//=======================================================================
//function : InverseInt
//purpose  : Inverses bytes in the word
//=======================================================================

inline Standard_Integer InverseInt (const Standard_Integer theValue)
{
  return (0 | (( theValue & 0x000000ff ) << 24 )
          |   (( theValue & 0x0000ff00 ) << 8  )
          |   (( theValue & 0x00ff0000 ) >> 8  )
          |   (( theValue >> 24 ) & 0x000000ff ) );
}

//=======================================================================
//function : InverseLong
//purpose  : Inverses bytes in the long word
//=======================================================================

inline long InverseLong (const long theValue)
{
  return (long) InverseInt ((Standard_Integer) theValue);
}

//=======================================================================
//function : InverseExtChar
//purpose  : Inverses bytes in the extended character
//=======================================================================

inline Standard_ExtCharacter InverseExtChar (const Standard_ExtCharacter theValue)
{
  return (0 | (( theValue & 0x00ff ) << 8  )
          |   (( theValue & 0xff00 ) >> 8  ) );
}

//=======================================================================
//function : InverseReal
//purpose  : Inverses bytes in the real value
//=======================================================================

inline Standard_Real InverseReal (const Standard_Real theValue)
{
  Standard_Real aResult;
  Standard_Integer *i = (Standard_Integer*) &theValue;
  Standard_Integer *j = (Standard_Integer*) &aResult;
  j[1] = InverseInt (i[0]);
  j[0] = InverseInt (i[1]);
  return aResult;
}

//=======================================================================
//function : InverseShortReal
//purpose  : Inverses bytes in the short real value
//=======================================================================

inline Standard_ShortReal InverseShortReal (const Standard_ShortReal theValue)
{
  Standard_ShortReal aResult;
  Standard_Integer *i = (Standard_Integer*) &aResult;
  *i = InverseInt (*(Standard_Integer*) &theValue);
  return aResult;
}

//=======================================================================
//function : InverseSize
//purpose  : Inverses bytes in size_t type instance
//=======================================================================

template<int size>
inline Standard_Size InverseSizeSpecialized (const Standard_Size theValue, int);

template<>
inline Standard_Size InverseSizeSpecialized <4> (const Standard_Size theValue, int)
{
  return (0 | (( theValue & 0x000000ff ) << 24 )
            | (( theValue & 0x0000ff00 ) << 8  )
            | (( theValue & 0x00ff0000 ) >> 8  )
            | (( theValue >> 24 ) & 0x000000ff ) );
}

template<>
inline Standard_Size InverseSizeSpecialized <8> (const Standard_Size theValue, int)
{
  Standard_Size aResult;
  Standard_Integer *i = (Standard_Integer*) &theValue;
  Standard_Integer *j = (Standard_Integer*) &aResult;
  j[1] = InverseInt (i[0]);
  j[0] = InverseInt (i[1]);
  return aResult;
}

inline Standard_Size InverseSize (const Standard_Size theValue)
{
  return InverseSizeSpecialized <sizeof(Standard_Size)> (theValue, 0);
}


#endif
