libs:
	as -g -o gpu.o ./include/gpu.s
	gcc -c ./include/accel.c
	mv *.o ./obj/

test: libs
	gcc ./src/movement_test.c -o run ./obj/*.o -lpthreads
	sudo ./run
