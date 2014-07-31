#include "stdafx.h"

#include "Fonc.hxx"

//=======================================================================
//function : grid2surf
//purpose  : grid2surf S FileName
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================

Standard_Boolean grid2surf(CString ShapeName, Handle(Geom_BSplineSurface)& S)
{
  CString aFileName = ShapeName;
  aFileName.Replace(L'\\', L'/');

  Handle(TColStd_HArray2OfReal) H;  

  Standard_Real    xmin, ymin, Dx, Dy;
  H = ReadRegularGrid (aFileName, xmin, ymin, Dx, Dy);
  GeomAPI_PointsToBSplineSurface MkSurf;
  MkSurf.Interpolate(H->Array2(), xmin, Dx, ymin, Dy);
  S = MkSurf.Surface();
  return Standard_True;
}

//=======================================================================
//function : ReadRegularGrid
//purpose  : 
//           cf jm-oliva@paris3.matra-dtv.fr  ( Merci JMO )
//=======================================================================
Handle(TColStd_HArray2OfReal) ReadRegularGrid(CString FileName,
					      Standard_Real& xmin,
					      Standard_Real& ymin,
					      Standard_Real& Deltax,
					      Standard_Real& Deltay)
{
  CString aFileName = FileName;
  aFileName.Replace (L'\\', L'/');

  Handle(TColStd_HArray2OfReal) H;
  Standard_Integer              R1 = 1, R2, C1 = 1, C2, R, C;
  Standard_Real                 x, y, z;

  xmin = ymin = 10000000;
  
  FILE *fp = NULL;
  _wfopen_s (&fp, aFileName, L"r");

  if (fp) 
    {
      fscanf_s(fp, "%d %d", &R2, &C2);
      cout << "Reading Grid : ( " << R2 << " , " << C2 << " )." << endl;
      
      H = new TColStd_HArray2OfReal(C1, C2, R1, R2);
      
      Standard_Real FirstX = 0.0;
      Standard_Real FirstY = 0.0;
      
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
