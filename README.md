# Clang with Null-Propagating Member Access Operator (?->)

This is a fork of LLVM/Clang implementing the `?->` operator for C++.  
Proof-of-concept for SG14 discussion.

## What is ?->

Null-propagating member access operator - syntactic sugar for chained null checks.

From this:
```cpp
UWorld* World = GEngine->GetWorld();
if (!World) return nullptr;

ULevel* Level = World->GetCurrentLevel();
if (!Level) return nullptr;

AActor* Actor = Level->GetActor();
return Actor;
```

Like this:
```cpp
return GEngine?->GetWorld()?->GetCurrentLevel()?->GetActor();
```

## Semantics

| Expression | Equivalent to | Result type |
|------------|-------------|-------------|
| `ptr?->field` | `ptr != nullptr ? ptr->field : nullptr` | Pointer type |
| `ptr?->method()` | `ptr != nullptr ? ptr->method() : (void)0` | void |
| `a?->b?->c` | Nested conditional operators | Pointer type |

**Constraints:**
- Base expression evaluated exactly once (OpaqueValueExpr)
- Result must be pointer or void type
- Chainable

## Why pointer and void only?

These are the only two types where we can safely represent "something exists" versus "nothing" (nullptr).

For pointers, - the semantics are clear: either we have a valid pointer to the member, or we return nullptr. 
For void methods - we either execute the call or do nothing (void).

Other types don't have this natural "null" representation. What should `obj?->field` return if `field` is `int`? Zero? Negative one? A magic constant? What if `field` is an object type like `AActor`? Return a default-constructed instance? That changes semantics - default constructor might have side effects. The answer depends entirely on the domain, so I chose to forbid it rather than guess wrong.

```cpp
// Valid
UWorld* World = Object?->GetWorld(); // Pointer - OK
World?->Tick();                      // Void - OK

// Invalid - compile error
int ID = Object?->GetID();                     // ERROR: int is not pointer or void
FVector Location = GetActor()?->GetLocation(); // ERROR: FVector is not pointer or void
```

## Implementation

### Files changed

| File | Change |
|------|--------|
| `clang/lib/Parse/ParseExpr.cpp` | Parse `?->` in postfix expressions |
| `clang/lib/Sema/SemaExprMember.cpp` | Sema analysis, lowering to ConditionalOperator |
| `clang/include/clang/Basic/DiagnosticSemaKinds.td` | Error message for invalid result type |
| `clang/include/clang/Basic/TokenKinds.def` | `questionarrow` token |

### Key design decisions

1. **No new AST node** - reuses `ConditionalOperator` + `OpaqueValueExpr` for common subexpression elimination (evaluate base expression exactly once)
2. **Explicit `!= nullptr`** - no implicit pointer-to-bool conversion
3. **Immediate method call parsing** - `obj?->method()` parsed atomically

## Building

**Requirements:** Visual Studio 2022, Ninja, CMake

**Open:** `x64 Native Tools Command Prompt for VS 2022`

```cmd
cd C:\Path\To\GitHub\Clones\null-propagating-member-access-operator\
mkdir build
cd build

cmake -G Ninja ^
    -DLLVM_ENABLE_PROJECTS="clang" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DLLVM_TARGETS_TO_BUILD="X86" ^
    -DLLVM_ENABLE_DIA_SDK=OFF ^
    -DLLVM_ENABLE_ZLIB=OFF ^
    -DLLVM_ENABLE_LIBXML2=OFF ^
    -DLLVM_ENABLE_TERMINFO=OFF ^
    -DLLVM_ENABLE_LIBEDIT=OFF ^
    -DLLVM_ENABLE_ASSERTIONS=OFF ^
    -DLLVM_ENABLE_EH=OFF ^
    -DLLVM_ENABLE_RTTI=OFF ^
    -DLLVM_ENABLE_FFI=OFF ^
    -DLLVM_INCLUDE_TESTS=OFF ^
    -DLLVM_INCLUDE_EXAMPLES=OFF ^
    -DLLVM_INCLUDE_BENCHMARKS=OFF ^
    -DLLVM_INCLUDE_UTILS=OFF ^
    -DLLVM_BUILD_TOOLS=OFF ^
    -DLLVM_BUILD_UTILS=OFF ^
    -DLLVM_BUILD_DOCS=OFF ^
    -DLLVM_BUILD_BENCHMARKS=OFF ^
    -DLLVM_USE_RELATIVE_PATHS_IN_DEBUG_INFO=ON ^
    -DLLVM_OPTIMIZED_TABLEGEN=ON ^
    -DLLVM_LINK_LLVM_DYLIB=ON ^
    -DCLANG_LINK_LLVM_DYLIB=ON ^
    ../llvm

cmake --build . --target clang -j 12
```

**Note:** These flags produce a minimal Release build focused on Clang only. Adjust `-j 12` to your CPU core count.

## Testing

```cpp
struct AActor { void Destroy() {} };

struct UWorld
{
    AActor Actor;
    AActor* GetActor() { return &Actor; }
};

UWorld World = UWorld();
UWorld* GetWorld() { return &World; }

int main()
{
    GetWorld()?->GetActor()?->Destroy();
    return 0;
}
```

```cmd
.\bin\clang.exe -Xclang -ast-dump -fsyntax-only test.cpp
```

Expected: Nested `ConditionalOperator` with `OpaqueValueExpr` for CSE, explicit `!=` comparisons.

## Limitations

- **Frontend only** - no LLVM IR optimizations
- **No constexpr support** (not tested)
- **Template interactions** - basic cases work, edge cases unknown
- **Error recovery** - minimal

## Motivation

Target use cases:
- **Game engines** (Unreal Engine) - deep object hierarchies
- **Low-latency** - predictable branching, CSE guarantees
- **Embedded** - safe chaining without exceptions

## Status

Proof-of-concept for SG14 discussion. Not production-ready.

## Links

- SG14 mailing list: sg14@lists.isocpp.org
- Author: RootTool, 17yo hobbyist, UE/embedded/C++ enthusiast

## License

Same as LLVM: Apache 2.0 with LLVM exceptions.
