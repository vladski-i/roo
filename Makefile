roo:roo.c
	gcc roo.c -ljack -lm -o roo
unroo:unroo.c
	gcc unroo.c -ljack -lm -o unroo
clean: roo
	rm roo