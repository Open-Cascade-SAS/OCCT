#include "cgmout.h"

/*  File stream name  */

extern char cgmroot[];
static FILE *cgmoc;

/*  declare void internal functions  */

# if (defined __STDC__ && __STDC__) || defined __cplusplus
/* use function prototypes, they are requred for ANSI C and C++ compilers */
/* Main Output driver */
void CGMOchar(FILE *stream, Code c, Long *pi, Float *pr, char *pc);
 /* Check Attributes */
static void MOCchkatt(Code type),
/* Conditional output of integer */
            MOCcint(Code code, Int n, Long *var1, Long *var2),
/* Conditional output of Enumerated types */
            MOCcenum(Code code, int n, Enum *var1, Enum *var2),
/* Conditional output of a real */
            MOCcreal(Code code, Float *var1, Float *var2, Enum  r),
/* Conditional output of a colour */
            MOCccol(Code code, int n, struct colour *var1, struct colour *var2),
/* Conditional output of a rectangle */
            MOCrectc(Code code, struct rect *var1, struct rect *var2),
/* Output n VDCs */
            MOCvdc(int n, Long *pi, Float *pr),
/* Output a points list */
            MOCpoints(Long n, Long *pi, Float *pr, Enum set),
/* Output an attribute */
            MOCattrib(Code code),
/* Output a real */
            MOCreal(Double x, Enum type, Prec *ptlist),
/* Output an Integer */
            MOCinteger(Long intval, Logical present, Logical allowed),
/* Output a colour list */
            MOCcells(register Long n, register Long *pi, Enum mode, Prec prec),
/* Output a text string */
            MOCstring(register char *s),
/* Output a direct colour */
            MOCdircol(Posint r, Posint g, Posint b, Prec prec),
/* Check if default colour prec & value are used */
            MOCcoldef(),
/* Output a character to the buffer */
            MOCout(Code hex);
#else
void CGMOchar();           /* Main Output driver */
static void MOCchkatt(),   /* Check Attributes */
            MOCcint(),     /* Conditional output of integer */
            MOCcenum(),    /* Conditional output of Enumerated types */
            MOCcreal(),    /* Conditional output of a real */
            MOCccol(),     /* Conditional output of a colour */
            MOCrectc(),    /* Conditional output of a rectangle */
            MOCvdc(),      /* Output n VDCs */
            MOCpoints(),   /* Output a points list */
            MOCattrib(),   /* Output an attribute */
            MOCreal(),     /* Output a real */
            MOCinteger(),  /* Output an Integer */
            MOCcells(),    /* Output a colour list */
            MOCstring(),   /* Output a text string */
            MOCdircol(),   /* Output a direct colour */
            MOCcoldef(),   /* Check if default colour prec & value are used */
            MOCout();      /* Output a character to the buffer */
#endif

/*  Local Variables */

static Long subchars=0;
static char charsub[CHARSUBNUM];

/*static char *func="CGMochar", mess[40];*/
static char mess[40];

/*  Macros to do common comparisons  */

#define RDIFF(x,y)  ( FABS(x-y) > cur.realmin )
#define VDIFF(x,y)  ( FABS(x-y) > cur.vdcmin )
#define RADIFF(x)   ( FABS(oldatt.x-curatt.x) > cur.realmin )
#define VADIFF(x)   ( FABS(oldatt.x-curatt.x) > cur.vdcmin )

#define ACINT(x,y)    ( MOCcint(x, (Int)1, &oldatt.y, &curatt.y) )
#define ACENUM(x,y)   ( MOCcenum(x, (Int)1, &oldatt.y, &curatt.y) )
#define ACREAL(x,y,z) ( MOCcreal(x, &oldatt.y, &curatt.y, z) )
#define ACCOL(x,y)    ( MOCccol(x, (Int)1, &oldatt.y, &curatt.y) )
#define NEWATTRIB(x)  ( oldatt.x = curatt.x )
#define ATTDIFF(x)    ( oldatt.x != curatt.x )
#define PUTINT(x)     ( MOCinteger( (Long) x, FALSE, FALSE) )
#define PUTREAL(x)  ( MOCreal( (Double)(x), REAL, (Prec *) NULL) )
#define PUTVDC(x)   ( MOCreal( (Double)(x), VDC, (Prec *) NULL) )

/***************************************************** CGMOchar ********/

