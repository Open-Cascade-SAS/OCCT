// Created by: DCB
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

// Notes:       Most of code is taken from Xw_save_gif_image.c file (ZOV)

#include <AlienImage_GIFLZWDict.hxx>
#include <stdio.h>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_GIFLZWDict)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("AlienImage_GIFLZWDict", sizeof (AlienImage_GIFLZWDict));
  return _atype;
}

//=============================================================================
// Functions to write GIF image to file.
//=============================================================================

//===================== Static definitions ===================
static AlienImage_GIFLZWDict* dict;
static int startBits, codeBits, nextCode, bumpCode, rack, mask, putIdx,
           ClearCode, EOFCode, FreeCode;

static void  _init_dict  (void);
static int   _find_child (int, int);
static BOOL  _put_bits   (BYTE *, ULONG, UINT, OSD_File&);
static BOOL  _flush_bits (BYTE *, OSD_File&);


//=============================================================================
int _lzw_encode (OSD_File& file, const BYTE* pData, int width, int height, int inc)
{
  int   i, x, y;
  BYTE  byte, OutBuff [BUFF_SIZE];
  const BYTE *pLine;
  int   strCode, chr;

  dict = (AlienImage_GIFLZWDict*) malloc (sizeof (AlienImage_GIFLZWDict)*TBL_SIZE);
  if (dict == NULL)
    goto _ExitError;

  x     = y = 0;
  pLine = pData;

  OutBuff[ 0 ] = 0;
  putIdx       = 1;
  mask         = 0x01;
  rack         = 0;
  startBits    = 8;
  ClearCode    = 1 << startBits;
  EOFCode      = ClearCode + 1;
  FreeCode     = EOFCode + 1;

  _init_dict ();

  byte = startBits;

  file.Write (&byte, sizeof(byte));
  if (file.Failed ())
    goto _ExitError;

  strCode = pLine[ x++ ];

  if (!_put_bits (OutBuff, (ULONG) ClearCode, codeBits, file))
    goto _ExitError;

  while (y < height) {

    chr = pLine[ x++ ];

    i = _find_child ( strCode, chr );

    if ( dict[ i ].code != UNUSED )
      strCode = dict[ i ].code;

    else {
      dict[ i ].code = nextCode++;
      dict[ i ].prnt = strCode;
      dict[ i ].byte = chr;

      if (!_put_bits (OutBuff, (ULONG) strCode, codeBits, file))
        goto _ExitError;

      strCode = chr;

      if (nextCode > MAX_CODE) {
        if (!_put_bits (OutBuff, (ULONG) ClearCode, codeBits, file))
          goto _ExitError;

        _init_dict ();
      }
      else if (nextCode > bumpCode) {
        ++codeBits;
        bumpCode <<= 1;
      }
    }

    if (x == width) {
      x = 0;
      ++y;
      pLine += inc;
    }
  }

  if (!_put_bits (OutBuff, (ULONG) strCode, codeBits, file)) goto _ExitError;
  if (!_put_bits (OutBuff, (ULONG) EOFCode, codeBits, file)) goto _ExitError;
  if (!_flush_bits (OutBuff, file)) goto _ExitError;

  if (dict)
    free (dict); 
  return TRUE;


_ExitError:
  if (dict) 
    free (dict); 
  return FALSE;
}

//====================================================================
static void _init_dict () 
{
  memset (dict, UNUSED, sizeof (AlienImage_GIFLZWDict)*TBL_SIZE);
  nextCode = FreeCode;
  codeBits = startBits + 1;
  bumpCode = 1 << codeBits;
}

//====================================================================
static int _find_child (int prntCode, int chr)
{
  int idx, offset;

  idx = (  chr << ( NBITS - 8 )  ) ^ prntCode;
  offset = idx ? TBL_SIZE - idx : 1;

  for (;;) {

    if (dict[ idx ].code == UNUSED ||
        dict[ idx ].prnt == prntCode && dict[ idx ].byte == ( BYTE )chr)
      return idx;

    idx = ( idx >= offset ) ? idx - offset : idx + TBL_SIZE - offset;
  }
}

