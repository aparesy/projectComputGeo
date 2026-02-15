OBJS = convex_hull.o
EXECS = convex_hull
RESULTS = *.txt

REBUILDABLES = $(OBJS) $(EXECS) $(RESULTS)

run : 

clean : rm -f $(REBUILDABLES)

$(LINK_TARGET) : $(OBJS)
	g++ -g -o $@ $^ 

%.o : %.c
	g++ -g -fsanitize=bounds -fsanitize=address -fsanitize=undefined -Wall -o $@ -c $<

