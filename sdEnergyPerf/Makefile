CC=gcc
CFLAGS=-Wall -DDEFAULT_CONFIG_PATH=\"/opt/floc/sdFeatures.conf\"

# define your targets
TARGET_SD=ePerfSD.out
SOURCE_SD=ePerfSD.c

# 'all' target will build both targets
all: $(TARGET_SD)

# 'sd' target will build only the SD target
sd: $(TARGET_SD)

$(TARGET_SD): $(SOURCE_SD)
	$(CC) $(CFLAGS) $(SOURCE_SD) -o $(TARGET_SD)

clean:
	rm -f $(TARGET_SD)

