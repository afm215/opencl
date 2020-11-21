EXE=vector_add
EXE2 = vector_map
EXE3 = matrix_cros
EXE4 = group_matrix_cross
SRCS=vector_add.cpp 
SRCS2 = vector_map.cpp
SRCS3 = matrix_cros.cpp
SRCS4 = group_matrix_cross.cpp
GCC=arm-linux-gnueabihf-g++  
OCLLIBSDIR=/opt/ComputeLibrary/build/
OCLINCSDIR=/opt/ComputeLibrary/include/
MGD=/opt/Mali_Graphics_Debugger_v4.4.1.0271762a_Linux_x64/target/linux/hard_float/
FLAGS=-g -Wno-deprecated-declarations -Wall -DARCH_ARM -Wextra -Wno-unused-parameter -pedantic -Wdisabled-optimization -Wformat=2 -Winit-self -Wstrict-overflow=2 -Wswitch-default -fpermissive -std=gnu++11 -Wno-vla -Woverloaded-virtual -Wctor-dtor-privacy -Wsign-promo -Weffc++ -Wno-format-nonliteral -Wno-overlength-strings -Wno-strict-overflow -Wno-implicit-fallthrough -Wlogical-op -Wnoexcept -Wstrict-null-sentinel -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -Werror -O3 -ftree-vectorize -fstack-protector-strong -DARM_COMPUTE_CL -I${OCLINCSDIR} -I.. -I..  
#LDFLAGS=../../build/utils/Utils.o -L../../build/ -L..  -larm_compute -larm_compute_core -lOpenCL
LDFLAGS=-L${OCLLIBSDIR} -larm_compute -larm_compute_core -lOpenCL

all: ${EXE} $(EXE2) $(EXE3) $(EXE4) 
${EXE}.o:${SRCS}
	$(GCC) -c ${FLAGS} ${SRCS} -o ${EXE}.o

${EXE2}.o:${SRCS2}
	$(GCC) -c ${FLAGS} ${SRCS2} -o ${EXE2}.o

${EXE3}.o:${SRCS3}
	$(GCC) -c ${FLAGS} ${SRCS3} -o ${EXE3}.o

${EXE4}.o:${SRCS4}
	$(GCC) -c ${FLAGS} ${SRCS4} -o ${EXE4}.o

${EXE}:${EXE}.o
	${GCC} -o ${EXE} ${EXE}.o  ${LDFLAGS}

${EXE2}:${EXE2}.o
	${GCC} -o ${EXE2} ${EXE2}.o  ${LDFLAGS}

${EXE3}:${EXE3}.o
	${GCC} -o ${EXE3} ${EXE3}.o  ${LDFLAGS}

${EXE4}:${EXE4}.o
	${GCC} -o ${EXE4} ${EXE4}.o  ${LDFLAGS}

debug:${EXE}
	LD_PRELOAD=${MGD}/libinterceptor.so ./${EXE}

clean:
	rm -rf ${EXE} ${EXE}.o	
