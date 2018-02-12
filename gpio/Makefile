SRC := gpio.c

OBJ_DIR := obj
OBJS := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

.PHONY: clean

$(OUT_DIR)/gpio.a: $(OBJS)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

