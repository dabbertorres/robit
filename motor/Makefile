SRC := motor.c

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))
DEPS	:= $(OUT_DIR)/gpio.a

.PHONY: clean

$(OUT_DIR)/motor.a: $(OBJS)
	$(AR) rcs $@ $^ $(DEPS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