void
CGMOchar(FILE *stream, Code c, Long *pi, Float *pr, char *pc )
{

/*   Character encoding Output. stream is output channel
                                c is MF element code  */

   static Logical first = TRUE, first_pic = TRUE;
   static Prec loc_prec;
   static Long pic_num = ZERO;
   register Long n, j, num;
   Code major;
   char pcx[STRING_MAX];

   if ( c == (Code) EOF )
   {
   exit(0);
   }

   cgmoc = stream;
   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
            case NONOP:        /* Ignore Non-Op */
               break;

            case LINE:         /*  Polyline   */
               if ( first )
               {
                  MOCchkatt(LINE);
                  MOCout(c);
               }
               first = ( num >= ZERO );
               MOCpoints ( num, pi, pr, NOSET);
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first )
               {
                  MOCchkatt(LINE);
                  MOCout(c);
               }
               first = ( num >= ZERO );
               MOCpoints ( num, pi, pr, NOSET);
               break;

            case MARKER:         /*  Polymarker  */
               if ( first )
               {
                  MOCchkatt(MARKER);
                  MOCout(c);
               }
               first = ( num >= ZERO );
               MOCpoints ( num, pi, pr, NOSET);
               break;

            case TEXT:         /*  Text   */
               MOCchkatt(TEXT);
               MOCout(c);
               MOCvdc ((Int)2, pi, pr);
               PUTINT (num);
               MOCstring (pc);
               break;

            case RESTRTEXT:         /*  Restricted Text */
               MOCchkatt(TEXT);
               MOCout(c);
               MOCvdc( (Int)4, pi, pr);
               PUTINT (num);
               MOCstring (pc);
               break;

            case APNDTEXT:         /*  Append Text   */
               MOCchkatt(TEXT);
               MOCout(c);
               PUTINT (num);
               MOCstring (pc);
               break;

            case POLYGON:         /*  Polygon   */
               if ( first )
               {
                  MOCchkatt(POLYGON);
                  MOCout(c);
               }
               first = ( num >= ZERO );
               MOCpoints ( num, pi, pr, NOSET);
               break;

            case POLYGONSET:         /*  Polygon Set  */
               if ( first )
               {
                  MOCchkatt(POLYGON);
                  MOCout(c);
               }
               first = ( num >= ZERO );
               MOCpoints ( num, pi, pr, SET);
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  MOCout(c);
                  MOCvdc( (Int)6, pi, pr);
                  pi +=6;
                  PUTINT ( *pi++ );
                  PUTINT ( *pi++ );
                  loc_prec = *pi++;
                  if ( num > 0 )
                  {
                     register Long max = ZERO, *ppc = pi;

/*  if within a buffer then find maximum colour  */

                     n = ( cur.color_mode == INDEXED ? num
                                                     : 3*num );
                     for ( j = 0; j < n; j++, ppc++ )
                                 if ( *ppc > max ) max = *ppc;
                     for ( loc_prec = 0; max ; max >>= 1, loc_prec++ )
                     ;
                  }
                  PUTINT ( loc_prec);
               }
               first = ( num >= ZERO );
               MOCcells ( num, pi, cur.color_mode, loc_prec);
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               if ( *pi == -1 ) MOCchkatt(LINE);  /* RAL-GKS arc */
               else             MOCchkatt(POLYGON);
               MOCout(c);
               PUTINT ( *pi++ );
               MOCpoints ( num, pi, pr, ZERO);
               MOCstring (pc);
               break;

            case RECT:         /*  Rectangle   */
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)4, pi, pr);
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch (c)
         {

            case BEGMF:         /*  Begin Metafile   */
               MOCout (c);

/*  Character Substitution */

               MOCout ( (Code)0x7e3e);      /* Tilde           0x7e */

               MOCout ( (Code)0x7e40);      /* Null            0x00*/

               MOCout ( (Code)0x7e4a);      /* Line feed       0x0a */
               MOCout ( (Code)0x7e4d);      /* carriage return 0x0d */

               MOCout (st_term);

/* now set up substitution list ( without NULL ) */
               charsub[subchars++] = 0x7e;
               charsub[subchars++] = 0x0a;
               charsub[subchars++] = 0x0d;

               curchar = chardef;

/*  Set old values to current defaults */

               old = cur;
               oldchar = curchar;
               oldatt = curatt;
               strcpy(pcx, pc);
               if ( strcmp (pcx, "") == 0 )
                  strcpy(pcx,"Generated with CGM DRIVER");
               MOCstring (pcx);
               break;

            case ENDMF:         /*  End Metafile   */
               MOCout (c);
               MOCout( (Code)0 );   /* Starts a new record  */
               break;

            case BEGPIC:          /*  Begin Picture Descriptor   */
               if ( first_pic )
               {
                  MOCcoldef ();
                  mfchar = curchar;
               }
               else
                  curchar = mfchar; /*  set current character defaults */

               first_pic = FALSE;
               old = cur;    /*  reset old settings to MF defaults  */
               oldchar = curchar;
               oldatt = curatt;

               MOCout( (Code)0 );   /*  Start on new record  */
               MOCout (c);

               if ( strcmp(pc, "\0") )  MOCstring (pc);
               else
               {
                   sprintf(pcx, "Picture %ld", pic_num++);
                   MOCstring (pcx);
               }
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               MOCout (c);
               break;

            case ENDPIC:         /*  End  Picture    */
               MOCout (c);
               break;

            default:
               (void) sprintf(mess,"(code 0x%x)",c);
               break;
         }
         break;


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:         /*  Metafile version   */
               MOCout(c);
               PUTINT (num);
               break;

            case MFDESC:         /*  Metafile Description  */
               MOCout(c);
               
                  MOCstring ( pc );
               
               break;

            case VDCTYPE:         /*  VDC Type   */
               MOCcenum (c, (Int)1, &old.vdc_type, &cur.vdc_type);
               break;

            case INTEGERPREC:         /*  Integer Precision  */
               curchar.int_prec = cur.int_bits;
               MOCcint (c, (Int)1, &oldchar.int_prec, &curchar.int_prec);
               break;

            case REALPREC:         /*  Real Precision   */
               curchar.real.prec = cur.real_bits;
               curchar.real.min = cur.real_places;
               curchar.real.defexp = cur.real_defexp;
               if ( curchar.real.defexp > curchar.real.min )
                  curchar.real.defexp = curchar.real.min;
               curchar.real.expald = cur.real_expald;

               MOCcint (c, (Int)4, &oldchar.real.prec, &curchar.real.prec);
               break;

            case INDEXPREC:         /*  Index Precision   */
               curchar.index_prec = cur.index_bits;
               MOCcint(c, (Int)1, &oldchar.index_prec, &curchar.index_prec);
               break;

            case COLRPREC:         /*  Colour Precision  */
               curchar.col_prec = cur.col_bits;
               MOCcint(c, (Int)1, &oldchar.col_prec, &curchar.col_prec);
               break;

            case COLRINDEXPREC:         /*  Colour Index Precision  */
               curchar.colind_prec = cur.colind_bits;
               MOCcint(c, (Int)1, &oldchar.colind_prec, &curchar.colind_prec);
               break;

            case MAXCOLRINDEX:         /*  Maximum Colour Index  */
               MOCcint(c, (Int)1, &old.max_colind, &cur.max_colind);
               break;

            case COLRVALUEEXT:         /*  Colour value extent  */
               MOCcint(COLRPREC, (Int)1, &curchar.col_prec, &cur.col_bits);
               curchar.min_rgb = cur.min_rgb;
               curchar.max_rgb = cur.max_rgb;
               MOCccol ( c, (Int)2, &oldchar.min_rgb, &curchar.min_rgb);
               break;

            case MFELEMLIST:         /*  Metafile element List  */
               MOCout(c);
               MOCout(st_start);
               for (j = ZERO; j < num ; j++, pi++ )
               {
                  switch ( (Int) *pi )
                  {
                     case 0:
                     case 1:
                        PUTINT ( *pi );
                        break;

                     default:
                        MOCout( (Code) *pi );
                        if ( *pi == BEGMFDEFAULTS )
                                    MOCout ( ENDMFDEFAULTS );
                        break;
                  }
               }
               MOCout(st_term);
               break;

            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement*/
               MOCcoldef ();

            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */
               MOCout(c);
               break;

            case FONTLIST:         /*  Font List   */
               MOCout(c);
               for (j = ZERO ; j < num ; )
               {
                  MOCstring ( &pc[j] );
                  j = *pi++;
               }
               break;

            case CHARSETLIST:         /*  Character set list  */
               MOCout(c);
               for ( j = ZERO; j < num ; )
               {
                  Int b1, b2;
                  char c1, chr[2];

                  PUTINT ( *pi++ );
                  sscanf( &pc[j], "%1d%c%d", &b1, &c1, &b2);
                  sprintf( chr, "%c", (b1<<4) + b2);

                  MOCstring ( chr );
                  j = *pi++;
               }
               break;

            case CHARCODING:         /*  Character coding Announcer  */
               MOCout(c);
               PUTINT (num);
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case SCALEMODE:         /*  Scaling Mode   */
               if (old.scale_mode != cur.scale_mode ||
                   RDIFF(old.scale_factor, cur.scale_factor) )
               {
                  MOCout(c);
                  old.scale_mode = cur.scale_mode;
                  old.scale_factor = cur.scale_factor;
                  PUTINT ( cur.scale_mode );
                  PUTREAL ( cur.scale_factor );
               }
               break;

            case COLRMODE:         /*  Colour Selection Mode */
               MOCcenum(c, (Int)1, &old.color_mode, &cur.color_mode);
               break;

            case LINEWIDTHMODE:    /*  Line width Specification  */
               MOCcenum(c, (Int)1, &old.linewidth_mode, &cur.linewidth_mode);
               break;

            case MARKERSIZEMODE:   /*  Marker size Specification  */
               MOCcenum(c, (Int)1, &old.markersize_mode, &cur.markersize_mode);
               break;

            case EDGEWIDTHMODE:     /*  Edge width Specification  */
               MOCcenum(c, (Int)1, &old.edgewidth_mode, &cur.edgewidth_mode);
               break;

            case VDCEXT:         /*  VDC Extent    */
               MOCrectc(c, &old.vdc_extent, &cur.vdc_extent);
               break;

            case BACKCOLR:         /*  Background Colour  */
               MOCout ( c );
               MOCdircol ( cur.back.red, cur.back.green, cur.back.blue,
                           curchar.col_prec);
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
         }
         break;

      case 0x33:  /* Control Elements  */
         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision  */
               MOCcint(c, (Int)1, &oldchar.vdcint_prec, &curchar.vdcint_prec);
               break;

            case VDCREALPREC:       /* VDC Real Precision  */
               curchar.vdc.prec = cur.vdc_bits;
               curchar.vdc.min  = cur.vdc_places;
               curchar.vdc.defexp = cur.vdc_defexp;
               if ( curchar.vdc.defexp > curchar.vdc.min )
                  curchar.vdc.defexp = curchar.vdc.min;
               curchar.vdc.expald = cur.vdc_expald;
               MOCcint(c, (Int)4, &oldchar.vdc.prec, &curchar.vdc.prec);
               break;

            case AUXCOLR:       /* Auxiliary Colour  */
               if (cur.color_mode == DIRECT)
               {
                  MOCccol(c, (Int)1, &old.aux, &cur.aux);
               }
               else
               {
                  MOCcint(c, (Int)1, &old.aux.index, &cur.aux.index);
               }
               break;

            case TRANSPARENCY:       /* Transparency  */
               MOCcenum(c, (Int)1, &old.transparency, &cur.transparency);
               break;

            case CLIPRECT:       /* Clip Rectangle  */
               MOCrectc(c, &old.clip_rect, &cur.clip_rect);
               break;

            case CLIP:       /* Clip Indicator  */
               MOCcenum(c, (Int)1, &old.clip_ind, &cur.clip_ind);
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               break;
         }
         break;

      case 0x34:  /* Circles and Ellipses  */
         switch(c)
         {
            case CIRCLE:       /* Circle      */
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)3, pi, pr);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               MOCchkatt(LINE);
               MOCout(c);
               MOCvdc( (Int)6, pi, pr);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)6, pi, pr);
               PUTINT (*(pi+6));
               break;

            case ARCCTR:       /* Circle Arc centre */
               MOCchkatt(LINE);
               MOCout(c);
               MOCvdc( (Int)7, pi, pr);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)7, pi, pr);
               PUTINT (*(pi+7));
               break;

            case ELLIPSE:       /* Ellipse    */
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)6, pi, pr);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               MOCchkatt(LINE);
               MOCout(c);
               MOCvdc( (Int)10, pi, pr);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               MOCchkatt(POLYGON);
               MOCout(c);
               MOCvdc( (Int)10, pi, pr);
               PUTINT (*(pi+10));
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               break;
         }
         break;

      case 0x35:  /* Attributes  */

         switch(c)
         {
            case LINEINDEX:       /*  Line Bundle index  */
               att.line_index = TRUE;
               break;

            case LINETYPE:       /*  Line Type   */
               att.line_type = TRUE;
               break;

            case LINEWIDTH:       /*  Line Width */
               att.line_width = TRUE;
               break;

            case LINECOLR:       /*  Line Colour  */
               att.line_color = TRUE;
               break;

            case MARKERINDEX:       /*  Marker Bundle index  */
               att.mark_index = TRUE;
               break;

            case MARKERTYPE:       /*  Marker Type   */
               att.mark_type = TRUE;
               break;

            case MARKERSIZE:       /*  Marker Size */
               att.mark_size = TRUE;
               break;

            case MARKERCOLR:       /*  Marker Colour  */
               att.mark_color = TRUE;
               break;

            case TEXTINDEX:       /*  Text Bundle index  */
               att.text_index = TRUE;
               break;

            case TEXTFONTINDEX:       /*  Text Font Index  */
               att.text_font = TRUE;
               break;

            case TEXTPREC:       /*  Text Precision   */
               att.text_prec = TRUE;
               break;

            case CHAREXPAN:       /*  Character Expansion Factor  */
               att.char_expan = TRUE;
               break;

            case CHARSPACE:       /*  Character Spacing  */
               att.char_space = TRUE;
               break;

            case TEXTCOLR:       /*  Text Colour   */
               att.text_color = TRUE;
               break;

            case CHARHEIGHT:       /*  Character Height   */
               att.char_height = TRUE;
               break;

            case CHARORI:       /*  Character Orientation */
               att.char_orient = TRUE;
               break;

            case TEXTPATH:       /*  Text Path */
               att.text_path = TRUE;
               break;

            case TEXTALIGN:       /*  Text Alignment */
               att.text_align = TRUE;
               break;

            case CHARSETINDEX:      /*  Character Set Index */
               att.char_set = TRUE;
               break;

            case ALTCHARSETINDEX:   /*  Alt Character Set Index */
               att.altchar_set = TRUE;
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               return;
         }
         if ( cgmstate == MF_DEFAULTS ) MOCattrib ( c );
         break;

      case 0x36:  /* More Attributes  */
         switch(c)
         {
            case FILLINDEX:       /*  Fill Bundle index  */
               att.fill_index = TRUE;
               break;

            case INTSTYLE:       /*  Interior Style  */
               att.int_style = TRUE;
               break;

            case FILLCOLR:       /*  Fill Colour */
               att.fill_color = TRUE;
               break;

            case HATCHINDEX:       /*  Hatch Index  */
               att.hatch_ind = TRUE;
               break;

            case PATINDEX:       /*  Pattern Index  */
               att.pat_ind = TRUE;
               break;

            case EDGEINDEX:       /*  Edge Bundle index  */
               att.edge_index = TRUE;
               break;

            case EDGETYPE:       /*  Edge Type  */
               att.edge_type = TRUE;
               break;

            case EDGEWIDTH:       /*  Edge Width */
               att.edge_width = TRUE;
               break;

            case EDGECOLR:       /*  Edge Colour  */
               att.edge_color = TRUE;
               break;

            case EDGEVIS:       /*  Edge Visibility  */
               att.edge_vis = TRUE;
               break;

            case FILLREFPT:       /*  Fill Reference Point  */
               att.fill_ref = TRUE;
               break;

            case PATSIZE:       /*  Pattern Size  */
               att.pat_size = TRUE;
               break;

            case PATTABLE:       /*  Pattern Table  */
               if ( first )
               {
                  register Long max = ZERO;
                  MOCout(c);
                  PUTINT ( *pi++ );
                  PUTINT ( *pi++ );
                  PUTINT ( *pi++ );
                  loc_prec = *pi++;
                  if ( num > 0 )
                  {
                     long *ppc=pi;
/*  if within a buffer then find maximum colour  */

                     n = ( cur.color_mode == INDEXED ? num
                                                     : 3*num );
                     for ( j = 0; j < n; j++, ppc++)
                        if ( *ppc > max ) max = *ppc;
                     for ( loc_prec = 0; max ; max >>= 1, loc_prec++ )
                     ;
                  }
                  PUTINT ( loc_prec);
               }
               first = ( num >= ZERO );
               MOCcells ( num, pi, cur.color_mode, loc_prec);
               return;

            case COLRTABLE:       /*  Colour Table  */
               if ( first )
               {
                  MOCout(c);
                  PUTINT ( *pi++ );
               }
               first = ( num >= ZERO );
               MOCcells ( num, pi, DIRECT, (Prec) 0);
               return;

            case ASF:       /*  Aspect source flags  */
            {
               short diff = FALSE, type, value, k, l;

/*  First check if any relevant ASF has changed  */

               for ( n = ZERO; n < ASFS ; n++ )
               {
                  if ( curatt.asf[n] != oldatt.asf[n] ) diff++;
               }
               if ( diff )
               {

/* Output ASFs as they were input - ignoring duplicates */

                  MOCout(c);
                  for ( n = ZERO; n < num ; n++ )
                  {
                     type  = (short) (*pi++);
                     value = (short) (*pi++);
                     if ( type < ASFS )
                     {
                        if ( value != oldatt.asf[type] )
                        {
                           PUTINT ( type );
                           PUTINT ( value );
                           oldatt.asf[type] = value;
                        }
                     }
                     else
                     {
                        PUTINT ( type );
                        PUTINT ( value );
                        switch (type)
                        {
                           case 506:   /*  all edge  */
                              k = 15;
                              l = 17;
                              break;
                           case 507:   /*  all fill  */
                              k = 11;
                              l = 14;
                              break;
                           case 508:   /*  all text  */
                              k =  6;
                              l = 10;
                              break;
                           case 509:   /*  all marker */
                              k =  3;
                              l =  5;
                              break;
                           case 510:   /*  all line */
                              k =  0;
                              l =  2;
                              break;
                           case 511:   /*  all  */
                              k =  0;
                              l = 17;
                              break;
                           default:   /*  ignore */
                              k =  1;
                              l =  0;
                              break;
                        }
                        for ( ; k <= l; k++) oldatt.asf[k] = value;
                     }
                  }
               }
               return;

            }
            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               return;
         }
         if ( cgmstate == MF_DEFAULTS ) MOCattrib ( c );
         break;

      case 0x37:  /* Escape And External Elements  */
         switch (c)
         {
            case ESCAPE:         /*  Escape  */
               MOCout(c);
               PUTINT (num);
               MOCstring (pc);
               break;

            case MESSAGE:         /*  Message  */
               MOCout(c);
               PUTINT (num);
               MOCstring (pc);
               break;

            case APPLDATA:         /*  Application Data  */
               MOCout(c);
               PUTINT (num);
               MOCstring (pc);
               break;

            default:
               (void) sprintf(mess,"(code: 0x%x)",c);
               break;
         }
         break;

      default:
        (void) sprintf(mess,"(code: 0x%x)",c);
        break;
   }

   return;
}

