#include <stdio.h>
#include <stdlib.h>

struct UComponent;
struct AActor;

typedef struct UComponent 
{
    void (*Activate)(struct UComponent* self);
} UComponent;

typedef struct AActor 
{
    UComponent* (*GetComp)(struct AActor* self);
    UComponent* InternalComp;
} AActor;

void ComponentActivate(UComponent* self) 
{
    printf("Success: Component activated.\n");
}

UComponent* ActorGetComp(AActor* self) 
{
    return self->InternalComp;
}

int main() 
{
    UComponent ValidComp = { .Activate = ComponentActivate };
    AActor ValidActor = { 
        .GetComp = ActorGetComp, 
        .InternalComp = &ValidComp 
    };

    AActor* ValidPtr = &ValidActor;
    AActor* NullPtr = NULL;

    printf("Starting Final Test of '?->' operator...\n\n");

    printf("Test 1: Valid call\n");
    ValidPtr?->GetComp(ValidPtr)?->Activate(ValidPtr->InternalComp);

    printf("\nTest 2: Safe chain with Null Actor (Starting with NULL)\n");
    NullPtr?->GetComp(NullPtr)?->Activate(NULL); 
    printf("Passed Test 2 (No crash).\n");

    printf("\nTest 3: Safe chain with Null Component (NULL in the middle)\n");
    AActor ActorWithNoComp = { .GetComp = ActorGetComp, .InternalComp = NULL };
    AActor* MiddleNullPtr = &ActorWithNoComp;
    MiddleNullPtr?->GetComp(MiddleNullPtr)?->Activate(NULL);
    printf("Passed Test 3 (No crash).\n");

    printf("\n--- All Tests Finished Successfully! ---\n");

    return 0;
}
