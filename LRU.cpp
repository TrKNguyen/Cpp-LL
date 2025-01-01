#include <bits/stdc++.h>
using namespace std;
#define ll  long long
#define pll pair<ll,ll>
#define ff first
#define ss second
#define eb emplace_back
#define endl "\n"
mt19937_64 rnd;

const ll maxn=2e6+50;
const ll mod =998244353 ;
const ll base=1e13;

struct Node {
    int key, value; 
    Node *next; 
    Node *prev; 
    Node(int key, int value): key(key), value(value), next(nullptr), prev(nullptr) {}
}; 
class LRUCache {
    
public:
    Node *head; 
    Node *tail; 
    unordered_map<int, Node *> nodeMap; 
    int capacity;
    LRUCache(int capacity): capacity(capacity) {
        head = new Node(-1, -1); 
        tail = new Node(-1, -1); 
        head->next = tail; 
        tail->prev = head;
        nodeMap.clear(); 
    }
    void put(int key, int value) {
        if (!nodeMap.count(key)) {
            if (nodeMap.size() == capacity) {
                Node *node_need_erase = tail->prev;
                delete_operation(node_need_erase->key);
                // cout << node_need_erase->key <<" check" <<" "<<capacity <<" "<<nodeMap.size()<<endl;
                nodeMap.erase(node_need_erase->key);
                // cout << nodeMap.size()<<" check"<<endl;
            }
            nodeMap[key] = new Node(key, value);
            insert_operation(key);
        } else {
            nodeMap[key]->value = value;
            delete_operation(key); 
            insert_operation(key);
        }
    }
    int get(int key) {
        if (!nodeMap.count(key)) {
            return -1;
        }
        delete_operation(key); 
        insert_operation(key);
        return nodeMap[key]->value;
    }
    void insert_operation(int key) {
        Node *curNode = nodeMap[key];
        Node *nextHead = head->next;
        head->next = curNode;
        nextHead->prev = curNode; 
        curNode->prev = head; 
        curNode->next = nextHead;
    }
    void delete_operation(int key) {
        if (!nodeMap.count(key)) {
            return;
        }
        Node *curNode = nodeMap[key]; 
        curNode->next->prev = curNode->prev; 
        curNode->prev->next = curNode->next;
    }
}; 

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    if (fopen("t.inp", "r"))
    {
        freopen("test.inp", "r", stdin);
        // freopen("test.out", "w", stdout);
    }
   LRUCache cache(2);
  
    cache.put(1, 1); 
    cache.put(2, 2);
    cout << cache.get(1) << endl;
    cache.put(3, 3);
    cout  << cache.get(2) << endl;
    cache.put(4, 4);
    cout << cache.get(1) << endl;
    cout << cache.get(3) << endl;
    cout << cache.get(4) << endl;


        
}

