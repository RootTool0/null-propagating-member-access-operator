#include <stdio.h>

struct AActor 
{
    int ID;
};

struct UWorld 
{
    constexpr AActor* GetConstexprActor() const { return nullptr; }
};

constexpr int TestConstexpr() 
{
    UWorld* LocalWorld = nullptr;
    AActor* LocalActor = LocalWorld?->GetConstexprActor();
    return LocalActor ? 1 : 0;
}

static_assert(TestConstexpr() == 0, "Constexpr Support: FAILED");

int main() 
{
    printf("Result of TestConstexpr(): %d\n", TestConstexpr());
    return 0;
}