//====================================================================
static BOOL _put_bits (BYTE *OutBuff, ULONG code, UINT nBits, OSD_File& file)
{
  BOOL  retVal = TRUE;
  ULONG msk;

  msk = 1;

  while (msk != (ULONG) (1 << nBits)) {
    if ( msk & code )
      rack |= mask;
    mask <<= 1;

    if ((mask & 0xFF) == 0) {
      OutBuff[ putIdx++ ] = rack;
      ++OutBuff[ 0 ];
      
      if (putIdx == BUFF_SIZE) {
        file.Write (OutBuff, BUFF_SIZE);
        if (file.Failed ()) {
          retVal = FALSE;
          break;
        }
        putIdx = 1;
        OutBuff[ 0 ] = 0;
      }
      rack = 0;
      mask = 0x01;
    }
    msk <<= 1;
  }
  return retVal;
}

//====================================================================
static BOOL _flush_bits (BYTE* OutBuff, OSD_File& file)
{
  BOOL  retVal = TRUE;
  BYTE  byte;
  
  if ( mask != 0x01 ) {
    OutBuff[ putIdx++ ] = rack;
    ++OutBuff[ 0 ];
  }
    
  if (putIdx != 1) {
    file.Write (OutBuff, putIdx);
    if (file.Failed ())
      retVal = FALSE;
  }

  if (retVal) {
    byte = 0;
    file.Write (&byte, 1);
    if (file.Failed ())
      retVal = FALSE;
  } 
  return retVal;
}

//=============================================================================
// Functions to convert from 24 to 8 color image
//=============================================================================
static int __fastcall quick_check ( PBYTE, int, int, PBYTE, LPRGBQUAD );
//static int __fastcall quick_quant ( PBYTE, int, int, PBYTE, LPRGBQUAD );
static int __fastcall ppm_quant   ( PBYTE, int, int, PBYTE, LPRGBQUAD );

BOOL __fastcall _convert24to8 (
                        LPRGBQUAD cTable, PBYTE pData24, PBYTE pData8,
                        int w, int h
                       )
{
  BOOL retVal = FALSE;
  while ( 1 ) {
    if (  quick_check ( pData24, w, h, pData8, cTable )  ) {
okRet:
      retVal = TRUE;
      break;
    } else if (  ppm_quant ( pData24, w, h, pData8, cTable ) != -1  ) {
////////////////////
      for ( int i = 0; i < 256; ++i ) {
        BYTE b = cTable[ i ].rgbRed;
        cTable[ i ].rgbRed  = cTable[ i ].rgbBlue;
        cTable[ i ].rgbBlue = b;
      }  // end for
////////////////////
      goto okRet;
    }  // end if
    break;
  }  // end while
  return retVal;
}  // end _convert24to8

//*************************************************************************//
//* The following code based on code from the 'pbmplus' package written by //
//*  Jef Poskanzer                                                         //
//*************************************************************************//
//***//
#define MAXCOLORS 32767

#define PPM_GETR( p ) (  ( p ).r  )
#define PPM_GETG( p ) (  ( p ).g  )
#define PPM_GETB( p ) (  ( p ).b  )

#define PPM_ASSIGN( p, red, grn, blu ) \
 { ( p ).r = ( red ); ( p ).g = ( grn ); ( p ).b = ( blu ); }

#define PPM_EQUAL( p, q ) \
 (  ( p ).r == ( q ).r && ( p ).g == ( q ).g && ( p ).b == ( q ).b  )


#define PPM_DEPTH( newp, p, oldmaxval, newmaxval )                              \
 PPM_ASSIGN(                                                                    \
  ( newp ),                                                                     \
  (  ( int )PPM_GETR( p )  ) * (  ( int )newmaxval  ) / (  ( int )oldmaxval  ), \
  (  ( int )PPM_GETG( p )  ) * (  ( int )newmaxval  ) / (  ( int )oldmaxval  ), \
  (  ( int )PPM_GETB( p )  ) * (  ( int )newmaxval  ) / (  ( int )oldmaxval  )  \
 )


#define HASH_SIZE 6553

#define ppm_hashpixel( p )                  \
 (    (   (  ( int )PPM_GETR( p ) * 33023 + \
             ( int )PPM_GETG( p ) * 30013 + \
             ( int )PPM_GETB( p ) * 27011   \
          ) & 0x7fffffff                    \
      ) % HASH_SIZE                         \
 )

#define PPM_LUMIN( p ) \
         (  77 * PPM_GETR( p ) + 150 * PPM_GETG( p ) + 29 * PPM_GETB( p )  )

typedef struct { BYTE r, g, b; } pixel;

struct chist_item {
        pixel color;
        int   value;
       };

