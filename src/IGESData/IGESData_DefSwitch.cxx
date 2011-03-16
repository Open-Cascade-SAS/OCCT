#include <IGESData_DefSwitch.ixx>

//  DefSwitch : represente une definition, soit vide (-> valeur = 0),
//  soit comme rang dans une table (-> valeur > 0 ce rang),
//  soit comme reference (-> valeur < 0), la reference elle-meme est ailleurs


    IGESData_DefSwitch::IGESData_DefSwitch ()             {  theval = 0;  }

    void IGESData_DefSwitch::SetVoid ()                   {  theval = 0;  }

    void IGESData_DefSwitch::SetReference ()              {  theval = -1;  }

    void IGESData_DefSwitch::SetRank (const Standard_Integer val)
                                                          {  theval = val;  }

    IGESData_DefType IGESData_DefSwitch::DefType () const
{
  if (theval <  0) return IGESData_DefReference;
  if (theval >  0) return IGESData_DefValue;
  return IGESData_DefVoid;
}

    Standard_Integer IGESData_DefSwitch::Value () const   {  return theval;  }
