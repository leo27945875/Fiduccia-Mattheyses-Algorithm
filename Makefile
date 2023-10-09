main: *.cpp structure.o partition.o include/*.hpp
	g++ main.cpp structure.o partition.o -o Lab1

structure.o: structure.cpp
	g++ -c structure.cpp

partition.o: partition.cpp
	g++ -c partition.cpp

clean:
	rm *.o Lab1 test

test: test.cpp
	g++ test.cpp -o test
	./test

run:
	make
	./Lab1 data/input0.txt data/output0.txt

push:
	git add .
	git commit -m "Commit"
	git push origin master