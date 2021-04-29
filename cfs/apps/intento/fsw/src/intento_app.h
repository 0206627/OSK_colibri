/*******************************************************************************
** File: intento_app.h
**
** Purpose:
**   Provide definitions for the INTENTO application used in cFE training.
**
**
*******************************************************************************/

#ifndef _intento_app_h_
#define _intento_app_h_

/*
**   Include Files:
*/

#include "cfe.h"


/*
** Event message ID's.
*/
#define INTENTO_INIT_INF_EID      1    /* start up message "informational" */

#define INTENTO_NOOP_INF_EID      2    /* processed command "informational" */
#define INTENTO_RESET_INF_EID     3
//Ex#1 #define INTENTO_NEW_CMD_INF_EID   4
  
#define INTENTO_MID_ERR_EID       5    /* invalid command packet "error" */
#define INTENTO_CC1_ERR_EID       6
#define INTENTO_LEN_ERR_EID       7
#define INTENTO_PIPE_ERR_EID      8

#define INTENTO_EVT_COUNT         8    /* count of event message ID's */

/*
** Command packet command codes
*/
#define INTENTO_NOOP_CC           0    /* no-op command */
#define INTENTO_RESET_CC          1    /* reset counters */
//Ex#1 #define INTENTO_NEW_CMD_CC         2    /* Perform Routine Processing */

/*
** Software Bus defines
*/
#define INTENTO_PIPE_DEPTH        12   /* Depth of the Command Pipe for Application */
#define INTENTO_LIMIT_HK          2    /* Limit of HouseKeeping Requests on Pipe for Application */
#define INTENTO_LIMIT_CMD         4    /* Limit of Commands on pipe for Application */


/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} INTENTO_NoArgsCmd_t;

/*************************************************************************/

/*
** Type definition (INTENTO housekeeping)
*/
typedef struct
{
  uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];

  /*
  ** Command interface counters
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

} OS_PACK INTENTO_HkPacket_t;

/*************************************************************************/

/*
** Type definition (INTENTO app global data)
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
  INTENTO_HkPacket_t         HkPacket;

  /*
  ** Operational data (not reported in housekeeping).
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;
  CFE_SB_MsgPtr_t       AppCommMsgPtr;
  CFE_SB_PipeId_t       AppCommCmdPipe;

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

  CFE_EVS_BinFilter_t   EventFilters[INTENTO_EVT_COUNT];

  INTENTO_NoArgsCmd_t   CommPckt;

} INTENTO_AppData_t;

/*************************************************************************/

/*
** Local function prototypes
**
** Note: Except for the entry point (INTENTO_AppMain), these
**       functions are not called from any other source module.
*/
void    INTENTO_AppMain(void);
int32   INTENTO_AppInit(void);
void    INTENTO_AppPipe(CFE_SB_MsgPtr_t msg);

void    INTENTO_HousekeepingCmd(CFE_SB_MsgPtr_t msg);

void    INTENTO_NoopCmd(CFE_SB_MsgPtr_t msg);
void    INTENTO_ResetCmd(CFE_SB_MsgPtr_t msg);
void    INTENTO_RoutineProcessingCmd(CFE_SB_MsgPtr_t msg);

boolean INTENTO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);


#endif /* _intento_app_h_ */

/************************/
/*  End of File Comment */
/************************/



