#include <stdio.h>

struct AActor 
{
    int* GetValPtr() { return nullptr; }
    AActor* GetActor() { return nullptr; }
    
    template<typename T>
    T GetNull() { return nullptr; }
};

struct UWorld 
{
    AActor* GetActor() { return nullptr; }
};

template<typename T>
T* GetNullPtr() { return nullptr; }

int main() 
{
    AActor* LocalActor = GetNullPtr<UWorld>()?->GetActor();
	
    int* LocalVar = LocalActor?->GetValPtr();
    void* LocalVoid = LocalActor?->GetNull<void*>();
    float* LocalPtr = LocalActor?->GetNull<float*>();

    printf("Template instantiation: OK\n");
    printf("Type deduction for '?->': OK\n");

    // Note: 
    // The following line will not compile (and rightly so!) because float is not a pointer or void
    // float LocalValue = LocalActor?->GetNull<float>(); 

    return 0;
}
