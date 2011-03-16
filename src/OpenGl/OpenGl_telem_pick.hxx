#ifndef OPENGL_TELEM_PICK_H
#define OPENGL_TELEM_PICK_H

typedef union
{
  unsigned  int  s[2];
  Tint   i;
} TEL_PACKED_NAME, *tel_packed_name;

typedef  enum
{
  TTopFirst, TBottomFirst
} TPickOrder;

typedef  struct
{
  Tint    el_num;
  Tint    pick_id;
  Tint    struct_id;
} TEL_PICK_PATH, *tel_pick_path;

typedef  struct
{
  Tint          depth;
  tel_pick_path pick_path;
} TEL_PICK_REPORT, *tel_pick_report;

extern Tint TglVpBeingPicked; /* defined in telem/pick */

extern TStatus TPick( Tint, Tint, Tint, Tfloat, Tfloat, /*wsid, x, y, apw, aph*/
                     TPickOrder, Tint, tel_pick_report );/*order, depth, rep*/

#endif
