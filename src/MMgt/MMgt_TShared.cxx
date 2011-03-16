#include <MMgt_TShared.ixx>

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void MMgt_TShared::Delete() const
{
  delete (MMgt_TShared *)this;
}

