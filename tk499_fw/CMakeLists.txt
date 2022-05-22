cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools")
include(arm-none-eabi-gcc)


set(PRJ_NAME tk499_fw)


project(${PRJ_NAME}
  LANGUAGES ASM C CXX
)


set(EXECUTABLE ${PRJ_NAME}.elf)


file(GLOB SRC_FILES CONFIGURE_DEPENDS
  src/bsp/*.c
  src/hw/*.c
  src/hw/driver/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  src/ap/*.cpp
  src/ap/*.c
  src/common/*.c    
  lib/Hal_lib/src/*.c
  lib/Usb_lib/*.c
)

add_executable(${EXECUTABLE} 
  src/main.c
  src/bsp/startup/startup_tkm32f499.s

  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)



target_include_directories(${EXECUTABLE} PRIVATE 
  src  
  src/ap
  src/bsp
  src/bsp/device
  src/common 
  src/common/core
  src/common/hw/include 
  src/hw
  lib
  
  lib/CMSIS/Include
  lib/CMSIS/DSP/Include
  lib/Hal_lib/inc
  lib/Usb_lib
)

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DSTM32F411xE
  -DARM_MATH_CM4
  -DLFS_THREADSAFE
  )

target_compile_options(${EXECUTABLE} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard

  -fdata-sections
  -ffunction-sections

  -Wall
  -specs=nano.specs

  -O0
  -g3
  )

target_link_options(${EXECUTABLE} PRIVATE
  -T../src/bsp/ldscript/TKM32F499_FLASH.ld
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard
  -specs=nano.specs
  --specs=nosys.specs

  -lc
  -lm
  -lstdc++
  -lsupc++

  -Wl,-Map=${PRJ_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
  )

target_link_libraries( ${EXECUTABLE} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/CMSIS/libarm_cortexM4lf_math.a 
  )

add_custom_command(TARGET ${EXECUTABLE} 
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
    COMMENT "Invoking: Make Binary"
    )

#set_target_properties(${EXECUTABLE} PROPERTIES SUFFIX ".elf")
#add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#        COMMAND arm-none-eabi-size --format=berkeley --totals "$<TARGET_FILE:${EXECUTABLE}>"
#        COMMENT "Invoking: Cross ARM GNU Print Size")