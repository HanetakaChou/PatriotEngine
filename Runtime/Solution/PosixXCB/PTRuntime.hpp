#include "PTCommon.hpp"

PTLauncher_LD: PTLauncher_CPP ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME 
PT_RECIPEPREFIX make -f .mk/PTLauncher.mk PTLauncher_LD

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME:
PT_RECIPEPREFIX mkdir -p ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME

PTLauncher_CPP: ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME
PT_RECIPEPREFIX make -f .mk/PTLauncher.mk PTLauncher_CPP

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME:
PT_RECIPEPREFIX mkdir -p ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME

