//--------------------------------------------------------------------
//
//  File Name : IGESAppli_Node.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
