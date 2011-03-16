/***********************************************************************

FONCTION :
----------
file OpenGl_tgl_util :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
06-03-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
29-04-96 : FMN ; Correction Warning.
30-01-97 : FMN ; Suppression call_util_init_indexes()

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <OpenGl_tgl_all.hxx>

#include <stdlib.h>
#include <string.h>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tsm.hxx>
#include <OpenGl_tgl_util.hxx>
#include <OpenGl_tgl_pick.hxx>
#include <OpenGl_telem_filters.hxx>

#include <OpenGl_context.hxx>

#ifdef WNT
#include <OpenGl_Extension.hxx>
#include <InterfaceGraphic_wntio.hxx>
#endif  /* WNT */

/*----------------------------------------------------------------------*/

Tintlst  inc_pick_filter;
Tintlst  exc_pick_filter;
Tintlst  inc_highl_filter;
Tintlst  exc_highl_filter;
Tintlst  inc_invis_filter;
Tintlst  exc_invis_filter;

/*----------------------------------------------------------------------*/

void
call_util_init_filters( Tint  wsid )
{
  Tint   pick_inc_names[1] = { CALL_DEF_STRUCTPICKABLE };
  Tint   highl_inc_names[1] = { CALL_DEF_STRUCTHIGHLIGHTED };
  Tint   invis_inc_names[1] = { CALL_DEF_STRUCTNOVISIBLE };

  Tint   pick_excl_names[1] = { CALL_DEF_STRUCTNOPICKABLE };

  inc_highl_filter.number = 1;
  inc_highl_filter.integers = highl_inc_names;
  exc_highl_filter.number = 0;
  exc_highl_filter.integers = 0;
  TglSetHighlFilter( wsid, 1, highl_inc_names, 0, 0 );

  inc_pick_filter.number = 1;
  inc_pick_filter.integers = pick_inc_names;
  exc_pick_filter.number = 1;
  exc_pick_filter.integers = pick_excl_names;
  TglSetPickFilter( wsid, 1, pick_inc_names, 1, pick_excl_names );

  inc_invis_filter.number = 1;
  inc_invis_filter.integers = invis_inc_names;
  exc_invis_filter.number = 0;
  exc_invis_filter.integers = 0;
  TglSetInvisFilter( wsid, 1, invis_inc_names, 0, 0 );

  return;
}

/*----------------------------------------------------------------------*/

Tint
call_util_test_structure( Tint stid )
{
  TStatus   stat;
  Tint      num;
  tsm_node  ptr;

  stat = TsmGetStructure( stid, &num, &ptr );
  if( stat == TSuccess )
  {
    if( num )
      return 2;
    return 1;
  }
  return 0;
}

/*----------------------------------------------------------------------*/

/*
call_util_osd_getenv (symbol, value)
char *symbol, *value

Gets an environment variable.
*/

EXPORT extern int    call_util_osd_getenv ( char *symbol,
                                           char *value,
                                           int lval )
{
  char *s;

  if ((s = getenv (symbol)) == NULL) return (0);
  if ((int)strlen (s) > lval) return (0);
  strcpy (value, s);
  return (1);
}
/*----------------------------------------------------------------------*/
