cmake_minimum_required(VERSION 3.17)
project(MEH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)


find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)
find_package (Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS ALL)
find_package(PythonLibs)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})







add_executable(MEH main.cpp company.cpp company.h currencies.h market_watch.h market_watch.cpp price.h price.cpp identity.h identity.cpp value_investor.h value_investor.cpp fund.cpp fund.h momentum_investor.cpp momentum_investor.h noisy_investor.cpp noisy_investor.h funds.cpp funds.h order.cpp order.h stochastic_math.cpp stochastic_math.h price_setter.cpp price_setter.h wiener.h Market.cpp Market.h)
target_link_libraries(MEH ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${MPI_C_LIBRARIES} )
target_link_libraries(MEH Eigen3::Eigen)
