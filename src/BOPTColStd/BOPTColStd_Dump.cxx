// File:	BOPTColStd_Dump.cxx
// Created:	Thu Aug  1 10:27:07 2002
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOPTColStd_Dump.ixx>
#include <stdlib.h>

//=======================================================================
// function: BOPTColStd::PrintMessage
// purpose: 
//=======================================================================
  void BOPTColStd_Dump::PrintMessage(const TCollection_AsciiString& aMessage)
{
  Standard_CString pCStr=aMessage.ToCString();
  //
  BOPTColStd_Dump::PrintMessage(pCStr); 
}
//=======================================================================
// function: BOPTColStd::PrintMessage
// purpose: 
//=======================================================================
  void BOPTColStd_Dump::PrintMessage(const Standard_CString aMessage)
{
  char* xr=getenv("BOP_PRINT_MESSAGES");
  if (xr!=NULL) {
    cout << aMessage << flush;
  } 
}