/****************************************************** MOCchkatt ******/
static void
MOCchkatt ( Code type )
{
/*   Check 'type' Attributes and send to Output if changed  */

    Logical bundled, indiv;

    switch (type)
    {
        case LINE:

          bundled = (curatt.asf[0] == BUNDLED
                  || curatt.asf[1] == BUNDLED
                  || curatt.asf[2] == BUNDLED );
          indiv = (curatt.asf[0] == INDIVIDUAL
                || curatt.asf[1] == INDIVIDUAL
                || curatt.asf[2] == INDIVIDUAL );

          if ( bundled && att.line_index)
          {
             ACINT(LINEINDEX, line_ind);
             att.line_index = FALSE;
          }

          if ( indiv && att.line_type)
          {
             ACINT(LINETYPE, line_type);
             att.line_type = FALSE;
          }

          if ( indiv && att.line_width )
          {
             if (cur.linewidth_mode == SCALED)
             {
                ACREAL(LINEWIDTH, line_width.real, REAL);
             }
             else if ( cur.vdc_type == REAL)
             {
                ACREAL(LINEWIDTH, line_width.real, VDC);
             }
             else
             {
                ACINT(LINEWIDTH, line_width.intr);
             }
             att.line_width = FALSE;
         }

         if ( indiv && att.line_color )
         {
            if (cur.color_mode == DIRECT)
            {
               ACCOL (LINECOLR, line );
            }
            else
            {
               ACINT(LINECOLR, line.index);
            }
            att.line_color = FALSE;
          }
          break;

        case MARKER:

          bundled = (curatt.asf[3] == BUNDLED
                  || curatt.asf[4] == BUNDLED
                  || curatt.asf[5] == BUNDLED );
          indiv = (curatt.asf[3] == INDIVIDUAL
                || curatt.asf[4] == INDIVIDUAL
                || curatt.asf[5] == INDIVIDUAL );

          if ( bundled && att.mark_index)
          {
             ACINT(MARKERINDEX, mark_ind);
             att.mark_index = FALSE;
          }

          if ( indiv && att.mark_type)
          {
             ACINT(MARKERTYPE, mark_type);
             att.mark_type = FALSE;
          }

          if ( indiv && att.mark_size )
          {
             if (cur.markersize_mode == SCALED)
             {
                ACREAL(MARKERSIZE, mark_size.real, REAL );
             }
             else if ( cur.vdc_type == REAL)
             {
                ACREAL(MARKERSIZE, mark_size.real, VDC );
             }
             else
             {
                ACINT(MARKERSIZE, mark_size.intr );
             }
             att.mark_size = FALSE;
         }

         if ( indiv && att.mark_color )
         {
            if (cur.color_mode == DIRECT)
            {
               ACCOL (MARKERCOLR, marker );
            }
            else
            {
               ACINT(MARKERCOLR, marker.index );
            }
            att.mark_color = FALSE;
          }
          break;

        case TEXT:

          bundled = (curatt.asf[6] == BUNDLED
                  || curatt.asf[7] == BUNDLED
                  || curatt.asf[8] == BUNDLED
                  || curatt.asf[9] == BUNDLED
                  || curatt.asf[10] == BUNDLED );
          indiv = (curatt.asf[6] == INDIVIDUAL
                || curatt.asf[7] == INDIVIDUAL
                || curatt.asf[8] == INDIVIDUAL
                || curatt.asf[9] == INDIVIDUAL
                || curatt.asf[10] == INDIVIDUAL );

          if ( bundled && att.text_index)
          {
             ACINT(TEXTINDEX, text_ind);
             att.text_index = FALSE;
          }

          if ( indiv && att.text_font)
          {
             ACINT(TEXTFONTINDEX, text_font);
             att.text_font = FALSE;
          }

          if ( indiv && att.text_prec)
          {
             ACENUM(TEXTPREC, text_prec);
             att.text_prec = FALSE;
          }

          if ( indiv && att.char_expan)
          {
             ACREAL(CHAREXPAN, char_exp, REAL );
             att.char_expan = FALSE;
          }

          if ( indiv && att.char_space)
          {
             ACREAL(CHARSPACE, char_space, REAL );
             att.char_space = FALSE;
          }

          if ( indiv && att.text_color )
          {
            if (cur.color_mode == DIRECT)
            {
               ACCOL (TEXTCOLR, text );
            }
            else
            {
               ACINT(TEXTCOLR, text.index );
            }
            att.text_color = FALSE;
          }

          if ( att.char_height)
          {
            if (cur.vdc_type == REAL)
            {
               ACREAL(CHARHEIGHT, char_height.real, VDC );
            }
            else
            {
               ACINT(CHARHEIGHT, char_height.intr );
            }
            att.char_height = FALSE;
          }

          if ( att.char_orient)
          {
            if (cur.vdc_type == REAL)
            {
               if ( VADIFF(char_up.x.real) || VADIFF(char_up.y.real) ||
                    VADIFF(char_base.x.real) || VADIFF(char_base.y.real) )
               {
                  MOCout(CHARORI);
                  NEWATTRIB (char_up.x.real);
                  NEWATTRIB (char_up.y.real);
                  NEWATTRIB (char_base.x.real);
                  NEWATTRIB (char_base.y.real);
                  PUTVDC (curatt.char_up.x.real);
                  PUTVDC (curatt.char_up.y.real);
                  PUTVDC (curatt.char_base.x.real);
                  PUTVDC (curatt.char_base.y.real);
               }
            }
            else
            {
               if ( ATTDIFF(char_up.x.intr)
                  || ATTDIFF(char_up.y.intr)
                  || ATTDIFF(char_base.x.intr)
                  || ATTDIFF(char_base.y.intr) )
               {
                  MOCout(CHARORI);
                  NEWATTRIB (char_up.x.intr);
                  NEWATTRIB (char_up.y.intr);
                  NEWATTRIB (char_base.x.intr);
                  NEWATTRIB (char_base.y.intr);
                  PUTINT (curatt.char_up.x.intr);
                  PUTINT (curatt.char_up.y.intr);
                  PUTINT (curatt.char_base.x.intr);
                  PUTINT (curatt.char_base.y.intr);
               }
            }
            att.char_orient = FALSE;
          }

          if ( att.char_set)
          {
             ACINT(CHARSETINDEX, char_set );
             att.char_set = FALSE;
          }

          if ( att.altchar_set)
          {
             ACINT(ALTCHARSETINDEX, altchar_set );
             att.altchar_set = FALSE;
          }

          if ( att.text_path )
          {
             ACENUM(TEXTPATH, text_path );
             att.text_path = FALSE;
          }

          if ( att.text_align )
          {
             if ( ATTDIFF(text_halign) ||  ATTDIFF(text_valign) ||
                  RADIFF(text_hcont) || RADIFF(text_vcont) )
             {
                MOCout(TEXTALIGN);
                NEWATTRIB (text_halign);
                NEWATTRIB (text_valign);
                PUTINT ( curatt.text_halign);
                PUTINT ( curatt.text_valign);
                NEWATTRIB (text_hcont);
                NEWATTRIB (text_vcont);
                PUTREAL( curatt.text_hcont );
                PUTREAL( curatt.text_vcont );
             }
             att.text_align = FALSE;
          }
          break;

        case POLYGON:  /*  Fill and edge attributes  */

          bundled = (curatt.asf[11] == BUNDLED
                  || curatt.asf[12] == BUNDLED
                  || curatt.asf[13] == BUNDLED
                  || curatt.asf[14] == BUNDLED );
          indiv = (curatt.asf[11] == INDIVIDUAL
                || curatt.asf[12] == INDIVIDUAL
                || curatt.asf[13] == INDIVIDUAL
                || curatt.asf[14] == INDIVIDUAL );

         if ( bundled && att.fill_index)
         {
            ACINT(FILLINDEX, fill_ind);
            att.fill_index = FALSE;
         }

         if ( indiv && att.int_style)
         {
            ACENUM(INTSTYLE, int_style);
            att.int_style = FALSE;
         }

         if ( indiv && att.hatch_ind )
         {
            ACINT(HATCHINDEX, hatch_ind);
            att.hatch_ind = FALSE;
         }

         if ( indiv && att.pat_ind )
         {
            ACINT(PATINDEX, pat_ind);
            att.pat_ind = FALSE;
         }

         if ( indiv && att.fill_color )
         {
            if (cur.color_mode == DIRECT)
            {
              ACCOL (FILLCOLR, fill );
            }
            else
            {
              ACINT(FILLCOLR, fill.index );
            }
            att.fill_color = FALSE;
         }

         if ( att.pat_size)
         {
           if (cur.vdc_type == REAL)
           {
              if ( VADIFF(pat_size.a.x.real) || VADIFF(pat_size.a.y.real) ||
                   VADIFF(pat_size.b.x.real) || VADIFF(pat_size.b.y.real) )
              {
                 MOCout(PATSIZE);
                 NEWATTRIB (pat_size.a.x.real);
                 NEWATTRIB (pat_size.a.y.real);
                 NEWATTRIB (pat_size.b.x.real);
                 NEWATTRIB (pat_size.b.y.real);
                 PUTVDC (curatt.pat_size.a.x.real);
                 PUTVDC (curatt.pat_size.a.y.real);
                 PUTVDC (curatt.pat_size.b.x.real);
                 PUTVDC (curatt.pat_size.b.y.real);
              }
           }
           else
           {
              if ( ATTDIFF(pat_size.a.x.intr) ||
                   ATTDIFF(pat_size.a.y.intr) ||
                   ATTDIFF(pat_size.b.x.intr) ||
                   ATTDIFF(pat_size.b.y.intr) )
              {
                 MOCout(PATSIZE);
                 NEWATTRIB (pat_size.a.x.intr);
                 NEWATTRIB (pat_size.a.y.intr);
                 NEWATTRIB (pat_size.b.x.intr);
                 NEWATTRIB (pat_size.b.y.intr);
                 PUTINT (curatt.pat_size.a.x.intr);
                 PUTINT (curatt.pat_size.a.y.intr);
                 PUTINT (curatt.pat_size.b.x.intr);
                 PUTINT (curatt.pat_size.b.y.intr);
              }
           }
           att.pat_size = FALSE;
         }

        /*   Edge characteristics   */

         bundled = (curatt.asf[15] == BUNDLED
                  || curatt.asf[16] == BUNDLED
                  || curatt.asf[17] == BUNDLED );
         indiv = (curatt.asf[15] == INDIVIDUAL
                || curatt.asf[16] == INDIVIDUAL
                || curatt.asf[17] == INDIVIDUAL );

         if ( bundled && att.edge_index)
         {
            ACINT(EDGEINDEX, edge_ind);
            att.edge_index = FALSE;
         }

         if ( indiv && att.edge_type)
         {
            ACINT(EDGETYPE, edge_type);
            att.edge_type = FALSE;
         }

         if ( indiv && att.edge_width )
         {
            if (cur.edgewidth_mode == SCALED)
            {
               ACREAL(EDGEWIDTH, edge_width.real, REAL );
            }
            else if ( cur.vdc_type == REAL)
            {
               ACREAL(EDGEWIDTH, edge_width.real, VDC );
            }
            else
            {
               ACINT(EDGEWIDTH, edge_width.intr );
            }
            att.edge_width = FALSE;
         }

         if ( indiv && att.edge_color )
         {
            if (cur.color_mode == DIRECT)
            {
              ACCOL (EDGECOLR, edge );
            }
            else
            {
              ACINT(EDGECOLR, edge.index );
            }
            att.edge_color = FALSE;
         }

         if ( att.edge_vis)
         {
            ACENUM(EDGEVIS, edge_vis);
            att.edge_vis = FALSE;
         }

         break;

     default:
        (void) sprintf(mess,"(type: 0x%x)", type);
        break;
   }

   return;
}

