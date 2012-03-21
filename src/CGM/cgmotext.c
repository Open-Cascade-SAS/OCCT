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

/* Standard include files */ 
#include "cgmout.h"
#include "cgminit.h"
/*  Output stream for functions */

static FILE *cgmot;

/*  Functions in this Module  */

# if (defined __STDC__ && __STDC__) || defined __cplusplus
/* use function prototypes, they are requred for ANSI C and C++ compilers */
void CGMOtext(FILE *stream, Code c, Long *pi, float *pr, char *str);     /* Main Output */
static void MOTpoints(Long n, Long *pi, float *pr, Enum set),    /* Output a points list */
            MOTvdc(int n, Long *pi, Float *pr),       /* Output n VDCs */
            MOTcol(struct colour *c, Enum type),       /* Output a colour */
            MOTstring(char *s),    /* Output a text string  */
            MOTenum(char *s, Enum k),      /* Output an enumerated type */
            MOTprcode(Code code);    /* Output a metafile element code */
#else
void CGMOtext();     /* Main Output */
static void MOTpoints(),    /* Output a points list */
            MOTvdc(),       /* Output n VDCs */
            MOTcol(),       /* Output a colour */
            MOTstring(),    /* Output a text string  */
            MOTenum(),      /* Output an enumerated type */
            MOTprcode();    /* Output a metafile element code */
#endif
/*  Local macro */

#define TEXTOUT       (void) fprintf( cgmot,

/*  Table for element decoding */

extern const struct commands cgmelement[];

/*  Local Variables */

static int vp = 4, rp = 4;   /* decimal places for real numbers */
static int indent;         /*  Indent for current element  */

#include "cgmatt.h"

/*static char *func = "CGMotext", mess[40];*/
static char mess[40];

/****************************************************** CGMOtext *******/
void
CGMOtext(FILE *stream, Code c, Long *pi, float *pr, char *str)
{
/*  Outputs Clear Text for MF element 'c' on Output stream 'stream'  */

   static Logical first = TRUE, first_pic = TRUE;
   static Prec loc_prec;
   static Long nx, ny;
   register Long n, i, j, k, num;
   Code major;
   Posint prec;
/*   char s[100];*/

   if (c == (Code) EOF)
   {
     exit(0);
   }

   cgmot = stream;
   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */
         switch (c)
         {
            case NONOP:  /* Ignore Non-Op */
               break;

            case LINE:         /*  Polyline   */
               if ( first ) MOTprcode ( LINE );
               MOTpoints ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) MOTprcode ( DISJTLINE );
               MOTpoints ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) MOTprcode ( MARKER );
               MOTpoints ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case TEXT:         /*  Text   */
               MOTprcode ( TEXT );
               MOTpoints ((Long)1, pi, pr, NOSET);
               MOTenum("notfinal/final", (Enum) num);
               MOTstring ( str );
               break;

            case RESTRTEXT:         /*  Restricted Text */
               MOTprcode ( RESTRTEXT );
               MOTvdc ((Int)2, pi, pr);
               MOTpoints ((Long)1, pi+2, pr+2, NOSET);
               MOTenum("notfinal/final", (Enum) num);
               MOTstring ( str );
               break;

            case APNDTEXT:         /*  Append Text   */
               MOTprcode ( APNDTEXT );
               MOTenum("notfinal/final", (Enum) num);
               MOTstring ( str );
               break;

            case POLYGON:         /*  Polygon   */
               if ( first ) MOTprcode ( POLYGON );
               MOTpoints ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case POLYGONSET:         /*  Polygon Set  */
               if ( first ) MOTprcode ( POLYGONSET );
               MOTpoints ( num, pi, pr, SET);
               first = ( num >= ZERO );
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  MOTprcode ( CELLARRAY );
                  MOTpoints ((Long)3, pi, pr, NOSET);
                  pi += 6;
                  nx = *pi++;
                  ny = *pi++;
                  loc_prec = *pi++;
                  k = ( loc_prec ? ( 1L<<loc_prec ) - 1 : 0 );
                  TEXTOUT ", %ld, %ld, %lu\n", nx, ny, k );
               }
               first = ( num >= ZERO );
               if ( ! first ) num = -num;

