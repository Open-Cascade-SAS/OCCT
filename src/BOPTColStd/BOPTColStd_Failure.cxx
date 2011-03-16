// File:	BOPTColStd_Failure.cxx
// Created:	Fri May 25 12:50:22 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTColStd_Failure.ixx>

//=======================================================================
// function: BOPErrors_Failure::BOPErrors_Failure
// purpose: 
//=======================================================================
  BOPTColStd_Failure::BOPTColStd_Failure (const Standard_CString AString)
{
  if(AString) {
     myMessage = new Standard_Character[strlen(AString) + 1];
     strcpy(myMessage,AString);
  }
}

//=======================================================================
// function: Message
// purpose: 
//=======================================================================
  Standard_CString BOPTColStd_Failure::Message() const
{
  return myMessage;
}
