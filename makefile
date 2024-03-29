# Add `include` to search path, turn on warnings, select c++11
CPPFLAGS = -I include -W -Wall -std=c++11
LDFLAGS =

# -lm to bring in unix math library, -ltbb to bring in tbb
LDLIBS = -lm -ltbb

# Turn on optimisations
CPPFLAGS += -O3

# The very basic parts
FOURIER_CORE_OBJS = src/fourier_transform.o src/fourier_transform_register_factories.o

# implementations
# TODO : Add your implementations to this list. Use \ to continue line
FOURIER_IMPLEMENTATION_OBJS =  \
	src/fast_fourier_transform.o \
	src/direct_fourier_transform.o\
	src/hs2715/direct_fourier_transform_parfor_inner.o\
	src/hs2715/direct_fourier_transform_parfor_outer.o\
	src/hs2715/fast_fourier_transform_taskgroup.o\
	src/hs2715/fast_fourier_transform_parfor.o\
	src/hs2715/fast_fourier_transform_combined.o

FOURIER_OBJS = $(FOURIER_CORE_OBJS) $(FOURIER_IMPLEMENTATION_OBJS)

bin/test_fourier_transform : src/test_fourier_transform.cpp $(FOURIER_OBJS)
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

bin/time_fourier_transform : src/time_fourier_transform.cpp $(FOURIER_OBJS)
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

all : bin/test_fourier_transform bin/time_fourier_transform
