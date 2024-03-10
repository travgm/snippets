///////////////////////////////////////////////////////////////////////////////
// qs.cpp - Quicksort sorting algorithm
// A very practical in place sorting algorithm with worst case O(n^2) and an
// expected running time of O(n log n)
// Travis Montoya
// Monday, May, 26, 2014 - Initial implementation
// Saturday, March, 09, 2024 - Fixed bugs, added large vector testing, timing
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "ctimer.h"

/////////////////////////////////////////////////////////////////////////////////
// Create a large unsorted vector
std::vector<int> cvector(void) {
    std::srand(std::time(nullptr));
    std::vector<int> vec;

    for (int i = 0; i < 5000; ++i) {
        vec.push_back(std::rand() % 500 + 1);
    }

    return vec;
}

///////////////////////////////////////////////////////////////////////////////
// Partition our vector for sorting
template <typename T>
unsigned int partition(std::vector<T>& a, unsigned int p, unsigned int r)
{
    auto x = a.at(r); // Select a pivot element to sort vector
    auto i = p - 1;   // i must be p - 1 and j must equal p at
    decltype(i) j;    // start because no values are between i,p
    
    // Reearrange a[p..r]
    for (j = p; j <= (r-1); j++)
    {
        if (a.at(j) <= x)
        {
            i++;
            std::swap(a.at(i), a.at(j));
        }
    }
    std::swap(a.at(i+1), a.at(r));

    // Return new index to quicksort
    return (i+1);
}

///////////////////////////////////////////////////////////////////////////////
// Sort our two subarrays
template <typename T>
std::vector<T> quicksort(std::vector<T>& a, unsigned int p, unsigned int r)
{
    // A vector with 1 element is sorted already.
    if (a.size() <= 1) return (a);

    if (p < r)
    {
        unsigned int q = partition(a, p, r);
        a = quicksort(a, p, q-1);
        a = quicksort(a, q+1, r);
    }
    return(a);
}

int main()
{
    std::vector<int> x = cvector();
    std::vector<int>::iterator xit = x.begin();
    
    ctimer_t ct;
    ctimer_start(&ct);

    quicksort(x, 0, x.size()-1);

    ctimer_stop(&ct);
    ctimer_measure(&ct);

    while(xit != x.end()) {
      std::cout << *xit++ << " ";
    }
    std::cout << std::endl;

    ctimer_print(ct, "qs");
    return 0;
}