typedef struct chist_item* chist_vec;

typedef struct chist_list_item* chist_list;
typedef chist_list*             chash_table;

struct chist_list_item {
        struct chist_item ch;
        chist_list        next;
       };

struct box {
        int index;
        int colors;
        int sum;
       };

typedef struct box* box_vector;

static chist_vec   __fastcall ppm_computechist ( pixel**,   int, int, int, PINT );
static void        __fastcall ppm_freechist    ( chist_vec                      );
static chist_vec   __fastcall mediancut        ( chist_vec, int, int, int, int  );
static chash_table            ppm_allocchash   ( void                           );
static void        __fastcall ppm_freechash    ( chash_table                    );
static chash_table __fastcall ppm_computechash ( pixel**,   int, int, int, PINT );
static chist_vec   __fastcall ppm_chashtochist ( chash_table, int               );

static int redcompare   ( const void*, const void* );
static int greencompare ( const void*, const void* );
static int bluecompare  ( const void*, const void* );
static int sumcompare   ( const void*, const void* );

static int __fastcall ppm_quant (
                       PBYTE pic24, int cols, int rows, PBYTE pic8, LPRGBQUAD rgbmap
                      ) {


 pixel**     pixels;
 pixel*      pP;
 int         row;
 int         col, limitcol;
 BYTE        maxval, newmaxval;
 int         colors;
 int         index;
 chist_vec   chv, colormap;
 chash_table cht;
 int         i;
 PBYTE       picptr;
 LPRGBQUAD   pRGB;
 int         pad;

 index  = 0;
 maxval = 255;
// pad    = PAD( cols * 3 );
 pad = 0;

 pixels = ( pixel** )MALLOC(  rows * sizeof ( pixel* )  );
  
 if ( pixels == NULL ) return -1;

 for ( row = 0; row < rows; ++row ) {

  pixels[ row ] = ( pixel* )MALLOC(  cols * sizeof ( pixel )  );

  if ( pixels[ row ] == NULL ) {
freeMemory:
   while ( --row >= 0 ) FREE( pixels[ row ] );
      
   FREE( pixels );

   return -1;

  }  // end if

  for ( col = 0, pP = pixels[ row ]; col < cols; ++col, ++pP ) {

   pP -> r = *pic24++;
   pP -> g = *pic24++;
   pP -> b = *pic24++;
    
  }  // end for

  pic24 += pad;
  
 }  // end for

 for ( ;; ) {

  chv = ppm_computechist ( pixels, cols, rows, MAXCOLORS, &colors );
  
  if (  chv != ( chist_vec )0  ) break;
    
  newmaxval = maxval / 2;

  for ( row = 0; row < rows; ++row )

   for ( col = 0, pP = pixels[ row ]; col < cols; ++col, ++pP )

    PPM_DEPTH( *pP, *pP, maxval, newmaxval );
    
  maxval = newmaxval;
  
 }  // end for

 colormap = mediancut ( chv, colors, rows * cols, maxval, 256 );
  
 if (  colormap == ( chist_vec )NULL  ) {
freeMemory_1:
  ppm_freechist ( chv );
  row = rows;
  goto freeMemory;

 }  // end if

 ppm_freechist ( chv );

 cht = ppm_allocchash ();

 picptr = pic8;

 for ( row = 0;  row < rows;  ++row ) {

  col      = 0;
  limitcol = cols;
  pP       = pixels[ row ];

  do {

   int        hash;
   chist_list chl;

   hash = ppm_hashpixel ( *pP );

   for ( chl = cht[ hash ];  chl;  chl = chl -> next )

    if (  PPM_EQUAL( chl -> ch.color, *pP )  ) {

     index = chl -> ch.value;
     break;

    }  // end if

   if ( !chl ) {

    int  i, r1, g1, b1, r2, g2, b2;
    long dist, newdist;

        r1 = PPM_GETR( *pP );
        g1 = PPM_GETG( *pP );
        b1 = PPM_GETB( *pP );
        dist = 2000000000;

        for ( i = 0; i < 256; ++i ) {

     r2 = PPM_GETR( colormap[ i ].color );
     g2 = PPM_GETG( colormap[ i ].color );
     b2 = PPM_GETB( colormap[ i ].color );

     newdist = ( r1 - r2 ) * ( r1 - r2 ) +
               ( g1 - g2 ) * ( g1 - g2 ) +
               ( b1 - b2 ) * ( b1 - b2 );

     if ( newdist < dist ) {

      index = i;
      dist  = newdist;

     }  // end if
        
    }  // end for

    hash = ppm_hashpixel( *pP );
        chl  = ( chist_list )MALLOC(  sizeof ( struct chist_list_item )  );

        if ( chl == NULL ) {

     ppm_freechash ( cht );
     goto freeMemory_1;

    }  // end if

        chl -> ch.color = *pP;
        chl -> ch.value = index;
        chl -> next     = cht[ hash ];
        cht[ hash ]     = chl;
      
   }  // end if

   *picptr++ = index;
   ++col;
   ++pP;
  
  } while ( col != limitcol );

 }  // end for

 for ( i = 0, pRGB = rgbmap; i < 256; ++i, ++pRGB ) {

  PPM_DEPTH( colormap[ i ].color, colormap[ i ].color, maxval, 255 );
    
  pRGB -> rgbRed   = PPM_GETR( colormap[ i ].color );
  pRGB -> rgbGreen = PPM_GETG( colormap[ i ].color );
  pRGB -> rgbBlue  = PPM_GETB( colormap[ i ].color );
  
 }  // end for

 for ( i = 0; i < rows; ++i ) FREE( pixels[ i ] );
  
 FREE( pixels );

 ppm_freechist ( colormap );
 ppm_freechash ( cht      );

 return 0;

}  // end ppm_quant

