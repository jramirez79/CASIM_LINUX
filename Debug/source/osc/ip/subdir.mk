################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/osc/ip/IpEndpointName.cpp \
../source/osc/ip/NetworkingUtils.cpp \
../source/osc/ip/UdpSocket.cpp 

OBJS += \
./source/osc/ip/IpEndpointName.o \
./source/osc/ip/NetworkingUtils.o \
./source/osc/ip/UdpSocket.o 

CPP_DEPS += \
./source/osc/ip/IpEndpointName.d \
./source/osc/ip/NetworkingUtils.d \
./source/osc/ip/UdpSocket.d 


# Each subdirectory must supply rules for building sources it contributes
source/osc/ip/%.o: ../source/osc/ip/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 -gencode arch=compute_30,code=sm_30  -odir "source/osc/ip" -M -o "$(@:%.o=%.d)" "$<"
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


