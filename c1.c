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
#include <time.h>
 #include <unistd.h>



// does arguments stack?    yes, all of them, use getopt
// range for N in samples and tdelay? N+
// do we memorize N? not across multiple calls
// how to display information: erase function -> erase everything and reprint them to show new information
// where to start?
// how do I do the thing shown in the demo?

// what if theres two same stuff in 1 command eg --system --system, --samples=1 --samples=100
    // error message? --> no need to deal with the rn, assume every input is valid
    // how should we deal with it? --> doesn't matter as long the code works with valid code (no repeat)

// if there a specific order for arguments? -> in order given in quercus pic
    // if given --system --user do I print same thing as give --user --system? -> yes

// anything specific on the gap in graphics of memory or num of core

// for Memory usage: 4092 kilobytes, do we need to find optimal unit st the number is in 4 decimal place?


// print all arguments given
void showArg(int argNum, char *argument[]){

    printf("%d arguments given, given arguments: \n",argNum);
    for (int i=0;i<argNum;i++){
        printf("    %s\n", argument[i]);
    }
}

// takes string as input, match with special argument syntax: prefix=N
// if match success, return total length of given string
int matchItem(char *argument, char *string_to_match, int prefix_len){

    int i = 0;

    // printf("given: %s\nmatch: %s\n", argument, string_to_match);
    // printf("%d\n", prefix_len);
    // match prefix (prefix=)
    while (*(argument+i) != '\0' && i < prefix_len){
        if (*(argument+i) != *(string_to_match+i)){
            // printf("argument match %s failed\n", string_to_match);
            return -1;
        }
        i++;
    }

    // match N (--samples=N)
    int sampleN = 0;
    int state = 0;
    while (*(argument+i) != '\0'){

        if (*(argument+i) <48 || *(argument+i)>=58){
            printf("invalid character for N, input: %s\n", string_to_match);
            return -1;
        }
        else {if (state == 1){
            printf("N value error, input: %s\n", string_to_match);
            return -1;
        }   else {if (*(argument+i) == '0' && i == prefix_len){
            state = 1;
            }   else {
                state = 2;
                }
            }
        }
        i++;
    }
    if (state == 0){
        printf("please provide arg value\n");
        return -1;
    }

    // printf("finishing state: %d\n", state);

    return i;
}

// takes a string as input, string must be valid syntax: prefix=N
int extractN(char *argument, int prefix_len, int arg_len){
    int starti = prefix_len;
    int n = 0;
    for (int i=0; i<arg_len-prefix_len; i++){

        // printf("a = %d, p = %d\n", (int)*(argument + starti + i)-48, arg_len - prefix_len - i - 1 );
        n += ((int)*(argument + starti + i)-48) * pow(10,(arg_len - prefix_len - i - 1));
    }
    // printf("N = %d\n", n);
    return n;
}

// reads arguments and decide which info is needed
// modified 
void processArg(int argNum, char *argument[], int *funcArr, int SP, int TP){

    int i=1;
    while (i<argNum){ // iterate all argument

        // printf("%d\n", i);
        if (!strcmp(argument[i], "--system")){
            funcArr[1] = 1;
            i++;

            continue;
        }

        if (!strcmp(argument[i], "--user")){
            funcArr[2] = 1;
            i++;

            continue;
        }

        if ((!strcmp(argument[i], "--graphics") || !strcmp(argument[i], "-g"))){
            funcArr[3] = 1;
            i++;

            continue;
        }

        if (!strcmp(argument[i], "--sequential")){
            funcArr[4] = 1;
            i++;

            continue;
        }

        int j = matchItem(argument[i], "--samples=", SP);
        if (j != -1){
            int sn = extractN(argument[i], SP, j);
            funcArr[5] = sn;
            i++;

            continue;
        }

        j = matchItem(argument[i], "--tdelay=", TP);
        if (j != -1){
            int tn = extractN(argument[i], TP, j);
            funcArr[6] = tn;
            i++;

            continue;
        }

        printf("failed to match argument: %s\n", argument[i]);
        // break;
    }

}

// print given funcArr
void printFunc(int *funcArr, int funcArrLen, char *funcArrMessage[]){

    printf("--system: %d\n", funcArr[1]);
    printf("--user %d\n", funcArr[2]);
    printf("--graphics %d\n", funcArr[3]);
    printf("--sequential %d\n", funcArr[4]);
    printf("--samples= %d\n", funcArr[5]);
    printf("--tdelay= %d\n", funcArr[6]);

}

void printDivider(){
    printf("---------------------------------------\n");
}

