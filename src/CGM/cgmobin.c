#include <limits.h>
#include "cgmout.h"

/* File stream name */

static FILE *cgmob;

/* declare internal functions */
# if (defined __STDC__ && __STDC__) || defined __cplusplus
/* use function prototypes, they are requred for ANSI C and C++ compilers */
void CGMObin(FILE *stream, Code c, Long *pi, Float *pr, char *str);
/* Main Output Routine */
static void MOBopcode(Code c, Long len),                         /* Output an Op code */
            MOBint(Long n, Prec prec, Enum sign),                /* Output an Integer */
            MOBreal(Double x, Enum real_type, Enum real_or_vdc), /* Output a real number */
            MOBcolour(struct colour *col, Enum type),            /* Output a colour */
            MOBvdc(Int n, Long *pi, Float *pr),                  /* Output a number of VDCs */
            MOBpointlist(Long n, Long *pi, Float *pr, Enum set), /* Output a Points List */
            MOBclist(register Long num, register Long *col,
                     Prec mode, Enum type, Prec prec),           /* Output a colour list */
            MOBbits(Posint value, Prec prec, Long *bit),         /* Output a value to a bit stream */
            MOBstring(char *s),                                  /* Output a string */
            MOBout(Posint hex, Prec bytes),                      /* Output values to the output buffer */
            MOBcharci(Code c, Int *class, Int *id);              /* Convert Op code value to Class and Id */
#else
void CGMObin();  /* Main Output Routine */
static void MOBopcode(),    /* Output an Op code */
            MOBint(),       /* Output an Integer */
            MOBreal(),      /* Output a real number */
            MOBcolour(),    /* Output a colour */
            MOBvdc(),       /* Output a number of VDCs */
            MOBpointlist(), /* Output a Points List */
            MOBclist(),     /* Output a colour list */
            MOBbits(),      /* Output a value to a bit stream */
            MOBstring(),    /* Output a string */
            MOBout(),       /* Output values to the output buffer */
            MOBcharci();    /* Convert Op code value to Class and Id */
#endif
/*  Local Parameters */

#define ENUMERATED   (Prec) 16

#define PARTITION    (Code) 0xff

#define UNSIGNED     (Enum) 0
#define SIGNED       (Enum) 1

#define FLOATING     (Enum) 0
#define FIXED        (Enum) 1

#define RUNLENGTH    (Enum) 0
#define PACKED       (Enum) 1

#define BYTE         (Prec) 1
#define WORD         (Prec) 2
#define LONGINT      (Prec) 4

/*  Parameter sizes */

#define ENUMSIZE     ( (Long) 2 )
#define INDEXSIZE    ( (Long) curbin.index_prec>>3 )
#define COLINDEXSIZE ( (Long) curbin.colind_prec>>3 )
#define INTSIZE      ( (Long) curbin.int_prec>>3 )
#define REALSIZE     ( (Long) (curbin.real_whole + curbin.real_fraction)>>3 )
#define VDCSIZE      ( (Long) (cur.vdc_type == REAL\
                       ? (curbin.vdc_whole + curbin.vdc_fraction)>>3 \
                       : curbin.vdcint_prec>>3 ) )
#define COLOURSIZE    ( (Long) (cur.color_mode == DIRECT\
                       ? 3*curbin.col_prec>>3 : curbin.colind_prec>>3 ) )
#define STRINGSIZE(x) ( (Long) (256 * strlen(x) / 255 + 1) )

/*  Basic Common Output functions */

#define PUTBYTE(x)    MOBout( (Posint) x, (Prec) 1 )
#define PUTWORD(x)    MOBout( (Posint) x, (Prec) 2 )
#define PUTINT(x)     MOBint( (Long) x, curbin.int_prec, SIGNED )
#define PUTREAL(x)    MOBreal ( (Double)x, curbin.real_type, REAL )
#define PUTINDEX(x)   MOBint( (Long) x, curbin.index_prec, SIGNED )
#define PUTENUM(x)    MOBint( (Long) x, ENUMERATED, SIGNED )
#define PUTINTVDC(x)  MOBint( (Long) x, curbin.vdcint_prec, SIGNED )
#define PUTREALVDC(x) MOBreal( (Double)x, curbin.vdc_type, VDC )

/* Local Variables */

static size_t mobblen = BUFF_LEN,   /* Normal Output Buffer length */
            mobparmlen,           /* Parameter length in bytes */
            mobremainder,         /* Bytes left after partition */
            mobdeflen,            /* MF Defaults replacement length */
            mobindex = 0;         /* Output buffer pointer */
static char mobstdbuff[BUFF_LEN], /* Normal Output buffer */
           *mobbuff = mobstdbuff, /* Buffer origin */
           *mobdefbuff;           /* MF Defaults replacement buffer */

/*static char *func="CGMobin", mess[40];*/
static char mess[40];

