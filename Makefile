OBJ=modbus.o
LIBS=libUseful-bundled/libUseful.a 
FLAGS=-g -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -D_FILE_OFFSET_BITS=64

all: $(OBJ) libUseful-bundled/libUseful.a 
	gcc $(FLAGS) -omodconductor.exe $(OBJ) main.c $(LIBS)


modbus.o: modbus.h modbus.c
	gcc -c modbus.c

libUseful-bundled/libUseful.a:
	$(MAKE) -C libUseful-bundled

clean:
	rm -rf *.o *.exe */*.o */*.a */*.so.* */*.so
