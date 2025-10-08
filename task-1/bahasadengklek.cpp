#include <iostream>
#include <string.h>

using namespace std;

int main()
{
    string kata;

    cin >> kata;

    int N;
    N = kata.length();

    for(int i = 0; i < N; i++) {
        if(kata[i] >= 'A' && kata[i] <= 'Z') {
            kata[i] += 32;
        } else {
            kata[i] -= 32;
        }
    }

    cout << kata;

    return 0;
}
