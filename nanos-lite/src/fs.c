#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset; //读写指针,这个是后来按照要求添加的
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
	int i;
	//Log("the total files : %d\n", NR_FILES);
	//Log("pathname %s\n", pathname);
	for (i = 0; i < NR_FILES; i++) {
		// printf("file name: %s\n", file_table[i].name);
		if (strcmp(file_table[i].name, pathname) == 0) {
			file_table[i].open_offset = 0;
			return i;
		}
	}
	//Log("i am here~~\n");
	assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
	
	if(fd > 2)
	{
		if(file_table[fd].open_offset >=  fs_filesz(fd)|| len == 0)
		{
			Log("file read hit the End!");
			return 0;
		}
				
		if(file_table[fd].open_offset + len > fs_filesz(fd))
			len = fs_filesz(fd) - file_table[fd].open_offset;
		ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
	}
	return len;
}

uintptr_t sys_write(int fd, const void *buf, size_t len) {
	// Log(" 1 ");
	uintptr_t i = 0;
	if (fd == 1 || fd == 2) {
		for(; len > 0; len--) {
			_putc(((char*)buf)[i]);
			i++;;
		}
	}
	return i;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
	if(fd == 1 || fd == 2) return sys_write(fd, buf, len);
	else if(fd > 2)
	{
		if(file_table[fd].open_offset >= fs_filesz(fd) || len == 0)
		{
			Log("file write hit the End!");
			return 0;
		}
				
		if(file_table[fd].open_offset + len > fs_filesz(fd))
			len = fs_filesz(fd) - file_table[fd].open_offset;
		ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
		return len;
	}
	
	return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
	size_t result = -1;

	switch(whence) {
		case SEEK_SET:
			if (offset >= 0 && offset <= fs_filesz(fd)) {
				file_table[fd].open_offset = offset;
				result = file_table[fd].open_offset = offset;
			}
			break;
		case SEEK_CUR:
			if ((offset + file_table[fd].open_offset >= 0) && 
					(offset + file_table[fd].open_offset <= fs_filesz(fd))) {
				file_table[fd].open_offset += offset;
				result = file_table[fd].open_offset;
			}
			break;
		case SEEK_END:
			file_table[fd].open_offset = fs_filesz(fd) + offset;
			result = file_table[fd].open_offset;
			break;
	}
	
	return result;
}

int fs_close(int fd)
{
	file_table[fd].open_offset = 0;
	return 0;
}

size_t fs_filesz(int fd)
{
	return file_table[fd].size;
}


