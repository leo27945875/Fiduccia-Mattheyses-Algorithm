FILE_NUM = $(i)


main: *.cpp structure.o partition.o include/*.hpp
	g++ main.cpp structure.o partition.o -o Lab1

structure.o: structure.cpp
	g++ -c structure.cpp

partition.o: partition.cpp
	g++ -c partition.cpp

clean:
	rm *.o Lab1

test: *.cpp structure.o partition.o include/*.hpp
	make
	./Lab1 ./data/input${i}.in ./data/output${1}.out

run: *.cpp structure.o partition.o include/*.hpp
	make
	./Lab1 $(input) $(output)

push:
	git add .
	git commit -m "Commit"
	git push origin master

check:
	./SolutionChecker/SolutionChecker ./data/input0.in ./data/output0.out