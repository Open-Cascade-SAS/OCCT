// Created on: 2002-11-19
// Created by: Vladimir ANIKIN
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _TDocStd_ApplicationDelta_HeaderFile
#define _TDocStd_ApplicationDelta_HeaderFile

#include <Standard.hxx>

#include <TDocStd_SequenceOfDocument.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>


class TDocStd_ApplicationDelta;
DEFINE_STANDARD_HANDLE(TDocStd_ApplicationDelta, Standard_Transient)


class TDocStd_ApplicationDelta : public Standard_Transient
{

public:

  
  Standard_EXPORT TDocStd_ApplicationDelta();
  
    TDocStd_SequenceOfDocument& GetDocuments();
  
    const TCollection_ExtendedString& GetName() const;
  
    void SetName (const TCollection_ExtendedString& theName);
  
  Standard_EXPORT void Dump (Standard_OStream& anOS) const;




  DEFINE_STANDARD_RTTIEXT(TDocStd_ApplicationDelta,Standard_Transient)

protected:




private:


  TDocStd_SequenceOfDocument myDocuments;
  TCollection_ExtendedString myName;


};


#include <TDocStd_ApplicationDelta.lxx>





#endif // _TDocStd_ApplicationDelta_HeaderFile
