SRC := motor.c
DEP := $(OUT_DIR)/gpio_dma.a

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

.PHONY: clean

$(OUT_DIR)/motor_dma.a: $(OBJS) $(DEP)
	$(AR) rcs $@ $^ $(DEP)

$(OBJS): $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -o $@ -c $(CFLAGS) $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
