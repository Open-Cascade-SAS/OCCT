#include <Prs3d_Arrow.ixx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void Prs3d_Arrow::Draw(const Handle(Prs3d_Presentation)& aPresentation,
                       const gp_Pnt& aLocation,
		       const gp_Dir& aDirection,
                       const Quantity_PlaneAngle anAngle,
    		       const Quantity_Length aLength) {

  Quantity_Length dx,dy,dz;  aDirection.Coord(dx,dy,dz);
//
// Point of the arrow:
  Quantity_Length xo,yo,zo;  aLocation.Coord(xo,yo,zo);

// Center of the base circle of the arrow:
  Quantity_Length xc = xo - dx * aLength;
  Quantity_Length yc = yo - dy * aLength;
  Quantity_Length zc = zo - dz * aLength;

// Construction of i,j mark for the circle:
  Quantity_Length xn=0., yn=0., zn=0.;

  if ( Abs(dx) <= Abs(dy) && Abs(dx) <= Abs(dz)) xn=1.;
  else if ( Abs(dy) <= Abs(dz) && Abs(dy) <= Abs(dx)) yn=1.;
  else zn=1.;
  Quantity_Length xi = dy * zn - dz * yn;
  Quantity_Length yi = dz * xn - dx * zn;
  Quantity_Length zi = dx * yn - dy * xn;

  Quantity_Length Norme = sqrt ( xi*xi + yi*yi + zi*zi );
  xi = xi / Norme; yi = yi / Norme; zi = zi/Norme;

  Quantity_Length  xj = dy * zi - dz * yi;
  Quantity_Length  yj = dz * xi - dx * zi;
  Quantity_Length  zj = dx * yi - dy * xi;

  Standard_Integer NbPoints = 15;

  Graphic3d_Array1OfVertex VN(1,NbPoints+1);
  Graphic3d_Array1OfVertex V2(1,2);
  V2(1).SetCoord(xo,yo,zo);

  Quantity_Length x,y,z;
  Standard_Real cosinus,sinus, Tg=tan(anAngle);

  for (Standard_Integer i = 1 ; i <= NbPoints ; i++) {

    cosinus = cos ( 2 * PI / NbPoints * (i-1) );   
    sinus   = sin ( 2 * PI / NbPoints * (i-1) );

    x = xc + (cosinus * xi + sinus * xj) * aLength * Tg;
    y = yc + (cosinus * yi + sinus * yj) * aLength * Tg;
    z = zc + (cosinus * zi + sinus * zj) * aLength * Tg;

    VN(i).SetCoord(x,y,z);
    if(i==1) VN(NbPoints+1).SetCoord(x,y,z);
    V2(2).SetCoord(x,y,z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V2);
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(VN);
}

//=======================================================================
//function : Fill
//purpose  : 
//=======================================================================

void Prs3d_Arrow::Fill(const Handle(Prs3d_Presentation)& /*aPresentation*/,
                       const gp_Pnt& /*aLocation*/,
                       const gp_Dir& /*aDirection*/,
                       const Quantity_PlaneAngle /*anAngle*/,
    		       const Quantity_Length /*aLength*/)
{
}
