add_library(Utils INTERFACE)
add_library(AoC::Utils ALIAS Utils)

target_include_directories(Utils
  INTERFACE
    include
)