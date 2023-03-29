SRCS:=$(wildcard *.cc)
OBJECT:=$(patsubst %.cc, %.o, $(SRCS))
ALL:=client.out

a.out:$(OBJECT)
	g++ $(OBJECT) -o ../../bin/client.out

%.o:%.cc
	g++ -c $^ -o $@


.PHONY:
	clean rebuild print
clean:
	rm -rf $(ALL) $(OBJECT)
rebuild: clean $(ALL)
print:
	echo $(OBJECT)
