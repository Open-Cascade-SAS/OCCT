// Created on: 2001-07-17
// Created by: Julia DOROVSKIKH <jfa@hotdox.nnov.matra-dtv.fr>
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlObjMgt_Persistent_HeaderFile
#define _XmlObjMgt_Persistent_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
#include <XmlObjMgt_DOMString.hxx>


//! root for XML-persistence
class XmlObjMgt_Persistent 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! empty constructor
  Standard_EXPORT XmlObjMgt_Persistent();
  
  //! constructor
  Standard_EXPORT XmlObjMgt_Persistent(const XmlObjMgt_Element& theElement);
  
  //! constructor from sub-element of Element referenced by theRef
  Standard_EXPORT XmlObjMgt_Persistent(const XmlObjMgt_Element& theElement, const XmlObjMgt_DOMString& theRef);
  
  //! myElement := <theType id="theID"/>
  Standard_EXPORT void CreateElement (XmlObjMgt_Element& theParent, const XmlObjMgt_DOMString& theType, const Standard_Integer theID);
  
  Standard_EXPORT void SetId (const Standard_Integer theId);
  
  //! return myElement
    const XmlObjMgt_Element& Element() const;
inline operator const XmlObjMgt_Element&() const;
  
  //! return myElement
    XmlObjMgt_Element& Element();
inline operator XmlObjMgt_Element&();
  
    Standard_Integer Id() const;




protected:





private:



  XmlObjMgt_Element myElement;
  Standard_Integer myID;


};


#include <XmlObjMgt_Persistent.lxx>





#endif // _XmlObjMgt_Persistent_HeaderFile