/*  set decimal places and number of elements per line 'k' */

               prec = (Prec)((loc_prec ? (Double)loc_prec
                                      : (Double)cur.col_bits )*LOG2) + 1;
               if ( prec < 2 ) prec = 2;
               if ( cur.color_mode == INDEXED ) k = 80L / (prec+1);
               else                             k = 80L / (3*prec+5);
               if ( k > nx ) k = nx;
               n = nx; i = k;

               for ( j = ZERO; j < num ; j++ )
               {
                  if ( j ) /*  Check for line skip after first pass */
                  {
                     if ( cur.color_mode == DIRECT ) TEXTOUT ",");
/*  Line skip if end of row or row count reached */
                     if ( j == n || j == i )
                     {
                        TEXTOUT "\n");
                        if ( j == n )
                        {
                           n += nx; i = j + k;
/* Extra line skip if row is Longer than one line */
                           if ( nx > k ) TEXTOUT "\n");
                        }
                        else  i += k;
                     }
                     else TEXTOUT " ");
                  }
                  if ( cur.color_mode == INDEXED )
                     TEXTOUT "%*ld", (int)prec, *pi++);
                  else
                  {
                     TEXTOUT "%*ld %*ld %*ld",
                 (int)prec, *pi, (int)prec, *(pi+1), (int)prec, *(pi+2));
                     pi += (Long)3;
                  }
               }
               if ( ! first )   /* Not finished yet */
               {
                  TEXTOUT "\n");
                  if ( nx > k ) TEXTOUT "\n");
               }
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               MOTprcode ( GDP );
               TEXTOUT " %ld  ", *pi++);
               MOTpoints (num, pi, pr, ZERO);
               MOTstring ( str );
               break;

            case RECT:         /*  Rectangle   */
               MOTprcode ( RECT );
               MOTpoints ((Long)2, pi, pr, ZERO);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
              break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */
         switch (c)
         {
            case BEGMF:         /*  Begin Metafile   */
               MOTprcode ( BEGMF );
               MOTstring ( str );
               curtext = textdef;
               break;

            case ENDMF:         /*  End Metafile   */
               MOTprcode ( ENDMF );
               break;

            case BEGPIC:         /*  Begin Picture Descriptor   */
               TEXTOUT "\n");
               MOTprcode ( BEGPIC );
               if ( first_pic )
               {
                  first_pic = FALSE;
                  mftext = curtext;  /* Set MF defaults */
               }
               else
                  curtext = mftext;  /* Set Text picture defaults */

               TEXTOUT "'Picture Number ");
               TEXTOUT "%ld'",*pi);
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               MOTprcode ( BEGPICBODY );
               break;

            case ENDPIC:         /*  End  Picture    */
               MOTprcode ( ENDPIC );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:         /*  Metafile version   */
               MOTprcode ( MFVERSION );
               TEXTOUT " %ld", num);
               break;

            case MFDESC:         /*  Metafile Description  */
               MOTprcode ( MFDESC );
               MOTstring ( str );
               break;

            case VDCTYPE:         /*  VDC Type   */
               MOTprcode ( VDCTYPE );
               MOTenum ("integer/real",cur.vdc_type);
               break;

            case INTEGERPREC:         /*  Integer Precision  */
               MOTprcode ( INTEGERPREC );
               curtext.max_int = (Long) (1L<< (cur.int_bits - 1)) -1;
               curtext.min_int = - curtext.max_int - 1;
               TEXTOUT " %ld, %ld", curtext.min_int, curtext.max_int);
               break;

            case REALPREC:         /*  Real Precision   */
               MOTprcode ( REALPREC );
               curtext.max_real = cur.max_real;
               curtext.min_real = cur.min_real;
               curtext.real_digits = (long)(1.0 +
                                  (cur.real_bits - cur.real_places) * LOG2);
               rp =4;
               TEXTOUT " %.*f, %.*f,", rp, (Double)curtext.min_real,
                                       rp,(Double)curtext.max_real);
               TEXTOUT " %ld", curtext.real_digits);
               break;

            case INDEXPREC:         /*  Index Precision   */
               MOTprcode ( INDEXPREC );
               curtext.max_index = (Long) (1L<<(cur.index_bits - 1)) -1;
               curtext.min_index = -curtext.max_index - 1;
               TEXTOUT " %ld, %ld", curtext.min_index, curtext.max_index);
               break;

            case COLRPREC:         /*  Colour Precision  */
               MOTprcode ( COLRPREC );
               curtext.col_prec = (Long) (1L<<cur.col_bits) - 1;
               TEXTOUT " %lu", curtext.col_prec);
               break;

            case COLRINDEXPREC:         /*  Colour Index Precision  */
               MOTprcode ( COLRINDEXPREC );
               curtext.colind_prec = (Long) (1L<<(cur.colind_bits-1)) - 1;
               TEXTOUT " %ld", curtext.colind_prec);
               break;

            case MAXCOLRINDEX:         /*  Maximum Colour Index  */
               MOTprcode ( MAXCOLRINDEX );
               TEXTOUT " %ld", cur.max_colind);
               break;

            case COLRVALUEEXT:         /*  Colour value extent  */
               MOTprcode ( COLRVALUEEXT );
               curtext.min_rgb.red   = cur.min_rgb.red;
               curtext.min_rgb.green = cur.min_rgb.green;
               curtext.min_rgb.blue  = cur.min_rgb.blue;
               curtext.max_rgb.red   = cur.max_rgb.red;
               curtext.max_rgb.green = cur.max_rgb.green;
               curtext.max_rgb.blue  = cur.max_rgb.blue;
               TEXTOUT " %ld %ld %ld,", curtext.min_rgb.red,
                                        curtext.min_rgb.green,
                                        curtext.min_rgb.blue);
               TEXTOUT " %ld %ld %ld", curtext.max_rgb.red,
                                       curtext.max_rgb.green,
                                       curtext.max_rgb.blue);
               break;

            case MFELEMLIST:         /*  Metafile element List  */
            {
/*               Logical endmfdef = FALSE;*/

               MOTprcode ( MFELEMLIST );
               TEXTOUT " '");
               switch ( (Int) *pi )
                  {
                     case ZERO:
                     case 1:
                        MOTenum ("DRAWINGSET/DRAWINGPLUS",(Enum) *pi );
                        break;

                     default:
                        MOTprcode ( (Code) *pi );
                        break;
                  }

               TEXTOUT "'");
               break;
            }

            case BEGMFDEFAULTS:     /*  Begin Metafile defaults Replacement */
            case ENDMFDEFAULTS:    /*  End Metafile defaults Replacement  */
               MOTprcode ( c );
               break;

            case FONTLIST:         /*  Font List   */
               MOTprcode ( FONTLIST );
               for ( j = k = ZERO ; j < num ; )
               {
                  if (j) TEXTOUT ",");
                  k += strlen ( &str[j] ) + 4;
                  if ( k > 70 )
                  {
                     TEXTOUT "\n%9s", " ");
                     k = strlen ( &str[j] ) + 4;
                  }
                  MOTstring ( &str[j] );
                  j = *pi++;
               }
               break;

            case CHARSETLIST:         /*  Character set list  */
               MOTprcode ( CHARSETLIST );
               for ( j = ZERO; j < num ; )
               {
                  MOTenum ( "STD94/STD96/STD94MULTIBYTE/STD96MULTIBYTE/COMPLETECODE", (Enum) *pi++);
                  MOTstring ( &str[j] );
                  j = *pi++;
               }
               break;

            case CHARCODING:         /*  Character coding Announcer  */
               MOTprcode ( CHARCODING );
               MOTenum ("BASIC7BIT/BASIC8BIT/EXTD7BIT/EXTD8BIT",
                                 (Enum) num);
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case SCALEMODE:         /*  Scaling Mode   */
              MOTprcode ( SCALEMODE );
              MOTenum ("abstract/metric", cur.scale_mode);
              TEXTOUT ", %.*f", rp, (Double)cur.scale_factor);
              break;

            case COLRMODE:         /*  Colour Selection Mode */
               MOTprcode ( COLRMODE );
               MOTenum ("indexed/direct", cur.color_mode);
               break;

            case LINEWIDTHMODE:    /*  Line width Specification  */
               MOTprcode ( LINEWIDTHMODE );
               MOTenum ("abs/scaled", cur.linewidth_mode);
               break;

            case MARKERSIZEMODE:   /*  Marker size Specification  */
               MOTprcode ( MARKERSIZEMODE );
               MOTenum ("abs/scaled", cur.markersize_mode);
               break;

            case EDGEWIDTHMODE:    /*  Edge width Specification  */
               MOTprcode ( EDGEWIDTHMODE );
               MOTenum ("abs/scaled", cur.edgewidth_mode);
               break;

            case VDCEXT:         /*  VDC Extent    */
               MOTprcode ( VDCEXT );
               if (cur.vdc_type == REAL)
                  TEXTOUT  " (%.*f,%.*f) (%.*f,%.*f)",
                          vp, (double)cur.vdc_extent.a.x.real,
                          vp, (double)cur.vdc_extent.a.y.real,
                          vp, (double)cur.vdc_extent.b.x.real,
                          vp, (double)cur.vdc_extent.b.y.real );
               else
                  TEXTOUT  " (%ld,%ld) (%ld,%ld)",
                          cur.vdc_extent.a.x.intr,
                          cur.vdc_extent.a.y.intr,
                          cur.vdc_extent.b.x.intr,
                          cur.vdc_extent.b.y.intr );
               break;

            case BACKCOLR:         /*  Background Colour  */
               MOTprcode ( BACKCOLR );
               MOTcol(&cur.back, DIRECT);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x33:  /* Control Elements  */
         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision  */
               MOTprcode ( VDCINTEGERPREC );
               curtext.max_vdc.intr = (Long) (1L<<(cur.vdcint_bits - 1)) -1;
               curtext.min_vdc.intr = (long) (- curtext.max_vdc.intr - 1);
               TEXTOUT " %ld, %ld", (long)curtext.min_vdc.intr,
                                  (long)curtext.max_vdc.intr);
               TEXTOUT " %ld", cur.vdcint_bits);
               break;

            case VDCREALPREC:       /* VDC Real Precision  */
               MOTprcode ( VDCREALPREC );
               curtext.max_vdc.real = cur.max_vdc;
               curtext.min_vdc.real = cur.min_vdc;
               curtext.vdc_digits = (long)(1.0 + (cur.vdc_bits - cur.vdc_places) * LOG2);
               vp = (long)( cur.vdc_places < 0 ? 1.0-cur.vdc_places*LOG2 : 0);
               TEXTOUT " %.*f, %.*f,", vp, (double)curtext.min_vdc.real,
                                       vp, (double)curtext.max_vdc.real);
               TEXTOUT " %ld", curtext.vdc_digits);
               TEXTOUT " %ld", cur.vdc_bits);
               break;

            case AUXCOLR:       /* Auxiliary Colour  */
               MOTprcode ( AUXCOLR );
               MOTcol(&cur.aux, cur.color_mode);
               break;

            case TRANSPARENCY:       /* Transparency  */
               MOTprcode ( TRANSPARENCY );
               MOTenum ("off/on", cur.transparency);
               break;

            case CLIPRECT:       /* Clip Rectangle  */
               MOTprcode ( CLIPRECT );
               if (cur.vdc_type == REAL)
                  TEXTOUT  " (%.*f,%.*f) (%.*f,%.*f)",
                           vp, (double)cur.clip_rect.a.x.real,
                           vp, (double)cur.clip_rect.a.y.real,
                           vp, (double)cur.clip_rect.b.x.real,
                           vp, (double)cur.clip_rect.b.y.real );
               else
                  TEXTOUT  " (%ld,%ld) (%ld,%ld)",
                       cur.clip_rect.a.x.intr, cur.clip_rect.a.y.intr,
                       cur.clip_rect.b.x.intr, cur.clip_rect.b.y.intr );
               break;

            case CLIP:       /* Clip Indicator  */
               MOTprcode ( CLIP );
               MOTenum ("off/on", cur.clip_ind);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x34:  /* Circles and Ellipses  */
         switch(c)
         {
            case CIRCLE:       /* Circle      */
               MOTprcode ( CIRCLE );
               MOTpoints ((Long)1, pi, pr, ZERO);
               MOTvdc ((Int)1, pi+2, pr+2 );
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               MOTprcode ( ARC3PT );
               MOTpoints ((Long)3, pi, pr, ZERO);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               MOTprcode ( ARC3PTCLOSE );
               MOTpoints ((Long)3, pi, pr, ZERO);
               MOTenum ("pie/chord", (Enum) *(pi+6) );
               break;

            case ARCCTR:       /* Circle Arc centre */
               MOTprcode ( ARCCTR );
               MOTpoints ((Long)3, pi, pr, ZERO);
               MOTvdc ((Int)1, pi+6, pr+6);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               MOTprcode ( ARCCTRCLOSE );
               MOTpoints ((Long)3, pi, pr, ZERO);
               MOTvdc((Int)1, pi+6, pr+6);
               MOTenum ("pie/chord", (Enum) *(pi+7) );
               break;

            case ELLIPSE:       /* Ellipse    */
               MOTprcode ( ELLIPSE );
               MOTpoints ((Long)3, pi, pr, ZERO);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               MOTprcode ( ELLIPARC );
               MOTpoints ((Long)5, pi, pr, ZERO);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               MOTprcode ( ELLIPARCCLOSE );
               MOTpoints ((Long)5, pi, pr, ZERO);
               MOTenum ("pie/chord", (Enum) *(pi+10) );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x35:  /* Attributes  */
         switch(c)
         {
            case LINEINDEX:       /*  Line Bundle index  */
               MOTprcode ( LINEINDEX );
               TEXTOUT " %ld", curatt.line_ind);
               break;

            case LINETYPE:       /*  Line Type   */
               MOTprcode ( LINETYPE );
               TEXTOUT " %ld", curatt.line_type);
               break;

            case LINEWIDTH:       /*  Line Width */
               MOTprcode ( LINEWIDTH );
               if (cur.linewidth_mode == SCALED || cur.vdc_type == REAL)
                     TEXTOUT  " %.*f", rp, (double)curatt.line_width.real);
               else
                     TEXTOUT  " %ld", curatt.line_width.intr);
               break;

            case LINECOLR:       /*  Line Colour  */
               MOTprcode ( LINECOLR );
               MOTcol(&curatt.line, cur.color_mode);
               break;

            case MARKERINDEX:       /*  Marker Bundle index  */
               MOTprcode ( MARKERINDEX );
               TEXTOUT " %ld", curatt.mark_ind);
               break;

            case MARKERTYPE:       /*  Marker Type   */
               MOTprcode ( MARKERTYPE );
               TEXTOUT " %ld", curatt.mark_type);
               break;

            case MARKERSIZE:       /*  Marker Size */
               MOTprcode ( MARKERSIZE );
               if (cur.markersize_mode == SCALED || cur.vdc_type == REAL)
                     TEXTOUT  " %.*f", rp, (double)curatt.mark_size.real);
               else
                     TEXTOUT  " %ld", curatt.mark_size.intr);
               break;

            case MARKERCOLR:       /*  Marker Colour  */
               MOTprcode ( MARKERCOLR );
               MOTcol(&curatt.marker, cur.color_mode);
               break;

            case TEXTINDEX:       /*  Text Bundle index  */
               MOTprcode ( TEXTINDEX );
               TEXTOUT " %ld", curatt.text_ind);
               break;

            case TEXTFONTINDEX:       /*  Text Font Index  */
               MOTprcode ( TEXTFONTINDEX );
               TEXTOUT " %ld", curatt.text_font);
               break;

            case TEXTPREC:       /*  Text Precision   */
               MOTprcode ( TEXTPREC );
               MOTenum ("string/char/stroke", curatt.text_prec);
               break;

            case CHAREXPAN:       /*  Character Expansion Factor  */
               MOTprcode ( CHAREXPAN );
               TEXTOUT " %.*f", rp, (double)curatt.char_exp);
               break;

            case CHARSPACE:       /*  Character Spacing  */
               MOTprcode ( CHARSPACE );
               TEXTOUT " %.*f", rp, (double)curatt.char_space);
               break;

            case TEXTCOLR:       /*  Text Colour   */
               MOTprcode ( TEXTCOLR );
               MOTcol(&curatt.text, cur.color_mode);
               break;

            case CHARHEIGHT:       /*  Character Height   */
               MOTprcode ( CHARHEIGHT );
               if ( cur.vdc_type == REAL)
                     TEXTOUT  " %.*f", vp, (double)curatt.char_height.real);
               else
                     TEXTOUT  " %ld", curatt.char_height.intr);
               break;

            case CHARORI:       /*  Character Orientation */
               MOTprcode ( CHARORI );
               if (cur.vdc_type == REAL)
                  TEXTOUT " %.*f %.*f, %.*f %.*f",
                          vp, (double)curatt.char_up.x.real,
                          vp, (double)curatt.char_up.y.real,
                          vp, (double)curatt.char_base.x.real,
                          vp, (double)curatt.char_base.y.real );
               else
                  TEXTOUT " %ld %ld, %ld %ld",
                          curatt.char_up.x.intr,
                          curatt.char_up.y.intr,
                          curatt.char_base.x.intr,
                          curatt.char_base.y.intr );
               break;

            case TEXTPATH:       /*  Text Path */
               MOTprcode ( TEXTPATH );
               MOTenum ("right/left/up/down", curatt.text_path);
               break;

            case TEXTALIGN:       /*  Text Alignment */
               MOTprcode ( TEXTALIGN );
               MOTenum ("normhoriz/left/ctr/right/conthoriz",
                                                curatt.text_halign);
               TEXTOUT ", ");
               MOTenum ("normvert/top/cap/half/base/bottom/contvert",
                                                curatt.text_valign);
               TEXTOUT ",  %.*f, %.*f", rp, (double)curatt.text_hcont,
                                        rp, (double)curatt.text_vcont );
               break;

            case CHARSETINDEX:       /*  Character Set Index */
               MOTprcode ( CHARSETINDEX );
               TEXTOUT " %ld", curatt.char_set);
               break;

            case ALTCHARSETINDEX:   /*  Alternative Character Set Index */
               MOTprcode ( ALTCHARSETINDEX );
               TEXTOUT " %ld", curatt.altchar_set);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         switch(c)
         {
            case FILLINDEX:       /*  Fill Bundle index  */
               MOTprcode ( FILLINDEX );
               TEXTOUT " %ld", curatt.fill_ind);
               break;

            case INTSTYLE:       /*  Interior Style  */
               MOTprcode ( INTSTYLE );
               MOTenum ("hollow/solid/pat/hatch/empty",curatt.int_style);
               break;

            case FILLCOLR:       /*  Fill Colour */
               MOTprcode ( FILLCOLR );
               MOTcol(&curatt.fill, cur.color_mode);
               break;

            case HATCHINDEX:       /*  Hatch Index  */
               MOTprcode ( HATCHINDEX );
               TEXTOUT " %ld", curatt.hatch_ind);
               break;

            case PATINDEX:       /*  Pattern Index  */
               MOTprcode ( PATINDEX );
               TEXTOUT " %ld", curatt.pat_ind);
               break;

            case EDGEINDEX:       /*  Edge Bundle index  */
               MOTprcode ( EDGEINDEX );
               TEXTOUT " %ld", curatt.edge_ind);
               break;

            case EDGETYPE:       /*  Edge Type  */
               MOTprcode ( EDGETYPE );
               TEXTOUT " %ld", curatt.edge_type);
               break;

            case EDGEWIDTH:       /*  Edge Width */
               MOTprcode ( EDGEWIDTH );
               if (cur.edgewidth_mode == SCALED || cur.vdc_type == REAL)
                     TEXTOUT  " %.*f", rp, (double)curatt.edge_width.real);
               else
                     TEXTOUT  " %ld", curatt.edge_width.intr);
               break;

            case EDGECOLR:       /*  Edge Colour  */
               MOTprcode ( EDGECOLR );
               MOTcol(&curatt.edge, cur.color_mode);
               break;

            case EDGEVIS:       /*  Edge Visibility  */
               MOTprcode ( EDGEVIS );
               MOTenum ("off/on",curatt.edge_vis);
               break;

            case FILLREFPT:       /*  Fill Reference Point  */
               MOTprcode ( FILLREFPT );
               if (cur.vdc_type == REAL)
                  TEXTOUT  " (%.*f,%.*f)", vp, (double)curatt.fill_ref.x.real,
                                           vp, (double)curatt.fill_ref.y.real );
               else
                  TEXTOUT  " (%ld,%ld)", curatt.fill_ref.x.intr,
                                       curatt.fill_ref.y.intr );
               break;

            case PATTABLE:       /*  Pattern Table  */
               if ( first )
               {
                  MOTprcode ( PATTABLE );
                  TEXTOUT "  %ld,", *pi++);
                  nx = *pi++;
                  ny = *pi++;
                  loc_prec = *pi++;
                  TEXTOUT " %ld, %ld, %ld\n",
                             nx, ny, (1L<<(loc_prec-1)) -1);
               }
               first = (num >= ZERO );
               if (num < ZERO ) num = -num;
               prec = (unsigned long) (loc_prec * LOG2 + 1);
               if ( prec < 2 ) prec = 2;
               if ( cur.color_mode == INDEXED ) k = 80 / (prec+1);
               else                             k = 80 / (3*prec+5);
               if ( k > nx ) k = nx;
               n = nx; i = k;

               for (j = ZERO; j < num; j++ )
               {
                  if ( j )
                  {
                     if ( j == k || j == n )
                     {
                          TEXTOUT ",\n");
                          k += i;
                          if ( j == n )
                          {
                             n += nx; k = j + i;
                          }
                     }
                     else TEXTOUT ", ");
                  }
                  if ( cur.color_mode == INDEXED )
                     TEXTOUT "%*ld", (int)prec, *pi++);
                  else
                  {
                     TEXTOUT "%*ld %*ld %*ld",
                     (int)prec, *pi, (int)prec, *(pi+1), (int)prec, *(pi+2));
                     pi += 3;
                  }
               }
               break;

            case PATSIZE:       /*  Pattern Size  */
               MOTprcode ( PATSIZE );
               if (cur.vdc_type == REAL)
                  TEXTOUT  " %.*f %.*f, %.*f %.*f",
                          vp, (double)curatt.pat_size.a.x.real,
                          vp, (double)curatt.pat_size.a.y.real,
                          vp, (double)curatt.pat_size.b.x.real,
                          vp, (double)curatt.pat_size.b.y.real );
               else
                  TEXTOUT  " %ld %ld, %ld %ld",
                          curatt.pat_size.a.x.intr,
                          curatt.pat_size.a.y.intr,
                          curatt.pat_size.b.x.intr,
                          curatt.pat_size.b.y.intr );
               break;

            case COLRTABLE:       /*  Colour Table  */
               MOTprcode ( COLRTABLE );
               TEXTOUT " %ld ", *pi++);
               for (j = ZERO; j < num ; j++ )
               {
                  if (j) TEXTOUT ",\n%*s", indent+12," ");
                  TEXTOUT " %ld %ld %ld", *pi, *(pi+1), *(pi+2));
                  pi += 3;
               }
               break;

            case ASF:       /*  Aspect source flags  */
               MOTprcode ( ASF );
               for (n = ZERO ; n < num ; n ++ )
               {
                  if ( n ) TEXTOUT ",\n%*s", indent+3, " ");
                  if ( *pi < ASFS )
                     MOTenum ("linetype/linewidth/linecolr/markertype/markersize/markercolr/textfontindex/textprec/charexp/charspace/textcolr/intstyle/fillcolr/hatchindex/patindex/edgetype/edgewidth/edgecolr", (Enum) *pi++);
