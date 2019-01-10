# dvector

Effective array-like data structure supporting logarithmic indexing, insertion and deletion.

## Features
Example:
```cpp
#include "pch.h"
#include <iostream>
#include "../dvector/dvector.h"

int main()
{
    dv::dvector<std::string> the_dvector; // Constructs an empty dv::dvector, each element being a std::string
    the_dvector.push_back("deep");
    the_dvector.push_back("dark");
    the_dvector.push_back("fantasies");

    std::cout << the_dvector[0] << "♂" << the_dvector[1] << "♂" << the_dvector[2] << std::endl;
    // deep♂dark♂fantasies

    the_dvector.erase(1); // Erases element at index 1, which is "dark"

    std::cout << the_dvector[0] << "♂" << the_dvector[1] << std::endl;
    // deep♂fantasies
}
```
Output:
```
deep♂dark♂fantasies
deep♂fantasies
```

+ Header-only library, simply `#include "../dvector/dvector.h"` is sufficient to use it
+ Easy APIs that implement std::vector interface (Under process)
+ Based on [AVL tree](https://en.wikipedia.org/wiki/AVL_tree), to support effective insertion, deletion and indexing at random positions

## Usage

```
git clone https://github.com/hanayashiki/dvector
cd dvector/dvector
cp *.h /your/target/directory/
```

## Methods

+ Constructors
  ```cpp
  dvector(); // Construct a dvector
  dvector(const Vector & init_vector); // Build the dvector from another std::vector of the same parameters
  dvector(Vector && init_vector); // Build the dvector from another std::vector of the same parameters
  ```
+ Destructor = default
+ Insert, time complexity = O(log(N) + M), N = count of exisiting elements, M = count of inserted elements this time
  ```cpp
  void insert(const size_t index, const Vector & elements); 
  // Copy and insert multiply elements from another std::vector to `index`
  void insert(const size_t index, Vector && elements); 
  // Move and insert multiply elements from another std::vector to `index`
  void insert(const size_t index, const T & element);
  // Copy and insert a single element from another std::vector to `index`
  void insert(const size_t index, T && element);
  // Move and insert a single element from another std::vector to `index`
  void push_back(const T & element);
  // Copy and insert multiply elements from another std::vector to the end of the array
  void push_back(T && element);
  // Move and insert multiply elements from another std::vector to the end of the array
  ```
  + Notice that if you use `void insert(const size_t index, const Vector & elements)` or `void insert(const size_t index, Vector && elements)` to insert multiply elements in one time, the elements will be stored in a continuous memory space, which is associated with one AVL-tree node. Before the node is split due to deletion, the height of the tree will be far less than `O(log(N))`, so will be the time complexity.
+ Deletion, time complexity = O(log(N))
  ```cpp
  void erase(const size_t index);
  ```
+ Indexing, time complexity = O(log(N))
  ```
  const T & operator[] (const size_t index) const;
  T & operator[] (const size_t index);
  ```
  The range of index will be checked with `assert`.
  
## Benchmarks

+ push_back
  + dvector is about 10 times slower because extra logics.

|                         | best/ms | average/ms |
|-------------------------|---------|------------|
| dvector_push_back_10000 | 0.0184  | 0.0292479  |
| vector_push_back_10000  | **0.0017**  | **0.0026618**  |

+ insert and erase
  + insert N integers at random indices and erase them randomly

|                             | best/ms | average/ms |
|-----------------------------|---------|------------|
| dvector_insert_erase_1000   | **0.0365**  | **0.0411**     |
| vector_insert_erase_1000    | 24.6891 | 24.8332    |
| dvector_insert_erase_10000  | **0.4465**  | **0.495**      |
| vector_insert_erase_10000   | 22.9091 | 23.6994    |
| dvector_insert_erase_100000 | **6.483**   | **7.50873**    |
| vector_insert_erase_100000  | 21.6434 | 21.9778    |

+ insert and read
  + insert N integers at random indices and read them randomly.
  + dvector inserts and reads are all `O(log(N))`, vector inserts are `O(N)` and reads are `O(1)`

|                                   | best/ms | average/ms |
|-----------------------------------|---------|------------|
| dvector_random_insert_read_1000   | 0.0815  | 0.0839     |
| vector_random_insert_read_1000    | **0.0072**  | **0.0104333**  |
| dvector_random_insert_read_10000  | 0.943   | 1.004      |
| vector_random_insert_read_10000   | **0.2352**  | **0.2516**     |
| dvector_random_insert_read_100000 | **17.8459** | **18.7672**    |
| vector_random_insert_read_100000  | 26.7403 | 27.0092    |
| dvector_random_insert_read_200000 | **45.4127** | **46.0354**    |
| vector_random_insert_read_200000  | 125.456 | 127.428    |
