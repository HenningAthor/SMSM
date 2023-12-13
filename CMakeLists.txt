cmake_minimum_required(VERSION 3.22)
project(SMSM)

set(CMAKE_CXX_STANDARD 20)
set(Boost_NO_WARN_NEW_VERSIONS 1)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -O3 -DNDEBUG -fopenmp -finline-functions -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fopenmp -std=c++20 -Wall -Wextra -pedantic")

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

file(GLOB Dir1_Sources "3rd_party_tools/blossom5-v2.05/*.cpp")
file(GLOB Dir2_Sources "3rd_party_tools/blossom5-v2.05/*.h")
file(GLOB Dir3_Sources "3rd_party_tools/blossom5-v2.05/GEOM/*.cpp")
file(GLOB Dir4_Sources "3rd_party_tools/blossom5-v2.05/GEOM/*.h")
file(GLOB Dir5_Sources "3rd_party_tools/blossom5-v2.05/MinCost/*.cpp")
file(GLOB Dir6_Sources "3rd_party_tools/blossom5-v2.05/MinCost/*.h")
set(Blossom5_files ${Dir1_Sources} ${Dir2_Sources} ${Dir3_Sources} ${Dir4_Sources} ${Dir5_Sources} ${Dir6_Sources})
list(FILTER Blossom5_files EXCLUDE REGEX "example.cpp")

add_executable(SMSM
        main.cpp
        ${Blossom5_files}
        src/algorithms/bf_tree_search.h
        src/algorithms/CandidateManager.h
        src/algorithms/PBFAlgorithm.h
        src/algorithms/SICache.h
        src/algorithms/tree_search_iterative.h
        src/algorithms/UB2DAlgorithm.h
        src/algorithms/UB2DStorage.h
        src/algorithms/UB2DStorage.cpp
        src/algorithms/UpperBoundManager.h
        src/structures/data_points.h
        src/structures/dataPoints_EuclidianDistance.h
        src/structures/facility_location.h
        src/structures/graph.h
        src/structures/graph_NegativeGroupFarness.h
        src/structures/graph_PartialDominatingSet.h
        src/structures/StructureInterface.h
        src/utility/AlgorithmConfiguration.h
        src/utility/AlgorithmConfiguration.cpp
        src/utility/data_point_generation.h
        src/utility/data_point_generation.cpp
        src/utility/graph_generation.h
        src/utility/graph_generation.cpp
        src/utility/JSON_util.h
        src/utility/JSON_util.cpp
        src/utility/util.h
        src/utility/util.cpp
        src/utility/VectorOfVectors.h)

if (supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET SMSM PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif ()

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(SMSM OpenMP::OpenMP_CXX)
endif ()

target_link_libraries(SMSM ${Boost_LIBRARIES})
