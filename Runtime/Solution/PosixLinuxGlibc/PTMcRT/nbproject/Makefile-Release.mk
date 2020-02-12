#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/a77151f3/PTSMemory.o \
	${OBJECTDIR}/_ext/a77151f3/PTSMemoryAllocator.o \
	${OBJECTDIR}/_ext/a77151f3/PTSTaskSchedulerImpl.o


# C Compiler Flags
CFLAGS=-fdiagnostics-format=msvc -stdlib=libc++ -Wall -fno-strict-aliasing -fno-exceptions -fstack-protector -fpic -fno-short-enums -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -pthread -fvisibility=hidden -mssse3 -g0 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections

# CC Compiler Flags
CCFLAGS=-fdiagnostics-format=msvc -stdlib=libc++ -Wall -fno-strict-aliasing -fno-exceptions -fstack-protector -fpic -fno-short-enums -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -pthread -fvisibility=hidden -mssse3 -g0 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections
CXXFLAGS=-fdiagnostics-format=msvc -stdlib=libc++ -Wall -fno-strict-aliasing -fno-exceptions -fstack-protector -fpic -fno-short-enums -fno-rtti -finput-charset=UTF-8 -fexec-charset=UTF-8 -pthread -fvisibility=hidden -mssse3 -g0 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libPTMcRT.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libPTMcRT.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libPTMcRT.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -fdiagnostics-format=msvc -fuse-ld=lld -stdlib=libc++ -lc++ -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -pthread -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wl,-Bsymbolic -Wl,-rpath,'$$ORIGIN' -shared

${OBJECTDIR}/_ext/a77151f3/PTSMemory.o: ../../../Private/McRT/PTSMemory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a77151f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DPTMCRTAPI=PTEXPORT -DNDEBUG=1 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a77151f3/PTSMemory.o ../../../Private/McRT/PTSMemory.cpp

${OBJECTDIR}/_ext/a77151f3/PTSMemoryAllocator.o: ../../../Private/McRT/PTSMemoryAllocator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a77151f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DPTMCRTAPI=PTEXPORT -DNDEBUG=1 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a77151f3/PTSMemoryAllocator.o ../../../Private/McRT/PTSMemoryAllocator.cpp

${OBJECTDIR}/_ext/a77151f3/PTSTaskSchedulerImpl.o: ../../../Private/McRT/PTSTaskSchedulerImpl.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a77151f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DPTMCRTAPI=PTEXPORT -DNDEBUG=1 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a77151f3/PTSTaskSchedulerImpl.o ../../../Private/McRT/PTSTaskSchedulerImpl.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
