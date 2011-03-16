// File:	MeshVS_BuilderPriority.hxx
// Created:	Thu Dec 11 2003
// Author:	Alexander SOLOVYOV
// Copyright:	Open CASCADE 2003

#ifndef MeshVS_BUILDERPRIORITY_HXX
#define MeshVS_BUILDERPRIORITY_HXX

Standard_EXPORT typedef Standard_Integer MeshVS_BuilderPriority;

enum
{
  MeshVS_BP_Mesh       =  5,
  MeshVS_BP_NodalColor = 10,
  MeshVS_BP_ElemColor  = 15,
  MeshVS_BP_Text       = 20,
  MeshVS_BP_Vector     = 25,
  MeshVS_BP_User       = 30,
  MeshVS_BP_Default    = MeshVS_BP_User

};

#endif
