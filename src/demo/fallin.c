#include "demo.h"
#include <pthread.h>

static bool done = false;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static int
patentpulser(struct notcurses* nc, struct ncplane* ncp, void* curry){
  (void)ncp;
  (void)curry;
  DEMO_RENDER(nc);
  bool donecheck;
  pthread_mutex_lock(&lock);
  donecheck = done;
  pthread_mutex_unlock(&lock);
  if(donecheck){
    return 2;
  }
  return 0;
}

static void*
patentpulsar(void* n){
  struct notcurses* nc = n;
  ncplane_pulse(notcurses_stdplane(nc), &demodelay, patentpulser, NULL);
  return NULL;
}

static int
drop_bricks(struct notcurses* nc, struct ncplane** arr, int arrcount){
  if(arrcount == 0 || arr == NULL){
    return -1;
  }
  int stdy, stdx;
  notcurses_term_dim_yx(nc, &stdy, &stdx);
  // an erase+render cycle ought not change the screen, as we duplicated it
  struct timespec iterdelay;
  // 5 * demodelay total
  ns_to_timespec(timespec_to_ns(&demodelay) / arrcount / 2, &iterdelay);
  // we've got a range of up to 10% total blocks falling at any given time. they
  // accelerate as they fall. [ranges, reange) covers the active range.
  int ranges = 0;
  int rangee = 0;
  const int FALLINGMAX = arrcount / 10;
  int speeds[FALLINGMAX];
  while(ranges < arrcount){
    // if we don't have a full set active, and there is another available, go
    // ahead and get it kicked off
    if(rangee - ranges + 1 < FALLINGMAX){
      if(rangee < arrcount){
        speeds[rangee - ranges] = 1;
        ++rangee;
      }
    }
    do{
      DEMO_RENDER(nc);
      // don't allow gaps in the active range. so long as felloff is true, we've only handled
      // planes which have fallen off the screen, and can be collected.
      bool felloff = true;
      for(int i = 0 ; i < rangee - ranges ; ++i){
        struct ncplane* ncp = arr[ranges + i];
        int x, y;
        ncplane_yx(ncp, &y, &x);
        if(felloff){
          if(y + speeds[i] >= stdy){
            ncplane_destroy(ncp);
            arr[ranges + i] = NULL;
            if(ranges + i + 1 == arrcount){
              ranges += i + 1;
              break;
            }
          }else{ // transition point
            if(i){
              if(rangee - ranges - i){
                memmove(speeds, speeds + i, (rangee - ranges - i) * sizeof(*speeds));
              }
              ranges += i;
              i = 0;
            }
            felloff = false;
          }
        }
        if(!felloff){
          ncplane_move_yx(ncp, y + speeds[i], x);
          ++speeds[i];
        }
        nanosleep(&iterdelay, NULL);
      }
    }while(rangee - ranges + 1 >= FALLINGMAX);
  }
  free(arr);
  return 0;
}

// Shuffle a new ncplane into the array of ncplanes having 'count' elements.
static struct ncplane**
shuffle_in(struct ncplane** arr, int count, struct ncplane* n){
  struct ncplane** tmp = realloc(arr, sizeof(*arr) * (count + 1));
  if(tmp == NULL){
    return NULL;
  }
  arr = tmp;
  // location of new element
  int pos = random() % (count + 1);
  if(pos < count){
    // move everything, starting at our new location, one spot right
    memmove(arr + pos + 1, arr + pos, sizeof(*arr) * (count - pos));
  }
  arr[pos] = n;
  return arr;
}

// ya playin' yourself
int fallin_demo(struct notcurses* nc){
  if(!notcurses_canopen(nc)){
    return 0;
  }
  int dimx, dimy;
  ncplane_dim_yx(notcurses_stdplane(nc), &dimy, &dimx);
  size_t usesize = sizeof(bool) * dimy * dimx;
  bool* usemap = malloc(usesize);
  memset(usemap, 0, usesize);
  // brick size is relative to the screen size
  const int maxx = dimx > 39 ? dimx / 10 : 2;
  const int maxy = dimy > 19 ? dimy / 10 : 2;
  // proceed from top to bottom, left to right, and partition the existing
  // content into 'arrcount' copies into small ncplanes in 'arr'.
  struct ncplane** arr = NULL;
  int arrcount = 0;
  // There are a lot of y/x pairs at this point:
  //  * dimx/dimy: geometry of standard plane
  //  * y/x: iterators through standard plane
  //  * maxy/maxx: maximum geometry of randomly-generated bricks
  //  * newy/newx: actual geometry of current brick
  //  * usey/usex: 
  ncplane_greyscale(notcurses_stdplane(nc));
  for(int y = 0 ; y < dimy ; ++y){
    int x = 0;
    while(x < dimx){
      if(usemap[y * dimx + x]){ // skip if we've already been copied
        ++x;
        continue;
      }
      int newy, newx;
      newy = random() % (maxy - 1) + 2;
      newx = random() % (maxx - 1) + 2;
      if(x + newx >= dimx){
        newx = dimx - x;
      }
      if(y + newy >= dimy){
        newy = dimy - y;
      }
      struct ncplane* n = ncplane_new(nc, newy, newx, y, x, NULL);
      if(n == NULL){
        return -1;
      }
      // copy the old content into this new ncplane
      for(int usey = y ; usey < y + newy ; ++usey){
        for(int usex = x ; usex < x + newx ; ++usex){
          if(usemap[usey * dimx + usex]){
            newx = usex - x;
            ncplane_resize_simple(n, newy, newx);
            continue;
          }
          cell c = CELL_TRIVIAL_INITIALIZER;
          if(ncplane_at_yx(notcurses_stdplane(nc), usey, usex, &c) < 0){
            return -1;
          }
          if(!cell_simple_p(&c)){
            const char* cons = cell_extended_gcluster(notcurses_stdplane(nc), &c);
            c.gcluster = 0;
            cell_load(n, &c, cons);
          }
          if(c.gcluster){
            if(ncplane_putc_yx(n, usey - y, usex - x, &c) < 0){
              // allow a fail if we were printing a wide char to the
              // last column of our plane
              if(!cell_double_wide_p(&c) || usex + 1 < x + newx){
                return -1;
              }
            }
          }
          usemap[usey * dimx + usex] = true;
          cell_release(n, &c);
        }
      }
      // shuffle the new ncplane into the array
      struct ncplane **tmp;
      tmp = shuffle_in(arr, arrcount, n);
      if(tmp == NULL){
        return -1;
      }
      arr = tmp;
      ++arrcount;
      x += newx;
    }
  }
  free(usemap);
  int averr = 0;
  char* path = find_data("lamepatents.jpg");
  struct ncvisual* ncv = ncplane_visual_open(notcurses_stdplane(nc), path, &averr);
  free(path);
  if(ncv == NULL){
    return -1;
  }
  if(ncvisual_decode(ncv, &averr) == NULL){
    ncvisual_destroy(ncv);
    return -1;
  }
  if(ncvisual_render(ncv, 0, 0, 0, 0)){
    ncvisual_destroy(ncv);
    return -1;
  }
  pthread_t tid;
  if(pthread_create(&tid, NULL, patentpulsar, nc)){
    return -1;
  }
  int ret = drop_bricks(nc, arr, arrcount);
  pthread_mutex_lock(&lock);
  done = true;
  pthread_mutex_unlock(&lock);
  if(pthread_join(tid, NULL)){
    return -1;
  }
  assert(ncvisual_decode(ncv, &averr) == NULL);
  assert(averr == AVERROR_EOF);
  ncvisual_destroy(ncv);
  return ret;
}
