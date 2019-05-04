final: sv
	gcc ma.c -o ma

sv: sv.c cv
	gcc sv.c -o sv

cv: cv.c 
	gcc cv.c -o cv

clean: 
	rm ma sv cv

cleanall:
	rm strings.txt artigos.txt