#include <Vrml_MatrixTransform.ixx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <Precision.hxx>

 Vrml_MatrixTransform::Vrml_MatrixTransform()
{
//  SetValues(me : in out;
//    	        a11, a12, a13, a14,
//	        a21, a22, a23, a24,
//	        a31, a32, a33, a34 : Real;
//	     Tolang, TolDist : Real)

  gp_Trsf T;
  T.SetValues ( 1, 0, 0, 0,
	        0, 1, 0, 0,
	        0, 0, 1, 0,
	        Precision::Angular(), Precision::Confusion());
  T.SetScaleFactor(1); 

  myMatrix = T;
}

 Vrml_MatrixTransform::Vrml_MatrixTransform(const gp_Trsf& aMatrix)
{
  myMatrix = aMatrix;
}

 void Vrml_MatrixTransform::SetMatrix(const gp_Trsf& aMatrix) 
{
  myMatrix = aMatrix;
}

 gp_Trsf Vrml_MatrixTransform::Matrix() const
{
  return myMatrix;
}

 Standard_OStream& Vrml_MatrixTransform::Print(Standard_OStream& anOStream) const
{
  Standard_Integer i,j;
  anOStream  << "MatrixTransform {" << endl;

  if ( Abs(myMatrix.Value(1,1) - 1) > 0.0000001 || Abs(myMatrix.Value(2,1) - 0) > 0.0000001 || Abs(myMatrix.Value(3,1) - 0) > 0.0000001 || 
       Abs(myMatrix.Value(1,2) - 0) > 0.0000001 || Abs(myMatrix.Value(2,2) - 1) > 0.0000001 || Abs(myMatrix.Value(3,2) - 0) > 0.0000001 || 
       Abs(myMatrix.Value(1,3) - 0) > 0.0000001 || Abs(myMatrix.Value(2,3) - 0) > 0.0000001 || Abs(myMatrix.Value(3,3) - 1) > 0.0000001 || 
       Abs(myMatrix.Value(1,4) - 0) > 0.0000001 || Abs(myMatrix.Value(2,4) - 0) > 0.0000001 || Abs(myMatrix.Value(3,4) - 0) > 0.0000001 )
    {
      anOStream  << "    matrix" << '\t';

      for ( j = 1; j <=4; j++ )
	{
	  for ( i = 1; i <=3; i++ )
	    {
//  Value (me; Row, Col : Integer)   returns Real
	      anOStream << myMatrix.Value(i,j) << ' ';
	    }
	  if (j!=4) 
	    {
	      anOStream  << '0' << endl;
	      anOStream  << "\t\t";
	    }
	  else 
	    {
	      anOStream  << myMatrix.ScaleFactor() << endl;
	    }
	}
    }
  anOStream  << '}' << endl;
  return anOStream;
}
