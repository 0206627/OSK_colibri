/* 
** Purpose: Demo C++ app using the OSK C++ Framework
**
** Notes:
**   1. See osk_cpp_fw App.h prologue.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _osk_cpp_demo_app_
#define _osk_cpp_demo_app_

/*
** Includes
*/

#include "App.h"
#include "AppCfg.h"
#include "ExAppObj.h"


/*
** Type Definitions
*/

typedef struct {

   uint8    header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   valid_cmd_cnt;
   uint16   invalid_cmd_cnt;

   /*
   ** Example Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    last_tbl_action;
   uint8    last_tbl_action_valid;

   
   /*
   ** EXOBJ Data
   */

   uint16   ex_app_obj_exec_cnt;

} OS_PACK OskCppDemoApp_HkPkt;

#define OSK_CPP_DEMO_APP_TLM_HK_LEN sizeof (OskCppDemoApp_HkPkt)


class OskCppDemoApp : public oskfw::App {
public:
   
   OskCppDemoApp(int major_ver, int minor_ver, int local_rev);
   ~OskCppDemoApp();
   
   int32   Init();
   void Execute();

private:
      
   boolean ResetState();   
   
   void ProcessCommands();
   void SendHousekeepingPkt();
   
   OskCppDemoApp_HkPkt  m_hk_pkt;
   
   // Application Objects
   ExAppObj  m_ex_app_obj;
      
};

#endif // _osk_cpp_demo_app_
