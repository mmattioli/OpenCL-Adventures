OBJS = main.cpp
OBJ_NAME = vadd.out
INC = -I ../../include -I ../../util
CCFLAGS = -std=c++17

# Linking OpenCL is different on Apple platforms.
ifeq ($(shell uname -s), Darwin)
	LIBS = -framework OpenCL
else
	LIBS = -lOpenCL
endif

# Specify the device type to be a GPU.
ifndef DEVICE
	DEVICE = CL_DEVICE_TYPE_GPU
endif
CCFLAGS += -D $(DEVICE)

all : $(OBJ_NAME)

$(OBJ_NAME) : $(OBJS)
	g++ -Wall $^ $(INC) $(LIBS) $(CCFLAGS) -o $@

clean:
	rm -rf $(OBJ_NAME)
