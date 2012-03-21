// Copyright (c) 1995-1999 Matra Datavision
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

//============================================================================
//==== Titre: AlienImage_SGIRGBFileHeader.hxx
//==== Role : The header file of primitve type "SGIRGBFileHeader" 
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef SGIRGBSGIRGBFileHeader AlienImage_SGIRGBFileHeader;
//============================================================================

#ifndef _AlienImage_SGIRGBFileHeader_HeaderFile
#define _AlienImage_SGIRGBFileHeader_HeaderFile

//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_SGIRGBFileHeader);
//============================================================================

#define IMAGIC 	0732

/* colormap of images */
#define CM_NORMAL		0	/* file contains rows of values which 
					 * are either RGB values (zsize == 3) 
					 * or greyramp values (zsize == 1) */
#define CM_DITHERED		1
#define CM_SCREEN		2	/* file contains data which is a screen
					 * image; getrow returns buffer which 
					 * can be displayed directly with 
					 * writepixels */
#define CM_COLORMAP		3	/* a colormap file */

#define TYPEMASK		0xff00
#define BPPMASK			0x00ff
#define ITYPE_VERBATIM		0x0000
#define ITYPE_RLE		0x0100
#define ISRLE(type)		(((type) & 0xff00) == ITYPE_RLE)
#define ISVERBATIM(type)	(((type) & 0xff00) == ITYPE_VERBATIM)
#define BPP(type)		((type) & BPPMASK)
#define RLE(bpp)		(ITYPE_RLE | (bpp))
#define VERBATIM(bpp)		(ITYPE_VERBATIM | (bpp))
#define	IBUFSIZE(pixels)	((pixels+(pixels>>6))<<2)
#define	RLE_NOP			0x00

#define	ierror(p)		(((p)->flags&_IOERR)!=0)
#define	ifileno(p)		((p)->file)
#define	getpix(p)		(--(p)->cnt>=0 ? *(p)->ptr++ : ifilbuf(p))
#define putpix(p,x)		(--(p)->cnt>=0 \
				    ? ((int)(*(p)->ptr++=(unsigned)(x))) \
				    : iflsbuf(p,(unsigned)(x)))

typedef struct {
    unsigned short	imagic;		/* stuff saved on disk . . */
    unsigned short 	type;
    unsigned short 	dim;
    unsigned short 	xsize;
    unsigned short 	ysize;
    unsigned short 	zsize;
    unsigned int 	min;
    unsigned int 	max;
    unsigned int	wastebytes;	
    char 		name[80];
    unsigned int	colormap;

    int 		file;		/* stuff used in core only */
    unsigned short 	flags;
    short		dorev;
    short		x;
    short		y;
    short		z;
    short		cnt;
    unsigned short	*ptr;
    unsigned short	*base;
    unsigned short	*tmpbuf;
    unsigned int	offset;
    unsigned int	rleend;		/* for rle images */
    unsigned int	*rowstart;	/* for rle images */
    int			*rowsize;	/* for rle images */
} AlienImage_SGIRGBFileHeader;

AlienImage_SGIRGBFileHeader *iopen();
AlienImage_SGIRGBFileHeader *icreate();

ostream& operator << ( ostream& s,  const AlienImage_SGIRGBFileHeader& h );

Standard_Boolean   operator==(const AlienImage_SGIRGBFileHeader& AnObject,
			      const AlienImage_SGIRGBFileHeader& AnotherObject)  ;
void ShallowDump (const AlienImage_SGIRGBFileHeader& AnObject,Standard_OStream& S)  ;
#endif
 
