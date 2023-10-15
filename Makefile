FILE_NUM = $(i)
CPP_FILES = ${wildcard *.cpp}
HDR_FILES = ${wildcard include/*.hpp}


main: ${CPP_FILES} ${HDR_FILES} structure.o partition.o
	g++ main.cpp structure.o partition.o -o Lab1

structure.o: structure.cpp ${HDR_FILES}
	g++ -c structure.cpp

partition.o: partition.cpp ${HDR_FILES}
	g++ -c partition.cpp

clean:
	rm *.o Lab1

depends:
	@echo "\nCPP_FILES: ${CPP_FILES}"
	@echo "HDR_FILES: ${HDR_FILES}\n"

test: ${CPP_FILES} ${HDR_FILES} structure.o partition.o
	make
	./Lab1 ./data/input${i}.in ./data/output${i}.out

run: ${CPP_FILES} ${HDR_FILES} structure.o partition.o
	make
	./Lab1 $(input) $(output)

push:
	git add .
	git commit -m "Commit"
	git push origin master

check:
	./SolutionChecker/SolutionChecker ./data/input${i}.in ./data/output${i}.out

gen:
	python3 generate.py