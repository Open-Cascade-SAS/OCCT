
// File:	MAT2d_BiInt.cxx
// Created:	Fri Nov 19 15:23:43 1993
// Author:	Yves FRICAUD
//		<yfr@phylox>

# include <MAT2d_BiInt.ixx>

MAT2d_BiInt::MAT2d_BiInt(const Standard_Integer I1,
			 const Standard_Integer I2)
     :i1(I1),i2(I2)
{
}


Standard_Integer  MAT2d_BiInt::FirstIndex()const 
{
  return i1;
}

Standard_Integer  MAT2d_BiInt::SecondIndex()const
{
  return i2;
}

void  MAT2d_BiInt::FirstIndex(const Standard_Integer I1)
{
  i1 = I1;
}

void  MAT2d_BiInt::SecondIndex(const Standard_Integer I2)
{
  i2 = I2;
}

Standard_Boolean MAT2d_BiInt::IsEqual(const MAT2d_BiInt& B)
const 
{
  return (i1 == B.FirstIndex() && i2 == B.SecondIndex());
}
     
