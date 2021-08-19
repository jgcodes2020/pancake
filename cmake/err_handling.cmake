# Command error handling
# ----------------------

macro(pancake_check_exit_code msg)
if(NOT ${PANCAKE_LAST_CMD_EXIT} EQUAL "0")
  message(FATAL_ERROR ${msg})
endif()
endmacro()
set(PANCAKE_LAST_CMD_EXIT "0")