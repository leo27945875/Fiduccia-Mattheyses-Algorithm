FILE_NUM = $(i)


main: *.cpp include/*.hpp structure.o partition.o
	g++ main.cpp structure.o partition.o -o Lab1

structure.o: structure.cpp
	g++ -c structure.cpp

partition.o: partition.cpp
	g++ -c partition.cpp

clean:
	rm *.o Lab1

test: *.cpp include/*.hpp structure.o partition.o
	make
	./Lab1 ./data/input${i}.in ./data/output${i}.out

run: *.cpp include/*.hpp structure.o partition.o
	make
	./Lab1 $(input) $(output)

push:
	git add .
	git commit -m "Commit"
	git push origin master

check:
	./SolutionChecker/SolutionChecker ./data/input${i}.in ./data/output${i}.out