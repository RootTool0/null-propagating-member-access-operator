#include <stdio.h>

struct FNode 
{
    FNode* Next;
    int Value;
};

int main() 
{
    FNode* Root = nullptr;

    printf("Attempting to get 'next' from nullptr root...\n");
    FNode* Result = Root?->Next?->Next;

    if (Result == nullptr)
        printf("Success: Chain correctly returned nullptr.\n");
    else
        printf("Error: Chain returned non-null value!\n");

    return 0;
}
