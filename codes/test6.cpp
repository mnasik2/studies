#include <vector>
#include <iostream>

using namespace std;

int main(){
vector<int> v = {1, 2, 3};
v.push_back(4);
const int* data = v.data();
for (size_t i = 0; i < v.size(); ++i) {
    cout << *(data + i) << ' ';
}
cout << endl;
for (size_t i = 0; i < v.capacity(); ++i) {
    cout << *(data + i) << ' ';
} 
cout << endl;
}