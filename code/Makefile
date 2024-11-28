libs:
	as -g -o gpu.o ./include/gpu.s
	gcc -c ./include/accel.c
	gcc -c ./include/mouse.c
	mv *.o ./obj/

test: libs
	gcc ./src/movement_test.c -o run ./obj/*.o -lpthread
	sudo ./run
