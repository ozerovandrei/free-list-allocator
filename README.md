# Custom free list allocator

Free list allocator that provides `first-fit`, `next-fit` and `best-fit`
allocation strategies.

## Compilation command (MacOS)

```
clang++ -std=c++17 -stdlib=libc++ -O3 -Wvla-extension ./src/main.cpp
```
