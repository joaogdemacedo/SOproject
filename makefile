ma: ma.c
	gcc ma.c -o ma
sv: sv.c
	gcc sv.c -o sv
cv: cv.c
	gcc cv.c -o cv
clean_all: 
	rm strings.txt && rm artigos.txt
clean_ma:
	rm ma
clean_sv:
	rm sv
clean_cv:
	rm cv
