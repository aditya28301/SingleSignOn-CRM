# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Login_Page_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Login_Page_autogen.dir\\ParseCache.txt"
  "Login_Page_autogen"
  )
endif()
