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


/***********************************************************************

     FONCTION :
     ----------
        Classe GUI_WidgetTools :

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      ??-??-?? : ??? ; Creation.
      19-01-98 : GG ; Xw_error_handler()
                      Ignorer les messages d'erreur en provenance de
                      XConfigureWindow (arrive souvent avec MOTIF).

************************************************************************/

#include <Xw_Extension.h>
#include <X11/Xproto.h>
#include <X11/Xmu/Xmu.h>


        /* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_TRACE
#endif

#define MAXERROR 8
#define MAXERRORSTRING 512

static int Nerror = 0 ;
static int Lstring = 0 ;
static char ErrorString[MAXERRORSTRING] ;
static char *PErrorString ;
static char *ErrorMessag[MAXERROR] ;
static char *ErrorRoutine[MAXERROR] ;
static int ErrorCount[MAXERROR] ;
static int ErrorCode[MAXERROR] ;
static int ErrorGravity[MAXERROR] ;
static int TraceLevel = 0 ;
static int ErrorLevel = 4 ;
static int TestMask = 0 ;

/*
   void Xw_set_trace (tracelevel,errorlevel):
   int tracelevel   ;      
   int errorlevel   ;        

        ENABLE/DISABLE Xw package Trace or Error printing 

                  TraceLevel can have the value :

                        0: Disable trace printing       (DEFAULT)
                        1: Print All first level routines calls tiny
                        2: Print routines calls offen but with a low amount of
                                                                trace info .
                        3: Print routines calls offen with an amount of infos .

                  NOTE than XSynchronize is call depending of the Trace level

                  ErrorLevel can have the value :

                        0: Disable Error printing 
                        1: Print any Errors or Warnings (DEFAULT)
                        2: Print high and low gravity errors
                        3: Print only high gravity errors
                        4: Print only Unimplemented errors

===============================================================================

        char* Xw_get_error(error,gravity)
        int *error ;
        int *gravity ;
  
        Returns the last Error Code,Gravity and Messag found if any

        Returns Messag string if any Error has been found
        Returns NULL if No Error has been found

===============================================================================

        STATUS Xw_print_error()
 
        Print Error Stack

        Returns XW_ERROR if no erreur exist
        Returns XW_SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
void Xw_set_trace(int tracelevel,int errorlevel)
#else
void Xw_set_trace(tracelevel,errorlevel)
int tracelevel ;
int errorlevel ;
#endif /*XW_PROTOTYPE*/
{

        TraceLevel = tracelevel ;
        ErrorLevel = errorlevel ;

        if( Xw_get_env("Xw_TEST",ErrorString,sizeof(ErrorString)) ){
            if( strlen(ErrorString) ) {
                sscanf(ErrorString,"%x",&TestMask) ;
            }
            printf(" Xw_TEST is %x\n",TestMask) ;
        }

        if( Xw_get_env("Xw_SET_TRACE",ErrorString,sizeof(ErrorString)) ){
            if( strlen(ErrorString) ) {
                sscanf(ErrorString,"%d %d",&TraceLevel,&ErrorLevel) ;
            }
            printf(" Xw_SET_TRACE is %d %d\n",TraceLevel,ErrorLevel) ;
        }

        Xw_set_synchronize(NULL,TraceLevel) ;
}

#ifdef XW_PROTOTYPE
int Xw_test(unsigned long mask)
#else
int Xw_test(mask)
unsigned long mask;
#endif /*XW_PROTOTYPE*/
/*
        Returns TRUE if mask & TestMask is != 0
*/
{
        if( mask & TestMask ) return (True) ;
        else return (False) ;
}

#ifdef XW_PROTOTYPE
int Xw_get_trace()
#else
int Xw_get_trace()
#endif /*XW_PROTOTYPE*/
/*
        Returns the Trace level
*/
{
        return (TraceLevel) ;
}

/*
   void Xw_set_error(code,routine,param):
   int code                     Error Number
   char *routine                Routine name from error occurs
   void *param                  Associated Error value

        Set Error Message And Print eventually depending of Error trace level .

        Returns NONE

*/

