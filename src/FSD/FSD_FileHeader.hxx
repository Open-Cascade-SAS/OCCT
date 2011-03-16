// File:      FSD_FileHeader.hxx
// Created:   20.06.08 18:01:46
// Copyright: Open Cascade 2008

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
