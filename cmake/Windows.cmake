add_definitions(-DON_WINDOWS)

function(add_external_library NAME)
	cmake_parse_arguments(LP "ENCOURAGED" "TARGET" "" ${ARGN})
	add_library ( ${NAME} SHARED IMPORTED )
	string(TOUPPER "${NAME}" UCNAME)
    add_definitions(-DWITH_${UCNAME})
	set_target_properties(${NAME} PROPERTIES
  IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binary/${NAME}.lib
  IMPORTED_LOCATION  ${PROJECT_SOURCE_DIR}/libs/binary/${NAME}.dll)
  target_link_libraries(webserver ${NAME})
  message(STATUS "library ${NAME} defined -- -DWITH_${UCNAME} -- ${LP_TARGET}")
endfunction()