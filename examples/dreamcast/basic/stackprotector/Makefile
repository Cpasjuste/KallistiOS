TARGET = stackprotector.elf
OBJS = stackprotector.o
KOS_CFLAGS += -fstack-protector-all

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	kos-cc -o $@ $^

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)


.PHONY: run dist clean rm-elf
