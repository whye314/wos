BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD =ld
LIB = -I ./lib/ -I ./kernel/ -I ./lib/kernel/ -I ./lib/user/ -I ./device/ -I ./thread/ -I ./userprog/
ASFLAGS = -f elf -I ./include/
ASFLAGS_BIN = -I ./include/
CCFLAGS = -m32 $(LIB) -static -c -fno-builtin -fno-stack-protector
LDFLAGS = -static -melf_i386 -e main -Ttext=0xc0001500 -Ttext-segment=0xc0001500
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/process.o $(BUILD_DIR)/tss.o $(BUILD_DIR)/switch.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/ioqueue.o $(BUILD_DIR)/console.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/thread.o $(BUILD_DIR)/list.o $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/string.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/debug.o $(BUILD_DIR)/print.o


$(BUILD_DIR)/main.o : kernel/main.c lib/stdint.h kernel/init.h lib/kernel/print.h kernel/interrupt.h kernel/memory.h device/console.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/init.o : kernel/init.c kernel/interrupt.h userprog/tss.h device/keyboard.h lib/kernel/print.h kernel/init.h device/timer.h kernel/memory.h thread/thread.h device/console.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o : kernel/interrupt.c kernel/interrupt.h lib/kernel/io.h lib/stdint.h kernel/global.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/timer.o : device/timer.c device/timer.h thread/thread.h lib/kernel/io.h lib/kernel/print.h kernel/debug.h thread/thread.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/debug.o : kernel/debug.c kernel/debug.h lib/kernel/print.h kernel/interrupt.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/memory.o : kernel/memory.c kernel/memory.h lib/kernel/print.h lib/kernel/bitmap.h lib/stdint.h kernel/debug.h lib/string.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/bitmap.o : lib/kernel/bitmap.c lib/kernel/bitmap.h lib/stdint.h kernel/debug.h lib/kernel/print.h kernel/interrupt.h lib/string.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/string.o : lib/string.c lib/string.h lib/stdint.h kernel/debug.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/thread.o : thread/thread.c thread/thread.h userprog/process.h kernel/memory.h lib/string.h kernel/interrupt.h lib/kernel/print.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/list.o : lib/kernel/list.c lib/kernel/list.h lib/string.h kernel/interrupt.h lib/stdint.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/sync.o : thread/sync.c thread/sync.h thread/thread.h lib/string.h kernel/interrupt.h lib/kernel/list.h lib/stdint.h kernel/debug.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/console.o : device/console.c device/console.h lib/stdint.h lib/kernel/print.h thread/sync.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/keyboard.o : device/keyboard.c device/keyboard.h device/ioqueue.h device/console.h lib/kernel/io.h kernel/interrupt.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/ioqueue.o : device/ioqueue.c device/ioqueue.h thread/sync.h thread/thread.h lib/string.h lib/stdint.h kernel/interrupt.h kernel/debug.h lib/kernel/print.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/tss.o : userprog/tss.c userprog/tss.h lib/string.h kernel/global.h thread/thread.h
	$(CC) $(CCFLAGS) $< -o $@

$(BUILD_DIR)/process.o : userprog/process.c userprog/process.h device/console.h thread/thread.h lib/string.h kernel/interrupt.h userprog/tss.h kernel/global.h kernel/debug.h kernel/memory.h
	$(CC) $(CCFLAGS) $< -o $@


$(BUILD_DIR)/kernel.o : kernel/kernel.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/print.o : lib/kernel/print.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/switch.o : thread/switch.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/mloader.bin : ./mloader.asm
	$(AS) $(ASFLAGS_BIN) $< -o $@


$(BUILD_DIR)/kernel.bin : $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@


.PHONY : mk_dir clean all hd
clean:
	rm -f ./$(BUILD_DIR)/*

build: $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/mloader.bin

mk_dir: 
	mkdir -p $(BUILD_DIR)
	#[ ! -d $(BUILD_DIR) ] && mkdir $(BUILD_DIR)

hd:
	dd if=./mboot.bin of=./bochs-2.6.8/hd60M.img bs=512 count=1 conv=notrunc seek=0
	dd if=$(BUILD_DIR)/mloader.bin of=./bochs-2.6.8/hd60M.img bs=512 count=4 conv=notrunc seek=2
	dd if=$(BUILD_DIR)/kernel.bin of=./bochs-2.6.8/hd60M.img bs=512 count=200 conv=notrunc seek=9
	

all : mk_dir build hd 