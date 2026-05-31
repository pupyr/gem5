#include <stdio.h>
#include <pthread.h>
#include <x86intrin.h>
#include <gem5/m5ops.h>

int TestSize = 8;

void printer(int* addr, int size){
    for(int i=0;i<size;i++)printf("%d ", addr[i]);
    printf("\n");
}

volatile int ready = 0;
volatile unsigned long long end = 0;

void connect(int *addr, int size){
    int mass_temp[size];
    int i=0, i1=0, i2=size/2;
    while(!(i1==size/2 && i2==size)){
        if(i1==size/2 || i2<size && addr[i1]>addr[i2]){
            mass_temp[i++] = addr[i2++];
        } else{
            mass_temp[i++] = addr[i1++];
        }
    }
    for(int i=0; i<size; i++) addr[i] = mass_temp[i];
}

void mergeSort(int* addr, int size){
    if(size>1){
        mergeSort(addr, size/2);
        mergeSort(addr+size/2, size-size/2);
    }
    connect(addr, size);
}

void* CPU0 (void* addr){
    while(!ready);
    mergeSort(addr, TestSize/2);
    end = __rdtsc();
    printf("CPU0 end %llu\n", end);
}

void* CPU1 (int* addr){
    mergeSort(addr, TestSize/2);
    end = __rdtsc();
    printf("CPU1 end %llu\n", end);
}

int main() {
    printf("============== %d ================\n", TestSize);

    pthread_t thread1;
    int mass[TestSize];
    for(int i=0; i<TestSize; i++) mass[i] = TestSize-i;

    // printer(mass, TestSize);

    unsigned long long start1, start2, tmp, true_end;
    start1 = __rdtsc();
    m5_reset_stats(0, 0);
    pthread_create(&thread1, NULL, CPU0, mass);
    ready=1;
    start2 = __rdtsc();
    printf("CPU start %llu\n", start2);
    CPU1(mass+TestSize/2);
    m5_dump_stats(0, 0);
    pthread_join(thread1, NULL);
    tmp = __rdtsc();
    connect(mass, TestSize);
    true_end = __rdtsc();
    end += true_end - tmp;

    printf("Код выполнился за %llu тактов.\n", true_end - start1);
    printf("Код без syscall выполнился за %llu тактов.\n", end - start2);
    printf("syscall выполнился за %llu тактов.\n", start2 - start1);
    printf("syscall/All %4f\n", (start2 - start1 + true_end - end)/((float)(true_end - start1))*100);
    printf("work/All %4f\n", (end - start2)/((float)(true_end - start1))*100);

    printf("================================\n");
    for(int i=0; i<TestSize; i++) mass[i] = TestSize-i;

    start2 = __rdtsc();
    printf("CPU start %llu\n", start2);
    m5_reset_stats(0, 0);
    mergeSort(mass, TestSize);
    m5_dump_stats(0, 0);
    end = __rdtsc();
    printf("CPU end %llu\n", end);

    printf("Код baseline выполнился за %llu тактов.\n", end - start2);

    // printer(mass, TestSize);
    return 0;
}
