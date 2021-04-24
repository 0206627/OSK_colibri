/************************************************************************
** Example Table Definitions
**
*/

#ifndef _example2_tbldefs_
#define _example2_tbldefs_


#include "cfe.h"


/*
** Definition of Table Data Structures
*/
typedef struct
{
  uint8                 TblElement1;
  uint16                TblElement2;
  uint32                TblElement3;

} OS_PACK EXAMPLE2_Tbl_1_t;

typedef struct
{
  int32                 TblElement1;
  int16                 TblElement2;
  int8                  TblElement3;

} OS_PACK EXAMPLE2_Tbl_2_t;


#endif  /* _example_tbldefs_ */

/************************/
/*  End of File Comment */
/************************/
