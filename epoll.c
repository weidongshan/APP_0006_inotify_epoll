#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#if 0
typedef union epoll_data {
   void        *ptr;
   int          fd;
   uint32_t     u32;
   uint64_t     u64;
} epoll_data_t;

#endif


#define DATA_MAX_LEN 500

/* usage: epoll <file1> [file2] [file3] ... */

int add_to_epoll(int fd, int epollFd)
{
	int result;
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(eventItem));
    eventItem.events = EPOLLIN;
    eventItem.data.fd = fd;
    result = epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &eventItem);
	return result;
}

void rm_from_epoll(int fd, int epollFd)
{
	epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}


int main(int argc, char **argv)
{
	int mEpollFd;
	int i;
	char buf[DATA_MAX_LEN];

    // Maximum number of signalled FDs to handle at a time.
    static const int EPOLL_MAX_EVENTS = 16;

    // The array of pending epoll events and the index of the next event to be handled.
    struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];

	
	if (argc < 2)
	{
		printf("Usage: %s <file1> [file2] [file3] ...\n", argv[0]);
		return -1;
	}

	/* epoll_create */
    mEpollFd = epoll_create(8);

	/* for each file:
	 * open it
	 * add it to epoll: epoll_ctl(...EPOLL_CTL_ADD...)
	 */
	for (i = 1; i < argc; i++)	 
	{
		//int tmpFd = open(argv[i], O_RDONLY|O_NONBLOCK);
		int tmpFd = open(argv[i], O_RDWR);
		add_to_epoll(tmpFd, mEpollFd);
	}

	/* epoll_wait */
	while (1)
	{
		
        int pollResult = epoll_wait(mEpollFd, mPendingEventItems, EPOLL_MAX_EVENTS, -1);
		for (i = 0; i < pollResult; i++)
		{
			printf("Reason: 0x%x\n", mPendingEventItems[i].events);
			int len = read(mPendingEventItems[i].data.fd, buf, DATA_MAX_LEN);
			buf[len] = '\0';
			printf("get data: %s\n", buf);
			//sleep(3);
		}
		
	}
	
	return 0;
}

