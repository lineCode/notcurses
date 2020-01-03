% notcurses_cell(3)
% nick black <nickblack@linux.com>
% v0.9.9

# NAME

**notcurses_cell**—operations on notcurses cells

# SYNOPSIS

**#include <notcurses.h>**

```c
typedef struct cell {
  uint32_t gcluster;
  uint32_t attrword;
  uint64_t channels;
} cell;

#define CELL_TRIVIAL_INITIALIZER \
 { .gcluster = '\0', .attrword = 0, .channels = 0, }
#define CELL_SIMPLE_INITIALIZER(c) \
 { .gcluster = (c), .attrword = 0, .channels = 0, }
#define CELL_INITIALIZER(c, a, chan) \
 { .gcluster = (c), .attrword = (a), .channels = (chan), }
```

**void cell_init(cell* c);**

**int cell_load(struct ncplane* n, cell* c, const char* gcluster);**

**int cell_prime(struct ncplane* n, cell* c, const char *gcluster, uint32_t attr, uint64_t channels);**

**int cell_duplicate(struct ncplane* n, cell* targ, const cell* c);**

**void cell_release(struct ncplane* n, cell* c);**

**void cell_styles_set(cell* c, unsigned stylebits);**

**unsigned cell_styles(const cell* c);**

**void cell_styles_on(cell* c, unsigned stylebits);**

**void cell_styles_off(cell* c, unsigned stylebits);**

**void cell_set_fg_default(cell* c);**

**void cell_set_bg_default(cell* c);**

**int cell_set_fg_alpha(cell* c, int alpha);**

**int cell_set_bg_alpha(cell* c, int alpha);**

**bool cell_double_wide_p(const cell* c);**

**bool cell_simple_p(const cell* c);**

**const char* cell_extended_gcluster(const struct ncplane* n, const cell* c);**

**bool cell_noforeground_p(const cell* c);**

**bool cell_nobackground_p(const struct ncplane* n, const cell* c);**

**int cell_load_simple(struct ncplane* n, cell* c, char ch);**

**uint32_t cell_egc_idx(const cell* c);**

**unsigned cell_bchannel(const cell* cl);**

**unsigned cell_fchannel(const cell* cl);**

**uint64_t cell_set_bchannel(cell* cl, uint32_t channel);**

**uint64_t cell_set_fchannel(cell* cl, uint32_t channel);**

**uint64_t cell_blend_fchannel(cell* cl, unsigned channel);**

**uint64_t cell_blend_bchannel(cell* cl, unsigned channel);**

**unsigned cell_fg(const cell* cl);**

**unsigned cell_bg(const cell* cl);**

**unsigned cell_fg_alpha(const cell* cl);**

**unsigned cell_bg_alpha(const cell* cl);**

**unsigned cell_fg_rgb(const cell* cl, unsigned* r, unsigned* g, unsigned* b);**

**unsigned cell_bg_rgb(const cell* cl, unsigned* r, unsigned* g, unsigned* b);**

**int cell_set_fg_rgb(cell* cl, int r, int g, int b);**

**int cell_set_bg_rgb(cell* cl, int r, int g, int b);**

**int cell_set_fg(cell* c, uint32_t channel);**

**int cell_set_bg(cell* c, uint32_t channel);**

**bool cell_fg_default_p(const cell* cl);**

**bool cell_bg_default_p(const cell* cl);**

# DESCRIPTION


# RETURN VALUES

# SEE ALSO

**notcurses(3)**, **notcurses_channels(3)**, **notcurses_ncplane(3)**,
**notcurses_output(3)**