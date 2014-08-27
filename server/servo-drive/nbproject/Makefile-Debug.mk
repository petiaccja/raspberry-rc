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
CC=arm-linux-gnueabihf-gcc
CCC=arm-linux-gnueabihf-g++
CXX=arm-linux-gnueabihf-g++
FC=gfortran
AS=arm-linux-gnueabihf-as

# Macros
CND_PLATFORM=GNU_armv6-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/driver.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/servo.o \
	${OBJECTDIR}/src/spinlock.o \
	${OBJECTDIR}/src/timer.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ./libservo-drived.a

./libservo-drived.a: ${OBJECTFILES}
	${MKDIR} -p .
	${RM} ./libservo-drived.a
	${AR} -rv ./libservo-drived.a ${OBJECTFILES} 
	$(RANLIB) ./libservo-drived.a

${OBJECTDIR}/src/driver.o: src/driver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_GLIBCXX_USE_NANOSLEEP -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/xenomai/INSTALL/usr/xenomai/include -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/driver.o src/driver.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_GLIBCXX_USE_NANOSLEEP -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/xenomai/INSTALL/usr/xenomai/include -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/servo.o: src/servo.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_GLIBCXX_USE_NANOSLEEP -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/xenomai/INSTALL/usr/xenomai/include -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/servo.o src/servo.cpp

${OBJECTDIR}/src/spinlock.o: src/spinlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_GLIBCXX_USE_NANOSLEEP -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/xenomai/INSTALL/usr/xenomai/include -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/spinlock.o src/spinlock.cpp

${OBJECTDIR}/src/timer.o: src/timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_GLIBCXX_USE_NANOSLEEP -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/xenomai/INSTALL/usr/xenomai/include -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/timer.o src/timer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ./libservo-drived.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