/****************************************************** MOCattrib ******/
static void
MOCattrib ( Code code )
{
/*  Outputs attribute code 'code'  */

   MOCout( code );

   switch ( code )
   {
      case LINEINDEX:       /*  Line Bundle index  */
         PUTINT ( curatt.line_ind );
         break;

      case LINETYPE:       /*  Line Type   */
         PUTINT ( curatt.line_type );
         break;

      case LINEWIDTH:       /*  Line Width */
         if ( cur.linewidth_mode == SCALED )
            PUTREAL ( curatt.line_width.real );
         else if ( cur.vdc_type == REAL )
            PUTVDC ( curatt.line_width.real );
         else
            PUTINT ( curatt.line_width.intr );
         break;

      case LINECOLR:       /*  Line Colour  */
         if ( cur.color_mode == DIRECT )
            MOCdircol ( curatt.line.red, curatt.line.green,
                        curatt.line.blue, curchar.col_prec );
         else
            PUTINT ( curatt.line.index );
         break;

      case MARKERINDEX:       /*  Marker Bundle index  */
         PUTINT ( curatt.mark_ind );
         break;

      case MARKERTYPE:       /*  Marker Type   */
         PUTINT ( curatt.mark_type );
         break;

      case MARKERSIZE:       /*  Marker Size */
         if ( cur.markersize_mode == SCALED )
            PUTREAL ( curatt.mark_size.real );
         else if ( cur.vdc_type == REAL )
            PUTVDC ( curatt.mark_size.real );
         else
            PUTINT ( curatt.mark_size.intr );
         break;

      case MARKERCOLR:       /*  Marker Colour  */
         if ( cur.color_mode == DIRECT )
            MOCdircol ( curatt.marker.red, curatt.marker.green,
                        curatt.marker.blue, curchar.col_prec );
         else
            PUTINT ( curatt.marker.index );
         break;

      case TEXTINDEX:       /*  Text Bundle index  */
         PUTINT ( curatt.text_ind );
         break;

      case TEXTFONTINDEX:       /*  Text Font Index  */
         PUTINT ( curatt.text_font );
         break;

      case TEXTPREC:       /*  Text Precision   */
         PUTINT ( curatt.text_prec );
         break;

      case CHAREXPAN:       /*  Character Expansion Factor  */
         PUTREAL ( curatt.char_exp );
         break;

      case CHARSPACE:       /*  Character Spacing  */
         PUTREAL ( curatt.char_space );
         break;

      case TEXTCOLR:       /*  Text Colour   */
         if ( cur.color_mode == DIRECT )
            MOCdircol ( curatt.text.red, curatt.text.green,
                        curatt.text.blue, curchar.col_prec );
         else
            PUTINT ( curatt.text.index );
         break;

      case CHARHEIGHT:       /*  Character Height   */
         if ( cur.vdc_type == REAL )
            PUTVDC ( curatt.char_height.real );
         else
            PUTINT ( curatt.char_height.intr );
         break;

      case CHARORI:       /*  Character Orientation */
         if ( cur.vdc_type == REAL )
         {
            PUTREAL ( curatt.char_up.x.real );
            PUTREAL ( curatt.char_up.y.real );
            PUTREAL ( curatt.char_base.x.real );
            PUTREAL ( curatt.char_base.y.real );
         }
         else
         {
            PUTINT ( curatt.char_up.x.intr );
            PUTINT ( curatt.char_up.y.intr );
            PUTINT ( curatt.char_base.x.intr );
            PUTINT ( curatt.char_base.y.intr );
         }
         break;

      case TEXTPATH:       /*  Text Path */
         PUTINT ( curatt.text_path );
         break;

      case TEXTALIGN:       /*  Text Alignment */
         PUTINT ( curatt.text_halign );
         PUTINT ( curatt.text_valign );
         PUTREAL ( curatt.text_hcont );
         PUTREAL ( curatt.text_vcont );
         break;

      case CHARSETINDEX:      /*  Character Set Index */
         PUTINT ( curatt.char_set );
         break;

      case ALTCHARSETINDEX:   /*  Alt Character Set Index */
         PUTINT ( curatt.altchar_set );
         break;

      case FILLINDEX:       /*  Fill Bundle index  */
         PUTINT ( curatt.fill_ind );
         break;

      case INTSTYLE:       /*  Interior Style  */
         PUTINT ( curatt.int_style );
         break;

      case FILLCOLR:       /*  Fill Colour */
         if ( cur.color_mode == DIRECT )
            MOCdircol ( curatt.fill.red, curatt.fill.green,
                        curatt.fill.blue, curchar.col_prec );
         else
            PUTINT ( curatt.fill.index );
         break;

      case HATCHINDEX:       /*  Hatch Index  */
         PUTINT ( curatt.hatch_ind );
         break;

      case PATINDEX:       /*  Pattern Index  */
         PUTINT ( curatt.pat_ind );
         break;

      case EDGEINDEX:       /*  Edge Bundle index  */
         PUTINT ( curatt.edge_ind );
         break;

      case EDGETYPE:       /*  Edge Type  */
         PUTINT ( curatt.edge_type );
         break;

      case EDGEWIDTH:       /*  Edge Width */
         if ( cur.edgewidth_mode == SCALED )
            PUTREAL ( curatt.edge_width.real );
         else if ( cur.vdc_type == REAL )
            PUTVDC ( curatt.edge_width.real );
         else
            PUTINT ( curatt.edge_width.intr );
         break;

      case EDGECOLR:       /*  Edge Colour  */
         if ( cur.color_mode == DIRECT )
            MOCdircol ( curatt.edge.red, curatt.edge.green,
                        curatt.edge.blue, curchar.col_prec );
         else
            PUTINT ( curatt.edge.index );
         break;

      case EDGEVIS:       /*  Edge Visibility  */
         PUTINT ( curatt.edge_vis );
         break;

      case FILLREFPT:       /*  Fill Reference Point  */
         if ( cur.vdc_type == REAL )
         {
            PUTVDC ( curatt.fill_ref.x.real );
            PUTVDC ( curatt.fill_ref.y.real );
         }
         else
         {
            PUTINT ( curatt.fill_ref.x.intr );
            PUTINT ( curatt.fill_ref.y.intr );
         }
         break;

      case PATSIZE:       /*  Pattern Size  */
         if ( cur.vdc_type == REAL )
         {
            PUTVDC ( curatt.pat_size.a.x.real );
            PUTVDC ( curatt.pat_size.a.y.real );
            PUTVDC ( curatt.pat_size.b.x.real );
            PUTVDC ( curatt.pat_size.b.y.real );
         }
         else
         {
            PUTINT ( curatt.pat_size.a.x.intr );
            PUTINT ( curatt.pat_size.a.y.intr );
            PUTINT ( curatt.pat_size.b.x.intr );
            PUTINT ( curatt.pat_size.b.y.intr );
         }
         break;

      default:
         (void) sprintf(mess,"(type: 0x%x)", code);
         break;
   }

   return;
}

