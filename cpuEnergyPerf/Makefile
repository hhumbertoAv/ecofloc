CC=gcc
CFLAGS=-Wall

# define your targets
TARGET_CPU=ePerfCPU.out
SOURCE_CPU=ePerfCPU.c

# 'all' target will build both targets
all: $(TARGET_CPU)

# 'cpu' target will build only the CPU target
cpu: $(TARGET_CPU)

$(TARGET_CPU): $(SOURCE_CPU)
	$(CC) $(CFLAGS) $(SOURCE_CPU) -o $(TARGET_CPU)

clean:
	rm -f $(TARGET_CPU)
