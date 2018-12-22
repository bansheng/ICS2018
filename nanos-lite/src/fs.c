#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);

extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
	char *name;
	size_t size;
	size_t disk_offset;
	size_t open_offset; //读写指针,这个是后来按照要求添加的
	ReadFn read;
	WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_TTY};

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
  {"stdin", 0, 0, 0,invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, 0, NULL, fb_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, 0, events_read},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0, 0, dispinfo_read},
  [FD_TTY] = {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
	// TODO: initialize the size of /dev/fb
	file_table[FD_FB].size = screen_width() * screen_height() * 4;  
/*	for(int i=6; i<NR_FILES; i++)*/
/*	{*/
/*		file_table[i].read = ramdisk_read;*/
/*		file_table[i].write = ramdisk_read;*/
/*	}*/
}

int fs_open(const char *pathname, int flags, int mode) {
	int i;
	//Log("the total files : %d\n", NR_FILES);
	//Log("pathname %s\n", pathname);
	for (i = 0; i < NR_FILES; i++) {
		// 
		if (strcmp(file_table[i].name, pathname) == 0) {
			file_table[i].open_offset = 0;
			printf("file open: %s\n", file_table[i].name);
			return i;
		}
	}
	//Log("i am here~~\n");
	assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	size_t fs_size = fs_filesz(fd);
	
	
	//Log("in the read, fd = %d, file size = %d, len = %d, file open_offset = %d\n", fd, fs_size, len, file_table[fd].open_offset);
	switch(fd) {
		case FD_STDIN:
		case FD_STDOUT:
		case FD_STDERR:
		case FD_FB:
			break;
		case FD_EVENTS:
			//Log("in the fs_read fd_fb\n");
			len = file_table[fd].read(buf, 0, len);
			break;
		case FD_DISPINFO:
			if (file_table[fd].open_offset >= file_table[fd].size)
				return 0;
			if (file_table[fd].open_offset + len > fs_size)
				len = file_table[fd].size - file_table[fd].open_offset;
			file_table[fd].read(buf, file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;	
			break;
		default:
			if(file_table[fd].open_offset >= fs_size)
				return 0;
			if(file_table[fd].open_offset + len > fs_size)
				len = fs_size - file_table[fd].open_offset;
			//printf("read file: size = %d, len = %d, file open_offset = %d\n", fs_size, len, file_table[fd].open_offset);
			ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
			break;
	}
	return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
	size_t fs_size = fs_filesz(fd);
	//Log("in the write, fd = %d, file size = %d, len = %d, file open_offset = %d\n", fd, fs_size, len, file_table[fd].open_offset);
	
	
	switch(fd) {
		case FD_STDIN: break;
		case FD_TTY:
		case FD_STDOUT:
		case FD_STDERR:
			file_table[fd].write(buf, 0, len);
			break;
		case FD_FB:
			if(file_table[fd].open_offset >= fs_size)
				return 0;
			if(file_table[fd].open_offset + len > fs_size)
				len = fs_size - file_table[fd].open_offset;
			file_table[fd].write(buf, file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
			break;
		case FD_EVENTS:
		case FD_DISPINFO:
			break;
		default:
			// write to ramdisk
			if(file_table[fd].open_offset >= fs_size)
				return 0;	
			if(file_table[fd].open_offset + len > fs_size)
				len = fs_size - file_table[fd].open_offset;
			
			//printf("write file: size = %d, len = %d, file open_offset = %d\n", fs_size, len, file_table[fd].open_offset);
			ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
			break;
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
	//file_table[fd].open_offset = 0;
	return 0;
}

size_t fs_filesz(int fd)
{
	return file_table[fd].size;
}


