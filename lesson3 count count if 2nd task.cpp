#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

bool StartsWithA(const string& str){
        return !str.empty() && (str[0] == 'A' || str[0] == 'a');
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