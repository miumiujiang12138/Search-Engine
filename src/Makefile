SRCS:=$(wildcard *.cc)
OBJECT:=$(patsubst %.cc, %.o, $(SRCS))
ALL:=server.out

a.out:$(OBJECT)
	g++ $(OBJECT) -o ../bin/server.out -lpthread -lhiredis -llog4cpp 

%.o:%.cc
	g++ -c $^ -o $@


.PHONY:
	clean rebuild print
clean:
	rm -rf $(ALL) $(OBJECT)
rebuild: clean $(ALL)
print:
	echo $(OBJECT)
