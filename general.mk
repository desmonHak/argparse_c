include config.mk

generate_lib: liblexpar_c.a $(TARGET).a
	ar -t $(TARGET).a

generate_lib_debug: liblexpar_c_debug.a $(TARGET)_debug.a
	ar -t $(TARGET).a
all: generate_lib
	$(MAKE) -C . -f $(MAKE_NAME) examples

examples: generate_lib
	$(CC) examples/code.c $(CFLAGS_EXAMPLES) -o code.$(EXTENSION)
examples_debug: generate_lib_debug
	$(CC) examples/code.c $(CFLAGS_EXAMPLES_DEBUG) -o code.$(EXTENSION)

liblexpar_c.a:
	echo "generando librerias estatica... $@"
	$(MAKE) -C ./$(PATH_LEXPAR) -f $(MAKE_NAME)

liblexpar_c_debug.a:
	echo "generando librerias estatica... $@"
	$(MAKE) -C ./$(PATH_LEXPAR) -f $(MAKE_NAME) generate_lib_debug

$(TARGET).a: $(OBJECTS)
	echo "generando librerias estatica... $@"
	$(ARR) $(ARR_FLAGS) $@ $^
	ranlib $@

$(TARGET)_debug.a: $(OBJECTS_DEBUG)
	$(ARR) $(ARR_FLAGS) $(TARGET).a $^
	ranlib $(TARGET).a

argparse_c.o: $(PATH_SRC)/argparse_c.c
	$(CC) $(CFLAGS) -c $^ -o $@

argparse_c_debug.o: $(PATH_SRC)/argparse_c.c
	$(CC) $(CFLAGS_DEBUG) -c $^ -o $@

cleanobj:
	$(RM) $(RMFLAGS) $(OBJECTS)
	$(MAKE) -C ./$(PATH_LEXPAR) -f $(MAKE_NAME) cleanobj

cleanall: cleanobj
	$(RM) $(RMFLAGS) *.o $(TARGET).a \
	$(TARGET_DEBUG).a *.$(EXTENSION) 
	$(MAKE) -C ./$(PATH_LEXPAR) -f $(MAKE_NAME) cleanall

.SILENT: clean cleanobj cleanall
.IGNORE: cleanobj cleanall
.PHONY:  cleanobj cleanall