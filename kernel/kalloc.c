// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct spinlock reflock; // 直接用一个全局的锁来保证引用计数吧
int refcount[PHYSTOP / PGSIZE + 1];

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  // lab5: 初始化引用计数使用的锁
  initlock(&reflock, "ref");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    refcount[(uint64)p / PGSIZE] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  r = (struct run*)pa;

  acquire(&reflock);
  refcount[(uint64)r / PGSIZE]--;
  if(refcount[(uint64)r / PGSIZE] <= 0) {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    refcount[(uint64)r / PGSIZE] = 0;
    release(&kmem.lock);
  }
  release(&reflock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
    memset((char *) r, 5, PGSIZE); // fill with junk
    // 这里无需加锁
    refcount[(uint64)r / PGSIZE] = 1; // 初始化引用计数为1
  }
  return (void*)r;
}

// utils for cow pages
// 返回对应page的引用计数
uint64 getrcount(void* pa) {
  acquire(&reflock);
  uint64 res = refcount[(uint64)pa / PGSIZE];
  release(&reflock);
  return res;
}

// 引用计数+1
void rincrease(void* pa) {
  // add error check
  if ((long) pa >= PHYSTOP || refcount[(uint64)pa / PGSIZE] < 1) {
    return;
  }
  acquire(&reflock);
  refcount[(uint64)pa / PGSIZE]++;
  release(&reflock);
}
