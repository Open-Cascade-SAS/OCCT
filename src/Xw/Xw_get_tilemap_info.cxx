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
#define TRACE_GET_TILEMAP_INFO
#endif

/*
   XW_STATUS Xw_get_tilemap_info (atilemap,mtile,utile,dtile,ftile):
   XW_EXT_TILEMAP *atilemap
   int *mtile		Return the maximum tile number of the tilemap
   int *utile		Return the User tile number used in the tilemap
   int *dtile		Return the User tile number defined in the tilemap
   int *ftile		Return the First free tile index in the tilemap
   NOTE than if ftile is < 0 No more Free Tile exist in the tilemap

	Returns XW_ERROR if the Extended Tilemap is not defined
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_tilemap_info (void *atilemap,int *mtile,int *utile,int *dtile,int *ftile)
#else
XW_STATUS Xw_get_tilemap_info (atilemap,mtile,utile,dtile,ftile)
void *atilemap;
int *mtile,*utile,*dtile,*ftile ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
  int i ;

	if( !Xw_isdefine_tilemap(ptilemap) ) {
	    /*ERROR*Bad EXT_TILEMAP Address */
	    Xw_set_error(49,"Xw_get_tilemap_info",ptilemap) ;
	    return (XW_ERROR) ;
	}

	*mtile = MAXTILE ;
	*utile = ptilemap->maxtile ;
	*dtile = 1 ;
	*ftile = -1 ;
	for( i=0 ; i< *utile ; i++ ) {
	    if( ptilemap->tiles[i] ) (*dtile)++ ;
	    else if( i && *ftile < 0 ) *ftile = i ;
	}
	
#ifdef  TRACE_GET_TILEMAP_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_tilemap_info(%lx,%d,%d,%d,%d)\n",
			(long ) ptilemap,*mtile,*utile,*dtile,*ftile) ;
}
#endif

	return (XW_SUCCESS);
}
