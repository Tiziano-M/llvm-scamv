add_llvm_library(LLVMScamv MODULE
	print_functions.cpp
	print_globals.cpp
	rename_bbs.cpp
  analyze_function.cpp
  automatic_block_specifier.cpp
  fence_insertion.cpp
  )
