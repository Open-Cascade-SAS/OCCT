#include <IGESData_LevelListEntity.ixx>
// LevelListEntity ne sert qu'au controle de type (pour le directory part)


    Standard_Boolean  IGESData_LevelListEntity::HasLevelNumber
  (const Standard_Integer level) const
{
  Standard_Integer nb = NbLevelNumbers();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (LevelNumber(i) == level) return Standard_True;
  }
  return Standard_False;
}

