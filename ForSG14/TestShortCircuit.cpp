#include <stdio.h>

struct FProcessor 
{
    void Execute(int Value) { printf("Executing with value %d\n", Value); }
};

int main() 
{
    FProcessor* Processor = nullptr;
    int SideEffectCounter = 0;
	
    auto GetValue = [&]() {
        SideEffectCounter++; // This increment should NOT happen
        return 100;
    };

    printf("Calling method on nullptr with side-effect argument...\n");
    
    Processor?->Execute(GetValue());

    if(SideEffectCounter == 0)
        printf("Success: Side-effect was correctly skipped.\n");
    else
        printf("ERROR: Side-effect occurred! Implementation bug.\n");

    return 0;
}
