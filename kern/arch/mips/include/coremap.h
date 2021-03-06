#ifndef _COREMAP_H_
#define _COREMAP_H_

/*
 * Coremap definitions
 */

#include <machine/vm.h>

#define CME_FREE 0
#define CME_FIXED 1
#define CME_CLEAN 2
#define CME_DIRTY 3

#define INVALID_PADDR ((paddr_t)0)
#define PADDR_IS_VALID(paddr) ((paddr >= base*PAGE_SIZE) && (paddr % PAGE_SIZE == 0))

uint32_t base; // Number of pages taken up by coremap

/* Core map structures and functions */
struct cm_entry{
    struct addrspace *as;
    int disk_offset;  // Stores the disk offset when page in memory
    vaddr_t vaddr_base:20;
    int junk:8;
    unsigned int state:2;
    unsigned int busy_bit:1;
    unsigned int use_bit:1;
};

struct cv *written_to_disk;
struct lock *cv_lock;

struct vnode *swapfile;
struct bitmap *disk_map;
struct lock *disk_map_lock;
struct semaphore *dirty_pages;

/*
 * Page selection APIs
 */
paddr_t alloc_one_page(struct addrspace *as, vaddr_t va);
vaddr_t alloc_kpages(int npages);
void free_coremap_page(paddr_t pa, bool iskern);
void free_kpages(vaddr_t va);
int find_free_page(void);
int choose_evict_page(void);
void pin_all_pages(struct addrspace *as);

/*
 * Acessor/setter methods
 */
int cm_get_index(paddr_t pa);

int cme_get_vaddr(int ix);
void cme_set_vaddr(int ix, int vaddr);

int cme_get_offset(int ix);
void cme_set_offset(int ix, int offset);

unsigned cme_get_state(int ix);
void cme_set_state(int ix, unsigned state);

/* core map entry pinning */
unsigned cme_get_busy(int ix);
void cme_set_busy(int ix, unsigned busy);

// Attempts to (synchronously) acquire busy bit on given CME and returns success or failure
unsigned cme_try_pin(int ix);

unsigned cme_get_use(int ix);
void cme_set_use(int ix, unsigned use);

/*
 * Bootstrap
 */
void coremap_bootstrap(void);

/*
 * Swap space functions
 */

void swapfile_init(void);
unsigned swapfile_reserve_index(void); // Called in mark_allocated()
void swapfile_free_index(unsigned index); // Called in free_coremap_page()

int swapout(paddr_t ppn);
int swapin(struct addrspace *as, vaddr_t vpn, paddr_t dest);
void evict_page(paddr_t ppn);
int write_page(void *page, unsigned offset);
int read_page(void *page, unsigned offset);
void writer_thread(void *junk, unsigned long num);


#endif /* _COREMAP_H_ */
