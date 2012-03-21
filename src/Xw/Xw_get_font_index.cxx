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


#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_FONT_INDEX
#endif

/*
   STATUS Xw_get_font_index(afontmap,size,fontname,index):

   XW_EXT_FONTMAP *afontmap	Fontmap extension structure
   float size ;		Font size in MM 
   char *fontname ;		Font descriptor ended with '\0'
   int *index ;		Return Font index 0 >= x < MAXFONT

	Gets the nearest font index from Font Descriptor.

	Returns ERROR if BadFont Index or font is not defined
	Returns SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_font_index(void* afontmap,float size,char* fontname,int* index)
#else
XW_STATUS Xw_get_font_index(afontmap,size,fontname,index)
void *afontmap ;
float size ;
char *fontname ;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap ;
XW_STATUS status = XW_ERROR ;
int i,j ;

    if( !pfontmap ) {
	/*ERROR*Bad EXT_FONTMAP Address*/
	Xw_set_error( 44,"Xw_get_font_index",pfontmap ) ;
	return( XW_ERROR ) ;
    }

    if( fontname && strlen(fontname) ) {

        for( i=j=0 ; i<pfontmap->maxfont ; i++ ) {
	    if( pfontmap->gnames[i] ) {
	      if( !strcmp(fontname,pfontmap->gnames[i]) ) {
		if( fabs(size-pfontmap->gsizes[i]) < 0.1 ) break;
	      }
	    } else if( !j ) j = i ;
        }

	if( i<pfontmap->maxfont ) {
	    *index = i ;
	    status = XW_SUCCESS ;
	} else {
	    *index = j ;
	    status = Xw_def_font (pfontmap,j,size,fontname) ;
	}
    } else {
	*index = 0 ;
	status = XW_SUCCESS ;
    }

#ifdef TRACE_GET_FONT_INDEX
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_font_index(%lx,%f,'%s',%d)\n",
			status,(long ) pfontmap,size,fontname,*index) ;
}
#endif

	return( status ) ;
}
