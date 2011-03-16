#include <GProp.ixx>

#include <Standard_DimensionError.hxx>

#include <gp.hxx>
#include <gp_XYZ.hxx>


void GProp::HOperator (

		       const gp_Pnt& G, 
		       const gp_Pnt& Q, 
		       const Standard_Real Mass,
		       gp_Mat&       Operator

) {

  gp_XYZ QG = G.XYZ() - Q.XYZ();
  Standard_Real Ixx = QG.Y() * QG.Y() + QG.Z() * QG.Z();
  Standard_Real Iyy = QG.X() * QG.X() + QG.Z() * QG.Z();
  Standard_Real Izz = QG.Y() * QG.Y() + QG.X() * QG.X();
  Standard_Real Ixy =  - QG.X() * QG.Y();
  Standard_Real Iyz =  - QG.Y() * QG.Z();
  Standard_Real Ixz =  - QG.X() * QG.Z();
  Operator.SetCols (gp_XYZ (Ixx, Ixy, Ixz), gp_XYZ (Ixy, Iyy, Iyz),
                    gp_XYZ (Ixz, Iyz, Izz));
  Operator.Multiply (Mass);
}
