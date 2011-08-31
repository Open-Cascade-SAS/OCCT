/*
File OpenGl_togl_parray.c
Created 16/06/2000 : ATS : G005
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT call_togl_parray
(
 CALL_DEF_GROUP *agroup,
 CALL_DEF_PARRAY *parray
 )
{
  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  call_subr_parray (parray);
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
  return;
}

//=======================================================================
//function : call_togl_parray_remove
//purpose  : Remove the driver's element corresponding to the primitives
//           array <thePArray> and clean its visualization data. The driver
//           clears all its references to array and stops displaying it.
//           <theGroup> is the group that has added <thePArray> to driver.
//=======================================================================

void EXPORT call_togl_parray_remove (CALL_DEF_GROUP*  theGroup,
                                     CALL_DEF_PARRAY* thePArray)
{
  CALL_DEF_PARRAY* anElData;
  Tint aBegId, aEndId, aCurId; 
  TSM_ELEM anElem;

  // set edit mode and open struct
  call_func_set_edit_mode (CALL_PHIGS_EDIT_REPLACE);
  call_func_open_struct (theGroup->Struct->Id);

  // get begin label
  call_func_set_elem_ptr (0);
  if (call_func_set_elem_ptr_label (theGroup->LabelBegin) == TFailure)
    return;
  call_func_inq_elem_ptr (&aBegId);

  // get end label
  if (call_func_set_elem_ptr_label (theGroup->LabelEnd) == TFailure)
    return;
  call_func_inq_elem_ptr (&aEndId);

  // iterate between labels and search for the array
  if (aBegId != aEndId)
  {
    // move one element back
    if (call_func_offset_elem_ptr (-1)   == TFailure ||
        call_func_inq_elem_ptr (&aCurId) == TFailure)
      return;

    // iterate from end label to begin label
    while (aCurId > aBegId)
    {
      call_func_inq_elem (&anElem);

      // compare element with the array
      if (anElem.el == TelParray && anElem.data.pdata == (void* )thePArray)
      {
        anElData = (CALL_DEF_PARRAY* )anElem.data.pdata;

        // validate for correct pointer
        if (anElData->num_bounds  == thePArray->num_bounds  && 
            anElData->num_edges   == thePArray->num_edges   &&
            anElData->num_vertexs == thePArray->num_vertexs &&
            anElData->type        == thePArray->type)
        {
          call_func_del_elem();
          break;
        }
      }
      else
      {
        call_func_offset_elem_ptr (-1);
        call_func_inq_elem_ptr (&aCurId);
      }
    }
  }

  call_func_close_struct();
}
