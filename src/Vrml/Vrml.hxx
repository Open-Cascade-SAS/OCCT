// Created on: 1996-12-23
// Created by: Alexander BRIVIN and Dmitry TARASOV
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_HeaderFile
#define _Vrml_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <Standard_CString.hxx>
class Vrml_SFImage;
class Vrml_SFRotation;
class Vrml_AsciiText;
class Vrml_Cone;
class Vrml_Cube;
class Vrml_Cylinder;
class Vrml_IndexedFaceSet;
class Vrml_IndexedLineSet;
class Vrml_PointSet;
class Vrml_Sphere;
class Vrml_Coordinate3;
class Vrml_FontStyle;
class Vrml_Info;
class Vrml_LOD;
class Vrml_Material;
class Vrml_MaterialBinding;
class Vrml_Normal;
class Vrml_NormalBinding;
class Vrml_Texture2;
class Vrml_Texture2Transform;
class Vrml_TextureCoordinate2;
class Vrml_ShapeHints;
class Vrml_MatrixTransform;
class Vrml_Rotation;
class Vrml_Scale;
class Vrml_Transform;
class Vrml_Translation;
class Vrml_OrthographicCamera;
class Vrml_PerspectiveCamera;
class Vrml_DirectionalLight;
class Vrml_PointLight;
class Vrml_SpotLight;
class Vrml_Group;
class Vrml_Separator;
class Vrml_Switch;
class Vrml_TransformSeparator;
class Vrml_WWWAnchor;
class Vrml_WWWInline;
class Vrml_Instancing;


//! Vrml package  implements the specification  of the
//! VRML ( Virtual  Reality Modeling Language ).  VRML
//! is a standard  language for describing interactive
//! 3-D objects and  worlds delivered across Internet.
//! Actual version of Vrml package have made for objects
//! of VRML version 1.0.
//! This package is used by VrmlConverter package.
//! The developer should  already be familiar with VRML
//! specification before using this package.
class Vrml 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Writes a header in anOStream (VRML file).
  //! Writes one line of commentary in  anOStream (VRML file).
  Standard_EXPORT static Standard_OStream& VrmlHeaderWriter (Standard_OStream& anOStream);
  
  Standard_EXPORT static Standard_OStream& CommentWriter (const Standard_CString aComment, Standard_OStream& anOStream);




protected:





private:




friend class Vrml_SFImage;
friend class Vrml_SFRotation;
friend class Vrml_AsciiText;
friend class Vrml_Cone;
friend class Vrml_Cube;
friend class Vrml_Cylinder;
friend class Vrml_IndexedFaceSet;
friend class Vrml_IndexedLineSet;
friend class Vrml_PointSet;
friend class Vrml_Sphere;
friend class Vrml_Coordinate3;
friend class Vrml_FontStyle;
friend class Vrml_Info;
friend class Vrml_LOD;
friend class Vrml_Material;
friend class Vrml_MaterialBinding;
friend class Vrml_Normal;
friend class Vrml_NormalBinding;
friend class Vrml_Texture2;
friend class Vrml_Texture2Transform;
friend class Vrml_TextureCoordinate2;
friend class Vrml_ShapeHints;
friend class Vrml_MatrixTransform;
friend class Vrml_Rotation;
friend class Vrml_Scale;
friend class Vrml_Transform;
friend class Vrml_Translation;
friend class Vrml_OrthographicCamera;
friend class Vrml_PerspectiveCamera;
friend class Vrml_DirectionalLight;
friend class Vrml_PointLight;
friend class Vrml_SpotLight;
friend class Vrml_Group;
friend class Vrml_Separator;
friend class Vrml_Switch;
friend class Vrml_TransformSeparator;
friend class Vrml_WWWAnchor;
friend class Vrml_WWWInline;
friend class Vrml_Instancing;

};







#endif // _Vrml_HeaderFile
