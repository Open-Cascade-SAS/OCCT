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
#define TRACE_ISDEFINE_TILE
#endif

/*
   XW_STATUS Xw_isdefine_tile (atilemap,index):
   XW_EXT_TILEMAP *atilemap
   int index			tile index

	Returns XW_ERROR if BadTile Index or tile is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_tile (void *atilemap,int index)
#else
XW_STATUS Xw_isdefine_tile (atilemap,index)
void *atilemap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
XW_STATUS status = XW_ERROR ;

	if( index ) {
	    if ( ptilemap && (index < ptilemap->maxtile) 
					&& ptilemap->tiles[index] ) {
	        status = XW_SUCCESS ;
	    }
	} else status = XW_SUCCESS ;


#ifdef  TRACE_ISDEFINE_TILE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_tile(%lx,%d)\n",status,(long ) ptilemap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_tileindex (XW_EXT_TILEMAP *atilemap,int index)
#else
XW_STATUS Xw_isdefine_tileindex (atilemap,index)
XW_EXT_TILEMAP *atilemap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Tile index Range Only
*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
XW_STATUS status = XW_ERROR ;

	if( ptilemap && (index > 0 ) && (index < ptilemap->maxtile) ) {
	   status = XW_SUCCESS ;
	}

#ifdef  TRACE_ISDEFINE_TILE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_tileindex(%lx,%d)\n",status,(long ) ptilemap,index) ;
}
#endif

	return (status) ;
}
