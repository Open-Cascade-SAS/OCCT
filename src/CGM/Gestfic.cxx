#include <stdio.h>
#include <sys/stat.h>

#ifdef WNT
# include <io.h>
#endif /* WNT */

static FILE* filed;
/***************************************************OPEN_FILE**************/
FILE* OPEN_FILE (char* f_name, int TypeCgm)
{
  switch (TypeCgm){
    case 1:
    case 2:
      if ((filed = fopen(f_name,"wb" ))) {
#ifdef WNT
        chmod (f_name, S_IREAD | S_IWRITE);
#else
        chmod (f_name, 00777);
#endif
/*        setbuf (filed, NULL);*/
      }
      break;
    case 3:
      if ((filed = fopen(f_name,"w" ))) {
#ifdef WNT
        chmod (f_name, S_IREAD | S_IWRITE);
#else
        chmod (f_name, 00777);
#endif
/*        setbuf (filed, NULL);*/
      }
      break;
  }
  return filed;
}

/**************************************************CLOSE_FILE************/
void CLOSE_FILE ()
{
  fclose (filed);
}

/**************************************************getfd************/
FILE* getfd ()
{
  return filed;
}
