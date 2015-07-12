// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IGESData_FileRecognizer_HeaderFile
#define _IGESData_FileRecognizer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class IGESData_IGESEntity;
class Standard_NoSuchObject;
class IGESData_IGESType;


class IGESData_FileRecognizer;
DEFINE_STANDARD_HANDLE(IGESData_FileRecognizer, Standard_Transient)


class IGESData_FileRecognizer : public Standard_Transient
{

public:

  
  Standard_EXPORT Standard_Boolean Evaluate (const IGESData_IGESType& akey, Handle(IGESData_IGESEntity)& res);
  
  Standard_EXPORT Handle(IGESData_IGESEntity) Result() const;
  
  Standard_EXPORT void Add (const Handle(IGESData_FileRecognizer)& reco);




  DEFINE_STANDARD_RTTI(IGESData_FileRecognizer,Standard_Transient)

protected:

  
  Standard_EXPORT IGESData_FileRecognizer();
  
  Standard_EXPORT void SetOK (const Handle(IGESData_IGESEntity)& aresult);
  
  Standard_EXPORT void SetKO();
  
  Standard_EXPORT virtual void Eval (const IGESData_IGESType& akey) = 0;



private:


  Handle(IGESData_IGESEntity) theres;
  Standard_Boolean hasnext;
  Handle(IGESData_FileRecognizer) thenext;


};







#endif // _IGESData_FileRecognizer_HeaderFile
