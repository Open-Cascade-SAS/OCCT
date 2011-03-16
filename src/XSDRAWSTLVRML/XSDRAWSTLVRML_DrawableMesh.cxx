// File:	XSDRAWSTLVRML_DrawableMesh.cxx
// Created:	Thu June 11 2004
// Author:	Alexander SOLOVYOV
// Copyright:	 Open CASCADE 2004

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
