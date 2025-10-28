#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// Function to solve Josephus problem using an array
void josephusArray(int n, int m) {
    if (n <= 0 || m <= 0) {
        cout << "WRONG" << endl;
        return;
    }

    vector<int> people(n);
    for (int i = 0; i < n; ++i) {
        people[i] = i + 1;
    }

    int index = 0;
    while (n > 1) {
        index = (index + m - 1) % n;
        cout << people[index] << " ";
        people.erase(people.begin() + index);
        --n;
    }
    cout << people[0] << endl;
}

// Function to solve Josephus problem using a linked list
struct Node {
    int data;
    Node* next;
    Node(int x) : data(x), next(nullptr) {}
};

void josephusList(int n, int m) {
    if (n <= 0 || m <= 0) {
        cout << "WRONG" << endl;
        return;
    }

    Node* head = new Node(1);
    Node* prev = head;
    for (int i = 2; i <= n; ++i) {
        prev->next = new Node(i);
        prev = prev->next;
    }
    prev->next = head; // Complete the circle

    Node* current = head;
    while (current->next != current) {
        for (int i = 1; i < m; ++i) {
            prev = current;
            current = current->next;
        }
        prev->next = current->next;
        cout << current->data << " ";
        delete current;
        current = prev->next;
    }
    cout << current->data << endl;

    // Clean up the remaining node
    delete current;
}

int main() {
    int n, m;
    ifstream infile("input.txt");
    if (infile.is_open()) {
        infile >> n >> m;
        infile.close();

        josephusArray(n, m);
        cout << endl;
        josephusList(n, m);
        cout << endl;
    } else {
        cout << "File cannot be opened" << endl;
    }
    return 0;
}