/****************************************************** MOCcint ********/
static void
MOCcint ( Code code, Int n, Long *var1, Long *var2)
{
/*  Conditionally outputs 'code' and 'n' integers starting at *var2
    only if at least one is different      */

    register Long i;
    register Logical diff = FALSE;

    for (i = ZERO; i < n; i++)
       if (var1[i] != var2[i]) diff = TRUE;
    if (diff)
    {
        MOCout(code);
        for (i = ZERO ; i < n ; i++)
        {
           PUTINT ( *var2 );
           *var1++ = *var2++;
        }
    }
    return;
}

/****************************************************** MOCcenum *******/
static void
MOCcenum (Code code, int n, Enum *var1, Enum *var2 )
{
/*  Conditionally outputs 'code' and 'n' short integers starting at *var2
    only if at least one is different      */

    register Long i;
    register Logical diff = FALSE;

    for (i = ZERO; i < n; i++)
       if (var1[i] != var2[i]) diff = TRUE;
    if (diff)
    {
        MOCout(code);
        for (i = ZERO ; i < n ; i++)
        {
           PUTINT ( *var2 );
           *var1++ = *var2++;
        }
    }
    return;
}

/****************************************************** MOCcreal *******/
static void
MOCcreal (Code code, Float *var1, Float *var2, Enum  r)
{
/*  Conditionally outputs 'code' and real *var2 if different from *var1
    using precision specified by r      */

    if ( FABS(*var1-*var2) > (r == REAL ? cur.realmin : cur.vdcmin) )
    {
        MOCout(code);
        MOCreal ((Double)*var2, r, null);
        *var1 = *var2;
    }
    return;
}