static void __fastcall ppm_freechist ( chist_vec chv ) {

 FREE(  ( LPVOID )chv  );

}  // end ppm_freechist

static chist_vec __fastcall mediancut (
                             chist_vec chv, int colors, int sum,
                             int maxval, int newcolors
                            ) {

 chist_vec  colormap;
 box_vector bv;
 int        bi, i;
 int        boxes;

 bv = ( box_vector )MALLOC(  sizeof( struct box        ) * newcolors  );

 if ( bv == NULL ) return ( chist_vec )NULL;

 colormap = ( chist_vec  )MALLOC(  sizeof( struct chist_item ) * newcolors  );

 if ( colormap == NULL ) {

  FREE( bv );
  return ( chist_vec )NULL;

 }  // end if

 for ( i = 0; i < newcolors; ++i ) PPM_ASSIGN( colormap[ i ].color, 0, 0, 0 );

 bv[ 0 ].index  = 0;
 bv[ 0 ].colors = colors;
 bv[ 0 ].sum    = sum;
 boxes          = 1;

 while ( boxes < newcolors ) {

  int indx, clrs;
  int sm;
  int minr, maxr, ming, maxg, minb, maxb, v;
  int halfsum, lowersum;

  for ( bi = 0; bv[ bi ].colors < 2 && bi < boxes; ++bi );

  if ( bi == boxes ) break;

  indx = bv[ bi ].index;
  clrs = bv[ bi ].colors;
  sm   = bv[ bi ].sum;

  minr = maxr = PPM_GETR( chv[ indx ].color );
  ming = maxg = PPM_GETG( chv[ indx ].color );
  minb = maxb = PPM_GETB( chv[ indx ].color );

  for ( i = 1; i < clrs; ++i ) {

   v = PPM_GETR( chv[ indx + i ].color );

   if ( v < minr ) minr = v;
   if ( v > maxr ) maxr = v;

   v = PPM_GETG( chv[ indx + i ].color );

   if ( v < ming ) ming = v;
   if ( v > maxg ) maxg = v;

   v = PPM_GETB( chv[ indx + i ].color );

   if ( v < minb ) minb = v;
   if ( v > maxb ) maxb = v;

  }  // end for

  {  // begin local block

   pixel p;
   int   rl, gl, bl;

   PPM_ASSIGN( p, maxr - minr, 0, 0 );      
   rl = PPM_LUMIN( p );

   PPM_ASSIGN( p, 0, maxg - ming, 0 );
   gl = PPM_LUMIN( p );

   PPM_ASSIGN( p, 0, 0, maxb - minb );
   bl = PPM_LUMIN( p );

   if ( rl >= gl && rl >= bl )

    qsort (
     ( char* )&chv[ indx ], ( size_t )clrs,
     sizeof ( struct chist_item ), redcompare
    );
      
   else if ( gl >= bl )

    qsort (
     ( char* )&chv[ indx ], ( size_t )clrs,
     sizeof ( struct chist_item ), greencompare
    );
      
   else 

    qsort (
     ( char* )&chv[ indx ], ( size_t )clrs,
     sizeof ( struct chist_item ), bluecompare
    );
    
  }  // end local block

  lowersum = chv[ indx ].value;
  halfsum  = sm / 2;

  for ( i = 1; i < clrs - 1; ++i) {

   if ( lowersum >= halfsum ) break;

   lowersum += chv[ indx + i ].value;

  }  // end for

  bv[ bi ].colors = i;
  bv[ bi ].sum    = lowersum;

  bv[ boxes   ].index  = indx + i;
  bv[ boxes   ].colors = clrs - i;
  bv[ boxes++ ].sum    = sm - lowersum;

  qsort (
   ( char* )bv, ( size_t )boxes, sizeof ( struct box ), sumcompare
  );
  
 }  // end while

 for ( bi = 0; bi < boxes; ++bi ) {

  int  indx = bv[ bi ].index;
  int  clrs = bv[ bi ].colors;
  long r = 0, g = 0, b = 0, sum = 0;

  for ( i = 0; i < clrs; ++i ) {

   r += PPM_GETR( chv[ indx + i ].color ) * chv[ indx + i ].value;
   g += PPM_GETG( chv[ indx + i ].color ) * chv[ indx + i ].value;
   b += PPM_GETB( chv[ indx + i ].color ) * chv[ indx + i ].value;
      
   sum += chv[ indx + i ].value;

  }  // end for ( i . . . )

  r = r / sum;  if ( r > maxval ) r = maxval;
  g = g / sum;  if ( g > maxval ) g = maxval;
  b = b / sum;  if ( b > maxval ) b = maxval;

  PPM_ASSIGN(  colormap[ bi ].color, ( BYTE )r, ( BYTE )g, ( BYTE )b  );
  
 }  // end for ( bi . . . )

 FREE( bv );

 return colormap;

}  // end mediancut

