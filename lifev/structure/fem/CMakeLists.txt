SET(fem_HEADERS
  fem/AssemblyElementalStructure.hpp
  fem/ExpressionDefinitions.hpp
CACHE INTERNAL "")

SET(fem_SOURCES
  fem/AssemblyElementalStructure.cpp
  fem/ExpressionDefinitions.cpp
CACHE INTERNAL "")

SET(LOCAL_HEADERS)
FOREACH(INC ${fem_HEADERS})
  STRING(REPLACE "fem/" "" LOCAL_INC ${INC})
  SET(LOCAL_HEADERS ${LOCAL_HEADERS} ${LOCAL_INC})
ENDFOREACH()

INSTALL(
  FILES ${LOCAL_HEADERS}
  DESTINATION "${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}/lifev/structure/fem"
  COMPONENT ${PACKAGE_NAME}
)