/****************************************************** MOCrectc *******/
static void
MOCrectc( Code code, struct rect *var1, struct rect *var2 )
{
/*  Conditionally outputs 'code' and 4 VDCs indicated by *var2
    if different from *var1 using vdc precision   */

   if (cur.vdc_type == INTEGER)
   {
      if ( (*var1).a.x.intr != (*var2).a.x.intr ||
           (*var1).a.y.intr != (*var2).a.y.intr ||
           (*var1).b.x.intr != (*var2).b.x.intr ||
           (*var1).b.y.intr != (*var2).b.y.intr  )
      {
          MOCout(code);
          (*var1).a.x.intr = (*var2).a.x.intr;
          PUTINT ( (*var1).a.x.intr );
          (*var1).a.y.intr = (*var2).a.y.intr;
          PUTINT ( (*var1).a.y.intr );
          (*var1).b.x.intr = (*var2).b.x.intr;
          PUTINT ( (*var1).b.x.intr );
          (*var1).b.y.intr = (*var2).b.y.intr;
          PUTINT ( (*var1).b.y.intr );
      }
   }
   else
   {
      if ( VDIFF( (*var1).a.x.real, (*var2).a.x.real ) ||
           VDIFF( (*var1).a.y.real, (*var2).a.y.real ) ||
           VDIFF( (*var1).b.x.real, (*var2).b.x.real ) ||
           VDIFF( (*var1).b.y.real, (*var2).b.y.real ) )
      {
          MOCout(code);
          (*var1).a.x.real = (*var2).a.x.real;
          PUTVDC ( (*var2).a.x.real );
          (*var1).a.y.real = (*var2).a.y.real;
          PUTVDC ( (*var2).a.y.real );
          (*var1).b.x.real = (*var2).b.x.real;
          PUTVDC ( (*var2).b.x.real );
          (*var1).b.y.real = (*var2).b.y.real;
          PUTVDC ( (*var2).b.y.real );
      }
   }
   return;
}

/****************************************************** MOCccol ********/
static void
MOCccol (Code code, int n, struct colour *var1, struct colour *var2)
{
/*  Conditional output 'code' and colour indicated by *var2
    if different from colour *var1  */

    register int i, j;
    register Logical diff = FALSE;

    for (i = j = ZERO; i < n; i++, j++ )
    {
       if ( (var1[j].red != var2[j].red) ||
            (var1[j].green != var2[j].green) ||
            (var1[j].blue != var2[j].blue) ) diff = TRUE;
    }

    if (diff)
    {
       MOCout(code);
       for (i = j = ZERO; i < n; i++, j++)
       {
          var1[j].red = var2[j].red;
          var1[j].green = var2[j].green;
          var1[j].blue = var2[j].blue;
          MOCdircol (var1[j].red, var1[j].green, var1[j].blue,
                     curchar.col_prec);
       }
    }
    return;
}