static int redcompare ( const void* p1, const void* p2 ) {

 return ( int )PPM_GETR(   (  ( chist_vec )p1  ) -> color   ) - 
        ( int )PPM_GETR(   (  ( chist_vec )p2  ) -> color   );

}  // end redcompare

static int greencompare ( const void* p1, const void* p2 ) {

 return ( int )PPM_GETG(   (  ( chist_vec )p1  ) -> color   ) - 
        ( int )PPM_GETG(   (  ( chist_vec )p2  ) -> color   );

}  // end greencompare

static int bluecompare ( const void* p1, const void* p2 ) {

 return ( int )PPM_GETB(   (  ( chist_vec )p1  ) -> color   ) - 
        ( int )PPM_GETB(   (  ( chist_vec )p2  ) -> color   );

}  // end bluecompare

static int sumcompare ( const void* p1, const void* p2 ) {

 return (  ( box_vector )p2  ) -> sum - (  ( box_vector )p1  ) -> sum;

}  // end sumcompare

static chash_table ppm_allocchash ( void ) {

 chash_table cht;

 cht = ( chash_table )MALLOC(  HASH_SIZE * sizeof ( chist_list )  );

 return cht;

}  // end ppm_allocchash

static chist_vec __fastcall ppm_computechist (
                             pixel** pixels, int cols, int rows, int maxcolors,
                             PINT colorsP
                            ) {

 chash_table cht;
 chist_vec chv;

 cht = ppm_computechash ( pixels, cols, rows, maxcolors, colorsP );
  
 if ( cht == NULL ) return ( chist_vec )NULL;

 chv = ppm_chashtochist ( cht, maxcolors );

 ppm_freechash ( cht );
  
 return chv;

}  // end ppm_computechist

static chash_table __fastcall ppm_computechash (
                               pixel** pixels, int cols, int rows, int maxcolors,
                               PINT colorsP
                              ) {

 chash_table     cht;
 register pixel* pP;
 chist_list      chl;
 int             col, row, hash;

 cht      = ppm_allocchash ();
 *colorsP = 0;

 for ( row = 0; row < rows; ++row )

  for ( col = 0, pP = pixels[ row ];  col < cols;  ++col, ++pP ) {

   hash = ppm_hashpixel ( *pP );

   for ( chl = cht[ hash ]; chl != ( chist_list )0; chl = chl -> next )

    if (  PPM_EQUAL( chl -> ch.color, *pP )  ) break;
      
   if (  chl != ( chist_list)0  )

    ++( chl->ch.value );
      
   else {

        if (  ( *colorsP )++ > maxcolors  ) {

     ppm_freechash ( cht );

     return ( chash_table )NULL;

    }  // end if
        
    chl = ( chist_list )MALLOC(  sizeof( struct chist_list_item )  );

    if ( chl == NULL ) return ( chash_table )NULL;
        
    chl -> ch.color = *pP;
    chl -> ch.value = 1;
    chl -> next     = cht[ hash ];
    cht[ hash ]     = chl;
      
  }  // end else
    
 }  // end for
  
 return cht;

}  // end ppm_computechash

