//--------------------------------------------------------------------
//
//  File Name : IGESGeom_TransformationMatrix.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_TransformationMatrix.ixx>
#include <Standard_OutOfRange.hxx>


    IGESGeom_TransformationMatrix::IGESGeom_TransformationMatrix ()    {  }


    void IGESGeom_TransformationMatrix::Init
  (const Handle(TColStd_HArray2OfReal)& aMatrix)
{
  if(aMatrix.IsNull()) {
    theData = new TColStd_HArray2OfReal(1,3,1,4);
    theData->Init(0.0);
    Standard_Integer i =1;
    for( ;i <=3; i++)
      theData->SetValue(i,i,1.0);
    
  }
  if ((aMatrix->RowLength() != 4) || (aMatrix->ColLength() != 3))
    Standard_DimensionMismatch::Raise
  ("IGESGeom_TransformationMatrix : Init");

  
  
  theData = aMatrix;
  if(theData.IsNull()) {
    return;
  }
  InitTypeAndForm(124,FormNumber());
}

    void  IGESGeom_TransformationMatrix::SetFormNumber (const Standard_Integer fm)
{
  if(theData.IsNull()) 
    cout<<"Inavalid Transformation Data"<<endl;
  if ((fm < 0 || fm > 1) && (fm < 10 || fm > 12)) Standard_OutOfRange::Raise
    ("IGESGeom_TransformationMatrix : SetFormNumber");
  InitTypeAndForm(124,fm);
}


    Standard_Real  IGESGeom_TransformationMatrix::Data
  (const Standard_Integer I, const Standard_Integer J) const
{
  return theData->Value(I,J);
}

    gp_GTrsf IGESGeom_TransformationMatrix::Value () const
{
  gp_GTrsf Matrix;
  if(!theData.IsNull()) {
    for (Standard_Integer I = 1; I <= 3; I++) {
      for (Standard_Integer J = 1; J <= 4; J++) {
        Matrix.SetValue(I, J, theData->Value(I, J));
      }
    }
  }
  
  return Matrix;
}