/********************************************************* CGMObin *****/
void
CGMObin(FILE *stream, Code c, Long *pi, Float *pr, char *str)
{
   register Long j, n, num;
   Code major;
   Long parmlen = ZERO;
   static Long defindex, saveparmlen;
   static Logical first = TRUE, first_pic = TRUE;
   Int class, id;

   if ( c == (Code) EOF )
   {
      MOBout ( (Posint) 0, (Prec) 0); /* flush output buffer */
      exit(0);
   }

   major=c>>8;
   num = *pi++;

   switch(major)
   {
      case 0x00: /* Graphics Primitives */

         switch(c)
         {
            case NONOP:  /* Ignore Non-Op */
               break;

            case LINE:      /* Polyline */
            case DISJTLINE: /* Disjoint Polyline */
            case MARKER: /* Polymarker */
               parmlen = 2*num*VDCSIZE;
               if ( first ) MOBopcode(c, parmlen);
               else         MOBopcode(PARTITION, parmlen);
               first = ( parmlen >= ZERO );
               MOBpointlist(num, pi, pr, NOSET);
               break;

            case TEXT: /* Text */
               parmlen = 2*VDCSIZE + ENUMSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               MOBvdc( (Int)2, pi, pr);
               PUTENUM ( num );
               MOBstring(str);
               break;

            case RESTRTEXT: /* Restricted Text */
               parmlen = 4*VDCSIZE + ENUMSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               MOBvdc( (Int)4, pi, pr);
               PUTENUM ( num );
               MOBstring(str);
               break;

            case APNDTEXT: /* Append Text */
               parmlen = ENUMSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               PUTENUM ( num );
               MOBstring(str);
               break;

            case POLYGON: /* Polygon */
               parmlen = 2*num*VDCSIZE;
               if ( first ) MOBopcode(c, parmlen);
               else         MOBopcode(PARTITION, parmlen);
               first = ( parmlen >= ZERO );
               MOBpointlist(num, pi, pr, NOSET);
               break;

            case POLYGONSET: /* Polygon Set */
               parmlen = num * ( 2*VDCSIZE + ENUMSIZE );
               if ( first ) MOBopcode(c, parmlen);
               else         MOBopcode(PARTITION, parmlen);
               first = ( parmlen >= ZERO );
               MOBpointlist(num, pi, pr, SET);
               break;

            case CELLARRAY: /* Cell Array */
            {
               register Long *pt = pi, i, k;
               Long red, green, blue, nc, ncol;
               unsigned short run, packed;
               Long last, lastred, lastgreen, lastblue;
               static Long nx, ny;
               static Prec lprec;
               static Enum runlength;

/*  Number of colour cells */

               nc = abs( num );
               ncol = abs(cur.color_mode == DIRECT ? 3*num : num );

               if ( first )
               {

/*  set up basic parameter length */
                  parmlen = 6*VDCSIZE + 3*INTSIZE + ENUMSIZE;
                  pt += 6;    /* skip 3 points */
                  nx = *pt++;     /* Number of columns */
                  ny = *pt++;     /* Number of rows */
                  j  = *pt++;     /* Local precision */

                  if ( j == ZERO || num > 0 )
                  {

/*  Find largest colour value if cell within buffer */

                     for ( i = 0, k = 0; i < ncol; i++, pt++ )
                     {
                        if ( *pt > k ) k = *pt;
                     }
                     j = 0;
                     while ( k )
                     {
                        k >>= 1; j++;
                     }
                  }
                  lprec = ( j <= 1  ?  1
                          : j <= 2  ?  2
                          : j <= 4  ?  4
                          : j <= 8  ?  8
                          : j <= 16 ? 16
                          : j <= 24 ? 24
                                    : 32 );
                  pt = pi + 9;
               }

/*  Find number of bytes for both coding methods */

               run = 0; packed = 0;
               for ( j = 0; j < nc; j += nx )
               {

/*  Make sure row starts on a word boundary  */

                  if ( packed & 1 ) packed++;
                  if ( run & 1 ) run++;

/*  Calculate length of packed list in bytes per row */

                  packed += ( (cur.color_mode == DIRECT ? 3*nx : nx)
                               * lprec + 7 )>>3;

/*  Calculate length of run length in bits */

                  k = 0;
                  if ( cur.color_mode == INDEXED )
                  {
                     last = -1;
                     for ( i = 0; i < nx; i++, pt++ )
                     {
                        if ( *pt != last)
                        {
                           k += curbin.int_prec + lprec;
                           last = *pt;
                        }
                     }
                  }
                  else
                  {
                     lastred = -1; lastgreen = -1; lastblue = -1;
                     for ( i = 0; i < nx; i++ )
                     {
                        red = *pt++; green = *pt++; blue = *pt++;
                        if ( red != lastred || green != lastgreen
                                       || blue != lastblue )
                        {
                           k += curbin.int_prec + 3*lprec;
                           lastred = red;
                           lastgreen = green;
                           lastblue = blue;
                        }
                     }
                  }

/*  Convert runlength to bytes */
                  run += (k + 7) >>3;
               }

               if ( first )
               {

/*  Decide which encoding is smallest  */

                  if ( run < packed )
                  {
                     runlength = RUNLENGTH;
                     parmlen += run;
                  }
                  else
                  {
                     runlength = PACKED;
                     parmlen += packed;
                  }

                  if ( num < 0 ) parmlen = - parmlen;
                  MOBopcode(c, parmlen);

                  MOBvdc ( (Int)6, pi, pr );
                  PUTINT ( nx );
                  PUTINT ( ny );
                  PUTINT ( lprec );

                  pi += 9;
                  PUTENUM ( runlength );
               }
               else
               {
                  parmlen = ( runlength = RUNLENGTH ? run : packed );
                  if ( num < 0 ) parmlen = - parmlen;
                  MOBopcode(PARTITION, parmlen);
               }

               first = ( num >= ZERO );

/*  Output cell array a row at a time */

               for ( j = 0; j < nc; j += nx )
               {
                  MOBclist (nx, pi, cur.color_mode, runlength, lprec);
                  pi += ( cur.color_mode == DIRECT ? 3*nx : nx );
               }
               break;
            }

            case GDP: /* Generalised Drawing Primitive */
               parmlen = 2*INTSIZE + 2*num*VDCSIZE + STRINGSIZE(str);
               if ( first ) MOBopcode(c, parmlen);
               else         MOBopcode(PARTITION, parmlen);
               first = ( parmlen >= ZERO );
               PUTINT ( *pi );
               PUTINT ( num );
               MOBpointlist(num, pi, pr, NOSET);
               MOBstring(str);
               break;

            case RECT: /* Rectangle */
               parmlen = 4*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)4, pi, pr);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch(c)
         {

            case BEGMF: /* Begin Metafile */
               cgmob = stream;
               curbin = bindef;
               parmlen = STRINGSIZE(str);
               MOBopcode(c, parmlen);
               MOBstring(str);
               break;

            case ENDMF: /* End Metafile */
               MOBopcode(c, (Long)0 );
               break;

            case BEGPIC: /* Begin Picture Descriptor */
               parmlen = STRINGSIZE(str);
               MOBopcode(c, parmlen);
               MOBstring(str);
               if ( first_pic )
               {
                  first_pic = FALSE;
                  mfbin = curbin;
               }
               else
                  curbin = mfbin;
               break;

            case BEGPICBODY: /* Begin Picture Body */
               MOBopcode(c, (Long)0 );
               break;

            case ENDPIC: /* End  Picture */
               MOBopcode(c, (Long)0 );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;


      case 0x31: /* Metafile Descriptor Elements */
         switch(c)
         {
            case MFVERSION: /* Metafile version */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               PUTINT ( num );
               break;

            case MFDESC: /* Metafile Description */
               parmlen = STRINGSIZE(str);
               MOBopcode(c, parmlen);
               MOBstring(str);
               break;

            case VDCTYPE: /* VDC Type */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.vdc_type );
               break;

            case INTEGERPREC: /* Integer Precision */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               j = ( cur.int_bits <= 8  ?  8
                   : cur.int_bits <= 16 ? 16
                   : cur.int_bits <= 24 ? 24
                                        :32 );
               PUTINT ( j );
               curbin.int_prec = j;
               break;

            case REALPREC: /* Real Precision */
               parmlen = ENUMSIZE + 2*INTSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curbin.real_type );
               if ( curbin.real_type == FIXED )
               { /* NB: only 16,16 and 32,32 are permitted */
                  curbin.real_whole =
                     (cur.real_bits > 16 || -cur.real_places > 16 ? 32 : 16);
                  curbin.real_fraction = curbin.real_whole;
               }
               PUTINT ( curbin.real_whole );
               PUTINT ( curbin.real_fraction );
               break;

            case INDEXPREC: /* Index Precision */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               curbin.index_prec = ( cur.index_bits <=  8 ?  8
                                   : cur.index_bits <= 16 ? 16
                                   : cur.index_bits <= 24 ? 24
                                                          : 32 );
               PUTINT ( curbin.index_prec );
               break;

            case COLRPREC: /* Colour Precision */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               curbin.col_prec = ( cur.col_bits <=  8 ?  8
                                 : cur.col_bits <= 16 ? 16
                                 : cur.col_bits <= 24 ? 24
                                                      : 32 );
               PUTINT ( curbin.col_prec );
               break;

            case COLRINDEXPREC: /* Colour Index Precision */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               curbin.colind_prec = ( cur.colind_bits <=  8 ?  8
                                    : cur.colind_bits <= 16 ? 16
                                    : cur.colind_bits <= 24 ? 24
                                                            : 32 );
               PUTINT ( curbin.colind_prec );
               break;

            case MAXCOLRINDEX: /* Maximum Colour Index */
               parmlen = COLINDEXSIZE;
               MOBopcode(c, parmlen);
               MOBint (cur.max_colind, curbin.colind_prec, UNSIGNED);
               break;

            case COLRVALUEEXT: /* Colour value extent */
               parmlen = 6 * curbin.col_prec >>3;
               MOBopcode(c, parmlen);
               curbin.min_rgb.red   = cur.min_rgb.red;
               curbin.min_rgb.green = cur.min_rgb.green;
               curbin.min_rgb.blue  = cur.min_rgb.blue;
               MOBcolour ( &curbin.min_rgb, DIRECT );
               curbin.max_rgb.red   = cur.max_rgb.red;
               curbin.max_rgb.green = cur.max_rgb.green;
               curbin.max_rgb.blue  = cur.max_rgb.blue;
               MOBcolour ( &curbin.max_rgb, DIRECT );
               break;

            case MFELEMLIST: /* Metafile element List */
               parmlen = INTSIZE + 2*num*INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINT ( num );
               for(n = 0; n < num; n++, pi++)
               {
                  switch ( (Int)*pi )
                  {
                     case 0:
                     case 1:
                        class = -1; id = (Int)*pi;
                        break;
                     default:
                        MOBcharci( (Code)*pi, &class, &id);
                        break;
                  }
                  PUTINDEX ( class );
                  PUTINDEX ( id );
               }
               break;

            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement */
/*  Save current index value */
               defindex = mobindex;
               saveparmlen = mobparmlen;
               mobparmlen = 0;
/*  Create new buffer  */
               mobblen = ARRAY_MAX;
               mobbuff = mobdefbuff = (char *) malloc( (size_t)mobblen );
               mobindex = 0;
               break;

            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */
/*  reset buffer and index  */
               mobbuff = mobstdbuff;
               mobdeflen = mobindex;
               mobindex = defindex;
               mobblen = BUFF_LEN;
               mobparmlen = saveparmlen;
               MOBopcode(BEGMFDEFAULTS, mobdeflen);

/* copy defaults buffer to output */
               for ( n = 0; n < (int)mobdeflen; n++ )
                     PUTBYTE( mobdefbuff[n] );
               FREE( mobdefbuff );
               break;

            case FONTLIST: /* Font List */
            {
               register Long *pt = pi;

               for ( j=0, parmlen=0; j < num; j = *pt++ )
                                   parmlen += STRINGSIZE( &str[j] );
               MOBopcode(c, parmlen);
               for ( j=0, pt = pi; j < num; j = *pt++ )
                                   MOBstring( &str[j] );
               break;
            }

            case CHARSETLIST: /* Character set list */
            {
               register Long *pt = pi;

               for ( j=0, parmlen=0; j < num; j = *pt++ )
               {
                  parmlen += ENUMSIZE + STRINGSIZE(&str[j]);
                  pt++;
               }
               MOBopcode(c, parmlen);
               for ( j=0, pt = pi; j < num; j = *pt++ )
               {
                  PUTENUM ( *pt++ );
                  MOBstring( &str[j] );
               }
               break;
            }

            case CHARCODING: /* Character coding Announcer */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( num );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x32: /* Picture Descriptor Elements */
         switch(c)
         {
            case SCALEMODE: /* Scaling Mode */
               parmlen = ENUMSIZE + REALSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.scale_mode );
               MOBreal ( (Double)cur.scale_factor, FLOATING, REAL );
               break;

            case COLRMODE: /* Colour Selection Mode */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.color_mode );
               break;

            case LINEWIDTHMODE: /* Line width Specification */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.linewidth_mode );
               break;

            case MARKERSIZEMODE: /* Marker size Specification */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.markersize_mode );
               break;

            case EDGEWIDTHMODE: /* Edge width Specification */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.edgewidth_mode );
               break;

            case VDCEXT: /* VDC Extent */
               parmlen = 4*VDCSIZE;
               MOBopcode(c, parmlen);
               if (cur.vdc_type == REAL)
               {
                 PUTREALVDC ( cur.vdc_extent.a.x.real );
                 PUTREALVDC ( cur.vdc_extent.a.y.real );
                 PUTREALVDC ( cur.vdc_extent.b.x.real );
                 PUTREALVDC ( cur.vdc_extent.b.y.real );
               }
               else
               {
                 PUTINTVDC ( cur.vdc_extent.a.x.intr );
                 PUTINTVDC ( cur.vdc_extent.a.y.intr );
                 PUTINTVDC ( cur.vdc_extent.b.x.intr );
                 PUTINTVDC ( cur.vdc_extent.b.y.intr );
               }
               break;

            case BACKCOLR: /* Background Colour */
               parmlen = 3 * curbin.col_prec >>3;
               MOBopcode(c, parmlen);
               MOBcolour( &cur.back, DIRECT );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
         }
         break;

      case 0x33:  /* Control Elements */
         switch(c)
         {
            case VDCINTEGERPREC: /* VDC Integer Precision */
               parmlen = INTSIZE;
               MOBopcode(c, parmlen);
               curbin.vdcint_prec = ( cur.vdcint_bits <= 8  ?  8
                                    : cur.int_bits <= 16 ? 16
                                    : cur.int_bits <= 24 ? 24
                                                         :32 );
               PUTINT ( curbin.vdcint_prec );
               break;

            case VDCREALPREC: /* VDC Real Precision */
               parmlen = ENUMSIZE + 2*INTSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curbin.vdc_type );
               if ( curbin.real_type == FIXED )
               {  /* NB: only 16,16 or 32,32 permitted */
                  curbin.vdc_whole =
                     (cur.vdc_bits > 16 || -cur.vdc_places > 16 ? 32 : 16);
                  curbin.vdc_fraction = curbin.vdc_whole;
               }
               PUTINT ( curbin.vdc_whole );
               PUTINT ( curbin.vdc_fraction );
               break;

            case AUXCOLR: /* Auxiliary Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &cur.aux, cur.color_mode );
               break;

            case TRANSPARENCY: /* Transparency */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.transparency );
               break;

            case CLIPRECT: /* Clip Rectangle */
               parmlen = 4*VDCSIZE;
               MOBopcode(c, parmlen);
               if ( cur.vdc_type == REAL )
               {
                 PUTREALVDC ( cur.clip_rect.a.x.real );
                 PUTREALVDC ( cur.clip_rect.a.y.real );
                 PUTREALVDC ( cur.clip_rect.b.x.real );
                 PUTREALVDC ( cur.clip_rect.b.y.real );
               }
               else
               {
                 PUTINTVDC ( cur.clip_rect.a.x.intr );
                 PUTINTVDC ( cur.clip_rect.a.y.intr );
                 PUTINTVDC ( cur.clip_rect.b.x.intr );
                 PUTINTVDC ( cur.clip_rect.b.y.intr );
               }
               break;

            case CLIP: /* Clip Indicator */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( cur.clip_ind );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x34: /* Circles and Ellipses */
         switch(c)
         {
            case CIRCLE: /* Circle */
               parmlen = 3*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)3, pi, pr );
               break;

            case ARC3PT: /* Circular Arc  3 point */
               parmlen = 6*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)6, pi, pr );
               break;

            case ARC3PTCLOSE: /* Circular Arc  3 point close */
               parmlen = 6*VDCSIZE + ENUMSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)6, pi, pr );
               PUTENUM ( *(pi+6) );
               break;

            case ARCCTR: /* Circle Arc centre */
               parmlen = 7*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)7, pi, pr );
               break;

            case ARCCTRCLOSE: /* Circle Arc centre close */
               parmlen = 7*VDCSIZE + ENUMSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)7, pi, pr );
               PUTENUM ( *(pi+7) );
               break;

            case ELLIPSE: /* Ellipse */
               parmlen = 6*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)6, pi, pr );
               break;

            case ELLIPARC: /* Elliptical Arc */
               parmlen = 10*VDCSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)10, pi, pr );
               break;

            case ELLIPARCCLOSE: /* Elliptical Arc close */
               parmlen = 10*VDCSIZE + ENUMSIZE;
               MOBopcode(c, parmlen);
               MOBvdc( (Int)10, pi, pr );
               PUTENUM ( *(pi+10) );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x35: /* Attributes */

         switch(c)
         {
            case LINEINDEX: /* Line Bundle index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.line_ind );
               break;

            case LINETYPE: /* Line Type */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.line_type );
               break;

            case LINEWIDTH: /* Line Width */
               if(cur.linewidth_mode == SCALED)
               {
                 parmlen = REALSIZE;
                 MOBopcode(c, parmlen);
                 PUTREAL ( curatt.line_width.real );
               }
               else
               {
                 parmlen = VDCSIZE;
                 MOBopcode(c, parmlen);
                 if ( cur.vdc_type == REAL )
                      PUTREALVDC ( curatt.line_width.real );
                 else PUTINTVDC ( curatt.line_width.intr );
               }
               break;

            case LINECOLR: /* Line Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &curatt.line, cur.color_mode);
               break;

            case MARKERINDEX: /* Marker Bundle index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.mark_ind );
               break;

            case MARKERTYPE: /* Marker Type */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.mark_type );
               break;

            case MARKERSIZE: /* Marker Size */
               if(cur.markersize_mode == SCALED)
               {
                 parmlen = REALSIZE;
                 MOBopcode(c, parmlen);
                 PUTREAL ( curatt.mark_size.real );
               }
               else
               {
                 parmlen = VDCSIZE;
                 MOBopcode(c, parmlen);
                 if ( cur.vdc_type == REAL )
                      PUTREALVDC ( curatt.mark_size.real );
                 else PUTINTVDC ( curatt.mark_size.intr );
               }
               break;

            case MARKERCOLR: /* Marker Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &curatt.marker, cur.color_mode);
               break;

            case TEXTINDEX: /* Text Bundle index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.text_ind );
               break;

            case TEXTFONTINDEX: /* Text Font Index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.text_font );
               break;

            case TEXTPREC: /* Text Precision */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curatt.text_prec );
               break;

            case CHAREXPAN: /* Character Expansion Factor */
               parmlen = REALSIZE;
               MOBopcode(c, parmlen);
               PUTREAL ( curatt.char_exp );
               break;

            case CHARSPACE: /* Character Spacing */
               parmlen = REALSIZE;
               MOBopcode(c, parmlen);
               PUTREAL ( curatt.char_space );
               break;

            case TEXTCOLR: /* Text Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &curatt.text, cur.color_mode);
               break;

            case CHARHEIGHT: /* Character Height */
               parmlen = VDCSIZE;
               MOBopcode(c, parmlen);
               if ( cur.vdc_type == REAL )
                    PUTREALVDC ( curatt.char_height.real );
               else PUTINTVDC ( curatt.char_height.intr );
               break;

            case CHARORI: /* Character Orientation */
               parmlen = 4*VDCSIZE;
               MOBopcode(c, parmlen);
               if ( cur.vdc_type == REAL )
               {
                  PUTREALVDC ( curatt.char_up.x.real );
                  PUTREALVDC ( curatt.char_up.y.real );
                  PUTREALVDC ( curatt.char_base.x.real );
                  PUTREALVDC ( curatt.char_base.y.real );
               }
               else
               {
                  PUTINTVDC (curatt.char_up.x.intr );
                  PUTINTVDC (curatt.char_up.y.intr );
                  PUTINTVDC (curatt.char_base.x.intr );
                  PUTINTVDC (curatt.char_base.y.intr );
               }
               break;

            case TEXTPATH: /* Text Path */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curatt.text_path );
               break;

            case TEXTALIGN:       /*  Text Alignment */
               parmlen = 2*ENUMSIZE + 2*REALSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curatt.text_halign );
               PUTENUM ( curatt.text_valign );
               PUTREAL ( curatt.text_hcont );
               PUTREAL ( curatt.text_vcont );
               break;

            case CHARSETINDEX: /* Character Set Index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.char_set );
               break;

            case ALTCHARSETINDEX: /* Alt Character Set Index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.altchar_set );
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x36: /* More Attributes */
         switch(c)
         {
            case FILLINDEX: /* Fill Bundle index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX ( curatt.fill_ind );
               break;

            case INTSTYLE: /* Interior Style */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curatt.int_style );
               break;

            case FILLCOLR: /* Fill Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &curatt.fill, cur.color_mode);
               break;

            case HATCHINDEX: /* Hatch Index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX(curatt.hatch_ind );
               break;

            case PATINDEX: /* Pattern Index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX(curatt.pat_ind );
               break;

            case EDGEINDEX: /* Edge Bundle index */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX(curatt.edge_ind );
               break;

            case EDGETYPE: /* Edge Type */
               parmlen = INDEXSIZE;
               MOBopcode(c, parmlen);
               PUTINDEX(curatt.edge_type );
               break;

            case EDGEWIDTH: /* Edge Width */
               if(cur.edgewidth_mode == SCALED)
               {
                 parmlen = REALSIZE;
                 MOBopcode(c, parmlen);
                 PUTREAL ( curatt.edge_width.real );
               }
               else
               {
                 parmlen = VDCSIZE;
                 MOBopcode(c, parmlen);
                 if ( cur.vdc_type == REAL )
                      PUTREALVDC ( curatt.edge_width.real );
                 else PUTINTVDC ( curatt.edge_width.intr );
               }
               break;

            case EDGECOLR: /* Edge Colour */
               parmlen = COLOURSIZE;
               MOBopcode(c, parmlen);
               MOBcolour( &curatt.edge, cur.color_mode);
               break;

            case EDGEVIS: /* Edge Visibility */
               parmlen = ENUMSIZE;
               MOBopcode(c, parmlen);
               PUTENUM ( curatt.edge_vis );
               break;

            case FILLREFPT: /* Fill Reference Point */
               parmlen = 2*VDCSIZE;
               MOBopcode(c, parmlen);
               if ( cur.vdc_type == REAL )
               {
                  PUTREALVDC ( curatt.fill_ref.x.real );
                  PUTREALVDC ( curatt.fill_ref.y.real );
               }
               else
               {
                  PUTINTVDC ( curatt.fill_ref.x.intr );
                  PUTINTVDC ( curatt.fill_ref.y.intr );
               }
               break;

            case PATTABLE: /* Pattern Table */
            {
               register Long *pt = pi, patind, i, k;
               Long nx, ny;
               Prec lprec;

               parmlen = INDEXSIZE + 3*INTSIZE;
               patind = *pt++;
               nx = *pt++;
               ny = *pt++;
               pt++;   /* ignore previous local precision */

/*  Find local precision */

               n = (cur.color_mode == DIRECT ? 3*num : num );
               for ( i = 0, k = 0; i < n; i++, pt++ )
               {
                  if ( *pt > k ) k = *pt;
               }
               j = 0;
               while ( k )
               {
                  k >>= 1; j++;
               }
               lprec = ( j <=  1 ?  1
                       : j <=  2 ?  2
                       : j <=  4 ?  4
                       : j <=  8 ?  8
                       : j <= 16 ? 16
                       : j <= 24 ? 24
                                 : 32 );

/* Work out parameter length in bytes  */

               parmlen += ( lprec * (cur.color_mode == DIRECT ? 3*num : num )
                          + 7 )>>3;

               MOBopcode(c, parmlen);

               PUTINDEX ( patind );
               PUTINT ( nx );
               PUTINT ( ny );
               PUTINT ( lprec );

/*  Output whole cell array */

               MOBclist ( num, pi + 4, cur.color_mode, PACKED, lprec);
               break;
            }

            case PATSIZE: /* Pattern Size */
               parmlen = 4*VDCSIZE;
               MOBopcode(c, parmlen);
               if ( cur.vdc_type == REAL )
               {
                  PUTREALVDC ( curatt.pat_size.a.x.real );
                  PUTREALVDC ( curatt.pat_size.a.y.real );
                  PUTREALVDC ( curatt.pat_size.b.x.real );
                  PUTREALVDC ( curatt.pat_size.b.y.real );
               }
               else
               {
                  PUTINTVDC ( curatt.pat_size.a.x.intr );
                  PUTINTVDC ( curatt.pat_size.a.y.intr );
                  PUTINTVDC ( curatt.pat_size.b.x.intr );
                  PUTINTVDC ( curatt.pat_size.b.y.intr );
               }
               break;

            case COLRTABLE: /* Colour Table */
               parmlen = COLINDEXSIZE + ((3*num*curbin.col_prec)>>3);
               MOBopcode(c, parmlen);
               MOBint(*pi++, curbin.colind_prec, UNSIGNED);
               for ( j=0; j < num; j++)
               {
                     MOBint(*pi++, curbin.col_prec, UNSIGNED);
                     MOBint(*pi++, curbin.col_prec, UNSIGNED);
                     MOBint(*pi++, curbin.col_prec, UNSIGNED);
               }
               break;

            case ASF: /* Aspect source flags */
            {
               Long k, l, *pt=pi;
               Int type, value;
               Logical asflag[ASFS];

               for ( j=0; j < ASFS; j++) asflag[j] = FALSE;
               for ( j=0; j < num; j++ )
               {
                  type = *pt++; value = (*pt++) + 1;
                  if ( type < ASFS ) asflag[type] = value;
                  else
                  {
                     switch ( type )
                     {
                        case 506:     /* all edge */
                           k = 15; l = 17; break;
                        case 507:     /* all fill */
                           k = 11; l = 14; break;
                        case 508:     /* all text */
                           k = 6; l = 10; break;
                        case 509:     /* all marker */
                           k = 3; l = 5; break;
                        case 510:     /* all line */
                           k = 0; l = 2; break;
                        case 511:     /* all */
                           k = 0; l = 17; break;
                        default:      /* ignore  */
                           k = 1; l = 0; break;
                     }
                     for ( n = k; n < l; n++ ) asflag[n] = value;
                  }
               }
               for ( j=0; j < ASFS; j++)
                  if ( asflag[j] ) parmlen += 2*ENUMSIZE;
               MOBopcode(c, parmlen);
               for ( j=0; j < ASFS; j++)
                  if ( asflag[j] )
                  {
                     PUTENUM ( j );
                     PUTENUM ( curatt.asf[j] );
                  }
               break;
            }

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               break;
         }
         break;

      case 0x37: /* Escape And External Elements */
         switch(c)
         {
            case ESCAPE: /* Escape */
               parmlen = INTSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               PUTINT ( num );
               MOBstring(str);
               break;

            case MESSAGE: /* Message */
               parmlen = ENUMSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               PUTENUM ( num );
               MOBstring(str);
               break;

            case APPLDATA: /* Application Data */
               parmlen = INTSIZE + STRINGSIZE(str);
               MOBopcode(c, parmlen);
               PUTINT ( num );
               MOBstring(str);
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
   return;
}

