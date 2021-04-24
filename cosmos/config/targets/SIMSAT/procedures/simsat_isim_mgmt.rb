###############################################################################
# SimpleSat (SimSat) Instrument Management Procedures 
#
# Perform operational functions during a simulated ground contact.  This script
# is designed to make you think about what types of functions need to be
# performed. It is not trying to be prescriptive in how to perform them. Each
# mission has its own unique ops concept and functional requirements.
#
# Notes:
#   1. This script is part of the cFS systems engineering training module
#
# Global Script Variables:
#   simsat_ops_enable - Boolean indicating whether ops example is active
#   simsat_ops_status - Text string displayed on ops example screen
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'

def simsat_isim_pwr_on

   Osk::flight.send_cmd("ISIM","POWER_ON_INSTR")

   wait("ISIM HK_TLM_PKT INSTR_STATE == 'READY'", 25) 

   Osk::flight.send_cmd("ISIM","START_SCI_DATA")

end #simsat_isim_pwr_on()



def simsat_isim_pwr_off

   Osk::flight.send_cmd("ISIM","POWER_OFF_INSTR")

   wait("ISIM HK_TLM_PKT INSTR_STATE == 'OFF'", 10) 

end #simsat_isim_pwr_off()


def simsat_isim_set_fault

   Osk::flight.send_cmd("ISIM","CONFIG_FAULT with FAULT_ENA 1")

   wait("ISIM HK_TLM_PKT FAULT == 'TRUE'", 10) 

end #simsat_isim_set_fault()

def simsat_isim_clear_fault

   Osk::flight.send_cmd("ISIM","CONFIG_FAULT with FAULT_ENA 0")

   wait("ISIM HK_TLM_PKT FAULT == 'FALSE'", 10) 

end #simsat_isim_clear_fault()
