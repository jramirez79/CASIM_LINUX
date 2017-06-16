################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../kernels/mdp_hexagon_paths_kernel.cu \
../kernels/mdp_square_paths_kernel.cu \
../kernels/mpi_kernel.cu \
../kernels/osc_bezier_paths_kernel.cu 

CU_DEPS += \
./kernels/mdp_hexagon_paths_kernel.d \
./kernels/mdp_square_paths_kernel.d \
./kernels/mpi_kernel.d \
./kernels/osc_bezier_paths_kernel.d 

OBJS += \
./kernels/mdp_hexagon_paths_kernel.o \
./kernels/mdp_square_paths_kernel.o \
./kernels/mpi_kernel.o \
./kernels/osc_bezier_paths_kernel.o 


# Each subdirectory must supply rules for building sources it contributes
kernels/%.o: ../kernels/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 -gencode arch=compute_30,code=sm_30  -odir "kernels" -M -o "$(@:%.o=%.d)" "$<"
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 --compile --relocatable-device-code=false -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