void printHeader(char *header, char *extraMessage) {
    // printDivider();
    if (extraMessage[0] == '\0'){
        printf("### %s ###\n", header);
        return;
    }

    printf("### %s ### (%s)\n", header, extraMessage);
}

// conversions are based on ga, so 1kb = 10**3 bits, 1mb = 10**6 bits, 1GB = 10**9 bites
double conversionB(long bits, int resultUnit){
   
    double d = bits*1.0;
    double c = resultUnit * 1.0;
    // printf("%ld / %d\n%lf / %lf\nresult: %f    %lf\n", b, resultUnit, d, c, b/resultUnit, d/c);
    return d/c;
}

// print programInfo (sample number, memory used, refresh rate)
void printProgramInfo(int sn, int tn, struct rusage res_useage){

    int bToGb = 1000000000;
    int bToMb = 1000000;
    int bToKb = 1000;
    int bToKByte = 8000;
    printf("Nbr of samples: %d -- every %d secs\n",sn,tn);

    // following uses mySystemStats.c, gets the memory used by your program
    printf("Memory useage: %ld kilobytes\n", res_useage.ru_maxrss);
    printDivider();
}

// physical memory = total memory, used physical = used ram
double getTotPhysicalMemory(struct sysinfo sys_info, int resultUnit){
    long totPhy = sys_info.totalram;
    double totPhyUnit = conversionB(totPhy, resultUnit);
    // printf("total phy ram: %lf\n", totPhyUnit);
    // long usedPhy = getUsedMemory(sys_info, resultUnit);
    return totPhyUnit;
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

/*
// print ramNode information
void printCurrentRamData(ramNode *node){
    printf("%.0lf GB / %.0lf GB  -- %.0lf GB / %.0lf GB\n", node->usedPhyRam, node->totPhyRam, node->usedVirRam, node->totVirRam);
}
*/

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
double** setupMemInfoArray(int sn){
    double **allMemInfo = malloc(sizeof(double*)* sn);
    for (int i =0;i<sn;i++){
        double *memInfoInstance = calloc(sizeof(double),4);
        allMemInfo[i] = memInfoInstance;
    }
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
    int i = 0;
    // printf("1: %lf\n", memInfo[0]);
    // printf("11: %lf\n", memInfo[1]);
    // printf("111: %lf\n", memInfo[2]);
    // printf("1111: %lf\n", memInfo[3]);
    
        printf("%.2lf GB / %.2lf GB  -- %.2lf GB / %.2lf GB\n", memInfo[0], memInfo[1], memInfo[2], memInfo[3]);

}

// given all meminfo, print the entire memory session out
void printAllMemInfo(double* allMemInfo[], int iteration, int sn, int sequencial){
    int i=0;
    while (i<sn){  
        if (sequencial == 0){
            if (i <= iteration ){
                printMemInfo(allMemInfo[i]);
            }
            else{
                printf("\n");
            }
        }
        else{
            if (i == iteration){
                printMemInfo(allMemInfo[i]);
            }
            else{
                printf("\n");
            }
        }
  
        i++;
    }
    printDivider();
}

// update idle time
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
            // printf("temp going to be added: %ld\n", temp);

                // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("%s info:\n", buffer);
            fscanf(fptr, "%s", buffer);
        temp = strtol(buffer, NULL, 10);
        totalTime = totalTime + temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("user time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("nice time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("system time:%s\n", buffer);
            fscanf(fptr, "%s", buffer);
            // printf("error here: scanned: %s, given number : %ld\n", buffer, strtol(buffer, NULL, 10));
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            idleTime = idleTime + temp;

            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("idle time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("iowait time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime + temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("irq time:%s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("softirq time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("steal time: %s\n", buffer);
            fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            // printf("temp going to be added: %ld\n", temp);
            totalTime = totalTime +temp;
            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

        // printf("guest time:%s\n", buffer);
                fscanf(fptr, "%s", buffer);
            temp = strtol(buffer, NULL, 10);
            totalTime = totalTime +temp;

            // printf("temp going to be added: %ld\n", temp);

        // printf("idleTime: %llu, totalTime: %llu\n", idleTime, totalTime);

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
    // printf("lastIdle: %llu, lasdtTotal: %llu, thisIdle: %llu, thisTotal: %llu\n", *lastIdle, *lastTotal, *thisIdle, *thisTotal);

    fclose(fptr);
    return;
}

// print number of cpu core
void displayCpuCore(){
    FILE *fptr;
   
    fptr = fopen("/proc/stat","r");
    if (fptr == NULL){
        printf("file cannot be found");
        return;
    }

    char buffer[1024];
    fscanf(fptr, "%s", buffer); // get first one
    fscanf(fptr, "%s", buffer); // skip first one


    int numOfCore = 0;
    while (buffer[0] != 'i' ){
            if (buffer[0] == 'c' && buffer[1] == 'p' && buffer[2] == 'u'){
                numOfCore +=1;
                // printf("__%s\n", buffer);
            }
           fscanf(fptr, "%s", buffer);
                // printf("__%s\n", buffer);
    }

    printf("num of core: %d \n", numOfCore);
    return;
}

// calcualte cpu usabe based on idle time, return cpu time
void calcCpuUsage(unsigned long long * lastIdle, unsigned long long * lastTotal, unsigned long long * thisIdle, unsigned long long * thisTotal){
    
    double cpu_use;

    cpu_use = 100 - (*thisIdle - *lastIdle) * 100.0 / (*thisTotal - *lastTotal);;

    printf(" total cpu use = %.2lf%%\n", cpu_use);
    return;
}

// print device info
void printDevInfo(struct utsname uts){
    printHeader("System Information", "");
    printf(" System Name = %s\n Machine Name = %s\n Version = %s\n Release = %s\n Architecture = %s\n",
            uts.sysname,    uts.nodename,   uts.version,    uts.release,   uts.machine);
    printDivider();
}

// print user info
void printAllUserInfo(){
    
    printHeader("Sessions/Users", "");
    
    setutent();
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
    // free(user_tmp);
    printDivider();
}

int refresh(int *funcArray, int iteration, double **allMemInfo, 
            unsigned long long * lastIdle, unsigned long long * lastTotal, 
            unsigned long long * thisIdle, unsigned long long * thisTotal){

    int bToGb = 1000000000;
    int sn = funcArray[5];
    int tn = funcArray[6];

    // p1
    struct rusage res_usage; // for current res used
    getrusage(RUSAGE_SELF, &res_usage);
    printProgramInfo(funcArray[5], funcArray[6], res_usage); // programinfo no need linkedlist, since only 1 item refresh

    // printFunc(funcArray,0,0);

    if (funcArray[1] == 1 || funcArray[2] == 0){
           // p2    
        printHeader("Memory", "Phys.Used/Tot -- Virtual Used/Tot");
        struct sysinfo sys_info;
        sysinfo (&sys_info);
        allMemInfo = updateMemInfo(sys_info,bToGb,allMemInfo,iteration);
        printAllMemInfo(allMemInfo, iteration, sn, funcArray[4]); // 5 iteration
    }

    if (funcArray[2] == 1 || funcArray[1] == 0){
         // p3
        printAllUserInfo();
    }

    if (funcArray[1] == 0 && funcArray[2] == 0){
    
        // p4 refere to this site https://www.kgoettler.com/post/proc-stat/
        displayCpuCore();
        calcCpuUsage(lastIdle, lastTotal, thisIdle, thisTotal); 
        printDivider();
    }


}

void displaySysInfo(){
    // p5
    struct utsname uts;
    uname(&uts);
    printDevInfo(uts);
}

int main(int argNum, char *argument[]){

    char SAMPLES[11] = "--samples=";
    char TDELAY[10] = "--tdelay=";

    int SP = 10; // lenght of s's prefix
    int TP = 9;  // length of p's prefix

    // showArg(argNum, argument);

    /*
    // if no argument, print everything
    if (argNum==1){
        return 0;
    }
    */

// argument handeling
    int *funcArray = (int*)calloc(7, sizeof(int)); // for argArray detail, follow notes
    funcArray[5] = 10; funcArray[6] = 1; // set up default value for the thing
    processArg(argNum, argument, funcArray, SP, TP);
    // printFunc(funcArray,0,0); // second and thrid are placeholder, not required but mice to implement
    double **allMemInfo = setupMemInfoArray(funcArray[5]); 

    // set up pointer to calculate cpu interval
    unsigned long long *thisIdle = calloc(sizeof(unsigned long long), 1);
    unsigned long long *thisTotal = calloc(sizeof(unsigned long long), 1);
    unsigned long long *lastIdle = calloc(sizeof(unsigned long long), 1);
    unsigned long long *lastTotal = calloc(sizeof(unsigned long long), 1);
    *thisTotal = -1;
    sampleTime(lastIdle, lastTotal, thisIdle, thisTotal);


    for (int i=0;i<funcArray[5];i++){

        if (funcArray[4] == 0){
            system("clear");
        }
        else{
            printf("\niteration: %d\n", i+1);
        }
        refresh(funcArray, i, allMemInfo, lastIdle, lastTotal, thisIdle, thisTotal);
        sleep(funcArray[6]);
    }
    
    displaySysInfo();

    return 0;
}
