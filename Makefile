lecteur: arbo clean
	gcc -c src/audio.c -o obj/audio.o
	gcc -Wall -c src/lecteur.c -o obj/lecteur.o
	gcc obj/audio.o obj/lecteur.o -o bin/lecteur

clean :
	rm -f bin/*
	rm -f obj/*

arbo : 
	[ ! -d "./bin/" ] && mkdir bin || true
	[ ! -d "./obj/" ] && mkdir obj || true
	[ ! -d "./src/" ] && mkdir src || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./lecteur.c" ] && mv ./lecteur.c ./src/ || true
	[ -f "./audio.c" ] && mv ./audio.c ./src/ || true
	[ -f "./audio.h" ] && mv ./audio.h ./src/ || true
	[ -f "./test.wav" ] && mv ./test.wav ./src/ || true