# System-Monitoring-Tool
  
 ##### argument handeling
  - The program should take stackable arugments in any order.
 
      | index | representation | Description |
      | --- | --- | --- |
      | 1 | --system | default value = 0, if arguement detected: 1|
      | 2 | --user | default value = 0, if arguement detected: 1 |
      | 3 | --graphics | default value = 0, if argument detected: 1 (unavaliable)|
      | 4 | --sequential | default value = 0, if argument detected: 1 |
      | 5 | --saples=N | default value = 10, if arguement detected: N, N can be arbitary positive natural number|
      | 6 | --tdelay=N | default value = 1, if argument detected: N, N can be arbitary positive natural number|
      
##### gather data
The full functionality of the program would require data gathering of the device the program runs in: program memory usage, used and total physical/virtual memory of the device, session users/terminal of connection and machine connected, number of cores, cpu usage, and general information of the device running the progra.
  - for program memory usage, I used `rusage` struct from `<sys/resource.h>` to get information about resource utilization of my program. By the [document](https://man7.org/linux/man-pages/man2/getrusage.2.html), `getrusage()` returns usage measure of who, and its attribute `ru_maxrss` gives memory usage in kb
  - for memory data, I used `sysinfo` struct from `<sys/sysinfo.h>`. By the [document](https://man7.org/linux/man-pages/man2/sysinfo.2.html), `sysinfo()` loads information to given location needed to calculate memory usage. All data are given in unit of bit. All memory information will be converted GB up to 2 decimal places, therefore some attributes insignificant to the result are omitted. The conversion rate: 1Gb = 10^9 bits. 
    - total physical memory:  `totalram` gives total physical ram for the device. no calculation needed
    - used physical memory:   `freeram` give amount of physical ram left for the device. we can calculate using previous attribute: total - free = used
    - total virtual memory:   `totalswap` give amount of extra memory reserved by the device. we can calculate using previous attribute: totalVirtual = totalPhysica + totalSwap
    - used virtual memory:    `freeswap` give amount of extra memory reserved by not used by the device. we can calculate using privious attribute: usedVirutla = totalVirtual - freePhysical - freeVirtual
  - for user data, I used `utmp` struct from `<sys/utmp.h>`. By the [document](https://man7.org/linux/man-pages/man3/getutent.3.html), I can use `setutent()` to get access to the beginning of utmp file and extract all user information as `utmp` by iterating the file using `getutent()`. For each iteration, a process is loaded, and we can filter out non-user process using `ut_type` attribute. All the information needed for this part [are stored here](https://man7.org/linux/man-pages/man5/utmp.5.html). 
    -  session user:  `ut_user` gives the name
    -  terminal:   `ut_line`  gives the terminal that user is connected to
    -  machine connected: `ut_host` give the information about user's device connected
  -  for cpu cores, I define core as number of threads we can work with on the software side, which will equal to the number of cpus shown on `/proc/stat`. I simply used `File` struct from `<stdio.h>`. The program open the file using `scanf()` and calculate the amount of threads it has by counting amount of valid "cpu" is has until the end of cpu secion
  -  for cpu usage, I calculated it using the percentage in the change between two measurements of cpu time found in `/proc/stat`. Source of how to read `/proc/stat` is from [here](https://www.kgoettler.com/post/proc-stat/). I consider iowait part of total cpu time because those occurr as part of the entire process of disk I/O handeling
    -  total cpu time would be the all the time recorded regaring cpu which is the sum of all numbers on the first row: 
      -  totalCpuTime = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_quest
    -  total idle cpu time would be the time cpu spend idling which is the fourth number on the first row of `/proc/stat`: 
      -  totalIdleTime = idle
    -  cpu usage = 100 - 100 * (thisIdle-lastIdle)/(thisTotal-lastTotal)
   
  -  for general inforamtion, I used `utsname` from `<sys/utsname.h>`. The [site](https://man7.org/linux/man-pages/man2/uname.2.html) lets you to extract desired device information. Function `uname()` initialize the structre, and I can access the wanted informaition using that pointer
    -  system name:`sysname`
    -  machine name:`nodename`
    -  version of the operating system:   `version`
    -  release of the operating system:   `release`
    -  machine's architecture:    `machine`

### Functions
|Name|Description|
|---|---|
|void processArg(int argNum, char *argument[], int *funcArr, int SP, int TP)| it reads user input and stores the result in `funcArr`. `argNum` and `argument` are the parameteres from main. `SP` and `TP` are the prefix length of `--samples=` and `--tdelay=`. it ignores invalid argument but will print message if given integer is invalid. if no valid argument given, the programs runs by default|
|void printProgramInfo(int sn, int tn, struct rusage res_useage)| prints first section that shows number of samples using `sn` and delay between samples using `tn`, and how much memory utilized by this program using `res_useage`|
|double** updateMemInfo(struct sysinfo sys_info, int resultUnit, double** allMemInfo, int iteration)| updates array that stores all memory info pointed by `allMemInfo` with a new sample of memory info and returns the same pointer. `sys_info` is used to extract current memory info, and resultUnit is to numer of bits required to convert the result into wanted unit.|
|void printAllMemInfo(double* allMemInfo[], int iteration, int sn, int sequencial)| prints the content of the entire memory secion. `allMemInfo` is the array storing all memory samples,  `iteration` is the number of iterations the program is on, `sn` is number of samples, and `sequential` tells it if it should print in sequentcial form or not.|
|void printAllUserInfo()|prints all user session. for each call, it would create `temp` structure and extract all user information from utmp file|
|void displayCpuCore()| reads through `proc/stat` file, count how many threads the current cpu has and print the number|
|void calcCpuUsage(unsigned long long * lastIdle, unsigned long long * lastTotal, unsigned long long * thisIdle, unsigned long long * thisTotal)| calculates the cpu usage as described above and print them. after each calculation, `thisTotal` and `thisIdle` will be replace by a newly sampled cpu time while the orginal will be stored in `lastTotal` and `lastIdle` respectively|
|void refresh(int *funcArray, int iteration, double **allMemInfo, unsigned long long * lastIdle, unsigned long long * lastTotal, unsigned long long * thisIdle, unsigned long long * thisTotal)|displays the information according to user input. `funcArray` contains users input. `iteration` is the number of samples the program has completed, `allMemInfo` points to the array that records all the memory info that has been sampled. The rest are the pointers to the cpu time needed to calculate cpu usage|
|void displaySysInfo()| prints device information at the end of all sampleing. create `utsname` structure and extract information to print|

### User Instruction
  1. goto the direction on terminal.
  2. type `gcc c1.c -lm -o systeminfo` to compile
  3. type `./systeminfo` to run the program with 0 or more of the following flag(s):
  
      | flag| Description |
      | --- | --- |
      | `--system` | display system memory usage |
      | `--user` | display system user info |
      | ~--graphics~ | ~adds graphics to cpu usage and memory info (currently not avaliable)~|
      | `--sequential` | display information one after another (gives you access to all information produced) |
      |  `--saples=N` | determines how many samples the program needs to do. When using it, swap N for any positive Natural number, default value is 10|
      | `--tdelay=N` | determines how long should the program wait before making another sample. When using it, swap N for any positive Natural number, default value is 10|
  

