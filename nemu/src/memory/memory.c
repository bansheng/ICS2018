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

paddr_t page_translate(paddr_t addr){
  paddr_t dir = (addr >> 22) & 0x3ff;
  paddr_t page = (addr >> 12) & 0x3ff;
  paddr_t offset = addr & 0xfff;
  if(cpu.cr0.paging){
    uint32_t pdb = cpu.cr3.page_directory_base;
    uint32_t pt = paddr_read((pdb << 12) + (dir << 2), 4);
    assert(pt & 1);

    uint32_t pf = paddr_read((pt & 0xfffff000) + (page << 2), 4);
    if(!(pf & 1)){
      printf("%x\n", cpu.eip);
    }
    assert(pf & 1);

    return (pf & 0xfffff000) + offset;
  }
  return addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  // data cross the page boundary 
  if((addr & 0xfff) + len > 0x1000){
    uint8_t temp[8];
    uint32_t temp_offset = addr & 3;
    
    paddr_t paddr = page_translate(addr);
    *(uint32_t *)(temp + temp_offset) = paddr_read(paddr, 4 - temp_offset);

    paddr = page_translate((addr & ~0xfff) + 0x1000);
    *(uint32_t *)(temp + 4) = paddr_read(paddr, len + temp_offset - 4);

    return (*(uint32_t *)(temp + temp_offset)) & (~0u >> ((4 - len) << 3));
    // assert(0);
  }
  return paddr_read(page_translate(addr), len);

}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if((addr & 0xfff) + len > 0x1000){
    assert(0);
  }
  paddr_write(page_translate(addr), data, len);
}


