/*******************************************************************************
** File: intento_app.c
**
** Purpose:
**   Provide functions for the INTENTO application used in cFE training.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "intento_app.h"
#include "intento_app_perfids.h"
#include "intento_app_msgids.h"


/*
** System Header files
*/
#include <string.h>

/*
** INTENTO global data
*/
INTENTO_AppData_t INTENTO_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void INTENTO_AppMain(void)
{
    int32 Status;
    
    /*
    ** Register the Application with Executive Services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(INTENTO_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Intialization fails, set the RunStatus to CFE_ES_APP_ERROR
    ** and the App will not enter the RunLoop.
    */
    Status = INTENTO_AppInit();
    if ( Status != CFE_SUCCESS )
    {
        INTENTO_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Application Main Loop. Call CFE_ES_RunLoop to check for changes
    ** in the Application's status. If there is a request to kill this
    ** App, it will be passed in through the RunLoop call.
    */
    while ( CFE_ES_RunLoop(&INTENTO_AppData.RunStatus) == TRUE )
    {
    
        /*
        ** Performance Log Exit Stamp.
        */
        CFE_ES_PerfLogExit(INTENTO_APPMAIN_PERF_ID);
   
        /*
        ** Pend on the arrival of the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&INTENTO_AppData.MsgPtr, INTENTO_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /*
        ** Performance Log Entry Stamp.
        */      
        CFE_ES_PerfLogEntry(INTENTO_APPMAIN_PERF_ID);

        /*
        ** Check the return status from the Software Bus. If a pend with timeout is 
        ** used in the CFE_SB_RcvMsg() call then this check needs to change.
        */        
        if (Status == CFE_SUCCESS)
        {
            /*
            ** Process Software Bus message. If there are fatal errors
            ** in command processing the command can alter the global 
            ** RunStatus variable to exit the main event loop.
            */
            INTENTO_AppPipe(INTENTO_AppData.MsgPtr);
            
            /* 
            ** Depending on the app you may want to add calls to functions
            ** that execute each loop.
            */

        }
        else
        {
            /*
            ** This is an intento of exiting on an error.
            ** Note that a SB read error is not always going to 
            ** result in an app quitting. 
            */
            CFE_EVS_SendEvent(INTENTO_PIPE_ERR_EID, CFE_EVS_ERROR,
                               "INTENTO: SB Pipe Read Error, INTENTO App Will Exit.");
            
            INTENTO_AppData.RunStatus = CFE_ES_APP_ERROR;
         }
                    
    } /* end while */

    /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(INTENTO_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(INTENTO_AppData.RunStatus);
    
} /* End of INTENTO_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_AppInit() -- INTENTO initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 INTENTO_AppInit(void)
{
    int32  Status;
    int32  ResetType;
    uint32 ResetSubType;

    /* 
    ** Determine Reset Type
    */
    ResetType = CFE_ES_GetResetType(&ResetSubType);
        
    /*
    ** Setup the RunStatus variable
    */
    INTENTO_AppData.RunStatus = CFE_ES_APP_RUN;
    
    /*
    ** Initialize app command execution counters.
    */
    INTENTO_AppData.CmdCounter = 0;
    INTENTO_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data.
    */
    strcpy(INTENTO_AppData.PipeName, "INTENTO_CMD_PIPE");
    INTENTO_AppData.PipeDepth = INTENTO_PIPE_DEPTH;

    INTENTO_AppData.LimitHK   = INTENTO_LIMIT_HK;
    INTENTO_AppData.LimitCmd  = INTENTO_LIMIT_CMD;

    /*
    ** Initialize event filter table for envents we want to filter.
    */
    
    INTENTO_AppData.EventFilters[0].EventID = INTENTO_RESET_INF_EID;
    INTENTO_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    
    INTENTO_AppData.EventFilters[1].EventID = INTENTO_CC1_ERR_EID;
    INTENTO_AppData.EventFilters[1].Mask    = CFE_EVS_EVERY_OTHER_TWO;
    
    INTENTO_AppData.EventFilters[2].EventID = INTENTO_LEN_ERR_EID;
    INTENTO_AppData.EventFilters[2].Mask    = CFE_EVS_FIRST_8_STOP;

    //Ex#1 INTENTO_AppData.EventFilters[3].EventID = INTENTO_NEW_CMD_INF_EID;
    //Ex#1 INTENTO_AppData.EventFilters[3].Mask    = CFE_EVS_EVERY_FOURTH_ONE;

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(INTENTO_AppData.EventFilters, 4,
                               CFE_EVS_BINARY_FILTER);
    
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("INTENTO App: Error Registering Events, RC = 0x%08X\n", Status);
       return ( Status );
    }
             
    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&INTENTO_AppData.HkPacket, INTENTO_HK_TLM_MID, sizeof(INTENTO_HkPacket_t), TRUE);
   
    /*
    ** Create Software Bus message pipe.
    */
    Status = CFE_SB_CreatePipe(&INTENTO_AppData.CmdPipe, INTENTO_AppData.PipeDepth, INTENTO_AppData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Could use an event at this point
       */
       CFE_ES_WriteToSysLog("INTENTO App: Error Creating SB Pipe, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(INTENTO_SEND_HK_MID,INTENTO_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("INTENTO App: Error Subscribing to HK Request, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to INTENTO ground command packets
    */
    Status = CFE_SB_Subscribe(INTENTO_CMD_MID,INTENTO_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("INTENTO App: Error Subscribing to INTENTO Command, RC = 0x%08X\n", Status);
       return ( Status );
    }
                 
    /*
    ** Application startup event message.
    */
    CFE_EVS_SendEvent(INTENTO_INIT_INF_EID,CFE_EVS_INFORMATION, "INTENTO: Application Initialized");
                         
    return(CFE_SUCCESS);

} /* End of INTENTO_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void INTENTO_AppPipe(CFE_SB_MsgPtr_t msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request.
        */
        case INTENTO_SEND_HK_MID:
            INTENTO_HousekeepingCmd(msg);
            break;

        /*
        ** INTENTO ground commands.
        */
        case INTENTO_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(msg);
            switch (CommandCode)
            {
                case INTENTO_NOOP_CC:
                    INTENTO_NoopCmd(msg);
                    break;

                case INTENTO_RESET_CC:
                    INTENTO_ResetCmd(msg);
                    break;
                    
                //Ex#1case INTENTO_NEW_CMD_CC:
                //Ex#1 INTENTO_NewCmd(msg);
                //Ex#1 break;

                default:
                    CFE_EVS_SendEvent(INTENTO_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(INTENTO_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            break;
    }

    return;

} /* End of INTENTO_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_HousekeepingCmd() -- On-board command (HK request)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void INTENTO_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(INTENTO_NoArgsCmd_t);
    uint16 i;

    /*
    ** Verify command packet length
    */
    if (INTENTO_VerifyCmdLength(msg, ExpectedLength))
    {
        /*
        ** Get command execution counters
        */
        INTENTO_AppData.HkPacket.CmdCounter = INTENTO_AppData.CmdCounter;
        INTENTO_AppData.HkPacket.ErrCounter = INTENTO_AppData.ErrCounter;

        /*
        ** Send housekeeping telemetry packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &INTENTO_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &INTENTO_AppData.HkPacket);

        /*
        ** This command does not affect the command execution counter
        */
        
    } /* end if */

    return;

} /* End of INTENTO_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_NoopCmd() -- INTENTO ground command (NO-OP)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void INTENTO_NoopCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(INTENTO_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (INTENTO_VerifyCmdLength(msg, ExpectedLength))
    {
        INTENTO_AppData.CmdCounter++;

        CFE_EVS_SendEvent(INTENTO_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "INTENTO Version 1.0.0: No-op command");
    }

    return;

} /* End of INTENTO_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_ResetCmd() -- INTENTO ground command (reset counters)             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void INTENTO_ResetCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(INTENTO_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (INTENTO_VerifyCmdLength(msg, ExpectedLength))
    {
        INTENTO_AppData.CmdCounter = 0;
        INTENTO_AppData.ErrCounter = 0;

        CFE_EVS_SendEvent(INTENTO_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "INTENTO: Reset Counters command");
    }

    return;

} /* End of INTENTO_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* INTENTO_VerifyCmdLength() -- Verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean INTENTO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(INTENTO_LEN_ERR_EID, CFE_EVS_ERROR,
           "INTENTO: Invalid cmd pkt: ID = 0x%X,  CC = %d, Len = %d",
                          MessageID, CommandCode, ActualLength);
        result = FALSE;
        INTENTO_AppData.ErrCounter++;
    }

    return(result);

} /* End of INTENTO_VerifyCmdLength() */

/************************/
/*  End of File Comment */
/************************/
