#include <stdio.h>
#include <stdlib.h>

#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h> 
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>

// #include <regex.h>
#include <string.h>
#include <math.h>

#include <utmpx.h>

// ssh luzushen@iits-b473-13.utsc-labs.utoronto.ca

// gcc code.c -lm

// conversions are based on ga, so 1kb = 10**3 bits, 1mb = 10**6 bits, 1GB = 10**9 bites
double conversionB(long bits, int resultUnit){
   
    double d = bits*1.0;
    double c = resultUnit * 1.0;
    // printf("%ld / %d\n%lf / %lf\nresult: %f    %lf\n", b, resultUnit, d, c, b/resultUnit, d/c);
    return d/c;
}

    
// used memory are calculated use total ram - used ram, we ignore buffered and shared as they 
// ususally have minimum effect to the result, 
double getUsedPhysicalMemory(struct sysinfo sys_info, int resultUnit){
    long totalR = sys_info.totalram * 1.0;
    long freeR = sys_info.freeram * 1.0;
    long usedR = totalR - freeR;
    double usedRUnit = conversionB(usedR, resultUnit);
    // printf("used phy ram: %lf\n", usedRUnit);
    return usedRUnit;  
}
    
    /*
// print header (sample number, memory used, refresh rate)
void printHeader(int sn, int tn, struct rusage res_useage){

    int bToGb = 1000000000;
    int bToMb = 1000000;
    int bToKb = 1000;
    int bToKByte = 8000;
    printf("Nbr of samples: %d -- every %d secs\n",sn,tn);

    // following uses mySystemStats.c, gets the memory used by your program
    printf("Memory useage: %ld kilobytes\n", res_useage.ru_maxrss);
}
*/

// physical memory = total memory, used physical = used ram
double getTotPhysicalMemory(struct sysinfo sys_info, int resultUnit){
    long totPhy = sys_info.totalram;
    double totPhyUnit = conversionB(totPhy, resultUnit);
    // printf("total phy ram: %lf\n", totPhyUnit);
    // long usedPhy = getUsedMemory(sys_info, resultUnit);
    return totPhyUnit;
}

// total virtual memory are calculated by total physica + total swap
double getTotVirtuallMemory(struct sysinfo sys_info, int resultUnit){
    long totPhy = sys_info.totalram;
    long totSwap = sys_info.totalswap;
    long totVir = totPhy + totSwap;
    double totVirUnit = conversionB(totVir, resultUnit);

    // printf("total vir ram: %lf\n", totVirUnit);
    // long usedPhy = getUsedMemory(sys_info, resultUnit);
    return totVirUnit;
}

// Used virutal memory are calculated by total virual - used swap - used physical
double getUsedVirtualMemory(struct sysinfo sys_info, int resultUnit){
    long totPhy = sys_info.totalram;
    long totSwap = sys_info.totalswap;
    long totVir = totPhy + totSwap;

    double usedPhy = sys_info.totalram - sys_info.freeram;
    double usedSwap = sys_info.totalswap - sys_info.freeswap;
    double usedVir = totVir * 1.0 - usedPhy - usedSwap;
    double usedVirUnit = conversionB(usedVir, resultUnit);

    // printf("used vir ram: %lf\n", usedVirUnit);
    // long usedPhy = getUsedMemory(sys_info, resultUnit);
    return usedVirUnit;
}

// return array containing memory info needed for this moment, also print them in desired format
double* getCurMemInfo(struct sysinfo sys_info, int resultUnit){

    double *p = malloc(sizeof(double) * 4);
    *p = getUsedPhysicalMemory(sys_info, resultUnit);
    *(p+1) = getTotPhysicalMemory(sys_info, resultUnit);
    *(p+2) = getUsedVirtualMemory(sys_info, resultUnit);
    *(p+3) = getTotVirtuallMemory(sys_info, resultUnit);

    return p;
}

// return array of double arrayes containing memory info needed for this moment
double** setupMemInfoArray(struct sysinfo sys_info, int resultUnit, int sn){

    double **allMemInfo = malloc(sizeof(double*) * sn);
    return allMemInfo;
}

// given current meminfo, update it with newest memInfo, return head
double** updateMemInfo(struct sysinfo sys_info, int resultUnit, double** allMemInfo, int iteration){

    double * curMemInfo = getCurMemInfo(sys_info, resultUnit);
    allMemInfo[iteration] = curMemInfo;
    return allMemInfo;
}

