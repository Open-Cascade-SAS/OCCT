//--------------------------------------------------------------------
//
//  File Name : IGESGeom_RuledSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_RuledSurface.ixx>


    IGESGeom_RuledSurface::IGESGeom_RuledSurface ()    {  }


    void IGESGeom_RuledSurface::Init
  (const Handle(IGESData_IGESEntity)& aCurve,
   const Handle(IGESData_IGESEntity)& anotherCurve,
   const Standard_Integer             aDirFlag,
   const Standard_Integer             aDevFlag)
{
  theCurve1  = aCurve;
  theCurve2  = anotherCurve;
  theDirFlag = aDirFlag;
  theDevFlag = aDevFlag;
  InitTypeAndForm(118,FormNumber());
//    FormNumber 0-1 : Ruling spec.  0/Arc Length  1/Parameter
}

    void  IGESGeom_RuledSurface::SetRuledByParameter (const Standard_Boolean F)
{
  InitTypeAndForm(118, (F ? 1 : 0));
}

    Handle(IGESData_IGESEntity) IGESGeom_RuledSurface::FirstCurve () const
{
  return theCurve1;
}

    Handle(IGESData_IGESEntity) IGESGeom_RuledSurface::SecondCurve () const
{
  return theCurve2;
}

    Standard_Integer IGESGeom_RuledSurface::DirectionFlag () const
{
  return theDirFlag;
}

    Standard_Boolean IGESGeom_RuledSurface::IsDevelopable () const
{
  return (theDevFlag == 1);
}

    Standard_Boolean  IGESGeom_RuledSurface::IsRuledByParameter () const
{
  return (FormNumber() != 0);
}
