wagger:client.c
	gcc client.c -ljack -lm -o wagger
clean: wagger
	rm wagger