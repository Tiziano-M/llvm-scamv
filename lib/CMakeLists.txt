add_llvm_library(LLVMScamv MODULE
	print_functions.cpp
	print_globals.cpp
	rename_bbs.cpp
  analyze_function.cpp
  automatic_block_specifier.cpp
  fence_insertion.cpp
  extend_loop_deletion.cpp
  check_loops.cpp
  add_aarch64_slh_attribute.cpp
  )
