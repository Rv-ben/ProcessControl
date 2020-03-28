fishy: swim_mill.c
	gcc swim_mill.c -o bin/swim -lpthread
	gcc fish.c -lm -o bin/fish
	gcc pellet.c -o bin/pellet -lm
