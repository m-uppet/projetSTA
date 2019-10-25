# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.15)
project(projet C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(Libs_Unirail/CAN)
include_directories(Libs_Unirail/ERTMS)
include_directories(Libs_Unirail/XBEE)

add_executable(projet
        Libs_Unirail/CAN/canLinux.c
        Libs_Unirail/CAN/canLinux.h
        Libs_Unirail/CAN/loco_MAIN_PARAMS.h
        Libs_Unirail/CAN/MESCAN1_DefinitionVariable.h
        Libs_Unirail/CAN/MESCAN1_MAIN_PARAMS.h
        Libs_Unirail/CAN/MESCAN1_Utilitaire.c
        Libs_Unirail/CAN/MESCAN1_Utilitaire.h
        Libs_Unirail/CAN/MESCAN1_VarStatusTrain.c
        Libs_Unirail/CAN/MESCAN1_VarStatusTrain.h
        Libs_Unirail/CAN/MESCAN1_VarTrain.c
        Libs_Unirail/CAN/MESCAN1_VarTrain.h
        Libs_Unirail/ERTMS/GSM_R_message.h
        Libs_Unirail/ERTMS/GSM_R_packet.h
        Libs_Unirail/XBEE/xbee.c
        Libs_Unirail/XBEE/xbee.h
        comm.c
        example.c
        rbc.c
        rbc.h
        surveillance_arret.c
        socket_example.c)
