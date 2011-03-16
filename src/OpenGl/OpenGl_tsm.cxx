/***********************************************************************

FONCTION :
----------
File OpenGl_togl_texture.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-08-97 : PCT ; Support texture mapping
ajout pour deuxieme passe du texture mapping

************************************************************************/

#define G003  /* EUG 16-09-99 ZBufferAuto treatment
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_Memory.hxx>

static  MtblPtr   TsmMtblArray[TelLast];

/* Fixed Header info for a structure */
struct  TSM_HEAD_STRUCT
{
  Tint    num;               /* number of elements present */
};
typedef TSM_HEAD_STRUCT  TSM_HEAD;
typedef TSM_HEAD_STRUCT* tsm_head;

/* The head of the list when the structure is in the form of a list */
struct  TSM_LHEAD_STRUCT
{
  tsm_node  next; // first list node
  tsm_node  prev; // last list node for reverse iteration
  TSM_HEAD  header;
};
typedef TSM_LHEAD_STRUCT  TSM_LHEAD;
typedef TSM_LHEAD_STRUCT *tsm_lhead;

/* A Structure having fixed header and variable list of elements */
struct  TSM_STRUCT_STRUCT
{
  TSM_LHEAD elemListHead;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TSM_STRUCT_STRUCT  TSM_STRUCT;
typedef TSM_STRUCT_STRUCT *tsm_struct;

#define    TsmStructureNotOpen      -1
#define    TsmInvalidStructure      -1

/* Context maintained for the currently open structure */
struct  TSM_CONTEXT_STRUCT
{
  Tint     stid;   /* Current  Open Structure */
  Tint      ind;   /* Current  Element Index  */
  /* = TsmStructureNotOpen if no structure currently open */
  tsm_struct  s;   /* Current  Structure Pointer */
  tsm_node    n;   /* Current  Element list node */
};
typedef TSM_CONTEXT_STRUCT  TSM_CONTEXT;
typedef TSM_CONTEXT_STRUCT* tsm_context;

static  TSM_CONTEXT   context = { TsmInvalidStructure, TsmStructureNotOpen, 0, 0 };

static  TEditMode     edit_mode;

#define  TSM_HTBL_SIZE  251
typedef NCollection_List<TSM_NODE*> NodeList;
typedef NCollection_DataMap<Tint, TSM_STRUCT*> StructMap;
Handle(NCollection_BaseAllocator)  _Allocator = NCollection_BaseAllocator::CommonBaseAllocator();
static StructMap _StructMap( 1, _Allocator );



/* Global workstation variable */
Tint    TglActiveWs;        /* valid only during traversals */

void
TsmInitAllClasses( MtblPtr  (**tbl)(TelType*), Tint size )
{
  register  Tint  i;
  TelType   el;
  register  MtblPtr   fp;

  for( i=0; i<size; i++ )
  {
    fp = tbl[i](&el);
    TsmMtblArray[el] = fp;
  }
  return;
}

/* This function invokes the required method of the element type by looking
up an array of Method Tables of each element.
The arguments passed to the method are the data of the element and any
other data that needs to be sent.
In the case of ADD_TO_STRUCTURE methods, data should be the address of the
element data. Other wise, it is the element data itself
*/

TStatus
TsmSendMessage( TelType el, TMsgType msg, TSM_ELEM_DATA data, Tint n, ... )
{
#if defined (SOLARIS) || defined (IRIXO32)
  cmn_key  *k;
#else
  cmn_key  k[TMaxArgs];

  // Nullifies 0-th element of the "k" array
  // because it might be used in a UserDraw function
  // to get information on view ID, for example.
  k[0] = NULL;
  // Uncomment the code below if you caught
  // exception because of k[1] is NULL.
  // Nullifies the whole array of "k"
  //int i = TMaxArgs;
  //while (--i)
  //    k[i] = NULL;

#endif
  TStatus  TheStatus = TSuccess;

  CMN_GET_STACK( n, k );

  if( TsmMtblArray[el] && TsmMtblArray[el][msg] )
    TheStatus = TsmMtblArray[el][msg]( data, n, k );
  return  TheStatus;
}

static  void
TsmFreeList( tsm_struct s )
{
  if ( !s )
    return;

  if( _Allocator.IsNull() )
    return ;

  tsm_node  node, prev;
  Tint      num, i;

  num = s->elemListHead.header.num;
  node = s->elemListHead.next;

  for( i=0; i<num; i++ )
  {
    TsmSendMessage( node->elem.el, Delete, node->elem.data, 0 );
    prev = node;
    node = node->next;
    delete prev;
  }

  s->elemListHead.next = s->elemListHead.prev = (tsm_node)&(s->elemListHead);
  s->elemListHead.header.num = 0;
  return;
}

TStatus
TsmOpenStructure( Tint  stid )
{
  tsm_struct s = 0;

  if( _Allocator.IsNull() )
    return TFailure;    

  if( context.ind != TsmStructureNotOpen )
    return TFailure; /* Some structure already open */

  if ( !_StructMap.IsBound(stid) ){
    s = new TSM_STRUCT();
    if ( !s )
      return TFailure; // Memory allocation failed

    _StructMap.Bind( stid, s );
    s->elemListHead.next = s->elemListHead.prev = (tsm_node)&(s->elemListHead);
    s->elemListHead.header.num = 0;
  }

  s = _StructMap.Find(stid);

  context.s = s;
  context.stid = stid;
  // Make the context point at the last node of the structure,
  // this is the list head in case of an empty structure
  context.ind = s->elemListHead.header.num;
  context.n = s->elemListHead.prev;

  /* Ajout CAL, 14/09/95 */
  /* on force l'update_state de toutes les wks a TNotDone */
  TsmInitUpdateState ();

  return TSuccess;
}

TStatus
TsmCloseStructure()
{
  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  context.ind = TsmStructureNotOpen;

  return TSuccess;
}


TStatus
TsmDisplayStructure( Tint stid, Tint  wsid )
{
  TSM_ELEM_DATA  data;
  CMN_KEY        key;

  TglActiveWs = wsid;

  if( context.ind != TsmStructureNotOpen )
    return TFailure; /* Some structure currently open */

  key.id = wsid;
  data.ldata = stid;
  TsmSendMessage( TelExecuteStructure, DisplayTraverse, data, 1, &key );

  return TSuccess;
}

TStatus
TsmPrintStructure( Tint stid )
{
  tsm_node       node;
  Tint           i, num;
  tsm_struct     s;

  if( context.ind != TsmStructureNotOpen )
    return TFailure; /* Some structure currently open */

  if ( _StructMap.IsEmpty() )
    return TFailure; /* No structure created yet */

  /* Obtain structure pointer from hash table */
  if ( !_StructMap.IsBound(stid) )
    return TFailure; /* Non-existent structure */

  s = _StructMap.Find(stid);

  printf( "\nPRINT:" );
  printf( "\n\tSTRUCTURE ID = %d", stid );
  if( !s ){
    num = 0;
    node = 0;
  }
  else{
    num = s->elemListHead.header.num;
    node = s->elemListHead.next;
  }

  printf( "\n\tNUMBER OF ELEMENTS = %d", num);
  printf( "\n" );

  /* For each element Send Message Print */
  for( i = 0; i < num; i++ ){
    printf("\n\tElement[%d] : ", i+1);
    TsmSendMessage( node->elem.el, Print, node->elem.data, 0 );
    node = node->next;
  }

  return TSuccess;
}

TStatus
TsmAddToStructure( TelType  el, Tint n, ... )
{
  TSM_ELEM_DATA    data;
#if defined (SOLARIS) || defined (IRIXO32)
  cmn_key  *k;
#else
  cmn_key  k[TMaxArgs];
#endif

  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  CMN_GET_STACK( n, k );
  data.pdata = &data;
  if( TsmSendMessage( el, Add, data, -n, k ) == TFailure )
    return TFailure;

  if( edit_mode == TEditReplace ){
    TsmSendMessage( context.n->elem.el, Delete, context.n->elem.data, 0 );
  }
  else{
    tsm_node node = new TSM_NODE();

    if( !node )
      return TFailure;

    /* insert the node in the linked list after the current node */
    node->prev       = context.n;
    node->next       = context.n->next;
    context.n->next  = node;
    node->next->prev = node;

    /* Make this node as current node */
    context.n = node;
    context.ind++;

    /* Increment the no of elements */
    context.s->elemListHead.header.num++;
  }

  context.n->elem.el = el;
  context.n->elem.data = data;

  return TSuccess;
}

TStatus
TsmDeleteStructure( Tint stid )
{
  if( context.ind != TsmStructureNotOpen && context.stid == stid )
    return TFailure; /* Same structure currently open */

  if ( _StructMap.IsEmpty() )
    return TFailure; /* No structure created yet */

  if ( !_StructMap.IsBound(stid) )
    return TFailure; /* Non-existent structure */

  tsm_struct s = _StructMap.ChangeFind(stid);

  if( stid == context.stid ){
    context.stid = TsmInvalidStructure;
    // Ensure the context doesn't point to freed memory
    context.s = 0;
    context.n = 0;
  }

  if( s ){
    TsmFreeList( s );

    /* Free structure memory */
    delete s;
  }

  /* Remove hash table entry */
  _StructMap.UnBind( stid );

  /* Ajout BGN, 27/05/97 */
  /* on force l'update_state de toutes les wks a TNotDone */
  TsmInitUpdateState ();

  return TSuccess;
}

TEditMode
TsmSetEditMode( TEditMode mode )
{
  TEditMode m;

  m = edit_mode;

  /* Set static variable for mode */
  edit_mode = mode;

  /* return previous mode */
  return m;
}

TStatus
TsmDeleteElement()
{
  tsm_node node;

  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  // Zero ind means the currently open structure is empty 
  // or the current node is the list head -> nothing to delete
  // See also TsmSetElementPointer()
  if( !context.ind )
    return TFailure;

  /* Send message DELETE element */
  TsmSendMessage( context.n->elem.el, Delete, context.n->elem.data, 0 );

  node = context.n;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  /* make previous node as current node */
  context.n = node->prev;
  context.ind--;

  /* Decrement the no of elements */
  context.s->elemListHead.header.num--;

  /* free the linked list node */
  delete node;

  return TSuccess;
}

TStatus
TsmDeleteElementsBetweenLabels( Tint label_id1, Tint label_id2 )
/* label_id1 & label_id2 exclusive */
{
  Tint elem1, elem2;

  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  TsmSetElementPointer( 0 );

  if( TsmSetElementPointerAtLabel( label_id1 ) == TFailure)
    return TFailure;
  elem1 = context.ind;

  if( TsmSetElementPointerAtLabel( label_id2 ) == TFailure)
    return TFailure;
  elem2 = context.ind;

  TsmDeleteElementRange( elem1, elem2 );

  return TSuccess;
}

TStatus
TsmDeleteElementRange( Tint elem1, Tint elem2 ) /* elem1 & elem2 exclusive */
{
  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  TsmSetElementPointer( elem2 - 1 );
  while( context.ind != elem1 && context.ind )
    TsmDeleteElement();

  return TSuccess;
}

TStatus
TsmSetElementPointer( Tint index )
{
  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  /* Set static variable accordingly */
  if( context.ind == index ){
    return TSuccess;
  }
  if( index >= context.s->elemListHead.header.num ){
    index = context.s->elemListHead.header.num;
    context.n = context.s->elemListHead.prev;
  }
  else if( index <= 0 ){
    index = 0;
    context.n = (tsm_node)&(context.s->elemListHead);
  }
  else{
    Tint     a, b, c, d;
    tsm_node node;

    a = index - 0;
    b = context.ind - index; if( b < 0 ) b = -b; /* b = labs(b) */
    c = context.s->elemListHead.header.num - index;
    d = ( a < b ) ? ( a < c ? a : c ) : ( b < c ? b : c ) ;

    if( a == d ){
      /* Traverse from head in forward direction */
      d = index-1;
      node = context.s->elemListHead.next;
      while( d-- ){
        node = node->next;
      }
    }
    else if( b == d ){
      /* Traverse from current node in appropriate direction */
      node = context.n;
      d    = context.ind;
      if( context.ind < index ){
        while( d != index ){
          node = node->next;
          d++;
        }
      }
      else{
        while( d != index ){
          node = node->prev;
          d--;
        }
      }
    }
    else{ /* ( c == d ) */
      /* Traverse from head in backward direction */
      d = context.s->elemListHead.header.num;
      node = context.s->elemListHead.prev;
      while( d != index ){
        node = node->prev;
        d--;
      }
    }

    context.n = node;
  }

  context.ind = index;

  return TSuccess;
}


TStatus
TsmSetElementPointerAtLabel( Tint label_id )
{
  Tint   i, num;
  tsm_node  node;

  if( context.ind == TsmStructureNotOpen )
    return TFailure; /* No structure currently open */

  num = context.s->elemListHead.header.num;
  for( i=context.ind+1, node=context.n->next; i<=num; i++, node=node->next ){
    if( node->elem.el == TelLabel && node->elem.data.ldata == label_id ){
      context.ind = i;
      context.n = node;
      return TSuccess;
    }
  }

  return TFailure;
}

TStatus
TsmOffsetElementPointer( Tint offset )
{
  return TsmSetElementPointer( context.ind + offset );
}

TStatus
TsmGetStructure( Tint stid, Tint *num, tsm_node *n )
{
  if ( _StructMap.IsEmpty() )
    return TFailure; /* No structure created yet */

  if ( !_StructMap.IsBound(stid) )
    return TFailure; /* Non-existent structure */

  tsm_struct s = _StructMap.Find(stid);

  *num = s ? s->elemListHead.header.num : 0;
  // Here we cannot return a pointer to the structure's list head,
  // as it will wrongly interpreted as a regular pointer to TSM_NODE,
  // thus returning null pointer - this should be checked by callers!
  *n = s ? s->elemListHead.next : 0;

  return TSuccess;
}

static TStatus
GetDepth( Tint stid, Tint *depth )
{
  tsm_struct     s;
  Tint           i, d, b, num;

  if ( _StructMap.IsEmpty() )
    return TFailure; /* No structure created yet */

  if ( !_StructMap.IsBound(stid) )
    return TFailure; /* Non-existent structure */

  s = _StructMap.Find(stid);

  (*depth)++;
  d = b = *depth;

  if( s ){
    num = s->elemListHead.header.num;
    tsm_node node = s->elemListHead.next;
    for( i = 0; i < num; i++ ){
      if( node->elem.el == TelExecuteStructure ){
        GetDepth( node->elem.data.ldata, &d );
        if( *depth < d )
          *depth = d;
      }
      d = b;
      node = node->next;
    }
  }

  return TSuccess;
}


TStatus
TsmGetStructureDepth( Tint stid, Tint *depth )
{
  *depth = 0;

  GetDepth( stid, depth );
  return TSuccess;
}

TStatus
TsmGetCurElem( TSM_ELEM *elem )
{
  // Zero ind means no current element
  if( context.ind == TsmStructureNotOpen || 
      !context.ind )
    return TFailure;

  *elem = context.n->elem;
  return TSuccess;
}

TStatus
TsmGetCurElemPtr( Tint *ptr )
{
  // Zero ind means no current element
  if( context.ind == TsmStructureNotOpen ||
      !context.ind )
    return TFailure;

  *ptr = context.ind;
  return TSuccess;
}
