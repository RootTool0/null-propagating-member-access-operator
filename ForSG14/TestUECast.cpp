#include <stdio.h>

struct UComponent 
{
    void Activate() { printf("Success: Component activated.\n"); }
};

struct AActor 
{
    UComponent* GetComp() { return nullptr; }
};

struct ADoor : public AActor 
{
    void Open() { printf("Success: Door opened.\n"); }
};

int main() 
{    
    ADoor* ValidDoor = new ADoor();
    AActor* ActorPtr = ValidDoor;
    AActor* NullActor = nullptr;

    printf("1. Testing valid Cast and call:\n");
    static_cast<ADoor*>(ActorPtr)?->Open();

    printf("2. Testing null Actor safe access:\n");
    NullActor?->GetComp()?->Activate();
    
    printf("Test finished.\n");
    delete ValidDoor;
    return 0;
}
