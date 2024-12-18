libs:
	as -g -o gpu.o ./include/gpu.s
	gcc -c ./include/*.c
	mv *.o ./obj/

assets:
	gcc ./include/assets/sprites.c ./obj/gpu.o -o exec
	gcc -c ./include/assets/backgrounds.c
	mv backgrounds.o ./obj
	sudo ./exec
	mv exec ./include/assets/

test: assets libs  
	gcc ./src/game.c -o run ./obj/*.o -lpthread
	sudo ./run

clean:
	rm -rf ./obj/*.o 
	rm -rf ./run