/****************************************************** MOCvdc *********/
static void
MOCvdc (int n, Long *pi, Float *pr )
{
/*  Outputs n VDCs starting at pi/pr  */

   register Long j;

   if (cur.vdc_type == REAL)
      for ( j = ZERO; j < n; j++)
         PUTVDC( *pr++ );
   else
      for ( j = ZERO; j < n; j++)
         PUTINT ( *pi++ );

   return;
}

/****************************************************** MOCpoints ******/
static void
MOCpoints(Long n, Long *pi, Float *pr, Enum set )
{
/*  Outputs n points starting at pi/pr
    'set' indicates if this is a Polygon set */

   static Logical first = TRUE;
   static Long ix, iy;
   static Long exp_x, exp_y;
   static Float xx, yy;
   register Long i;

   if ( first )
   {
      exp_x = exp_y = curchar.vdc.defexp;
      xx = yy = 0.0F;
      ix = iy = ZERO;
   }
   if ( n >= ZERO ) first = TRUE;
   else
   {
      first = FALSE;
      n = -n;
   }

   if (cur.vdc_type == REAL)
   {
      for ( i=0; i < n; i++ )
      {
        xx += *pr -= xx;
        MOCreal ( (Double)*pr++, VDC, &exp_x);
        yy += *pr -= yy;
        MOCreal ( (Double)*pr++, VDC, &exp_y);
        if (set) PUTINT ( *pi++ );
      }
   }
   else
   {
      for ( i=0; i < n; i++ )
      {
        ix += *pi -= ix;
        PUTINT ( *pi++ );
        iy += *pi -= iy;
        PUTINT ( *pi++ );
        if ( set)  PUTINT ( *pi++ );
      }
   }
   return;
}

/****************************************************** MOCreal ********/
static void
MOCreal ( Double x, Enum type, Prec *ptlist )
{
/*  Outputs real value 'x', using type 'type'
    pointlist indicates if this is part of a pointslist  */

   register Long def_exp, mantissa, expnt;
   register Double y, realmin, prec;
   register Logical present = ZERO, expald;

   if ( type == REAL )
   {
      def_exp = curchar.real.defexp;
      realmin = (Double) cur.realmin;
      prec = (Double) ( (1L<<curchar.real.prec) - 1);
      prec = (Double)( curchar.real.prec > 0 ? (1L<<curchar.real.prec) - 1
                                          : 1.0/((1L<<-curchar.real.prec)-1) );
      expald = ( curchar.real.expald == ALLOWED );
   }
   else
   {
      def_exp = curchar.vdc.defexp;
      realmin = (Double) cur.vdcmin;
      prec = (Double)( curchar.vdc.prec > 0 ? (1L<<curchar.vdc.prec) - 1
                                            : 1.0/((1L<<-curchar.vdc.prec)-1) );
      expald = ( curchar.vdc.expald == ALLOWED );
   }

   if (ptlist != NULL) def_exp = *ptlist;

   y = (Double) ( x>0 ? x : -x );
   expnt = ZERO;

   if ( y < realmin )
   {
      mantissa = ZERO;
   }
   else
   {
      if ( expald ) /* if Exponent allowed */
      {
         while (y >= prec)
         {
            y /= (Double) 2.0;
            expnt++;
         }
         while (y < prec)
         {
            y *= (Double) 2.0;
            expnt--;
         }
         mantissa = (long)y;

 /*  Strip off trailing zeros   */

         while ( mantissa && !(mantissa & 0x01) )
         {
            mantissa >>= 1;
            expnt++;
         }
         present = (expnt != def_exp);
      }
      else
      {
         while (expnt < def_exp)
         {
            y /= (Double) 2.0;
            expnt++;
         }
         while (expnt > def_exp)
         {
            y *= (Double) 2.0;
            expnt--;
         }
         mantissa = (long) y;
      }

      if ( x < 0.0 )  mantissa = -mantissa;
   }

   MOCinteger ( (Long) mantissa, present, expald);
   if (present) PUTINT ( expnt );

   if ( mantissa && (ptlist != NULL) ) *ptlist = expnt;

   return;
}

/****************************************************** MOCinteger *****/
static void
MOCinteger (Long intval, Logical present, Logical allowed)
{
/*  Output an integer 'intval', 'present' indicates if exponent present
    and 'allowed' if allowed */

   register Long ival;
   register Long i, j;
   Code hex[16];

   ival = abs(intval);

 /*  Strip of bits in 5 bit chunks  */

   for (j = -1; ival > ZERO && j < curchar.int_prec; ival >>= 5)
              hex[++j] = 0x60 | (ival & 0x1F);

 /* if zero or top bit set or Exponent follows and bit set
    then add another byte  */

   if ( (j < ZERO) | ( hex[j] & (1L<<(4-present)) ) |
      ( allowed && (hex[j] & 0x18) ) )     hex[++j] = 0x60;

   if (allowed) hex[j] |= present << 3;   /*  set exponent present bit */
   hex[j] |= (intval++ < ZERO) << 4;      /*  set sign on first byte  */
   hex[0] &= 0x5F;                        /*  set continuation bit off */

 /*  Reverse bits to buffer  */
   for (i = j; i >= ZERO; i--)  MOCout( hex [i] );

   return;
}

/****************************************************** MOCstring ******/
static void
MOCstring (register char *s)
{
/*  Output a text string 'string'
    if CMS translate from EBCDIC to ASCII  */

   register short c;

   MOCout(st_start);

   while ( (c = *s++) != '\0')
     MOCout (c);

   MOCout(st_term);

   return;
}

