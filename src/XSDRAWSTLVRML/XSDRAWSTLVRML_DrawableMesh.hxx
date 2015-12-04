// Created on: 2004-06-11
// Created by: Alexander SOLOVYOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XSDRAWSTLVRML_DrawableMesh_HeaderFile
#define _XSDRAWSTLVRML_DrawableMesh_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Drawable3D.hxx>
class MeshVS_Mesh;
class Draw_Display;


class XSDRAWSTLVRML_DrawableMesh;
DEFINE_STANDARD_HANDLE(XSDRAWSTLVRML_DrawableMesh, Draw_Drawable3D)


class XSDRAWSTLVRML_DrawableMesh : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT XSDRAWSTLVRML_DrawableMesh(const Handle(MeshVS_Mesh)& aMesh);
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(MeshVS_Mesh) GetMesh() const;




  DEFINE_STANDARD_RTTIEXT(XSDRAWSTLVRML_DrawableMesh,Draw_Drawable3D)

protected:




private:


  Handle(MeshVS_Mesh) myMesh;


};







#endif // _XSDRAWSTLVRML_DrawableMesh_HeaderFile
