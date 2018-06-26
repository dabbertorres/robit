SRC := main.c

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

DEPS := -l:gpio.a -l:motor.a

.PHONY: clean

$(OUT_DIR)/robit: $(OBJS)
	$(CC) -o $@ $^ -L$(OUT_DIR) $(DEPS)

$(OBJS): $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
