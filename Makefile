OBJS = convex_hull.o
EXECS = convex_hull tmp visu
RESULTS = *.txt
FLAGS = -fsanitize=bounds -fsanitize=address -fsanitize=undefined -Wall -O3 -std=c++17 -g
VISU_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
REBUILDABLES = $(OBJS) $(EXECS) $(RESULTS)

run : 
	g++ $(FLAGS) -DSAMPLE -o convex_hull convex_hull.h convex_hull.cpp ; ./convex_hull ; python3 visualizer.py

perf : 
	g++ $(FLAGS) -DPERF -o convex_hull convex_hull.h convex_hull.cpp  ; ./convex_hull

visu:
	g++ $(FLAGS) $(VISU_FLAGS) -o visu visu_hull.cpp ; ./visu
 
clean : 
		rm -f $(REBUILDABLES)

$(LINK_TARGET) : $(OBJS)
	g++ -g -o $@ $^ 

%.o : %.c
	g++ -g $(FLAGS) -o $@ -c $<

