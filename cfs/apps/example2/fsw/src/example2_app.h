/*******************************************************************************
** File: example_app.h
**
** Purpose:
**   Provide definitions for the EXAMPLE application used in cFE training.
**
**
*******************************************************************************/

#ifndef _example2_app_h_
#define _example2_app_h_

/*
**   Include Files:
*/

#include "cfe.h"


/*
** Event message ID's.
*/
#define EXAMPLE2_INIT_INF_EID      1    /* start up message "informational" */

#define EXAMPLE2_NOOP_INF_EID      2    /* processed command "informational" */
#define EXAMPLE2_RESET_INF_EID     3
#define EXAMPLE2_NEW_CMD_INF_EID   4
  
#define EXAMPLE2_MID_ERR_EID       5    /* invalid command packet "error" */
#define EXAMPLE2_CC1_ERR_EID       6
#define EXAMPLE2_LEN_ERR_EID       7
#define EXAMPLE2_PIPE_ERR_EID      8

#define EXAMPLE2_EVT_COUNT         8    /* count of event message ID's */

/*
** Command packet command codes
*/
#define EXAMPLE2_NOOP_CC           0    /* no-op command */
#define EXAMPLE2_RESET_CC          1    /* reset counters */
#define EXAMPLE2_NEW_CMD_CC         2    /* Perform Routine Processing */

/*
** Software Bus defines
*/
#define EXAMPLE2_PIPE_DEPTH        12   /* Depth of the Command Pipe for Application */
#define EXAMPLE2_LIMIT_HK          2    /* Limit of HouseKeeping Requests on Pipe for Application */
#define EXAMPLE2_LIMIT_CMD         4    /* Limit of Commands on pipe for Application */


/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} EXAMPLE2_NoArgsCmd_t;

typedef struct
{
  uint8                num;

} EXAMPLE2_ArgsCmd_t;

/*************************************************************************/

// /*
// ** Type definition (generic "arguments" command)
// */
// typedef struct
// {
//   uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

// } EXAMPLE2_ArgsCmd_t;

/*************************************************************************/

/*
** Type definition (EXAMPLE housekeeping)
*/
typedef struct
{
  uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];

  /*
  ** Command interface counters
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

} OS_PACK EXAMPLE2_HkPacket_t;

/*************************************************************************/

/*
** Type definition (EXAMPLE app global data)
*/
typedef struct
{
  /*
  ** Command interface counters.
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

  /*
  ** Housekeeping telemetry packet
  */
  EXAMPLE2_HkPacket_t         HkPacket;

  /*
  ** Operational data (not reported in housekeeping).
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;
  
  /*
  ** RunStatus variable used in the main processing loop
  */
  uint32                RunStatus;


  /*
  ** Initialization data (not reported in housekeeping)
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  uint8                 LimitHK;
  uint8                 LimitCmd;

  CFE_EVS_BinFilter_t   EventFilters[EXAMPLE2_EVT_COUNT];

} EXAMPLE2_AppData_t;

/*************************************************************************/

/*
** Local function prototypes
**
** Note: Except for the entry point (EXAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void    EXAMPLE2_AppMain(void);
int32   EXAMPLE2_AppInit(void);
void    EXAMPLE2_AppPipe(CFE_SB_MsgPtr_t msg);

void    EXAMPLE2_HousekeepingCmd(CFE_SB_MsgPtr_t msg);

void    EXAMPLE2_NoopCmd(CFE_SB_MsgPtr_t msg);
void    EXAMPLE2_ResetCmd(CFE_SB_MsgPtr_t msg);
void    EXAMPLE2_NewCmd(CFE_SB_MsgPtr_t msg);
void    EXAMPLE2_RoutineProcessingCmd(CFE_SB_MsgPtr_t msg);

boolean EXAMPLE2_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _example2_app_h_ */

/************************/
/*  End of File Comment */
/************************/



