//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SolidInstance.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SolidInstance.ixx>


    IGESSolid_SolidInstance::IGESSolid_SolidInstance ()    {  }


    void  IGESSolid_SolidInstance::Init
  (const Handle(IGESData_IGESEntity)& anEntity)
{
  theEntity = anEntity;
  InitTypeAndForm(430,0);
}

    Standard_Boolean  IGESSolid_SolidInstance::IsBrep () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_SolidInstance::SetBrep (const Standard_Boolean brep)
      {  InitTypeAndForm(430, (brep ? 1 : 0));  }

    Handle(IGESData_IGESEntity)  IGESSolid_SolidInstance::Entity () const
{
  return theEntity;
}