static chist_vec __fastcall ppm_chashtochist ( chash_table cht, int maxcolors ) {

 chist_vec  chv;
 chist_list chl;
 int        i, j;

 chv = ( chist_vec )MALLOC(  maxcolors * sizeof ( struct chist_item )  );

 if ( chv == NULL) return chv;

 j = 0;

 for ( i = 0; i < HASH_SIZE; ++i )

  for ( chl = cht[ i ];  chl != ( chist_list )0;  chl = chl -> next ) {

   chv[ j ] = chl -> ch;
   ++j;
    
  }  // end for

  return chv;

}  // end ppm_chashtochist

static void __fastcall ppm_freechash ( chash_table cht ) {

 int        i;
 chist_list chl, chlnext;

 for ( i = 0; i < HASH_SIZE; ++i )

  for ( chl = cht[ i ];  chl != ( chist_list )0; chl = chlnext ) {

   chlnext = chl -> next;
   FREE( chl );
    
  }  // end for

 FREE( cht );

}  // end ppm_freechash

static int __fastcall quick_check (
                       PBYTE pic24, int w, int h, PBYTE pic8, LPRGBQUAD rgbmap
                      ) {

 unsigned long colors[ 256 ], col;
 int           i, j, nc, low, high, mid, pad;
 PBYTE         p, pix;

 nc  = 0;
 mid = 0;
// pad = PAD( w * 3 );
 pad = 0;

 for ( i = 0, p = pic24; i < h; ++i ) {

  for ( j = 0; j < w; ++j ) {

   col  = (   (  ( unsigned long )*p++  ) << 16   );  
   col += (   (  ( unsigned long )*p++  ) <<  8   );
   col += (   (  ( unsigned long )*p++  ) <<  0   );

   low = 0;  high = nc - 1;

   while ( low <= high ) {

    mid = ( low + high ) / 2;

    if ( col < colors[ mid ] )

     high = mid - 1;

    else if ( col > colors[ mid ] )

     low  = mid + 1;
      
    else break;
    
   }  // end while

   if ( high < low ) {

    if ( nc >= 256 ) return FALSE;

    memcpy (
     ( char* )&colors[ low + 1 ], ( char* )&colors[ low ],
     ( nc - low ) * sizeof ( unsigned long )
    );

    colors[ low ] = col;
    ++nc;
    
   }  // end if

  }  // end for ( j . . . )

  p += pad;

 }  // end for ( i . . . )

 for ( i = 0, p = pic24, pix = pic8; i < h; ++i ) {

  for ( j = 0; j < w; ++j ) {

   col  = (   (  ( unsigned long )*p++  ) << 16   );  
   col += (   (  ( unsigned long )*p++  ) <<  8   );
   col += *p++;

   low = 0;  high = nc - 1;

   while ( low <= high ) {

    mid = ( low + high ) / 2;

    if ( col < colors[ mid ] )

     high = mid - 1;
      
    else if ( col > colors[ mid ] )

     low  = mid + 1;

    else break;
    
   }  // end while

   *pix++ = mid;

  }  // end for ( j . . . )

  p += pad;

 }  // end for ( i . . . )

 for ( i = 0; i < nc; ++i, ++rgbmap ) {

  rgbmap -> rgbRed   = ( BYTE )( colors[ i ] >>  0 ) & 0xFF;  
  rgbmap -> rgbGreen = ( BYTE )( colors[ i ] >>  8 ) & 0xFF;
  rgbmap -> rgbBlue  = ( BYTE )( colors[ i ] >> 16 ) & 0xFF;
  
 }  // end for

 return nc;

}  // end quick_check

