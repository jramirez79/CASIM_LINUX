################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/CASIM_LINUX.cpp \
../source/behaviorMPI.cpp \
../source/cAnimation.cpp \
../source/cAxes.cpp \
../source/cBroadcastServer.cpp \
../source/cCamera.cpp \
../source/cCharacterGroup.cpp \
../source/cCharacterModel.cpp \
../source/cCrowd.cpp \
../source/cCrowdGroup.cpp \
../source/cCrowdManager.cpp \
../source/cCrowdParser.cpp \
../source/cFboManager.cpp \
../source/cFrameBuffer.cpp \
../source/cFrustum.cpp \
../source/cGlErrorManager.cpp \
../source/cGlslManager.cpp \
../source/cLogManager.cpp \
../source/cMDPCudaPathManager.cpp \
../source/cMDPHexagonManager.cpp \
../source/cMDPSquareManager.cpp \
../source/cMPICudaPathManager.cpp \
../source/cModel3D.cpp \
../source/cModelProps.cpp \
../source/cOSCCudaPathManager.cpp \
../source/cObstacleManager.cpp \
../source/cPacketListener.cpp \
../source/cProjectionManager.cpp \
../source/cRenderBuffer.cpp \
../source/cScenario.cpp \
../source/cScreenText.cpp \
../source/cShaderObject.cpp \
../source/cSkyboxManager.cpp \
../source/cStaticLod.cpp \
../source/cStringUtils.cpp \
../source/cTexture.cpp \
../source/cTexture3D.cpp \
../source/cTextureManager.cpp \
../source/cTimer.cpp \
../source/cVboManager.cpp \
../source/cXmlParser.cpp \
../source/glx_x11_events.cpp 

OBJS += \
./source/CASIM_LINUX.o \
./source/behaviorMPI.o \
./source/cAnimation.o \
./source/cAxes.o \
./source/cBroadcastServer.o \
./source/cCamera.o \
./source/cCharacterGroup.o \
./source/cCharacterModel.o \
./source/cCrowd.o \
./source/cCrowdGroup.o \
./source/cCrowdManager.o \
./source/cCrowdParser.o \
./source/cFboManager.o \
./source/cFrameBuffer.o \
./source/cFrustum.o \
./source/cGlErrorManager.o \
./source/cGlslManager.o \
./source/cLogManager.o \
./source/cMDPCudaPathManager.o \
./source/cMDPHexagonManager.o \
./source/cMDPSquareManager.o \
./source/cMPICudaPathManager.o \
./source/cModel3D.o \
./source/cModelProps.o \
./source/cOSCCudaPathManager.o \
./source/cObstacleManager.o \
./source/cPacketListener.o \
./source/cProjectionManager.o \
./source/cRenderBuffer.o \
./source/cScenario.o \
./source/cScreenText.o \
./source/cShaderObject.o \
./source/cSkyboxManager.o \
./source/cStaticLod.o \
./source/cStringUtils.o \
./source/cTexture.o \
./source/cTexture3D.o \
./source/cTextureManager.o \
./source/cTimer.o \
./source/cVboManager.o \
./source/cXmlParser.o \
./source/glx_x11_events.o 

CPP_DEPS += \
./source/CASIM_LINUX.d \
./source/behaviorMPI.d \
./source/cAnimation.d \
./source/cAxes.d \
./source/cBroadcastServer.d \
./source/cCamera.d \
./source/cCharacterGroup.d \
./source/cCharacterModel.d \
./source/cCrowd.d \
./source/cCrowdGroup.d \
./source/cCrowdManager.d \
./source/cCrowdParser.d \
./source/cFboManager.d \
./source/cFrameBuffer.d \
./source/cFrustum.d \
./source/cGlErrorManager.d \
./source/cGlslManager.d \
./source/cLogManager.d \
./source/cMDPCudaPathManager.d \
./source/cMDPHexagonManager.d \
./source/cMDPSquareManager.d \
./source/cMPICudaPathManager.d \
./source/cModel3D.d \
./source/cModelProps.d \
./source/cOSCCudaPathManager.d \
./source/cObstacleManager.d \
./source/cPacketListener.d \
./source/cProjectionManager.d \
./source/cRenderBuffer.d \
./source/cScenario.d \
./source/cScreenText.d \
./source/cShaderObject.d \
./source/cSkyboxManager.d \
./source/cStaticLod.d \
./source/cStringUtils.d \
./source/cTexture.d \
./source/cTexture3D.d \
./source/cTextureManager.d \
./source/cTimer.d \
./source/cVboManager.d \
./source/cXmlParser.d \
./source/glx_x11_events.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 -gencode arch=compute_30,code=sm_30  -odir "source" -M -o "$(@:%.o=%.d)" "$<"
	/usr/bin/nvcc -D_WEBSOCKETPP_CPP11_STL_ -DD_WEBSOCKETPP_NO_CPP11_REGEX_ -I/usr/lib/openmpi/include -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/osc" -I"/home/jramirez/cuda-workspace/CASIM_LINUX/header/osc/ip" -I"/home/jramirez/cuda-workspace/CUDA_MDP/header" -G -g -O0 -std=c++11 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


