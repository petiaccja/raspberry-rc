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
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/net_server.o \
	${OBJECTDIR}/src/servo_state.o \
	${OBJECTDIR}/src/spinlock.o


# C Compiler Flags
CFLAGS=-std=gnu99

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../external/boost_1_54_0_arm/stage/arm/lib -L../../external/xenomai/INSTALL/usr/xenomai/lib -lboost_system -lrt -lnative -lxenomai ../servo-drive/libservo-drived.a ../gpio-lib/libgpio-libd.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server: ../servo-drive/libservo-drived.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server: ../gpio-lib/libgpio-libd.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/boost_1_54_0_arm -I../servo-drive/src -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/net_server.o: src/net_server.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/boost_1_54_0_arm -I../servo-drive/src -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/net_server.o src/net_server.cpp

${OBJECTDIR}/src/servo_state.o: src/servo_state.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/boost_1_54_0_arm -I../servo-drive/src -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/servo_state.o src/servo_state.cpp

${OBJECTDIR}/src/spinlock.o: src/spinlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__GXX_EXPERIMENTAL_CXX0X__ -I../../external/boost_1_54_0_arm -I../servo-drive/src -I../gpio-lib/src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/spinlock.o src/spinlock.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rc-server

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
