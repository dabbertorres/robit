SRC := main.c

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

DEPS := -l:gpio.a -l:motor.a

.PHONY: clean

$(OUT_DIR)/robit: $(OBJS)
	$(CC) -o $@ $^ -L$(OUT_DIR) $(DEPS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

