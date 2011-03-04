# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.45 NO_MODULE)
include("${Boost_DEV_FILE}")

boost_project(Dynamic Any
  AUTHORS
    "Daniel Larimer"
  MAINTAINERS
    ""
  DESCRIPTION
    "Safe, generic container for single values of different value types with support for dynamic casts."
  DEPENDS
    utility
  )

file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/boost/*.?pp)
boost_add_headers(${headers})

boost_documentation(doc/any.xml)

boost_add_test(dynamic_any_test)
