// Created on: 2007-07-31
// Created by: OCC Team
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Draw_Printer_HeaderFile
#define _Draw_Printer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Address.hxx>
#include <Message_Printer.hxx>
#include <Draw_Interpretor.hxx>
#include <Message_Gravity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class TCollection_ExtendedString;
class TCollection_AsciiString;


class Draw_Printer;
DEFINE_STANDARD_HANDLE(Draw_Printer, Message_Printer)

//! Implementation of Printer class with output
//! (Message_Messenge) directed to Draw_Interpretor
class Draw_Printer : public Message_Printer
{

public:

  
  //! Creates a printer connected to the interpretor.
  Standard_EXPORT Draw_Printer(const Draw_Interpretor& theTcl);
  
  //! Send a string message with specified trace level.
  //! The parameter putEndl specified whether end-of-line
  //! should be added to the end of the message.
  //! This method must be redefined in descentant.
  Standard_EXPORT virtual void Send (const TCollection_ExtendedString& theString, const Message_Gravity theGravity, const Standard_Boolean putEndl) const Standard_OVERRIDE;
  
  //! Send a string message with specified trace level.
  //! The parameter putEndl specified whether end-of-line
  //! should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const Standard_CString theString, const Message_Gravity theGravity, const Standard_Boolean putEndl) const Standard_OVERRIDE;
  
  //! Send a string message with specified trace level.
  //! The parameter putEndl specified whether end-of-line
  //! should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const TCollection_AsciiString& theString, const Message_Gravity theGravity, const Standard_Boolean putEndl) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draw_Printer,Message_Printer)

protected:




private:


  Standard_Address myTcl;


};







#endif // _Draw_Printer_HeaderFile
