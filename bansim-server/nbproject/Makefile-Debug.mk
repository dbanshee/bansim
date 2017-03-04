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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/lib/ACApi.o \
	${OBJECTDIR}/src/lib/jSon.o \
	${OBJECTDIR}/src/lib/logger.o \
	${OBJECTDIR}/src/lib/pCarsDump.o \
	${OBJECTDIR}/src/lib/pcarsApi.o \
	${OBJECTDIR}/src/lib/restWS.o \
	${OBJECTDIR}/src/lib/serial.o \
	${OBJECTDIR}/src/lib/serialwin.o \
	${OBJECTDIR}/src/lib/serversocket.o \
	${OBJECTDIR}/src/lib/simController.o \
	${OBJECTDIR}/src/lib/simSource.o \
	${OBJECTDIR}/src/lib/stringutils.o \
	${OBJECTDIR}/src/server/bsServer.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Lsrc/lib/ext -lcygjansson-4 -lcygmicrohttpd-10

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bansim-server.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bansim-server.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bansim-server ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/lib/ACApi.o: src/lib/ACApi.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/ACApi.o src/lib/ACApi.c

${OBJECTDIR}/src/lib/jSon.o: src/lib/jSon.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/jSon.o src/lib/jSon.c

${OBJECTDIR}/src/lib/logger.o: src/lib/logger.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/logger.o src/lib/logger.c

${OBJECTDIR}/src/lib/pCarsDump.o: src/lib/pCarsDump.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/pCarsDump.o src/lib/pCarsDump.c

${OBJECTDIR}/src/lib/pcarsApi.o: src/lib/pcarsApi.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/pcarsApi.o src/lib/pcarsApi.c

${OBJECTDIR}/src/lib/restWS.o: src/lib/restWS.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/restWS.o src/lib/restWS.c

${OBJECTDIR}/src/lib/serial.o: src/lib/serial.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/serial.o src/lib/serial.c

${OBJECTDIR}/src/lib/serialwin.o: src/lib/serialwin.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/serialwin.o src/lib/serialwin.c

${OBJECTDIR}/src/lib/serversocket.o: src/lib/serversocket.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/serversocket.o src/lib/serversocket.c

${OBJECTDIR}/src/lib/simController.o: src/lib/simController.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/simController.o src/lib/simController.c

${OBJECTDIR}/src/lib/simSource.o: src/lib/simSource.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/simSource.o src/lib/simSource.c

${OBJECTDIR}/src/lib/stringutils.o: src/lib/stringutils.c 
	${MKDIR} -p ${OBJECTDIR}/src/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lib/stringutils.o src/lib/stringutils.c

${OBJECTDIR}/src/server/bsServer.o: src/server/bsServer.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/bsServer.o src/server/bsServer.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bansim-server.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
