// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Standard_ProgramError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>
#if !defined( WNT ) && !defined(__hpux) && !defined(HPUX)
#include <OSD_MailBox.ixx>
#include <OSD_WhoAmI.hxx>

#include <errno.h>

const OSD_WhoAmI Iam = OSD_WMailBox;

extern "C"{
int create_mailbox(char *,int, int (*)(...));
int open_mailbox(char *,int);
int remove_mailbox(int *, char *);
int write_mailbox(int *,char *,char *,int);
}


//====================== Create a mail box =========================

OSD_MailBox::OSD_MailBox(){
}



//============== Create an instanciated mail box ===================

OSD_MailBox::OSD_MailBox(const TCollection_AsciiString& box_name,
                         const Standard_Integer         box_size,
                         const OSD_Function&            async_function){


 if (!box_name.IsAscii())
  Standard_ConstructionError::Raise("OSD_MailBox::OSD_MailBox : box_name");

 myName = box_name;

 if (box_size <= 0)
  Standard_ProgramError::Raise("OSD_MailBox::OSD_MailBox : box_size");
 mySize = box_size;

 if (async_function == NULL)
  Standard_NullObject::Raise("OSD_MailBox::OSD_MailBox : async_function");
 myFunc = async_function;

}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void OSD_MailBox::Build()
{
  Standard_PCharacter pStr;
  pStr=(Standard_PCharacter)myName.ToCString();
  myId = create_mailbox(pStr, (int)mySize, myFunc);
  
  if (myId == 0)
    myError.SetValue (errno, Iam, "OSD_MailBox::Build");
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
void OSD_MailBox::Open ( const TCollection_AsciiString& box_name,
                         const Standard_Integer box_size)
{

 // Test function security 

 if (box_name == NULL)
  Standard_NullObject::Raise("OSD_MailBox::Open : box_name");

 if (!box_name.IsAscii())
  Standard_ConstructionError::Raise("OSD_MailBox::Open : box_name");

 myName = box_name;

 if (box_size <= 0)
  Standard_ProgramError::Raise("OSD_MailBox::Open : box_size");
 mySize = box_size;

 Standard_PCharacter pStr;
 //
 pStr=(Standard_PCharacter)box_name.ToCString();
 myId = open_mailbox(pStr, (int)box_size );
 if (myId == 0)
   myError.SetValue (errno, Iam, "OSD_MailBox::Open");
  
}





//====================== Close a mail box =======================

void OSD_MailBox::Delete ()
{
 
 if (myError.Failed()) myError.Perror();

 if (myId == 0)
  Standard_ProgramError::Raise("OSD_MailBox::Delete : mail box not opened/created");

 if (myName == NULL) 
  Standard_ProgramError::Raise("OSD_MailBox::Delete : mail box not opened/created");

 Standard_PCharacter pStr;
 pStr=(Standard_PCharacter)myName.ToCString();
 if (remove_mailbox((int *)&myId, pStr) == 0)
  myError.SetValue(errno, Iam, "OSD_MailBox::Delete");

 myId = 0;
}



//====================== Write into a mail box =======================

void OSD_MailBox::Write(const TCollection_AsciiString& message,
                        const Standard_Integer length)
{


 if (length <= 0 || length > mySize) 
  Standard_ProgramError::Raise("OSD_Mailbox::Write : bad length");

 Standard_PCharacter pStr, pStrM;
 //
 pStr=(Standard_PCharacter)myName.ToCString();
 pStrM=(Standard_PCharacter)message.ToCString();
 //
 if (write_mailbox((int *)&myId, pStr, pStrM, (int)length) == 0)
  myError.SetValue(errno, Iam, "OSD_Mailbox::Write");
}


void OSD_MailBox::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_MailBox::Failed()const{
 return( myError.Failed());
}

void OSD_MailBox::Perror() {
 myError.Perror();
}


Standard_Integer OSD_MailBox::Error()const{
 return( myError.Error());
}
#endif



