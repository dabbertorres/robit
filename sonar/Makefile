SRC := sonar.c

OBJ_DIR := obj
OBJS    := $(addprefix $(OBJ_DIR)/,$(SRC:%.c=%.o))

.PHONY: clean

$(OUT_DIR)/sonar.a: $(OBJS)
	$(AR) rcs $@ $^

$(OBJS): $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