// given current meminfo, print out in required format
void printMemInfo(double* memInfo){
    printf("%.2lf GB / %.2lf GB  -- %.2lf GB / %.2lf GB\n", memInfo[0], memInfo[1], memInfo[2], memInfo[3]);
}

// given all meminfo, print the entire memory session out
void printAllMemInfo(double** allMemInfo, int iteration){
    int i=0;
    while (i<iteration){
        printMemInfo(allMemInfo[i]);
        i++;
    }
}

// print device info
void printDevInfo(struct utsname uts){
    printf("System Name = %s\nMachine Name = %s\nVersion = %s\nRelease = %s\nArchitecture = %s\n",
            uts.sysname,    uts.nodename,   uts.version,    uts.release,   uts.machine);
}

// print user info
void printAllUserInfo(){
    struct utmp *user_tmp;
    user_tmp = getutent();
    
    while (user_tmp != NULL){
        // device with many user is different somehow
        // doc says type = 7 = USER_PROCESS is normal process, which means its for user not system
        if (user_tmp->ut_type == 7){ 
            printf(" %s       %s (%s)\n", user_tmp->ut_user, user_tmp->ut_line, user_tmp-> ut_host);
        }
        
        user_tmp = getutent();
    }
}

// update information of given info
void sampleTime(unsigned long long * lastIdle, unsigned long long * lastTotal, unsigned long long * thisIdle, unsigned long long * thisTotal){
   
   FILE *fptr;
   
   fptr = fopen("/proc/stat","r");
   if (fptr == NULL){
    printf("file cannot be found");
    return;
   }

    unsigned long long totalTime = 0;
    unsigned long long  idleTime = 0;
    long temp = 0;
    char buffer[1024];


        fscanf(fptr, "%s", buffer);
            printf("temp going to be added: %ld\n", temp);

                printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("%s info:\n", buffer);
            fscanf(fptr, "%s", buffer);
        temp = strtol(buffer, NULL, 10);
        totalTime = totalTime + temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("user time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("nice time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("system time:%s\n", buffer);
            fscanf(fptr, "%s", buffer);
            printf("error here: scanned: %s, given number : %ld\n", buffer, strtol(buffer, NULL, 10));
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            idleTime = idleTime + temp;

            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("idle time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("iowait time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("irq time:%s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("softirq time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("steal time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            printf("temp going to be added: %ld\n", temp);
            totalTime = totalTime +temp;
            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        printf("guest time:%s\n", buffer);
                fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;

            printf("temp going to be added: %ld\n", temp);

        printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

    if (*thisTotal==-1) {
        *thisIdle = idleTime;
        *thisTotal = totalTime;
    } 
    else{
        *lastIdle = *thisIdle;
        *lastTotal = *thisTotal;
        *thisIdle = idleTime;
        *thisTotal = totalTime;
    }
    printf("lastIdle: %llu, lasdtTotal: %llu, thisIdle: %llu, thisTotal: %llu\n", *lastIdle, *lastTotal, *thisIdle, *thisTotal);



    fclose(fptr);
}

double calcCpuUsage(unsigned long long * lastIdle, unsigned long long * lastTotal, unsigned long long * thisIdle, unsigned long long * thisTotal){
    
    double cpu_usage;

    if (*lastTotal == -1 && *lastIdle == -1) { // if calculation is made for the first time (no previous time point)
        cpu_usage = 0;    
    } 
    else { // if previous time point exists
        cpu_usage = 100 - (*thisIdle - *lastIdle) * 100.0 / (*thisTotal - *lastTotal);;
    }

    return cpu_usage;
}


int main(int arg, char *arguments[]){

    /* for memory usage
    struct rusage res_usage; // struct for memory usage
    getrusage(RUSAGE_SELF, &res_usage);
    
    printHeader(10,1,res_usage);
    */
    
    /* for memory usage
    struct sysinfo sys_info;
    sysinfo (&sys_info);
     
    if(sysinfo(&sys_info) != 0)
        perror("sysinfo error");

    // Uptime
    int days = sys_info.uptime / 86400;
    int hours = (sys_info.uptime / 3600) - (days * 24);
    int mins = (sys_info.uptime / 60) - (days * 1440) - (hours * 60);
    int bToGb = 1000000000;
    int bToMb = 1000000;
    int bToKb = 1000;

    printf("sys_info:\n");
    printf("    up time: %ddays, %dhours, %dminutes, %ldseconds\n   total ram: %lf\n    ram left: %lf\n    shared ram: %lf\n    buffered ram: %lf\n",
            days, hours, mins, sys_info.uptime % 60, 
            conversionB(sys_info.totalram, bToGb), 
            conversionB(sys_info.freeram, bToGb), 
            conversionB(sys_info.sharedram, bToGb), 
            conversionB(sys_info.bufferram, bToGb));
    

    // printf("%.2lf\n", getUsedMemory(sys_info, bToGb));
    // printf("%.2lf\n", getUsedMemory(sys_info, bToMb));
    // printf("%.2lf\n", getUsedMemory(sys_info, bToKb));
    getTotPhysicalMemory(sys_info, bToGb);
    getUsedPhysicalMemory(sys_info, bToGb);
    getTotVirtuallMemory(sys_info,bToGb);
    getUsedVirtualMemory(sys_info, bToGb);

    double *cm = getCurMemInfo(sys_info, bToGb);
    printf("%.2lf GB / %.2lf GB  -- %.2lf GB / %.2lf GB\n", cm[0], cm[1], cm[2], cm[3]);
    
    printf("---------------------------------------------------\n");
    double **allMemInfo = setupMemInfoArray(sys_info, bToGb, 5);
    // allMemInfo[0] = updateMemInfo(sys_info,bToGb,allMemInfo,0);
    // allMemInfo[1] = updateMemInfo(sys_info,bToGb,allMemInfo,1);
    // allMemInfo[2] = updateMemInfo(sys_info,bToGb,allMemInfo,2);
    // allMemInfo[3] = updateMemInfo(sys_info,bToGb,allMemInfo,3);
    // allMemInfo[4] = updateMemInfo(sys_info,bToGb,allMemInfo,4);

    // test if meminfo print work
    for (int i=0;i<5;i++){
        printf("---------------------------------------------------\n");
        allMemInfo = updateMemInfo(sys_info,bToGb,allMemInfo,i);
        printAllMemInfo(allMemInfo, i);
    }
    */
    
    /* for system infomation
        struct utsname uts;
        uname(&uts);
        printf("sysName: %s\n\n", uts.sysname);
        printf("nodeName: %s\n\n", uts.nodename);
        printf("release: %s\n\n", uts.release);
        printf("version: %s\n\n", uts.version);
        printf("machine: %s\n\n", uts.machine);
        printDevInfo(uts);

    */

    /*
        // idea from piazza post https://piazza.com/class/lbolnarwe9w6st/post/96, user_tmp give pointer to utmp struct
        struct utmp *user_tmp;
        user_tmp = getutent();
        
        while (user_tmp != NULL){
            // if (*(user_tmp->ut_user) != '\0'){


            // }

            printf("----------------------\n");
                printf("user: %s|end\n",user_tmp->ut_user);
                printf("device name of tty: %s|end\n", user_tmp->ut_line);
                printf("device host: %s\n", user_tmp->ut_host);
            user_tmp = getutent();
        }

        printAllUserInfo();
    */

   FILE *fptr;
//    FILE *fptr2;
   fptr = fopen("/proc/stat","r");
    // fptr2=fptr;
   if (fptr == NULL){
    printf("file cannot be found");
    return 0;
   }

    unsigned long long *thisIdle = calloc(sizeof(unsigned long long), 1);
    unsigned long long *thisTotal = calloc(sizeof(unsigned long long), 1);
    unsigned long long *lastIdle = calloc(sizeof(unsigned long long), 1);
    unsigned long long *lastTotal = calloc(sizeof(unsigned long long), 1);

    *thisTotal = -1;
    int a = 15045910;
    a = a + 5107544;
    printf("heres a number: %d\n", a);

    // sampleTime(lastIdle, lastTotal, thisIdle, thisTotal);
    // sleep(2);
    // sampleTime(lastIdle, lastTotal, thisIdle, thisTotal);

    // printf("cpu usage: %.2lf\n", calcCpuUsage(lastIdle, lastTotal, thisIdle, thisTotal));

    char buffer[1024];
        fscanf(fptr, "%s", buffer);
        fscanf(fptr, "%s", buffer);

    int numOfCore = 0;
    while (buffer[0] != 'i' ){
            if (buffer[0] == 'c' && buffer[1] == 'p' && buffer[2] == 'u'){
                numOfCore +=1;
                printf("__%s\n", buffer);
            }
           fscanf(fptr, "%s", buffer);
                printf("__%s\n", buffer);
    }

    printf("num of core: %d\n", numOfCore);

    // while (buffer != NULL){
    //     fgets(buffer, 255, (FILE*)fptr);
    //     printf("__ %s\n", buffer);  
    //     sleep(100);  
    // }
    fclose(fptr);
    // fclose(fptr2);


    return 0;
}