/*  charexp should be changed to charexpan */
                  else
                    MOTenum ("alledge/allfill/alltext/allmarker/allline/all",
                               (Enum) (*(pi++) - 506) );
                  MOTenum ("indiv/bundled", (Enum) *pi++);
               }
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
           }
           break;

        case 0x37:  /* Escape And External Elements  */
          switch (c)
          {
            case ESCAPE:         /*  Escape  */
               MOTprcode ( ESCAPE );
               TEXTOUT " %ld ", num);
               MOTstring ( str );
               break;

            case MESSAGE:         /*  Message  */
               MOTprcode ( MESSAGE );
               MOTenum ("noaction /action ", (Enum) num);
               MOTstring ( str );
               break;

            case APPLDATA:         /*  Application Data  */
               MOTprcode ( APPLDATA );
               TEXTOUT " %ld ", num);
               MOTstring ( str );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
          }
          break;

       default:
          (void) sprintf( mess, "(code: 0x%x)", c);
          break;
   }
   if ( first && c != NONOP ) TEXTOUT ";\n");
   return;
}

/******************************************************* MOTpoints *****/
static void
MOTpoints (Long n, Long *pi, float *pr, Enum set)
{
/*  Output 'n' points starting at pi/pr
    'set' indicates if this is a POLYGONSET   */

    register Long i, k, no;
    register int prec;

/*  Set number of points to print on each line  */

    if (cur.vdc_type == REAL)
    {
       prec = vp;
       no = 80 - 2*(prec + 6*set + 5);
    }
    else
    {
       prec = (int) (cur.vdcint_bits*LOG2 + 1);
       no = 80 - 2*(prec + 6*set + 3);
    }

    n = abs(n);
    for (i = 0, k = 10 ; i < n ; i++ )
    {
       if ( k > no )
       {
           TEXTOUT "\n");
           k = 0;
       }
       if (cur.vdc_type == REAL)
       {
          k += fprintf(cgmot, " (%.*f,%.*f)", prec, (double)*pr,
                                             prec, (double)*(pr+1));
          pr += 2;
       }
       else
       {
          k += fprintf(cgmot,  " (%ld,%ld)", *pi, *(pi+1) );
          pi += 2;
       }
       if (set)
       {
          MOTenum("invis/vis/closeinvis/closevis", (Enum) *pi++ );
          k += 10;
       }
    }
    return;
}

