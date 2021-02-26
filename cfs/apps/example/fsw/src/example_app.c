/*******************************************************************************
** File: example_app.c
**
** Purpose:
**   Provide functions for the EXAMPLE application used in cFE training.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "example_app.h"
#include "example_app_perfids.h"
#include "example_app_msgids.h"


/*
** System Header files
*/
#include <string.h>

/*
** EXAMPLE global data
*/
EXAMPLE_AppData_t EXAMPLE_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE_AppMain(void)
{
    int32 Status;
    
    /*
    ** Register the Application with Executive Services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(EXAMPLE_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Intialization fails, set the RunStatus to CFE_ES_APP_ERROR
    ** and the App will not enter the RunLoop.
    */
    Status = EXAMPLE_AppInit();
    if ( Status != CFE_SUCCESS )
    {
        EXAMPLE_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Application Main Loop. Call CFE_ES_RunLoop to check for changes
    ** in the Application's status. If there is a request to kill this
    ** App, it will be passed in through the RunLoop call.
    */
    while ( CFE_ES_RunLoop(&EXAMPLE_AppData.RunStatus) == TRUE )
    {
    
        /*
        ** Performance Log Exit Stamp.
        */
        CFE_ES_PerfLogExit(EXAMPLE_APPMAIN_PERF_ID);
   
        /*
        ** Pend on the arrival of the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&EXAMPLE_AppData.MsgPtr, EXAMPLE_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /*
        ** Performance Log Entry Stamp.
        */      
        CFE_ES_PerfLogEntry(EXAMPLE_APPMAIN_PERF_ID);

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
            EXAMPLE_AppPipe(EXAMPLE_AppData.MsgPtr);
            
            /* 
            ** Depending on the app you may want to add calls to functions
            ** that execute each loop.
            */

        }
        else
        {
            /*
            ** This is an example of exiting on an error.
            ** Note that a SB read error is not always going to 
            ** result in an app quitting. 
            */
            CFE_EVS_SendEvent(EXAMPLE_PIPE_ERR_EID, CFE_EVS_ERROR,
                               "EXAMPLE: SB Pipe Read Error, EXAMPLE App Will Exit.");
            
            EXAMPLE_AppData.RunStatus = CFE_ES_APP_ERROR;
         }
                    
    } /* end while */

    /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(EXAMPLE_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(EXAMPLE_AppData.RunStatus);
    
} /* End of EXAMPLE_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_AppInit() -- EXAMPLE initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 EXAMPLE_AppInit(void)
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
    EXAMPLE_AppData.RunStatus = CFE_ES_APP_RUN;
    
    /*
    ** Initialize app command execution counters.
    */
    EXAMPLE_AppData.CmdCounter = 0;
    EXAMPLE_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data.
    */
    strcpy(EXAMPLE_AppData.PipeName, "EXAMPLE_CMD_PIPE");
    EXAMPLE_AppData.PipeDepth = EXAMPLE_PIPE_DEPTH;

    EXAMPLE_AppData.LimitHK   = EXAMPLE_LIMIT_HK;
    EXAMPLE_AppData.LimitCmd  = EXAMPLE_LIMIT_CMD;

    /*
    ** Initialize event filter table for envents we want to filter.
    */
    
    EXAMPLE_AppData.EventFilters[0].EventID = EXAMPLE_RESET_INF_EID;
    EXAMPLE_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    
    EXAMPLE_AppData.EventFilters[1].EventID = EXAMPLE_CC1_ERR_EID;
    EXAMPLE_AppData.EventFilters[1].Mask    = CFE_EVS_EVERY_OTHER_TWO;
    
    EXAMPLE_AppData.EventFilters[2].EventID = EXAMPLE_LEN_ERR_EID;
    EXAMPLE_AppData.EventFilters[2].Mask    = CFE_EVS_FIRST_8_STOP;

    //Ex#1 EXAMPLE_AppData.EventFilters[3].EventID = EXAMPLE_NEW_CMD_INF_EID;
    //Ex#1 EXAMPLE_AppData.EventFilters[3].Mask    = CFE_EVS_EVERY_FOURTH_ONE;

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(EXAMPLE_AppData.EventFilters, 4,
                               CFE_EVS_BINARY_FILTER);
    
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE App: Error Registering Events, RC = 0x%08X\n", Status);
       return ( Status );
    }
             
    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&EXAMPLE_AppData.HkPacket, EXAMPLE_HK_TLM_MID, sizeof(EXAMPLE_HkPacket_t), TRUE);
   
    /*
    ** Create Software Bus message pipe.
    */
    Status = CFE_SB_CreatePipe(&EXAMPLE_AppData.CmdPipe, EXAMPLE_AppData.PipeDepth, EXAMPLE_AppData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Could use an event at this point
       */
       CFE_ES_WriteToSysLog("EXAMPLE App: Error Creating SB Pipe, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(EXAMPLE_SEND_HK_MID,EXAMPLE_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE App: Error Subscribing to HK Request, RC = 0x%08X\n", Status);
       return ( Status );
    }

    /*
    ** Subscribe to EXAMPLE ground command packets
    */
    Status = CFE_SB_Subscribe(EXAMPLE_CMD_MID,EXAMPLE_AppData.CmdPipe);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("EXAMPLE App: Error Subscribing to EXAMPLE Command, RC = 0x%08X\n", Status);
       return ( Status );
    }
                 
    /*
    ** Application startup event message.
    */
    CFE_EVS_SendEvent(EXAMPLE_INIT_INF_EID,CFE_EVS_INFORMATION, "EXAMPLE: Application Initialized");
                         
    return(CFE_SUCCESS);

} /* End of EXAMPLE_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE_AppPipe(CFE_SB_MsgPtr_t msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request.
        */
        case EXAMPLE_SEND_HK_MID:
            EXAMPLE_HousekeepingCmd(msg);
            break;

        /*
        ** EXAMPLE ground commands.
        */
        case EXAMPLE_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(msg);
            switch (CommandCode)
            {
                case EXAMPLE_NOOP_CC:
                    EXAMPLE_NoopCmd(msg);
                    break;

                case EXAMPLE_RESET_CC:
                    EXAMPLE_ResetCmd(msg);
                    break;
                    
                //Ex#1case EXAMPLE_NEW_CMD_CC:
                //Ex#1 EXAMPLE_NewCmd(msg);
                //Ex#1 break;

                default:
                    CFE_EVS_SendEvent(EXAMPLE_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(EXAMPLE_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            break;
    }

    return;

} /* End of EXAMPLE_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_HousekeepingCmd() -- On-board command (HK request)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE_NoArgsCmd_t);
    uint16 i;

    /*
    ** Verify command packet length
    */
    if (EXAMPLE_VerifyCmdLength(msg, ExpectedLength))
    {
        /*
        ** Get command execution counters
        */
        EXAMPLE_AppData.HkPacket.CmdCounter = EXAMPLE_AppData.CmdCounter;
        EXAMPLE_AppData.HkPacket.ErrCounter = EXAMPLE_AppData.ErrCounter;

        /*
        ** Send housekeeping telemetry packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &EXAMPLE_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &EXAMPLE_AppData.HkPacket);

        /*
        ** This command does not affect the command execution counter
        */
        
    } /* end if */

    return;

} /* End of EXAMPLE_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_NoopCmd() -- EXAMPLE ground command (NO-OP)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE_NoopCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (EXAMPLE_VerifyCmdLength(msg, ExpectedLength))
    {
        EXAMPLE_AppData.CmdCounter++;

        CFE_EVS_SendEvent(EXAMPLE_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "EXAMPLE Version 1.0.0: No-op command");
    }

    return;

} /* End of EXAMPLE_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_ResetCmd() -- EXAMPLE ground command (reset counters)             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EXAMPLE_ResetCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(EXAMPLE_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (EXAMPLE_VerifyCmdLength(msg, ExpectedLength))
    {
        EXAMPLE_AppData.CmdCounter = 0;
        EXAMPLE_AppData.ErrCounter = 0;

        CFE_EVS_SendEvent(EXAMPLE_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "EXAMPLE: Reset Counters command");
    }

    return;

} /* End of EXAMPLE_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EXAMPLE_VerifyCmdLength() -- Verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean EXAMPLE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(EXAMPLE_LEN_ERR_EID, CFE_EVS_ERROR,
           "EXAMPLE: Invalid cmd pkt: ID = 0x%X,  CC = %d, Len = %d",
                          MessageID, CommandCode, ActualLength);
        result = FALSE;
        EXAMPLE_AppData.ErrCounter++;
    }

    return(result);

} /* End of EXAMPLE_VerifyCmdLength() */

/************************/
/*  End of File Comment */
/************************/
