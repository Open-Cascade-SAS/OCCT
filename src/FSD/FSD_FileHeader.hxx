// Created on: 2008-06-20
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

inline Standard_Size InverseSize (const Standard_Size theValue)
{
  if (sizeof(Standard_Size) == 4)
    return (0 | (( theValue & 0x000000ff ) << 24 )
            |   (( theValue & 0x0000ff00 ) << 8  )
            |   (( theValue & 0x00ff0000 ) >> 8  )
            |   (( theValue >> 24 ) & 0x000000ff ) );
  else if (sizeof(Standard_Size) == 8) {
    Standard_Size aResult;
    Standard_Integer *i = (Standard_Integer*) &theValue;
    Standard_Integer *j = (Standard_Integer*) &aResult;
    j[1] = InverseInt (i[0]);
    j[0] = InverseInt (i[1]);
    return aResult;
  }
  else
    return 0;
}

#endif