/******************************************************* MOTvdc ********/
static void
MOTvdc (int n, Long *pi, Float *pr)
{
/*  Output 'n' VDCs strating at pi/pr  */

    register Long j, k, no;
    register Prec prec;

    if (cur.vdc_type == REAL)
    {
       prec = vp;
       no = 80 / (prec + 5);
    }
    else
    {
       prec = (long) (cur.vdcint_bits*LOG2 + 1);
       no = 80 / (prec + 3);
    }

    for ( j = 0, k = no ; j < n ; j++ )
    {
       if (cur.vdc_type == REAL)
          TEXTOUT " %.*f ", (int)prec, (double)*pr++ );
       else
          TEXTOUT " %ld ", *pi++ );
       if ( !k-- )
       {
          TEXTOUT "\n");
          k = no;
       }
    }
    return;
}

/******************************************************* MOTcol ********/
static void
MOTcol (struct colour *c, Enum type)
{
/*  Output colour 'c' depending on Color_mode 'type'  */

    if (type == DIRECT)
       TEXTOUT " %ld %ld %ld",  c->red, c->green, c->blue);
    else
       TEXTOUT " %ld", c->index);

    return;
}

/******************************************************* MOTstring *****/
static void
MOTstring( char *s )
{
/*  Output text string 's'
    if ' then output ''   */

   register Long i;

   TEXTOUT  " '");
   for ( i = ZERO; i < (int)strlen(s); i++ )
   {
      TEXTOUT  "%c", s[i] );
      if ( s[i] == '\'' ) TEXTOUT "'");
   }
   TEXTOUT  "'");
   return;
}