#ifdef BSHIFT		//HPUX COMPATIBILLITY
#undef BSHIFT
#endif
#define RMASK  0xE0
#define RSHIFT    0
#define GMASK  0xE0
#define GSHIFT    3
#define BMASK  0xC0
#define BSHIFT    6
#define RANGE( a, b, c ) { if ( a < b ) a = b;  if ( a > c ) a = c; }

//static int __fastcall quick_quant (
//                       PBYTE p24, int w, int h, PBYTE p8, LPRGBQUAD rgbmap
//                      ) {

// PBYTE pp;
// int   r1, g1, b1;
// PINT  thisline, nextline, thisptr, nextptr, tmpptr;
// int   i, j, val, pwide3;
// int   imax, jmax;
// int   pad;

//// pad = PAD( w * 3 );
// pad = 0;

// pp     = p8;
// pwide3 = w * 3;
// imax   = h - 1;
// jmax   = w - 1;

// for ( i = 0; i < 256; ++i ) {

//  rgbmap[ i ].rgbRed   = (   (  ( i << RSHIFT ) & RMASK  ) * 255 + RMASK / 2   ) / RMASK;
//  rgbmap[ i ].rgbGreen = (   (  ( i << GSHIFT ) & GMASK  ) * 255 + GMASK / 2   ) / GMASK;
//  rgbmap[ i ].rgbBlue  = (   (  ( i << BSHIFT ) & BMASK  ) * 255 + BMASK / 2   ) / BMASK;
  
// }  // end for
  
// thisline = ( PINT )MALLOC(  pwide3 * sizeof ( int )  );

// if ( thisline == NULL ) return 1;

// nextline = ( PINT )MALLOC(  pwide3 * sizeof ( int )  );

// if ( nextline == NULL ) {

//  FREE( thisline );
//  return 1;

// }  // end if

// for ( j = pwide3, tmpptr = nextline; j; --j ) *tmpptr++ = ( int )*p24++;

// p24 += pad;
  
// for ( i = 0; i < h; ++i ) {

//  tmpptr   = thisline;
//  thisline = nextline;
//  nextline = tmpptr;
//    
//  if ( i != imax ) {

//   for ( j = pwide3, tmpptr = nextline; j; --j ) *tmpptr++ = ( int )*p24++;
//
//   p24 += pad;

//  }  // end if
    
//  for ( j = 0, thisptr = thisline, nextptr = nextline; j < w; ++j, ++pp ) {

//   r1 = *thisptr++;
//   RANGE( r1, 0, 255 );

//   g1 = *thisptr++;
//   RANGE(g1,0,255);

//   b1 = *thisptr++;
//   RANGE(b1,0,255);  
      
//   val = (   (  ( r1 & RMASK ) >> RSHIFT  ) |
//             (  ( g1 & GMASK ) >> GSHIFT  ) | 
//                 (  ( b1 & BMASK ) >> BSHIFT  )   );
//   *pp = val;
      
//   r1 -= rgbmap[ val ].rgbRed;
//   g1 -= rgbmap[ val ].rgbGreen;
//   b1 -= rgbmap[ val ].rgbBlue;
      
//   if ( j != jmax ) {

//    thisptr[ 0 ] += ( r1 * 7 ) / 16;
//    thisptr[ 1 ] += ( g1 * 7 ) / 16;
//    thisptr[ 2 ] += ( b1 * 7 ) / 16;
      
//   }  // end if
      
//   if ( i != imax ) {

//    nextptr[ 0 ] += ( r1 * 5 ) / 16;
//    nextptr[ 1 ] += ( g1 * 5 ) / 16;
//    nextptr[ 2 ] += ( b1 * 5 ) / 16;

//    if ( j > 0 ) {

//     nextptr[ -3 ] += ( r1 * 3 ) / 16;
//     nextptr[ -2 ] += ( g1 * 3 ) / 16;
//     nextptr[ -1 ] += ( b1 * 3 ) / 16;
        
//    }  // end if

//    if ( j != jmax ) {

//     nextptr[ 3 ] += ( r1 ) / 16;
//     nextptr[ 4 ] += ( g1 ) / 16;
//     nextptr[ 5 ] += ( b1 ) / 16;
        
//    }  // end if

//        nextptr += 3;

//   }  // end if

//  }  // end for ( j . . . )
  
// }  // end for ( i . . . )
  
// FREE( thisline );
// FREE( nextline );
  
// return 0;

//}  // end quick_quant
//***//
//*************************************************************************//
 
