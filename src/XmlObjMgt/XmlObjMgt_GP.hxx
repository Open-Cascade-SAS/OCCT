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

#ifndef _XmlObjMgt_GP_HeaderFile
#define _XmlObjMgt_GP_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XmlObjMgt_DOMString.hxx>
#include <Standard_Boolean.hxx>
class gp_Trsf;
class gp_Mat;
class gp_XYZ;


//! Translation of gp (simple geometry) objects
class XmlObjMgt_GP 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static XmlObjMgt_DOMString Translate (const gp_Trsf& aTrsf);
  
  Standard_EXPORT static XmlObjMgt_DOMString Translate (const gp_Mat& aMat);
  
  Standard_EXPORT static XmlObjMgt_DOMString Translate (const gp_XYZ& anXYZ);
  
  Standard_EXPORT static Standard_Boolean Translate (const XmlObjMgt_DOMString& aStr, gp_Trsf& T);
  
  Standard_EXPORT static Standard_Boolean Translate (const XmlObjMgt_DOMString& aStr, gp_Mat& T);
  
  Standard_EXPORT static Standard_Boolean Translate (const XmlObjMgt_DOMString& aStr, gp_XYZ& T);




protected:





private:





};







#endif // _XmlObjMgt_GP_HeaderFile
