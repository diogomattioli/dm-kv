GIT_INFO	= -DGIT_BRANCH='$(shell echo \"`git rev-parse --abbrev-ref HEAD`\")' -DGIT_REVISION='$(shell echo \"`git rev-parse HEAD`\")'
CC			= gcc
CC_FLAGS	= -c -Wall -g3 $(GIT_INFO)
LD_FLAGS	= -pthread
#LD_FLAGS	= --static -Wl,--whole-archive -pthread -Wl,--no-whole-archive

SRCDIR		= src
BINDIR		= bin

SOURCES		= $(wildcard $(SRCDIR)/*.c)
OBJECTS		= $(addprefix $(BINDIR)/, $(notdir $(SOURCES:.c=.o)))
EXEC		= $(addprefix $(BINDIR)/, dm-kv)

all: $(BINDIR) $(EXEC)

$(EXEC): $(OBJECTS) 
	$(CC) $(LD_FLAGS) -o $@ $(OBJECTS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CC_FLAGS) $< -o $@

$(BINDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJECTS) $(EXEC)