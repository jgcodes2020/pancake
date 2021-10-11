if (UNIX)
  set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
  set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
elseif(WIN32)
  set(CMAKE_STATIC_LIBRARY_PREFIX "")
  set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
else()
  message(FATAL_ERROR "What kind of esoteric OS are you running?")
endif()

set(combined_file "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}pancake${CMAKE_STATIC_LIBRARY_SUFFIX}")
file(GLOB libs "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}*${CMAKE_STATIC_LIBRARY_SUFFIX}")

execute_process(
  COMMAND python3 "${CMAKE_CURRENT_LIST_DIR}/../scripts/lib_merger.py" ${libs} -o ${combined_file}
)