SRC := main.c

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

DEPS := motor_dma.a gpio_dma.a

.PHONY: clean

$(OUT_DIR)/robit: $(OBJS) $(addprefix $(OUT_DIR)/,$(DEPS))
	$(CC) -o $@ $^ -L$(OUT_DIR) $(addprefix -l:,$(DEPS))

$(OBJS): $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