/****************************************************** MOCcells *******/
static void
MOCcells (register Long n, register Long *pi, Enum mode, Prec prec)
{
/*  Output a Colour list of 'n' cells, starting at pi
    using local precision 'prec' (if zero use current precision)
    and colour mode 'mode'  */

   register Long i, j, k, num;
   static Logical first = TRUE;
   static Long normal_list, bit_stream, run_length, run_bits, colbytes;
   Posint red, green, blue, lastred, lastgreen, lastblue;
   Logical more;
   Long col, last, run, *pi0 = pi;

   more = (n < 0);
   num = ( more ? -n : n);

   if ( first )
   {
      Long bytes, cols;

/* if Prec = 0 the use Current precision */
      if ( !prec ) prec = ( mode == INDEXED ? curchar.colind_prec
                                            : curchar.col_prec );
      colbytes = ( mode == INDEXED ? (prec+5)/6
                                   : (3*prec+5)/6 );

      normal_list = bit_stream = ZERO;
      run_bits = run_length = ZERO;

/*  Work out shortest method of coding list  */

      if ( mode == INDEXED )
      {
         cols = 1;
         for ( i = ZERO, last = *pi, run = 1 ; i < num; )
         {
/*  make sure last value forces an end */

            col = ( ++i == num ? -1 : *pi++ );

            if (col == last) run++;
            else
            {

/*  Work out bytes for run count  */

               for ( j=run, bytes=1; j > 0x0f; bytes++, j>>=5);
               run_length += bytes;
               run_bits += bytes;

/*  Work out bytes for normal colour value  */

               for ( j=last, bytes=1; j > 0x0f; bytes++, j>>=5);
               run_length += bytes;
               run_bits += colbytes;
               normal_list += run*bytes;

               run = 1;
               last = col;
            }
         }
      }
      else  /*  Direct Colour */
      {
         cols = 3;
         lastred = *pi++; lastgreen = *pi++; lastblue = *pi++;
         for ( i = ZERO, run = 1 ; i < num ; )
         {
            red = *pi++ ; green = *pi++ ; blue = *pi++;

            if ( ++i != num && red == lastred && green == lastgreen
                                          && blue == lastblue )
               run++;
            else
            {
               for ( j=run , bytes=1; j > 0x0f; bytes++, j>>=5);
               normal_list += run*colbytes;
               run_length += bytes + colbytes;
               run_bits += colbytes;

               run = 1;
               lastred = red; lastgreen = green; lastblue = blue;
            }
         }
      }

/* work out list lengths */
      bit_stream = (cols * num * prec + 5) / 6;

/*   Work out best coding method   */
     if ( mode == INDEXED && run_bits < run_length ) run_length = run_bits;
     else run_bits = 0;

     if ( run_length < normal_list && run_length < bit_stream )
     {
         run_length = TRUE;  normal_list = FALSE; bit_stream = FALSE;
         i = ( run_bits ? 0x43 : 0x42 );
     }
     else if ( bit_stream < normal_list)
     {
         run_length = FALSE;  normal_list = FALSE; bit_stream = TRUE;
         i = 0x41;
     }
     else
     {
         run_length = FALSE;  normal_list = TRUE; bit_stream = FALSE;
         i = 0x40;
     }
   }

/*  Reset continuation marker */

   if ( first ) MOCout( (Code) i);
   first = ( n >= ZERO );
   pi = pi0;

/*   Now send points to Metafile  */

   if ( run_length )
   {
     if ( mode == INDEXED )
      {
         for ( i = ZERO, last = *pi, run = 1 ; i < num ; )
         {
            pi++;
            col = ( ++i == num ? -1 : *pi );

            if (col == last)  run++;
            else
            {
             if ( run_bits )
               {
                  Code byte;

/*  Run length bitstream */
                  for ( j = 0, k = prec-6; j < colbytes; j++, k-=6 )
                  {
                     byte = ( k >= 0 ? (last>>k & 0x3f) | 0x40
                                     : (last<<(-k) & 0x3f) | 0x40 );
                     MOCout( byte );
                  }
               }
               else  PUTINT ( last );

               PUTINT ( run );
               run = 1;
               last = col;
            }
         }
      }
      else  /*  DIRECT Colour (Run length) */
      {
         lastred = *pi++; lastgreen = *pi++; lastblue = *pi++;
         for ( i = ZERO, run = 1 ; i < num ; i++ )
         {
            last = TRUE;
            red = *pi++; green = *pi++; blue = *pi++;
            if ( i != num && red == lastred && green == lastgreen
                                          && blue == lastblue )
            {
               last = FALSE;
               run++;
            }

            if ( last )
            {
               MOCdircol(lastred, lastgreen, lastblue, prec);
               PUTINT ( run );
               run = 1;
               lastred = red; lastgreen = green; lastblue = blue;
            }
         }
      }
   }
   else if ( normal_list )
   {
      if ( mode == INDEXED )
      {
         for ( i = ZERO ; i < num ; i++ )
         {
             PUTINT ( *pi++ );
         }
      }
      else
         for (i = ZERO; i < num ; i++ )
         {
            red = *pi++; green = *pi++; blue = *pi++;
            MOCdircol ( red, green, blue, prec );
         }
   }
   else
   {
      Code byte;
      int bits;

      if ( mode == DIRECT ) num *= 3;

      for (i = ZERO, bits = 6, byte = 0x40 ; i < num ; i++ )
      {
         col = *pi++;
         for ( j = prec - 1 ; j >= ZERO ; j--)
         {
            byte |= ((col>>j) & 1) <<--bits;
            if ( !bits )
            {
               MOCout (byte);
               bits = 6;
               byte = 0x40;
            }
         }
      }
      if ( bits < 6 ) MOCout(byte);
   }
   return;
}

/****************************************************** MOCdircol ******/
static void
MOCdircol ( Posint r, Posint g, Posint b, Prec prec )
{
/*  Output a direct colour (r, g, b) using precision 'prec'  */

   register Long i;
   register Code c;

   if (prec & 0x01)
   {
      /*   odd number of bits  */

      r <<= 1; g <<= 1; b <<= 1;
   }
   else prec--;

/* if default colour precision convert to 8 bits */
   if ( ! cur.colprec_flag )
   {
      r >>= 2; g >>= 2; b >>= 2;
   }
   for (i = prec; i > ZERO ; i-- )
   {
      c = (short) (0x40 | (((r>>i)&1)<<5) | (((g>>i)&1)<<4) | (((b>>i)&1)<<3));
      i--;
      c = (short) (c    | (((r>>i)&1)<<2) | (((g>>i)&1)<<1) | ((b>>i)&1));
      MOCout( c );
   }
   return;
}

/****************************************************** MOCcoldef ******/
static void
MOCcoldef ()
{
/*  Check if colour precision or Value extent are not set use defaults */

   if ( ! cur.colprec_flag && cur.col_bits != curchar.col_prec )
   {
      curchar.col_prec = cur.col_bits;
      MOCout ( COLRPREC );
      PUTINT ( curchar.col_prec );
   }
   if ( ! cur.colval_flag && ( cur.min_rgb.red != curchar.min_rgb.red
                          ||  cur.min_rgb.green != curchar.min_rgb.green
                          ||  cur.min_rgb.blue != curchar.min_rgb.blue
                          ||  cur.max_rgb.red != curchar.max_rgb.red
                          ||  cur.max_rgb.green != curchar.max_rgb.green
                          ||  cur.max_rgb.blue != curchar.max_rgb.blue ))
   {
      curchar.min_rgb = cur.min_rgb;
      curchar.max_rgb = cur.max_rgb;
      MOCout ( COLRVALUEEXT );
      MOCdircol ( curchar.min_rgb.red,
                  curchar.min_rgb.green,
                  curchar.min_rgb.blue,
                  curchar.col_prec );
      MOCdircol ( curchar.max_rgb.red,
                  curchar.max_rgb.green,
                  curchar.max_rgb.blue,
                  curchar.col_prec );
   }
   return;
}

/****************************************************** MOCout *********/
static void
MOCout ( Code hex )
{
/*   Add character to buffer and Output if buffer is full */

   register Int bits, j;
   register unsigned char c;
   static size_t index = ZERO;
   static unsigned char buffer[BUFF_LEN+1];

   bits = ( (hex & 0xff00) > 0 )*8;

   for (; bits >= 0 ; bits -= 8)
   {
      c = (hex>>bits) & 0xff;

      if ( c <= 0x20 || c >= 0x7e )
         for ( j = 0; j < subchars; j++)
         {
            if ( c == charsub[j] )
            {
               hex ^= c<<bits;

               c = (c > 0x20 ? c - 0x40 : c + 0x40);
               hex |= c<<bits;
               c = 0x7e;
               bits += 8;
               break;
            }
         }
      buffer[index++] = c;

      if( ! hex ) for ( ; index < BUFF_LEN; buffer[index++] = '\0');

      fwrite (buffer, index, (size_t)1, cgmoc);
      fflush (cgmoc);
      index = ZERO;
   }
/*
   register Int bits, j;
   register unsigned char c;
   static size_t index = ZERO;
   static unsigned char buffer[BUFF_LEN+1];

   bits = ( (hex & 0xff00) > 0 )*8;

   for (; bits >= 0 ; bits -= 8)
   {
      c = (hex>>bits) & 0xff;

      if ( c <= 0x20 || c >= 0x7e )
         for ( j = 0; j < subchars; j++)
         {
            if ( c == charsub[j] )
            {
               hex ^= c<<bits;

               c = (c > 0x20 ? c - 0x40 : c + 0x40);
               hex |= c<<bits;
               c = 0x7e;
               bits += 8;
               break;
            }
         }
     buffer[index++] = c;

      if( ! hex ) for ( ; index < BUFF_LEN; buffer[index++] = '\0');

      if (index == BUFF_LEN)
      {
         fwrite (buffer, BUFF_LEN, (size_t)1, cgmoc);
	 fflush (cgmoc);
         index = ZERO;
      }
   }
*/
   return;
}
