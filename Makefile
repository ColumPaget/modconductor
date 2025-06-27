OBJ=modbus.o settings.o interogate_device.o
LIBS= -lz -lcrypto -lssl -lssl -lUseful-5 
FLAGS=-g -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -D_FILE_OFFSET_BITS=64 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_LIBUSEFUL_5_LIBUSEFUL_H=1 -DHAVE_LIBUSEFUL5=1 -DHAVE_LIBUSEFUL_5=1 -DHAVE_LIBSSL=1 -DHAVE_LIBSSL=1 -DHAVE_LIBCRYPTO=1 -DHAVE_LIBZ=1

all: $(OBJ)  
	gcc $(FLAGS) -omodconductor.exe $(OBJ) main.c $(LIBS)

interogate_device.o: interogate_device.h interogate_device.c
	gcc -c interogate_device.c

modbus.o: modbus.h modbus.c
	gcc -c modbus.c

settings.o: settings.h settings.c
	gcc -c settings.c


libUseful-bundled/libUseful.a:
	$(MAKE) -C libUseful-bundled

clean:
	rm -rf *.orig *.o *.exe */*.o */*.a */*.so.* */*.so
