// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_FaceAttribute_HeaderFile
#define _BRepMesh_FaceAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <BRepMesh_Collections.hxx>
#include <Standard_Transient.hxx>


//! auxiliary class for FastDiscret and FastDiscretFace classes <br>
class BRepMesh_FaceAttribute : public Standard_Transient
{
public:

  Standard_EXPORT BRepMesh_FaceAttribute();
  
  inline Standard_Real& GetDefFace()
  {
    return mydefface;
  }

  inline Standard_Real& GetUMin()
  {
    return myumin;
  }

  inline Standard_Real& GetVMin()
  {
    return myvmin;
  }

  inline Standard_Real& GetUMax()
  {
    return myumax;
  }

  inline Standard_Real& GetVMax()
  {
    return myvmax;
  }

  inline Standard_Real& GetDeltaX()
  {
    return mydeltaX;
  }

  inline Standard_Real& GetDeltaY()
  {
    return mydeltaY;
  }

  inline Standard_Real& GetMinX()
  {
    return myminX;
  }

  inline Standard_Real& GetMinY()
  {
    return myminY;
  }

  inline BRepMeshCol::HClassifier& GetClassifier()
  {
    return myclassifier;
  }


  DEFINE_STANDARD_RTTI(BRepMesh_FaceAttribute)

private: 

  Standard_Real mydefface;
  Standard_Real myumin;
  Standard_Real myumax;
  Standard_Real myvmin;
  Standard_Real myvmax;
  Standard_Real mydeltaX;
  Standard_Real mydeltaY;
  Standard_Real myminX;
  Standard_Real myminY;
  BRepMeshCol::HClassifier myclassifier;
};

DEFINE_STANDARD_HANDLE(BRepMesh_FaceAttribute, Standard_Transient)

#endif
