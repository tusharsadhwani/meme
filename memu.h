// Code taken from https://github.com/kabsly/memu
// Info:
//     Original Author: David Robert Nadeau
//     Original Site:   http://NadeauSoftware.com/
//     Modified by: Kabsly
//     License: Creative Commons Attribution 3.0 Unported License
//     http://creativecommons.org/licenses/by/3.0/deed.en_US
#ifndef MEMU_HEADER
#define MEMU_HEADER

#include <stddef.h>
#include <stdio.h>

#ifdef MEMU_IMPLEMENTATION
#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>
#endif
#else
#error "memu error: Unsupported operating system"
#endif
#endif /* MEMU_IMPLEMENTATION */

#ifdef MEMU_STATIC
#define MEMU_PREFIX static
#else
#define MEMU_PREFIX
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /* Prototypes */
  MEMU_PREFIX size_t memu_get_peak_rss();
  MEMU_PREFIX size_t memu_get_curr_rss();

#ifdef MEMU_IMPLEMENTATION

  MEMU_PREFIX size_t memu_get_peak_rss()
  {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.PeakWorkingSetSize;

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    struct psinfo psinfo;
    int fd = -1;
    if ((fd = open("/proc/self/psinfo", O_RDONLY)) == -1)
      return (size_t)0L;
    if (read(fd, &psinfo, sizeof(psinfo)) != sizeof(psinfo))
    {
      close(fd);
      return (size_t)0L;
    }
    close(fd);
    return (size_t)(psinfo.pr_rssize * 1024L);

#elif true || defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
#if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif

#else
    return (size_t)0L;
#endif
  }

  MEMU_PREFIX size_t memu_get_curr_rss()
  {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                  (task_info_t)&info, &infoCount) != KERN_SUCCESS)
      return (size_t)0L;
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    long rss = 0L;
    FILE *fp = NULL;

    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
      return (size_t)0L;
    if (fscanf(fp, "%*s%ld", &rss) != 1)
    {
      fclose(fp);
      return (size_t)0L;
    }

    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);

#else
    return (size_t)0L;
#endif
  }

  // Defined by Tushar: 
  MEMU_PREFIX size_t memu_proc_get_curr_rss(unsigned int procid)
  {
#if defined(_WIN32)
    HANDLE proc;
    proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procid);
    if (proc == NULL) {
      return (size_t)0L;
    }
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(proc, &info, sizeof(info));
    return (size_t)info.WorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    mach_port_t task = MACH_PORT_NULL;  
    kern_return_t err = KERN_SUCCESS;
    err = task_for_pid(mach_task_self(), (pid_t)procid, &task);
    if (err != KERN_SUCCESS) {
      fprintf(stderr, "Error %d\n", err);
      return (size_t)0L;
    }

    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(task, MACH_TASK_BASIC_INFO,
                  (task_info_t)&info, &infoCount) != KERN_SUCCESS) {
      mach_port_deallocate(mach_task_self(), task);
      return (size_t)0L;
    }
    
    mach_port_deallocate(mach_task_self(), task);
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    long rss = 0L;
    FILE *fp = NULL;

    char procpath[30];
    sprintf(procpath, "/proc/%d/statm", procid);
    if ((fp = fopen(procpath, "r")) == NULL)
      return (size_t)0L;
    if (fscanf(fp, "%*s%ld", &rss) != 1)
    {
      fclose(fp);
      return (size_t)0L;
    }

    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);

#else
    return (size_t)0L;
#endif
  }

#endif /* MEMU_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* MEMU_HEADER */
