################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/osc/osc/OscOutboundPacketStream.cpp \
../source/osc/osc/OscPrintReceivedElements.cpp \
../source/osc/osc/OscReceivedElements.cpp \
../source/osc/osc/OscTypes.cpp 

OBJS += \
./source/osc/osc/OscOutboundPacketStream.o \
./source/osc/osc/OscPrintReceivedElements.o \
./source/osc/osc/OscReceivedElements.o \
./source/osc/osc/OscTypes.o 

CPP_DEPS += \
./source/osc/osc/OscOutboundPacketStream.d \
./source/osc/osc/OscPrintReceivedElements.d \
./source/osc/osc/OscReceivedElements.d \
./source/osc/osc/OscTypes.d 


# Each subdirectory must supply rules for building sources it contributes
source/osc/osc/%.o: ../source/osc/osc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 -gencode arch=compute_30,code=sm_30  -odir "source/osc/osc" -M -o "$(@:%.o=%.d)" "$<"
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


