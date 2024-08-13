#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

bool StartsWithA(const string& x){
        if(!x.empty()){
           char first_letter = x[0];
           return isupper(first_letter); //проверка первого символа
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