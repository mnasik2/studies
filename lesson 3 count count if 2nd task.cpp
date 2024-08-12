#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
bool StartsWithA(const string& y){
        if(!y.empty()){
           char first_letter = y[0];
           return isupper(first_letter);
        }
        return false;
  }

int CountStartsWithA(const vector<string>& xs) {
    int answer = count_if(xs.begin(), xs.end(), StartsWithA);
    return answer;
}

int main() {
    // не меняйте тело main
    cout << CountStartsWithA({"And"s, "another"s, "one"s, "gone"s, "another"s,
                              "one"s
                              "bites"s,
                              "the"s, "dust"s});

}