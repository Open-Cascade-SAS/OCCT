#include "stdafx.h"

#include "Fonc.hxx"

//=======================================================================
//function : grid2surf
//purpose  : grid2surf S FileName
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================

Standard_Boolean grid2surf(Standard_CString ShapeName,Handle(Geom_BSplineSurface)& S )
{
  Handle(TColStd_HArray2OfReal) H;  

  Standard_Real    xmin, ymin, Dx, Dy;
  H = ReadRegularGrid(ShapeName, xmin, ymin, Dx, Dy);
  GeomAPI_PointsToBSplineSurface MkSurf;
  MkSurf.Interpolate(H->Array2(), xmin, Dx, ymin, Dy);
  S = MkSurf.Surface();
  return Standard_True;
}


//=======================================================================
//function : grid2surf
//purpose  : 
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================

Standard_Boolean grid2surf(const CString  aCStringShapeName,
			   Handle(Geom_BSplineSurface)& S )
{
    char tmp_char[256] ="";
    strcpy_s(tmp_char,aCStringShapeName);
    int i = 0, len = strlen(tmp_char);
    while (i < len)
    {
        if (tmp_char[i] == '\\')
            tmp_char[i] = '/';
        i++;
    }
    Standard_CString aFileName = tmp_char;	
    return grid2surf(aFileName,S);
}

//=======================================================================
//function : ReadRegularGrid
//purpose  : 
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================
Handle(TColStd_HArray2OfReal) ReadRegularGrid(Standard_CString FileName,
					      Standard_Real& xmin,
					      Standard_Real& ymin,
					      Standard_Real& Deltax,
					      Standard_Real& Deltay)
{
  Handle(TColStd_HArray2OfReal) H;
  Standard_Integer              R1 = 1, R2, C1 = 1, C2, R, C;
  Standard_Real                 x, y, z;

  xmin = ymin = 10000000;
  
  FILE *fp = NULL;
  fopen_s(&fp, FileName, "r");

  if (fp) 
    {
      fscanf_s(fp, "%d %d", &R2, &C2);
      cout << "Reading Grid : ( " << R2 << " , " << C2 << " )." << endl;
      
      H = new TColStd_HArray2OfReal(C1, C2, R1, R2);
      
      Standard_Real	FirstX;
      Standard_Real	FirstY;
      
      for(R = R1; R <= R2; R++) 
	{
	  for(C = C1; C <= C2; C++) 
	    {
	      fscanf_s(fp, "%lf %lf %lf ", &x, &y, &z);
	    
	      if(R == 1 && C == 1)
		{	
		  FirstX=x; 
		  FirstY=y;
		}

	      // First step for X
	      if(R == 1 && C == 2)
		Deltax = x - FirstX;

	      // First step for Y
	      if(R == 2 && C == 1)
		Deltay = y - FirstY;
	
	      //	H->SetValue(R, C, z);
	      H->SetValue(C, R, z);
	      if(x < xmin) xmin = x;
	      if(y < ymin) ymin = y;
	    }
	}
      
      cout << "Deltax = " << Deltax << endl;
      cout << "Deltay = " << Deltay << endl;
      
      fclose(fp);
    } 
  else 
    {
      cout << "cannot open file : " << FileName << endl;
    }
  return H;
}

//=======================================================================
//function : ReadRegularGrid
//purpose  : 
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================
Handle(TColStd_HArray2OfReal) ReadRegularGrid(const CString aCStringFileName,
                                              Standard_Real& xmin,
                                              Standard_Real& ymin,
                                              Standard_Real& Deltax,
                                              Standard_Real& Deltay)
{
    char tmp_char[256] ="";
    strcpy_s(tmp_char,aCStringFileName);
    int i = 0, len = strlen(tmp_char);
    while (i < len)
    {
        if (tmp_char[i] == '\\')
            tmp_char[i] = '/';
        i++;
    }
    Standard_CString aFileName = tmp_char;	
    Handle(TColStd_HArray2OfReal) H;
    H = ReadRegularGrid(aFileName,xmin,ymin,Deltax,Deltay);
    return H;
}
