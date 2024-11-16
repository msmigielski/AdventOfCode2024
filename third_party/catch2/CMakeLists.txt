Include(FetchContent)

set(FETCHCONTENT_QUIET off)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/third_party/catch2)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        fa43b77429ba76c462b1898d6cd2f2d7a9416b14 # v3.7.1
)

FetchContent_MakeAvailable(Catch2)