#ifdef XW_PROTOTYPE
void Xw_set_error(int code,const char *routine,void* param)
#else
void Xw_set_error(code,routine,param)
int code ;
const char *routine ;
void *param ;
#endif /*XW_PROTOTYPE*/
{
union {
    void *pv ;
    char *pc ;
    short *ps ;
    int *pi ;
    long *pl ;
    float *pf ;
} value ;
char error[512] ;
int gravity ;

    value.pv = param ;

    switch (code) {
        case 1 :
            gravity = 1 ;
            sprintf(error,"Bad Color Index %d",*value.pi) ;
            break ;
        case 2 :
            gravity = 3 ;
            sprintf(error,"Colormap creation failed") ;
            break ;
        case 3 :
            gravity = 3 ;
            sprintf(error,"USER Colormap %lx allocation failed",(long ) *value.pl) ;
            break ;
        case 4 :
            gravity = 3 ;
            sprintf(error,"STD Colormap %lx allocation failed",(long ) *value.pl) ;
            break ;
        case 5 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Visual class %d",*value.pi) ;
            break ;
        case 6 :
            gravity = 3 ;
            sprintf(error,"EXT_COLORMAP allocation failed") ;
            break ;
        case 7 :
            gravity = 1 ;
            sprintf(error,"Bad Font Index %d",*value.pi) ;
            break ;
        case 8 :
            gravity = 1 ;
            if( value.pc ) sprintf(error,"Bad Font Name '%s'",value.pc) ;
            else           sprintf(error,"Bad Font Name address") ;
            break ;
        case 9 :
            gravity = 3 ;
            sprintf(error,"EXT_FONTMAP allocation failed") ;
            break ;
        case 10 :
            gravity = 1 ;
            sprintf(error,"Bad Marker Index %d",*value.pi) ;
            break ;
        case 11 :
            gravity = 2 ;
            sprintf(error,"Marker Allocation failed") ;
            break ;
        case 12 :
            gravity = 3 ;
            sprintf(error,"EXT_MARKMAP allocation failed") ;
            break ;
        case 13 :
            gravity = 1 ;
            sprintf(error,"Bad Tile Index %d",*value.pi) ;
            break ;
        case 14 :
            gravity = 1 ;
            sprintf(error,"Bad Tile Index %d Size",*value.pi) ;
            break ;
        case 15 :
            gravity = 1 ;
            sprintf(error,"Bad Tile %d Data",*value.pi) ;
            break ;
        case 16 :
            gravity = 2 ;
            sprintf(error,"Tile Bitmap Allocation failed") ;
            break ;
        case 17 :
            gravity = 3 ;
            sprintf(error,"EXT_TILEMAP allocation failed") ;
            break ;
        case 18 :
            gravity = 1 ;
            sprintf(error,"Bad Type Index %d",*value.pi) ;
            break ;
        case 19 :
            gravity = 1 ;
            sprintf(error,"Bad Type %d Data",*value.pi) ;
            break ;
        case 20 :
            gravity = 3 ;
            sprintf(error,"EXT_TYPEMAP allocation failed") ;
            break ;
        case 21 :
            gravity = 1 ;
            sprintf(error,"Bad Width Index %d",*value.pi) ;
            break ;
        case 22 :
            gravity = 1 ;
            sprintf(error,"Bad Width Thickness %f",*value.pf) ;
            break ;
        case 23 :
            gravity = 3 ;
            sprintf(error,"EXT_WIDTHMAP allocation failed") ;
            break ;
        case 24 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_WINDOW Address %lx",(long ) value.pl) ;
            break ;
        case 25 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_IMAGEDATA Address %lx",(long ) value.pl) ;
            break ;
        case 26 :
            gravity = 1 ;
            sprintf(error,"Try to Draw Image %lx outside the window",(long ) value.pl) ;
            break ;
        case 27 :
            gravity = 3 ;
            sprintf(error,"EXT_IMAGE allocation failed") ;
            break ;
        case 28 :
            gravity = 2 ;
            sprintf(error,"Too many points in POLYLINE,max is %d",*value.pi) ;
            break ;
        case 29 :
            gravity = 1 ;
            sprintf(error,"POLYLINE is empty !!!") ;
            break ;
        case 30 :
            gravity = 3 ;
            sprintf(error,"EXT_POLYLINE allocation failed") ;
            break ;
        case 31 :
            gravity = 3 ;
            sprintf(error,"EXT_MARKER allocation failed") ;
            break ;
        case 32 :
            gravity = 2 ;
            sprintf(error,"Too many points in POLYGONE,max is %d",*value.pi) ;
            break ;
        case 33 :
            gravity = 1 ;
            sprintf(error,"POLYGONE is empty !!!") ;
            break ;
        case 34 :
            gravity = 3 ;
            sprintf(error,"EXT_POLYGONE allocation failed") ;
            break ;
        case 35 :
            gravity = 3 ;
            sprintf(error,"EXT_SEGMENT allocation failed") ;
            break ;
        case 36 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_POLYLINE Address") ;
            break ;
        case 37 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_POLYGONE Address") ;
            break ;
        case 38 :
            gravity = 2 ;
            sprintf(error,"Too big TEXT string,max is %d",*value.pi) ;
            break ;
        case 39 :
            gravity = 3 ;
            sprintf(error,"EXT_TEXT allocation failed") ;
            break ;
        case 40 :
            gravity = 1 ;
            sprintf(error,"Try to Erase outside the window %lx",(long ) value.pl) ;
            break ;
        case 41 :
            gravity = 2 ;
            sprintf(error,"Bad Defined Color at index %d",*value.pi) ;
            break ;
        case 42 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_COLORMAP Address %lx",(long ) value.pl) ;
            break ;
        case 43 :
            gravity = 1 ;
            sprintf(error,"Bad Defined Font at index %d",*value.pi) ;
            break ;
        case 44 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_FONTMAP Address %lx",(long ) value.pl) ;
            break ;
        case 45 :
            gravity = 1 ;
            sprintf(error,"Try to Get Image from outside the window") ;
            break ;
        case 46 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_MARKMAP Address %lx",(long ) value.pl) ;
            break ;
        case 47 :
            gravity = 1 ;
            sprintf(error,"Bad PIXEL position,max is %d",*value.pi) ;
            break ;
        case 48 :
            gravity = 3 ;
            sprintf(error,"Visual %d must be TRUECOLOR",*value.pi) ;
            break ;
        case 49 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_TILEMAP Address %lx",(long ) value.pl) ;
            break ;
        case 50 :
            gravity = 1 ;
            sprintf(error,"Bad Defined Type at index %d",*value.pi) ;
            break ;
        case 51 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_TYPEMAP Address %lx",(long ) value.pl) ;
            break ;
        case 52 :
            gravity = 1 ;
            sprintf(error,"Bad Defined Width at index %d",*value.pi) ;
            break ;
        case 53 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_WIDTHMAP Address %lx",(long ) value.pl) ;
            break ;
        case 54 :
            gravity = 3 ;
            sprintf(error,"Bad Window %lx Attributes",(long ) *value.pl) ;
            break ;
        case 55 :
            gravity = 2 ;
            if(value.pc) sprintf(error,"Bad XWD FileName '%s'",value.pc) ;
            else         sprintf(error,"Bad XWD FileName %lx",(long ) value.pc) ;
            break ;
        case 56 :
            gravity = 2 ;
            sprintf(error,"Unable to read XWD File Header of '%s'",value.pc) ;
            break ;
        case 57 :
            gravity = 2 ;
            sprintf(error,"Bad XWD File format of '%s'",value.pc) ;
            break ;
        case 58 :
            gravity = 2 ;
            sprintf(error,"Bad XWD Pixmap format of '%s'",value.pc) ;
            break ;
        case 59 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Image Visual class %d",*value.pi) ;
            break ;
        case 60 :
            gravity = 3 ;
            sprintf(error,"XImage Allocation failed");
            break ;
        case 61 :
            gravity = 3 ;
            sprintf(error,"Unable to read Image data of '%s'",value.pc) ;
            break ;
        case 62 :
            gravity = 3 ;
            sprintf(error,"XImage Creation failed");
            break ;
        case 63 :
            gravity = 3 ;
            sprintf(error,"No more FREE pixel in Colormap %lx",(long ) *value.pl);
            break ;
        case 64 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Image Visual depth %d",*value.pi) ;
            break ;
        case 65 :
            gravity = 3 ;
            sprintf(error,"Display Stack OVERFLOW") ;
            break ;
        case 66 :
            gravity = 3 ;
            if( value.pc ) sprintf(error,"Bad Display connexion '%s'",value.pc);
            else           sprintf(error,"Bad Display connexion '%lx'",(long ) value.pc);
            break ;
        case 67 :
            gravity = 3 ;
            sprintf(error,"Unmatchable Visual class %d",*value.pi) ;
            break ;
        case 68 :
            gravity = 1 ;
            sprintf(error,"Bad AREA position") ;
            break ;
        case 69 :
            gravity = 3 ;
            sprintf(error,"EXT_WINDOW allocation failed") ;
            break ;
        case 70 :
            gravity = 1 ;
            sprintf(error,"Bad Cursor Index %d",*value.pi) ;
            break ;
        case 71 :
            gravity = 1 ;
            sprintf(error,"Double Buffering is DISABLE") ;
            break ;
        case 72 :
            gravity = 1 ;
            sprintf(error,"Lose RETAIN lines buffer attrib %x",*value.pi) ;
            break ;
        case 73 :
            gravity = 1 ;
            sprintf(error,"Lose RETAIN polygons buffer attrib %x",*value.pi) ;
            break ;
        case 74 :
            gravity = 1 ;
            sprintf(error,"Lose RETAIN texts buffer attrib %x",*value.pi) ;
            break ;
        case 75 :
            gravity = 1 ;
            sprintf(error,"Lose RETAIN markers buffer attrib %x",*value.pi) ;
            break ;
        case 76 :
            gravity = 1 ;
            sprintf(error,"Bad Mapping Size %f",*value.pf) ;
            break ;
        case 77 :
            gravity = 1 ;
            sprintf(error,"Bad Defined Marker at index %d",*value.pi) ;
            break ;
        case 78 :
            gravity = 1 ;
            sprintf(error,"Bad Defined Tile at index %d",*value.pi) ;
            break ;
        case 79 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Marker type %d",*value.pi) ;
            break ;
        case 80 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Polygon type %d",*value.pi) ;
            break ;
        case 81 :
            gravity = 4 ;
            sprintf(error,"Unimplemented Text type %d",*value.pi) ;
            break ;
        case 82 :
            gravity = 3 ;
            sprintf(error,"Bad Line Attrib %lx",(long ) *value.pl) ;
            break ;
        case 83 :
            gravity = 3 ;
            sprintf(error,"Bad Polygon Attrib %lx",(long ) *value.pl) ;
            break ;
        case 84 :
            gravity = 3 ;
            sprintf(error,"Bad Text Attrib %lx",(long ) *value.pl) ;
            break ;
        case 85 :
            gravity = 3 ;
            sprintf(error,"Bad Marker Attrib %lx",(long ) *value.pl) ;
            break ;
        case 86 :
            gravity = 1 ;
            sprintf(error,"Unimplemented EVENT type %d",*value.pi) ;
            break ;
        case 87 :
            gravity = 1 ;
            sprintf(error,"Use def_background_color instead on index %d",
                                                                *value.pi) ;
            break ;
        case 88 :
            gravity = 2 ;
            sprintf(error,"Bad RGB file name %s",value.pc) ;
            break ;
        case 89 :
            gravity = 1 ;
            sprintf(error,"Bad Named Color %s",value.pc) ;
            break ;
        case 90 :
            gravity = 3 ;
            sprintf(error,"Unable to Create DisplayPostScript Context") ;
            break ;
        case 91 :
            gravity = 2 ;
            sprintf(error,"Coordinate must be in [0,1] space") ;
            break ;
        case 92 :
            gravity = 2 ;
            sprintf(error,"ColorCube Creation failed width %d Colors",*value.pi) ;
            break ;
        case 93 :
            gravity = 2 ;
            sprintf(error,"ColorCube Allocation failed");
            break ;
        case 94 :
            gravity = 3 ;
            sprintf(error,"EXT_DISPLAY allocation failed") ;
            break ;
        case 95 :
            gravity = 1 ;
            sprintf(error,"Bad Mouse Button number %d",*value.pi) ;
            break ;
        case 96 :
            gravity = 3 ;
            sprintf(error,"Bad EXT_DISPLAY Address %lx",(long ) value.pl) ;
            break ;
        case 97 :
            gravity = 1 ;
            sprintf(error,"Unimplemented Internal Event number %d",*value.pi) ;
            break ;
        case 98 :
            gravity = 1 ;
            sprintf(error,"Can't GRAB Cursor id %d on this Window",*value.pi) ;
            break ;
        case 99 :
            gravity = 3 ;
            sprintf(error,"Can't OPEN IconBox Window") ;
            break ;
        case 100 :
            gravity = 1 ;
            if( value.pc ) 
                sprintf(error,"Can't OPEN Icon File Directory '%s'",value.pc);
            else sprintf(error,"Can't OPEN Icon File Directory '%lx'",(long ) value.pc);
            break ;
        case 101 :
            gravity = 3 ;
            sprintf(error,"EXT_ICON allocation failed") ;
            break ;
        case 102 :
            gravity = 1 ;
            sprintf(error,"Icon Box is EMPTY !") ;
            break ;
        case 103 :
            gravity = 1 ;
            if( value.pc ) 
                sprintf(error,"Bad Icon Name '%s'",value.pc);
            else sprintf(error,"Bad Icon Name '%lx'",(long ) value.pc);
            break ;
        case 104 :
            gravity = 3 ;
            sprintf(error,"PIXMAP allocation failed") ;
            break ;
        case 105 :
            gravity = 1 ;
            sprintf(error,"Allocates ONLY %d contiguous colors",*value.pi);
            break ;
        case 106 :
            gravity = 1 ;
            sprintf(error,"Bad Icon Index %d",*value.pi);
            break ;
        case 107 :
            gravity = 1 ;
            sprintf(error,"XImage BAD pixel color %x",*value.pi);
            break ;
        case 108 :
            gravity = 1 ;
            sprintf(error,"BAD Icon Name size,must be < %d",*value.pi);
            break ;
        case 109 :
            gravity = 1 ;
            if( value.pc ) 
                sprintf(error,"Bad Icon FileName '%s'",value.pc);
            else sprintf(error,"Bad Icon FileName '%lx'",(long ) value.pc);
            break ;
        case 110 :
            gravity = 1 ;
            sprintf(error,"No more FREE pixel in STD Colormap %d",*value.pi);
            break ;
        case 111 :
            gravity = 2 ;
            sprintf(error,"Too many SEGMENTS,max is %d",*value.pi) ;
            break ;
        case 112 :
            gravity = 2 ;
            sprintf(error,"Too many points in MARKER,max is %d",*value.pi) ;
            break ;
        case 113 :
            gravity = 2 ;
            sprintf(error,"Bad Image zoom factor %f",*value.pf) ;
            break ;
        case 114 :
            gravity = 2 ;
            sprintf(error,"Bad Image translation") ;
            break ;
        case 115 :
            gravity = 2 ;
            sprintf(error,"Bad arc radius %f",*value.pf) ;
            break ;
        case 116 :
            gravity = 1 ;
            sprintf(error,"Too big arc radius") ;
            break ;
        case 117 :
            gravity = 3 ;
            sprintf(error,"EXT_POINT allocation failed") ;
            break ;
        case 118 :
            gravity = 3 ;
            sprintf(error,"EXT_CHAR allocation failed") ;
            break ;
        case 119 :
            gravity = 2 ;
            sprintf(error,"BUFFER %d is not opened",*value.pi) ;
            break ;
        case 120 :
            gravity = 1 ;
            sprintf(error,"BUFFER %d is already opened",*value.pi) ;
            break ;
        case 121 :
            gravity = 3 ;
            sprintf(error,"Too many opened BUFFERS,max is %d",*value.pi) ;
            break ;
        case 122 :
            gravity = 3 ;
            sprintf(error,"Bad BUFFER identification %d",*value.pi) ;
            break ;
        case 123 :
            gravity = 2 ;
            sprintf(error,"Bad BUFFER scale factor %f",*value.pf) ;
            break ;
        case 124 :
            gravity = 1 ;
            sprintf(error,"Bad margin poly text ratio %f",*value.pf) ;
            break ;
        case 125 :
            gravity = 1 ;
            sprintf(error,"Bad BUFFER drawing mode %d",*value.pi) ;
            break ;
        case 126 :
            gravity = 1 ;
            sprintf(error,"MultiBuffering Extension is not accessible here") ;
            break ;
        case 127 :
            gravity = 1 ;
            sprintf(error,"Bad Color Index Allocation %d",*value.pi) ;
            break ;
        case 128 :
            gravity = 3 ;
            sprintf(error,"XDPS_EXT_WINDOW allocation failed") ;
            break ;
        case 129 :
            gravity = 1 ;
            sprintf(error,"Too many entry in the virtual colormap %lx",(long ) value.pl) ;
            break ;
        case 130 :
            gravity = 1;
            sprintf(error,"Too big ZOOM factor (%f)", *value.pf);
            break;
        default :
            gravity = 4 ;
            if( code >= 1000 ) {
              sprintf(error,"code %d/'%s'\n",code-1000,value.pc) ;
            } else {
              sprintf(error,"Bad Error Code %d",code) ;
            }
    }
    
    if( gravity > ErrorLevel ) {
        printf("*Xw_Error_%d*%s from %s routine\n",gravity,error,routine) ;
    } else {                    /* Put Messag in Error Stack */
        int lr = strlen(routine) + 1 ;
        int lm = strlen(error) + 1 ;
        int i = 0 ;

        if( Nerror > 0 ) {
          for( i=0 ; i<Nerror ; i++ ) {
            if( code == ErrorCode[i] ) break;
          }
        }

        if( i < Nerror ) {
          ErrorCount[i]++;
        } else {
          if( (Nerror >= MAXERROR) || (Lstring + lr + lm > MAXERRORSTRING) ) {
                                        /* Stack Overflow */
            printf("*Xw_Error*Stack OVERFLOW,use Xw_get_error a lot !!!\n") ;
            Xw_print_error() ;
          }

          ErrorCode[Nerror] = code ;            /* Push Error Stack */
          ErrorGravity[Nerror] = gravity ;
          ErrorCount[Nerror] = 1 ;
          ErrorRoutine[Nerror] = &ErrorString[Lstring] ;
          strcpy(&ErrorString[Lstring],routine) ; Lstring += lr ;
          ErrorMessag[Nerror] = &ErrorString[Lstring] ;
          strcpy(&ErrorString[Lstring],error) ; Lstring += lm ;
          Nerror++ ;
        }
    }
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_print_error()
#else
XW_STATUS Xw_print_error()
#endif /*XW_PROTOTYPE*/
/*
        Print Error Stack

        Returns XW_ERROR if no erreur exist
        Returns XW_SUCCESS if successful 
*/
{
XW_STATUS status = XW_SUCCESS ;
int i ;

    if( Nerror ) {
       for( i=0 ; i<Nerror ; i++ ) {
         printf("*Xw_Error_%d/%d*%s from %s routine\n",
                        ErrorGravity[i],ErrorCount[i],ErrorMessag[i],ErrorRoutine[i]) ;
       }
       Nerror = 0 ;
       Lstring = 0 ;
    } else status = XW_ERROR ;

    return (status) ;
}

#ifdef XW_PROTOTYPE
char* Xw_get_error(int *error,int *gravity)
#else
char* Xw_get_error(error,gravity)
int *error ;
int *gravity ;
#endif /*XW_PROTOTYPE*/
/*
        Returns the last Error Code,Gravity and Messag found if any

        Returns Messag string if any Error has been found
        Returns NULL if No Error has been found
*/
{

    if( Nerror > 0 ) {
        *error = ErrorCode[Nerror-1] ;
        *gravity = ErrorGravity[Nerror-1] ;
        PErrorString = ErrorMessag[Nerror-1] ;
        --Nerror ;                      /* Pop Error Stack */
        Lstring -= strlen(ErrorMessag[Nerror]) + 
                                strlen(ErrorRoutine[Nerror]) + 2 ;
    } else {
        *error = 0 ;
        *gravity = 0 ;
        PErrorString = NULL ;
    }

    return (PErrorString) ;
}

#ifdef XW_PROTOTYPE
int Xw_error_handler(Display *display,XErrorEvent *event)
#else
int Xw_error_handler(display,event)
Display *display ;
XErrorEvent *event ;
#endif /*XW_PROTOTYPE*/
{
  if( TraceLevel != 0 ) {
    XmuPrintDefaultErrorMessage(display,event,stderr);
  } else {
    char xerrormessag[128] ;
    if( event->request_code != X_ConfigureWindow
#ifndef DEBUG
	&& event->request_code != X_ChangeWindowAttributes //error reproduced for several X servers (Mandriva 2008)
#endif
	) {
      XGetErrorText(display,event->error_code,xerrormessag,sizeof(xerrormessag))
 ;
      Xw_set_error(1000+event->error_code,"Xw_error_handler",xerrormessag) ;
    }
  }
  return True;
}
