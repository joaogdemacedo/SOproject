final: sv
	gcc ma.c -o ma

sv: sv.c cv
	gcc sv.c -o sv

cv: cv.c ag
	gcc cv.c -o cv

ag: ag.c
	gcc ag.c -o ag

clean: 
	rm ma sv cv

cleanall:
	rm strings.txt artigos.txt