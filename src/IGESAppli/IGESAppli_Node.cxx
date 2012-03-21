// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_Node.ixx>
#include <gp_GTrsf.hxx>
#include <Interface_Macros.hxx>

IGESAppli_Node::IGESAppli_Node ()    {  }


    void  IGESAppli_Node::Init
  (const gp_XYZ& aCoord,
   const Handle(IGESGeom_TransformationMatrix)& aCoordSystem)
{
  theCoord  = aCoord;
  theSystem = aCoordSystem;
  InitTypeAndForm(134,0);
}

    gp_Pnt  IGESAppli_Node::Coord () const
{
  return  gp_Pnt(theCoord);
}

    Handle(IGESData_TransfEntity)  IGESAppli_Node::System () const
{
  //if Null, Global Cartesian Coordinate System
  return GetCasted(IGESData_TransfEntity,theSystem);
}

    Standard_Integer  IGESAppli_Node::SystemType () const
{
  if (theSystem.IsNull()) return 0;      // 0 Global Cartesien
  return (theSystem->FormNumber() - 9);  // 1 Cartesien, 2 Cylind. 3 Spher.
}


    gp_Pnt IGESAppli_Node::TransformedNodalCoord () const
{
  gp_XYZ tempCoord = Coord().XYZ();
  Handle(IGESData_TransfEntity) temp = System();
  if (!temp.IsNull())    temp->Value().Transforms(tempCoord);
  return gp_Pnt(tempCoord);
}
