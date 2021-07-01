CC=gcc
CFLAGS=-lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image -lm -g

compile: src/*.c src/*.h
	$(CC) src/*.c src/*.h -o Logistruct $(CFLAGS)
