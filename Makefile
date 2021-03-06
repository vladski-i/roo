roo:client.c
	gcc client.c -ljack -lm -o roo
clean: roo
	rm roo