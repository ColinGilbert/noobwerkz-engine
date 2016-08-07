include makefile.defs

all: $(OUT)

.cpp.o:
	$(CC) $(INCLUDE) $(CCFLAGS) $(LIBS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)
	
clean:
	rm -f $(OBJ) $(OUT)

include makefile.dep
