///////////////////////////////////////////////////////////////////////////////
// qs.cpp - Quicksort sorting algorithm
// A very practical in place sorting algorithm with worst case O(n^2) and an
// expected running time of O(n log n)
// Travis Montoya
// Monday, May, 26, 2014
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Partition our vector for sorting
template <typename T>
unsigned int partition(vector<T>& a, unsigned int p, unsigned int r)
{
    auto x = a.at(r);          // Select a pivot element to sort vector
    register auto i = p - 1;   // i must be p - 1 and j must equal p at
    register decltype(i) j;    // start because no values are between i,p
    
    // Reearrange a[p..r]
    for (j = p; j <= (r-1); j++)
    {
        if (a.at(j) <= x)
        {
            i++;
            swap(a.at(i), a.at(j));
        }
    }
    swap(a.at(i+1), a.at(r));

    // Return new index to quicksort
    return (i+1);
}

///////////////////////////////////////////////////////////////////////////////
// Sort our two subarrays
template <typename T>
vector<T> quicksort(vector<T>& a, unsigned int p, unsigned int r)
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
    vector<int> x { 9,1,7,8,3,2,4,6,5,10,20,18,15,12,11,14,16,19,17,13,
                    21,30,22,26,29,25,23,28,27,24,31,33,35,32,34 
                  };

    vector<int>::iterator xit = x.begin();

    quicksort(x, 0, x.size()-1);

    while(xit != x.end()) cout << *xit++ << " ";
    cout << endl;
}
