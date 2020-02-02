% notcurses_menu(3)
% nick black <nickblack@linux.com>
% v1.1.3

# NAME

notcurses_menu - operations on menus

# SYNOPSIS

**#include <notcurses.h>**

```c
struct ncmenu;

struct ncmenu_section {
  char* name;  // utf-8 c string
  struct ncmenu_item {
    char* desc;  // utf-8 menu item, NULL for horizontal separator
    ncinput shortcut;  // shortcut, all should be distinct
  }* items;
  int itemcount;
};

typedef struct ncmenu_options {
  bool bottom;              // on the bottom row, as opposed to top row
  bool hiding;              // hide the menu when not being used
  struct ncmenu_section* sections; // array of 'sectioncount' menu_sections
  int sectioncount;         // must be positive
  uint64_t headerchannels;  // styling for header
  uint64_t sectionchannels; // styling for sections
} ncmenu_options;
```

**struct ncmenu* ncmenu_create(struct notcurses* nc, const menu_options* opts);**

**int ncmenu_unroll(struct ncmenu* n, int sectionidx);**

**int ncmenu_rollup(struct ncmenu* n);**

**int ncmenu_destroy(struct ncmenu* n);**

# DESCRIPTION


# RETURN VALUES


# SEE ALSO

**notcurses(3)**, **notcurses_input(3)**