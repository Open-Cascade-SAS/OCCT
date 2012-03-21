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

#ifndef __Aspect_WNTXWD_HXX
# define __Aspect_WNTXWD_HXX

# ifndef WNT
#  include <X11/XWDFile.h>
# else

# ifdef WORD64
#  define B32 :32
# else
#  define B32
# endif  /* WORD64 */

# define XWD_FILE_VERSION  7

# define XYBitmap          0
# define XYPixmap          1
# define ZPixmap           2

# define LSBFirst          0
# define MSBFirst                  1

# define DoRed             ( 1 << 0 )
# define DoGreen           ( 1 << 1 )
# define DoBlue            ( 1 << 2 )

# define StaticGray        0
# define GrayScale         1
# define StaticColor       2
# define PseudoColor       3
# define TrueColor         4
# define DirectColor       5

typedef unsigned int CARD32;

typedef struct _xwd_file_header {

                CARD32 header_size      B32;
                                CARD32 file_version         B32;
                                CARD32 pixmap_format    B32;
                                CARD32 pixmap_depth     B32;
                                CARD32 pixmap_width     B32;
                                CARD32 pixmap_height    B32;
                                CARD32 xoffset          B32;
                                CARD32 byte_order       B32;
                                CARD32 bitmap_unit      B32;
                                CARD32 bitmap_bit_order B32;
                                CARD32 bitmap_pad       B32;
                                CARD32 bits_per_pixel   B32;
                                CARD32 bytes_per_line   B32;
                                CARD32 visual_class     B32;
                                CARD32 red_mask         B32;
                                CARD32 green_mask       B32;
                                CARD32 blue_mask        B32;
                                CARD32 bits_per_rgb     B32;
                                CARD32 colormap_entries B32;
                                CARD32 ncolors          B32;
                                CARD32 window_width     B32;
                                CARD32 window_height    B32;
                                CARD32 window_x         B32;
                                CARD32 window_y         B32;
                                CARD32 window_bdrwidth  B32;
# ifdef WORD64
                                CARD32 header_end       B32;
# endif  /* WORD64 */
               } XWDFileHeader;

typedef struct _xcolor {

                                unsigned long  pixel;
                                unsigned short red;
                                unsigned short green;
                                unsigned short blue;
                                char           flags;
                                char           pad;

               } XColor;

# endif /* WNT */
#endif  /* __Aspect_WNTXWD_HXX */
