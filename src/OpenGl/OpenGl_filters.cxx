/***********************************************************************

FONCTION :
----------
File OpenGl_filters :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stdlib.h>
#include <stddef.h>

#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Definition des types
*/

struct TGL_FILTERS
{
  Tint     invis_incl_num,
    invis_excl_num,
    pick_incl_num,
    pick_excl_num,
    highl_incl_num,
    highl_excl_num;

  Tint     invis_incl_siz,
    invis_excl_siz,
    pick_incl_siz,
    pick_excl_siz,
    highl_incl_siz,
    highl_excl_siz;

  Tint    *invis_incl_lis,
    *invis_excl_lis,
    *pick_incl_lis,
    *pick_excl_lis,
    *highl_incl_lis,
    *highl_excl_lis;

  IMPLEMENT_MEMORY_OPERATORS
};
typedef TGL_FILTERS* tgl_filters;

struct TGL_NAMESET_NODE_STRUCT
{
  Tint    num, siz, *ptr;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TGL_NAMESET_NODE_STRUCT  TGL_NAMESET_NODE;
typedef TGL_NAMESET_NODE_STRUCT* tgl_nameset_node;


/*----------------------------------------------------------------------*/
/*
* Variables locales
*/

typedef NCollection_Stack<TGL_NAMESET_NODE*> NodeStack;
typedef NCollection_List<TGL_FILTERS*> FilterList;

static NodeStack _NodeStack;
static FilterList _FilterList;


/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define  LIST_GROW_SIZE  25
#define  FIL_STBL_SIZE   23

/*----------------------------------------------------------------------*/

static int
num_comp( const void* a, const void* b )
{
  return *( Tint* )a - *( Tint* )b;
}

/*----------------------------------------------------------------------*/

static TStatus
set_filter( Tint n, Tint *ls, Tint *num, Tint *siz, Tint **list )
{
  Tint  size;

  size =  n,
    size %= LIST_GROW_SIZE,
    size += 1,
    size *= LIST_GROW_SIZE;
  if( !*siz )
  {
    //cmn_memreserve( *list, size, 0 );
    *list = new Tint[size];
    if( !*list )
      return TFailure;

    *siz = size;
  }
  else if( *siz < n )
  {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
    *list = (Tint*)realloc( *list, size*sizeof(Tint) );
#else
    *list = cmn_resizemem<Tint>( *list, size );
#endif
    if( !*list )
      return TFailure;

    *siz = size;
  }  
  //cmn_memcpy<Tint>( *list, ls, n );
  memcpy( *list, ls, n*sizeof(Tint) );
  qsort( *list, n, sizeof(Tint), num_comp );
  *num = TelRemdupnames( *list, n );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  tgl_filters
getfiltrec( Tint  wsid )
{
  tgl_filters  filt;
  CMN_KEY_DATA data;

  if( TsmGetWSAttri( wsid, WSFilters, &data ) == TFailure )
    return 0;

  filt = (tgl_filters)data.pdata;
  if( !filt )
  {
    filt = new TGL_FILTERS();
    data.pdata = filt;
    TsmSetWSAttri( wsid, WSFilters, &data );
  }

  return filt;
}

/*----------------------------------------------------------------------*/

TStatus
TglSetHighlFilter( Tint wsid, Tint in_num, Tint *in_lis,
                  Tint ex_num, Tint *ex_lis )
{
  TStatus  ret;
  tgl_filters  filter;

  filter = getfiltrec( wsid );
  if( !filter )
    return TFailure;

  ret = set_filter( in_num, in_lis, &filter->highl_incl_num,
    &filter->highl_incl_siz, &filter->highl_incl_lis );
  if( ret == TSuccess )
    ret = set_filter( ex_num, ex_lis, &filter->highl_excl_num,
    &filter->highl_excl_siz, &filter->highl_excl_lis );
  return ret;
}

/*----------------------------------------------------------------------*/

TStatus
TglSetPickFilter( Tint wsid, Tint in_num, Tint *in_lis,
                 Tint ex_num, Tint *ex_lis )
{
  TStatus  ret;
  tgl_filters  filter;

  filter = getfiltrec( wsid );
  if( !filter )
    return TFailure;

  ret = set_filter( in_num, in_lis, &filter->pick_incl_num,
    &filter->pick_incl_siz, &filter->pick_incl_lis );
  if( ret == TSuccess )
    ret = set_filter( ex_num, ex_lis, &filter->pick_excl_num,
    &filter->pick_excl_siz, &filter->pick_excl_lis );
  return ret;
}

/*----------------------------------------------------------------------*/

TStatus
TglSetInvisFilter( Tint wsid, Tint in_num, Tint *in_lis,
                  Tint ex_num, Tint *ex_lis )
{
  TStatus  ret;
  tgl_filters  filter;

  filter = getfiltrec( wsid );
  if( !filter )
    return TFailure;

  ret = set_filter( in_num, in_lis, &filter->invis_incl_num,
    &filter->invis_incl_siz, &filter->invis_incl_lis );
  if( ret == TSuccess )
    ret = set_filter( ex_num, ex_lis, &filter->invis_excl_num,
    &filter->invis_excl_siz, &filter->invis_excl_lis );
  return ret;
}

/*----------------------------------------------------------------------*/

TStatus
TglNamesetPush( void )
{
  tgl_nameset_node  node;

  node = new TGL_NAMESET_NODE();
  if ( _NodeStack.IsEmpty() || !_NodeStack.Top() || !_NodeStack.Top()->ptr )
    /* abd
    if( !name_tail || !name_tail->ptr )*/
  {
    node->ptr = 0, node->num = node->siz = 0;
  }
  else /* copy previous stack entry to new one */
  {
    Tint  size = _NodeStack.Top()->siz;

    node->ptr = new Tint[size];
    if( !node->ptr )
      return TFailure;

    node->siz = size;
    node->num = _NodeStack.Top()->num;
    memcpy( node->ptr, _NodeStack.Top()->ptr, node->num*sizeof(Tint) );
  }

  _NodeStack.Push( node );

  return TSuccess;
}

/*----------------------------------------------------------------------*/


TStatus
TglNamesetPop( void )
{
  tgl_nameset_node node =
    _NodeStack.ChangeTop();

  _NodeStack.Pop();
  if( node->ptr )
  {
    /* Remove entries from sorted list*/
    delete[] node->ptr;
  }
  delete node;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TglNamesetAdd( Tint  num, Tint *set )
{
  if (_NodeStack.IsEmpty())
    return TFailure;

  Tint  size;
  tgl_nameset_node name_tail = _NodeStack.ChangeTop();

  size = num,
    size %= LIST_GROW_SIZE,
    size++,
    size *= LIST_GROW_SIZE;

  if( !name_tail->ptr )
  {
    name_tail->ptr = new Tint[size];
    if( !name_tail->ptr )
      return TFailure;

    name_tail->siz = size;
  }
  else if( name_tail->siz < name_tail->num + num )
  {
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
    name_tail->ptr = (Tint*)realloc( name_tail->ptr, size*sizeof(Tint) );
#else
    name_tail->ptr = cmn_resizemem<Tint>( name_tail->ptr, size );
#endif
    if( !name_tail->ptr )
      return TFailure;

    name_tail->siz = size;
  }
  memcpy( &name_tail->ptr[name_tail->num], set, num*sizeof(Tint) );
  name_tail->num += num;

  /* sort new set */
  qsort( name_tail->ptr, name_tail->num, sizeof(Tint), num_comp );
  name_tail->num = TelRemdupnames( name_tail->ptr, name_tail->num );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TglNamesetRemove( Tint num, Tint *set )
{
  Tint  size, *ptr, *temp;
  register  Tint  i, j, k, *prev;

  if (_NodeStack.IsEmpty())
    return TFailure;

  tgl_nameset_node name_tail = _NodeStack.ChangeTop();

  if( !name_tail || !name_tail->ptr || !num )
    return TFailure;

  ptr = new Tint[name_tail->siz];
  if( !ptr )
    return TFailure;

  size = num, size %= LIST_GROW_SIZE, size++, size *= LIST_GROW_SIZE;
  temp = new Tint[size];
  if( !temp )
    return TFailure;
  memcpy( temp, set, num*sizeof(Tint) );

  /* sort new set */
  qsort( temp, num, sizeof(Tint), num_comp );
  num = TelRemdupnames( temp, num );

  prev = name_tail->ptr;
  i = j = k = 0;
  while(1)
  {
    if( prev[i] < temp[j] )
      ptr[k++] = prev[i++];
    else if( prev[i] > temp[j] )
      j++;
    else /* prev[i] == temp[j] */
      i++, j++;
    if( i == name_tail->num )
      break;
    if( j == num )
    {
      while( i < name_tail->num )
        ptr[k++] = prev[i++];
      break;
    }
  }
  //cmn_freemem( temp );
  delete[] temp;
  //cmn_freemem( name_tail->ptr );
  delete[] name_tail->ptr;

  name_tail->ptr = ptr;
  name_tail->num = k;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static Tint
find( Tint num1, Tint *array1, Tint num2, Tint *array2 )
{
  Tint i=0, j=0;

  while(num1 && num2)
  {
    if( array1[i] < array2[j] )
      i++;
    else if( array1[i] > array2[j] )
      j++;
    else
      return 1;

    if( i == num1 || j == num2 )
      return 0;
  }
  return 0;
}

/*----------------------------------------------------------------------*/

TStatus
TglFilterNameset( Tint  wsid, TFilterSet set )
{
  tgl_filters  filter;
  if (_NodeStack.IsEmpty())
    return TFailure;

  tgl_nameset_node name_tail = _NodeStack.ChangeTop();

  filter = getfiltrec( wsid );
  if( !filter )
    return TFailure;

  switch( set )
  {
  case InvisFilter:
    {
      if( find( filter->invis_excl_num, filter->invis_excl_lis,
        name_tail->num, name_tail->ptr ) )
        return TFailure;

      if( find( filter->invis_incl_num, filter->invis_incl_lis,
        name_tail->num, name_tail->ptr ) )
        return TSuccess;

      break;
    }

  case HighlFilter:
    {
      if( find( filter->highl_excl_num, filter->highl_excl_lis,
        name_tail->num, name_tail->ptr ) )
        return TFailure;

      if( find( filter->highl_incl_num, filter->highl_incl_lis,
        name_tail->num, name_tail->ptr ) )
        return TSuccess;

      break;
    }

  case PickFilter:
    {
      if( find( filter->pick_excl_num, filter->pick_excl_lis,
        name_tail->num, name_tail->ptr ) )
        return TFailure;

      if( find( filter->pick_incl_num, filter->pick_incl_lis,
        name_tail->num, name_tail->ptr ) )
        return TSuccess;

      break;
    }
  }

  return TFailure;
}

/*----------------------------------------------------------------------*/

TStatus
TglDeleteFiltersForWS( Tint wsid )
{
  CMN_KEY_DATA key;
  tgl_filters  f;

  TsmGetWSAttri( wsid, WSFilters, &key );
  f = (tgl_filters)key.pdata;

  if( !f )
    return TSuccess;

  if( f->invis_incl_siz )
    //cmn_freemem( f->invis_incl_lis );
    delete[] f->invis_incl_lis;
  if( f->pick_incl_siz )
    //cmn_freemem( f->pick_incl_lis );
    delete[] f->pick_incl_lis;
  if( f->highl_incl_siz )
    //cmn_freemem( f->highl_incl_lis );
    delete[] f->highl_incl_lis;
  if( f->invis_excl_siz )
    //cmn_freemem( f->invis_excl_lis );
    delete[] f->invis_excl_lis;
  if( f->pick_excl_siz )
    //cmn_freemem( f->pick_excl_lis );
    delete[] f->pick_excl_lis;
  if( f->highl_excl_siz )
    //cmn_freemem( f->highl_excl_lis );
    delete[] f->highl_excl_lis;

  //cmn_stg_tbl_free( f );
  delete f;
  return TSuccess;
}

/*----------------------------------------------------------------------*/
