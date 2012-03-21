// Created on: 2003-12-11
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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
