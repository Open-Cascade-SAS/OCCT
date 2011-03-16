#ifndef  OPENGL_TELEM_FILTERS_S
#define  OPENGL_TELEM_FILTERS_S

typedef  enum
{
  InvisFilter, HighlFilter, PickFilter
} TFilterSet;

/* Filter functions */ /* Wsid, Inclusion num, list, Exclusion num, list */
extern  TStatus TglSetHighlFilter( Tint, Tint, Tint*, Tint, Tint* );
extern  TStatus TglSetPickFilter( Tint, Tint, Tint*, Tint, Tint* );
extern  TStatus TglSetInvisFilter( Tint, Tint, Tint*, Tint, Tint* );

/* Nameset functions */
extern  TStatus TglNamesetPush( void );
extern  TStatus TglNamesetPop( void );
extern  TStatus TglNamesetAdd( Tint, Tint* );
extern  TStatus TglNamesetRemove( Tint, Tint* );
extern  TStatus TglDeleteFiltersForWS( Tint wsid );

/* Compare Nameset with filters */ /* Failure if not found in inclusion set */
extern  TStatus TglFilterNameset( Tint, TFilterSet ); /* Wsid, enum */

#endif
