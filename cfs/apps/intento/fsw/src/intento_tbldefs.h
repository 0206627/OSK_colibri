/************************************************************************
** Intento Table Definitions
**
*/

#ifndef _intento_tbldefs_
#define _intento_tbldefs_


#include "cfe.h"


/*
** Definition of Table Data Structures
*/
typedef struct
{
  uint8                 TblElement1;
  uint16                TblElement2;
  uint32                TblElement3;

} OS_PACK INTENTO_Tbl_1_t;

typedef struct
{
  int32                 TblElement1;
  int16                 TblElement2;
  int8                  TblElement3;

} OS_PACK INTENTO_Tbl_2_t;


#endif  /* _intento_tbldefs_ */

/************************/
/*  End of File Comment */
/************************/
