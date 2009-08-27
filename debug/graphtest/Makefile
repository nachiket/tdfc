LEDA     = /project/cs/brass/a/tools/free/LEDA-3.7.1/LEDA-3.7.1-i386-linux-g++-2.96
FAKELEDA = /tmp/fakeLEDA

TARGETS = graphtest graphtest-fakeLEDA
OUT     = $(TARGETS:=.out)

all:	$(TARGETS)

graphtest:	graphtest.cc
	g++ -I$(LEDA)/incl -L$(LEDA) $^ -lG -lL -o $@

graphtest-fakeLEDA:	graphtest.cc
	g++ -I$(FAKELEDA)/incl -L$(FAKELEDA) $^ -lG -lL -o $@

%.out:
	./$* > $@

out:	$(TARGETS) $(OUT)

test:	out

clean:
	rm $(TARGETS) $(OUT)
