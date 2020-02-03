include(ExternalProject)

if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/include/boost)

ExternalProject_Add(
  boost
  PREFIX ${CMAKE_CURRENT_LIST_DIR}/boost_cmake_prefix
  URL https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.bz2
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./bootstrap.sh --prefix=${CMAKE_CURRENT_LIST_DIR}
  BUILD_COMMAND ./b2 -j12 link=static
  INSTALL_COMMAND ./b2 -j12 install
)

else()

add_custom_target(boost)

endif()
MESSAGE( STATUS "new target: boost" )