/******************************************************** MOBopcode ****/

static void
MOBopcode( Code c, Long len )
{
   Int class, id;
   unsigned Long oneword, plen, remainder;
   Logical part;

/* Pad out last element if necessary */
   while ( mobparmlen && mobparmlen < 0x8000 ) PUTBYTE( ZERO );

   plen = abs(len);
   part = (len < 0);

/*  check for parameter length > 32766 */

   if ( plen > 0x7ffe )
   {
      remainder = plen - 0x7ffe;
      part = TRUE;
      plen = 0x7ffe;
   }
   else remainder = 0;

   if ( c != PARTITION )
   {
      MOBcharci(c, &class, &id);
      oneword = (class<<12) + (id<<5) + ( part || (plen > 0x1e) ? 0x1f : plen );
      PUTWORD ( oneword );
   }

/* Put out parameter length as integer */

   if ( c == PARTITION || plen > 0x1e )
   {
      oneword = ( part ? plen | 0x8000 : plen );
      PUTWORD ( oneword );
   }

   mobparmlen = plen;
   mobremainder = remainder;
   if (mobparmlen & 1) mobparmlen++;
   return;
}

/******************************************************** MOBcharci ****/
static void
MOBcharci( Code c, Int *class, Int *id )
{
/*  Converts Code c to class and Id  */

   Code major = c>>8, minor = c & 0xff;

   *class = -1; *id = 0;
   switch ( major )
   {
      case 0x00:  /* Graphics Primitives */
         if ( minor >= 0x20 && minor <= 0x2A )
         {
            *class = 4; *id = minor - 0x20 + 1;
         }
         else if ( minor == 0 ) *class = ZERO;  /*  Non-Op */
         break;

      case 0x30:  /* Metafile delimiters */
         if ( minor >= 0x20 && minor <= 0x24 )
         {
            *class = 0; *id = minor - 0x20 + 1;
         }
         break;

      case 0x31:  /* Metafile Descriptor Elements */
         if ( minor >= 0x20 && minor <= 0x2b )
         {
            *class = 1; *id = minor - 0x20 + 1;
         }
         else if ( minor > 0x2c && minor <= 0x2f )
         {
            *class = 1; *id = minor - 0x20;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements */
         if ( minor >= 0x20 && minor <= 0x26 )
         {
            *class = 2; *id = minor - 0x20 + 1;
         }
         break;

      case 0x33:  /* Control Elements */
         if ( minor >= 0x20 && minor <= 0x25 )
         {
            *class = 3; *id = minor - 0x20 + 1;
         }
         break;

      case 0x34:  /* More Graphics Primitives */
         if ( minor >= 0x20 && minor <= 0x27 )
         {
            *class = 4; *id = minor - 0x20 + 12;
         }
         break;

      case 0x35:  /* Attributes */
         if ( minor >= 0x20 && minor <= 0x27 )
         {
            *class = 5; *id = minor - 0x20 + 1;
         }
         else if ( minor >= 0x30 && minor <= 0x3b )
         {
            *class = 5; *id = minor - 0x30 + 9;
         }
         break;

      case 0x36:  /* More Attributes */
         if ( minor >= 0x20 && minor <= 0x2c )
         {
            *class = 5; *id = minor - 0x20 + 21;
         }
         else if ( minor >= 0x30 && minor <= 0x31 )
         {
            *class = 5; *id = minor - 0x30 + 34;
         }
         break;

      case 0x37:  /* External elements */
         if ( minor == 0x20 )
         {
            *class = 6; *id = 1;
         }
         else if ( minor >= 0x21 && minor <= 0x22 )
         {
            *class = 7; *id = minor - 0x20;
         }
         break;

      default:
         break;
   }

   if ( *class < 0 )
   {
      (void) sprintf( mess, "(code: 0x%x)", c);
      *class = 0;
   }

   return;
}

/******************************************************** MOBint *******/
static void
MOBint( Long n, Prec prec, Enum sign )
{
/*
   if ( sign == SIGNED && n < 0 )
      n = (-n | (Posint) 1L<<(prec-1));
*/
   MOBout( (Posint) n, prec>>3 );
   return;
}

/******************************************************** MOBcolour ****/
static void
MOBcolour( struct colour *col, Enum type )
{

   if ( type == DIRECT )
   {
      MOBint( (Long) col->red, curbin.col_prec, UNSIGNED );
      MOBint( (Long) col->green, curbin.col_prec, UNSIGNED );
      MOBint( (Long) col->blue, curbin.col_prec, UNSIGNED );
   }
   else
   {
      MOBint( (Long) col->index, curbin.colind_prec, UNSIGNED );
   }

   return;
}

/******************************************************** MOBvdc *******/
static void
MOBvdc( Int n, Long *pi, Float *pr)
{
   Int i;
   for ( i = 0; i < n; i++)
   {
      if ( cur.vdc_type == INTEGER )
      {
         PUTINTVDC ( *pi++ );
      }
      else
      {
         PUTREALVDC ( *pr++ );
      }
   }

   return;
}

/******************************************************** MOBpointlist */
static void
MOBpointlist(Long n, Long *pi, Float *pr, Enum set)
{
   Int i;
   for ( i = 0; i < n; i++)
   {
      if ( cur.vdc_type == INTEGER )
      {
         PUTINTVDC ( *pi++ );
         PUTINTVDC ( *pi++ );
      }
      else
      {
         PUTREALVDC ( *pr++ );
         PUTREALVDC ( *pr++ );
      }
      if ( set ) PUTENUM ( *pi++ );
   }

   return;
}

/******************************************************** MOBreal ******/
static void
MOBreal(Double x, Enum real_type, Enum real_or_vdc )
{
   Posint whole, exponent, fract, neg;
   Prec prec;
   Double f;
   neg = ( x < 0.0 ) << 15;

   if ( real_type == FIXED )
   {
      prec = (real_or_vdc == VDC ? curbin.vdc_whole
                                 : curbin.real_whole);
      whole = (Posint)( neg ? -((floor(x))) : x );
      fract = (Posint)( ( neg ? x + (Double)whole : x - (Double)whole )
                            * ( 1L<<(prec-2) ) * 4.0 );

      if ( prec == 32 )
      {
         PUTWORD( whole>>16 | neg );
         PUTWORD( whole & (Posint)0xffff );
         PUTWORD( fract>>16 );
         PUTWORD( fract & (Posint)0xffff );
      }
      else
      {
         PUTWORD( whole | neg );
         PUTWORD( fract );
      }
   }
   else
   {
/*  IEEE Floating point reals */

      prec = (real_or_vdc == VDC ? curbin.vdc_whole + curbin.vdc_fraction
                                 : curbin.real_whole + curbin.real_fraction);
      prec = (prec == 64 ? 12 : 9);
      f = ( neg ? -x : x );

      if ( f < (Double)(real_or_vdc == VDC ? cur.vdcmin : cur.realmin) )
      {
         exponent = ZERO;
         fract = ZERO;
      }
      else
      {

/*  check if greater than smallest exponent  */

         exponent = ( prec == 12 ? 1023 : 127 );

         if ( f <= 1.0 / (Double) (prec-1) )
         {
            exponent = ZERO;
         }
         else
         {
            while ( f >= 2.0 )
            {
               f /= 2.0;
               exponent++;
            }
            while ( f < 1.0 )
            {
               f *= 2.0;
               exponent--;
            }
         }
         fract = (Long) ( (f - 1.0) * (Double)(1L<<(prec == 12 ? 52 : 23)) );
      }

#if defined(_LP64)
      if ( prec == 12 )
      {
         PUTWORD( (exponent<<4) | neg | (fract>>48) );
         PUTWORD( (fract >> 32) & (Posint)0xffff );
         PUTWORD( (fract >> 16) & (Posint)0xffff );
         PUTWORD( fract & (Posint)0xffff );
      }
      else
#endif
      {
         PUTWORD( (exponent<<7) | neg | (fract>>16) );
         PUTWORD( fract & (Posint)0xffff );
      }
   }
   return;
}

/******************************************************** MOBstring ****/
static void
MOBstring( char *s )
{
/*  Output a text string if CMS translate from EBCDIC to ASCII  */

   register Int i, len, slen = strlen(s);
   register char chr;
   len = slen;
   do
   {
      PUTBYTE ( len >= 0xff ? 0xff : len );
      len -= 0xff;
   } while ( len >= 0 );

   for (i = 0; i < slen; i++)
   {
   chr = *s++;
   PUTBYTE ( chr );
   }
return;
}

/******************************************************** MOBclist *****/
static void
MOBclist( register Long num, register Long *col,
          Prec mode, Enum type, Prec prec )
{
/*  Output a colour list */

   register Long j, k, n;
   Long bits, bytes = 0;
   if ( type == RUNLENGTH )
   {
      Long run = 1, bit = 0;

      if ( mode == DIRECT )
      {
         Posint red, green, blue, lastred, lastgreen, lastblue;

         lastred = (*col++);
         lastgreen = (*col++);
         lastblue = (*col++);
         for ( j = 1; j <= num; j++ )
         {
            red = (*col++); green = (*col++); blue = (*col++);
            if ( j == num ||
                 red != lastred || green != lastgreen || blue != lastblue )
            {
               MOBbits ( run, curbin.int_prec, &bit );
               MOBbits ( lastred, prec, &bit );
               MOBbits ( lastgreen, prec, &bit );
               MOBbits ( lastblue, prec, &bit );
               if ( j == num ) break;
               lastred = red; lastgreen = green; lastblue = blue;
               run = 1;
            }
            else run++;
         }
      }
      else   /* Indexed runlength */
      {
         Long lastcol;

         lastcol = (*col++);
         for ( j = 1; j <= num; j++, col++ )
         {
            if ( j == num || *col != lastcol )
            {
               MOBbits ( run, curbin.int_prec, &bit );
               MOBbits ( lastcol, prec, &bit );
               if ( j == num ) break;
               lastcol = *col;
               run = 1;
            }
            else run++;
         }
      }

/* make sure list ends on a word boundary */

      if ( bit ) MOBbits ( (Posint) 0 , (Prec) 16, &bit );

   }
   else   /* Packed mode */
   {
      n = ( mode == DIRECT ? 3*num : num );
      bytes = ( n*prec + 7 ) >>3;

      for ( j = 0, bits = 0, k = 0; j < n; j++)
      {
         if ( bits ) k <<= prec;
         else        k = 0;
         k += (*col++);
         bits += prec;

         while ( bits >= 8 )
         {
            bits -= 8;
            PUTBYTE ( (k>>bits) & 0xff );
         }
      }
      if ( bits > 0 ) PUTBYTE ( k<<(8-bits) );

/* Pad buffer if necessary */
      if ( bytes & 1 ) PUTBYTE ( 0 );
   }
   return;
}

/******************************************************** MOBbits ******/
static void
MOBbits ( Posint value, Prec prec, Long *bit )
{
/*  Output 'value' as 'prec' bits
    using 'word' to hold current value
    returns number of bits left in 'bits'  */

   static Posint oneword = 0;
   Posint mask = (Posint)0xffffffff;
   *bit += prec;
   if ( *bit <= 16 )
   {
      oneword |= value<<(16 - *bit);
   }
   else
   {
      while ( *bit > 16 )
      {
         *bit -= 16;
/*       k = prec + *bit - 16; */
         oneword |= value>>*bit;
         value &= mask>>(32 - *bit);
         MOBout ( oneword, WORD );
         oneword = value<<16;
      }
      oneword = value<<(16 - *bit);
   }
   return;
}

/******************************************************** MOBout *******/
static void
MOBout( Posint hex, Prec bytes )
{
/*  Output 'hex' as 'bytes' bytes to output file 'cgmob'
    which is either the file or MF defaults buffer */

   register Int /*i,*/ b_len;
   b_len = bytes;
   mobindex = ZERO;
   if ( bytes > 0 ) for( ; bytes ; mobindex++ )
   {
      bytes--;
      mobbuff[mobindex] = (char)((hex >> (8*bytes) ) & 0xff);
      mobparmlen--;
   }
   fwrite(mobbuff,b_len,(size_t)1,cgmob);
/*
start:
   if ( bytes > 0 ) for( ; bytes ; mobindex++ )
   {
      if ( mobindex == mobblen )
      {
         if ( cgmstate != MF_DEFAULTS )
         {
            fwrite(mobbuff,mobblen,(size_t)1,cgmob);
            mobindex=ZERO;
         }
         else
         {
            mobblen += BUFF_LEN;
            mobbuff = (char *)realloc (mobbuff, (size_t)mobblen);
         }
      }
      if ( mobindex == ZERO )
      {
         for(i = ZERO; i < mobblen ; i++) mobbuff[i] = '\0';
      }
      bytes--;
      mobbuff[mobindex] = ( hex>>(8*bytes) ) & 0xff;
      mobparmlen--;
   }
   else
   {
      fwrite(mobbuff,BUFF_LEN,(size_t)1,cgmob);
      mobindex = ZERO;
   }
   fflush(cgmob);
   if ( mobremainder && mobparmlen == 0 )
   {
      if ( mobremainder > 0x7ffe )
      {
         hex = 0xfffe;
         mobremainder -= 0x7ffe;
      }
      else
      {
         hex = mobremainder;
         mobremainder = 0;
      }
      mobparmlen = hex & 0x7fff;
      bytes = 2;
      mobparmlen += 2;
      goto start;
   }
*/
   return;
}
