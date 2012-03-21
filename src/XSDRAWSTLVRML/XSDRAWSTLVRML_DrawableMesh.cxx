// Created on: 2004-06-11
// Created by: Alexander SOLOVYOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XSDRAWSTLVRML_DrawableMesh.ixx>

//================================================================
// Function : DrawOn
// Purpose  :
//================================================================
XSDRAWSTLVRML_DrawableMesh::XSDRAWSTLVRML_DrawableMesh( const Handle( MeshVS_Mesh )& aMesh )
{
  myMesh = aMesh;
}

//================================================================
// Function :
// Purpose  :
//================================================================
void XSDRAWSTLVRML_DrawableMesh::DrawOn(Draw_Display& d) const
{
}

//================================================================
// Function :
// Purpose  :
//================================================================
Handle( MeshVS_Mesh ) XSDRAWSTLVRML_DrawableMesh::GetMesh() const
{
  return myMesh;
}
