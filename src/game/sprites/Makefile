G_REL_CURR_DIR:=$(G_REL_CURR_DIR)/$(shell basename $$PWD)
C_SOURCE_FILES := $(wildcard *.c)
OBJ_DIR=$(G_OBJ_DIR)$(G_REL_CURR_DIR)
C_OBJ_FILES=$(C_SOURCE_FILES:%.c=$(OBJ_DIR)/%.o)
CFLAGS=$(G_CFLAGS)
LIBS=$(G_LIBS)

.PHONY: default
default : record-obj check-directory $(C_OBJ_FILES)

$(C_OBJ_FILES) : $(OBJ_DIR)/%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

check-directory:
	if [ ! -d "$(OBJ_DIR)" ]; then \
		mkdir -p "$(OBJ_DIR)"; \
	fi

record-obj:
	echo $(C_OBJ_FILES) >> $(G_SRC_DIR)/.obj_files.tmp

clean:
	rm -rf $(OBJ_DIR)/*.o

print-%: ;@echo $* = $($*)