/******************************************************* MOTenum *******/
static void
MOTenum ( char *s, Enum k )
{
/*   Output enumerated type 'k' using string 's'
     enumerated values are seperated by '/'    */

   register Long i = ZERO, j = ZERO, n = k;

   char s1[16];

/*   Find nth '/'   */

   while ( n-- )
   {
      while ( s[i++] != '/' )
      {
         if ( ! s[i] )  /*  end of string found  */
         {
            (void) sprintf( mess, "(type: %d)", k);
            i = 0;    /* defaults to first value */
            n = 0;    /* exit loop */
         }
      }
   }

/*   Extract string into s1 until next '/' or end of string  */

   while ( (s[i] != '/') && s[i] ) s1[j++] = s[i++];
   s1[j] = '\0';

/*   Print enumerated value  */
TEXTOUT " %s", s1);
return;
}

/******************************************************* MOTprcode *****/
static void
MOTprcode ( Code code )
{
/*   Output Metafile element name 'code ' */

   register int i, group, index, upcase = FALSE;
   static int pos[16] = { 0, 2, 4, 6, 7, 8, 9, 11, 13, 14, 15,
                         -1, -1, -1, -1, -1};
   char c[16];
   group = code & 0xfff0;
   index = code & 0x000f;
   switch ( group )
   {
      case LINE:
         index = pos[index];
         upcase = TRUE;
         break;
      case CIRCLE:
         index += 16;
         upcase = TRUE;
         break;
      case LINEINDEX:
         index += 24;
         break;
      case TEXTINDEX:
         index += 32;
         break;
      case FILLINDEX:
         index += 44;
         break;
      case COLRTABLE:
         index += 57;
         break;
      case VDCINTEGERPREC:
         index += 59;
         break;
      case BEGMF:
         index += 65;
         upcase = TRUE;
         break;
      case MFVERSION:
         index += 70;
         if ( code == BEGMFDEFAULTS || code == ENDMFDEFAULTS ) upcase = TRUE;
         break;
      case SCALEMODE:
         index += 86;
         break;
      case ESCAPE:
         index += 93;
         upcase = TRUE;
         break;
      default:
         index = -1;
         break;
   }

   if ( index >= 0 && code == cgmelement[index].code )
   {
      if ( indent ) TEXTOUT "%*s", indent, " " );

      strcpy(c, cgmelement[index].string);

      if ( ! upcase )
         for ( i = 0; i < (int)strlen(c); i++) c[i] = tolower( c[i] );

      TEXTOUT "%s", c );
      return;
   }

   (void) sprintf(mess, "(code: %x)", code);
   return;
}
