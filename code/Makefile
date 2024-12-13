assets:
	gcc ./include/assets/sprites.c -o exec
	sudo ./exec
	mv exec ./include/assets/

libs:
	as -g -o gpu.o ./include/gpu.s
	gcc -c ./include/*.c
	mv *.o ./obj/

test: assets libs
	gcc ./src/game.c -o run ./obj/*.o -lpthread
	sudo ./run
