/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef CGMSTRUCT_H
#define CGMSTRUCT_H

/*  Include all files on which this is dependent.  */

#include "cgmtypes.h"
#include "cgmelem.h"
#include "cgmpar.h"

/*  Define real precision structure (character encoding) */

struct real_prec {
   Prec prec, min, defexp, expald;
};

/*  CGM element codes for clear text input and output */

struct commands {
     Code code;
     char *string;
  };

/*   Encoding Specific defaults   */

struct char_defaults{
      Prec int_prec, index_prec, vdcint_prec;
      struct real_prec real, vdc;
      Prec col_prec, colind_prec;
      struct colour min_rgb, max_rgb;
      long ring_angres, ring_int, ring_real;
};

struct text_defaults{
      long min_int, max_int, min_index, max_index;
      float min_real, max_real;
      Prec real_digits;
      struct vdc min_vdc, max_vdc;
      Prec vdc_digits, colind_prec, col_prec;
      struct colour min_rgb, max_rgb;
};

struct bin_defaults{
      Prec int_prec, index_prec, vdcint_prec;
      Enum real_type, vdc_type;
      Prec real_whole, real_fraction, vdc_whole, vdc_fraction;
      Prec col_prec, colind_prec;
      struct colour min_rgb, max_rgb;
};

struct defaults {

/*  Encoding independent defaults        */

      Index max_colind;
      Enum vdc_type, transparency, clip_ind;
      Enum color_mode, scale_mode;
      float scale_factor;
      Enum linewidth_mode, markersize_mode, edgewidth_mode;
      struct colour back, aux;
      struct rect vdc_extent, clip_rect;

/*  Retain encoding defaults for common access  */

      Prec int_bits,     /*  integer prec in bits */
           index_bits,   /*  index prec in bits  */
           vdcint_bits,  /*  vdc integer precision in bits */
           real_bits,    /*  real prec in bits */
           real_places,  /*  binary bits after decimal place */
           vdc_bits,     /*  vdc prec in bits  */
           vdc_places,   /*  binary bits after decimal place */
           col_bits,     /*  colour prec in bits  */
           colind_bits;  /*  colour index prec in bits */
      float min_real,   /*  min real value */
            max_real,   /*  max real value */
            realmin,    /*  smallest real value  */
            min_vdc,    /*  min real vdc value   */
            max_vdc,    /*  max real vdc value   */
            vdcmin;     /*  smallest vdc real value */
      Prec  real_defexp, /* Real default exponent */
            real_expald, /* Real exponenet allowed */
            vdc_defexp,  /* VDC default exponent */
            vdc_expald;   /* VDC exponenet allowed */
      struct colour min_rgb,  /* integer values: index/RGB */
                    max_rgb;  /* integer values: index/RGB */
      Logical colprec_flag,   /* Flag to show if colour precision set */
              colval_flag;    /* Flag to show if colour value extent set */
  };

/*  Graphics attributes  */

struct attributes {
      Index line_ind, mark_ind, text_ind, fill_ind, edge_ind;
      Index line_type, mark_type, text_font, edge_type;
      struct vdc line_width, mark_size, edge_width;
      struct colour line, marker, text, fill, edge;
      Enum text_prec, text_path, text_halign, text_valign, edge_vis;
      float text_hcont, text_vcont;
      Index char_set, altchar_set;
      float char_exp, char_space;
      struct point char_up, char_base;
      struct vdc char_height;
      Enum int_style;
      Index hatch_ind, pat_ind;
      struct point fill_ref;
      struct rect pat_size;
      Enum asf[ASFS];
/*  Flags for VDC dependent defaults */
      Logical linewidth_flag, markersize_flag, edgewidth_flag,
              charheight_flag, cliprect_flag;
  };


/* Structure for attribute change flags  */

struct flags {
  Logical line_index, line_type, line_width, line_color;
  Logical mark_index, mark_type, mark_size, mark_color;
  Logical text_index, text_font, text_prec, char_expan, char_space,
          text_color, char_height, char_orient, char_set,
          altchar_set, text_path, text_align;
  Logical fill_index, int_style, hatch_ind,
          pat_ind, pat_size, fill_color, fill_ref;
  Logical edge_index, edge_type, edge_width, edge_color, edge_vis;
};

/*  structure for random frame searching */

 struct data_frame
 {
    fpos_t disk_addr;
    char *cur_ptr, *end_ptr;
    union {
       int meta;
       int frame;
    } index;
 };

     /*  Basic data held about CGM character sets and CGM fonts, as
         provided in CGM CHARACTERSETLIST and FONTLIST elements.
     */

struct cgmfont

   { char **names;
     int    count;
   };

struct cgmcset

   { char **names;
     int *types;
     int    count;
   };

#endif  /* end of cgmstruc.h */
