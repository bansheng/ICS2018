#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
	int mmio_n;
	if ((mmio_n = is_mmio(addr)) != -1)
		return mmio_read(addr, len, mmio_n);
	else
		return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
	int mmio_n;
	if ((mmio_n = is_mmio(addr)) != -1)
		mmio_write(addr, len, data, mmio_n);
	else
	{
		memcpy(guest_to_host(addr), &data, len);
	}
 		
}

paddr_t page_translate(vaddr_t vaddr){
  //printf("Virtual Address:%d\n",vaddr);
  uint32_t pdirent= paddr_read(((0xfffff000)&cpu.cr3.val)|((vaddr>>20)&0xffc),4);
  //printf("Page Directory Table entry:%d\n",pdirent);
  Assert(pdirent&1,"Page Directory Table Entry Invalid! Attempted address:%x",vaddr);
  uint32_t ptblent=paddr_read(((0xfffff000)&pdirent)|((vaddr>>10)&0xffc),4);
  //printf("Page Table Entry:%d\n",ptblent);
  Assert(ptblent&1,"Page Table Entry Invalid! Current CR3:%d\nAttempted Address:%x \n The page table entry is %x\n The page table directory entry is %x\n Current EIP is %x",
    cpu.cr3.val,vaddr,ptblent,pdirent,cpu.eip);
  //printf("Actual address:%d\n",(ptblent&0xfffff000)|(vaddr&0xfff));
  return (ptblent&0xfffff000)|(vaddr&0xfff);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.cr0.paging==0)
    return paddr_read(addr, len);
  else{
    //printf("Virtual Address!\n");
    if((addr&0xfff)+len>0x1000){
      uint32_t lbytes = 0x1000-(addr&0xfff);
      uint32_t lower = paddr_read(page_translate(addr),lbytes);
      uint32_t higher = vaddr_read(addr+lbytes,len-lbytes);
      return (higher<<(lbytes*8))|lower;
    }
    else {
      uint32_t result = paddr_read(page_translate(addr),len);
      return result;
      }

  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if(cpu.cr0.paging==0)
    paddr_write(addr, data, len);
  else{
    //printf("Virtual Address!\n");
    if((addr&0xfff)+len>0x1000){
      uint32_t lbytes = 0x1000-(addr&0xfff);
      paddr_write(page_translate(addr),data,lbytes);
      vaddr_write(addr+lbytes,data>>(8*lbytes),len-lbytes);
    }
    else paddr_write(page_translate(addr),data,len);
  }
}
